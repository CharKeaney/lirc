#include "parser.h"

#define DEBUG_PARSER true

#define PARSING_STACK_BUFFER (2<<8)

typedef enum ASTNodeNames {
	AST_ARGS,
	AST_BINOP,
	AST_BLOCKS,
	AST_CONDITION,
	AST_CONSTANT,
	AST_ERROR,
	AST_FBOP,
	AST_COMMAND,
	AST_MODULE,
	AST_TERMINAL,
	AST_TYP,
	AST_TERMINATOR,
	AST_VAL,
	AST_PROD
} ast_node_name;

char* ast_name_lookup[] = {
	[AST_ARGS] = "AST_ARGS",
	[AST_BINOP] = "AST_BINOP",
	[AST_BLOCKS] = "AST_BLOCKS",
	[AST_ERROR] = "AST_ERROR",
	[AST_FBOP] = "AST_FBOP",
	[AST_CONDITION] = "AST_CONDITION",
	[AST_CONSTANT] = "AST_CONSTANT",
	[AST_COMMAND] = "AST_COMMAND",
	[AST_MODULE] = "AST_MODULE",
	[AST_TERMINAL] = "AST_TERMINAL",
	[AST_TERMINATOR] = "AST_TERMINATOR",
	[AST_VAL] = "AST_VAL",
	[AST_TYP] = "AST_TYP",
	[AST_PROD] = "AST_PROD"
};

#define lookup_ast_as_name(a) ast_name_lookup[a]


typedef enum  ParsingStates {
	ENCOUNTERED_,
	ENCOUNTERED_ARGS,
	ENCOUNTERED_BLOCKS,
	ENCOUNTERED_BR,
	ENCOUNTERED_BR_TYP,
	ENCOUNTERED_BR_TYP_ID,
	ENCOUNTERED_BR_TYP_ID_LABEL,
	ENCOUNTERED_BR_TYP_ID_LABEL_ID,
	ENCOUNTERED_BR_TYP_ID_LABEL_ID_LABEL,
	ENCOUNTERED_COMMAND,
	ENCOUNTERED_CONSTANT,
	ENCOUNTERED_DEFINE,
	ENCOUNTERED_DEFINE_ID,
	ENCOUNTERED_DEFINE_ID_ARGS,
	ENCOUNTERED_DEFINE_ID_ARGS_BLOCKS,
	ENCOUNTERED_GETELEMENTPTR,
	ENCOUNTERED_GETELEMENTPTR_TYP,
	ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR,
	ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL,
	ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL_TYP,
	ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL_TYP_VAL,
	ENCOUNTERED_INTEGERTYPE,
	ENCOUNTERED_ID,
	ENCOUNTERED_ID_EQ,
	ENCOUNTERED_ID_EQ_BOP,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_VAL,
	ENCOUNTERED_ID_EQ_FBOP_INTEGERTYPE_ID_COMMA_ID,
	ENCOUNTERED_ID_EQ_GETELEMENTPTR,
	ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP,
	ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP_VAL,
	ENCOUNTERED_ID_EQ_LOAD,
	ENCOUNTERED_ID_EQ_LOAD_TYP,
	ENCOUNTERED_ID_EQ_LOAD_TYP_PTR,
	ENCOUNTERED_ID_EQ_LOAD_TYP_TYP_ID,
	ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN,
	ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN_SZ,
	ENCOUNTERED_ID_EQ_LOAD_TYP_TYP,
	ENCOUNTERED_ID_EQ_ALLOCA,
	ENCOUNTERED_ID_EQ_ICMP,
	ENCOUNTERED_ID_EQ_ICMP_COND,
	ENCOUNTERED_ID_EQ_ICMP_COND_TYP,
	ENCOUNTERED_ID_EQ_ICMP_COND_TYP_VAL,
	ENCOUNTERED_OPENBRACKET,
	ENCOUNTERED_OPENBRACKET_SZ,
	ENCOUNTERED_OPENBRACKET_SZ_X,
	ENCOUNTERED_OPENBRACKET_SZ_X_TYP,
	ENCOUNTERED_BR_LABEL,
	ENCOUNTERED_STORE,
	ENCOUNTERED_STORE_TYP,
	ENCOUNTERED_STORE_TYP_VAL,
	ENCOUNTERED_STORE_TYP_VAL_VAL,
	ENCOUNTERED_BLOCKLABEL,
	ENCOUNTERED_TYP,
	ENCOUNTERED_TYP_PTR,
	ENCOUNTERED_TYP_PTR_IDENTIFIER,
	ENCOUNTERED_TYP_ID,
	ENCOUNTERED_TYP_ID_LABEL,
	ENCOUNTERED_TYP_ID_LABEL_ID,
	ENCOUNTERED_TYP_ID_LABEL_ID_LABEL,
	ENCOUNTERED_ID_EQ_ALLOCA_TYP,
	ENCOUNTERED_VAL,
	ENCOUNTERED_RET,
	ENCOUNTERED_RET_TYP,
	ENCOUNTERED_TERMINATOR,
	ENCOUNTERED_PROD,
	ENCOUNTERED_LAYOUT_NAMEDT,
	ENCOUNTERED_LAYOUT,
	ENCOUNTERED_MODULE
} parsing_state;

typedef struct ASTNode {
	ast_node_name name;
	token* terminal;
	ast_node* sibling;
	ast_node* child;
	int size;
} ast_node;

ast_node* create_ast_node(ast_node_name name, token* terminal) {
	if (DEBUG_PARSER) {
		printf("CREATING %s.\n", lookup_ast_as_name(name));
	}
	ast_node *node = malloc(sizeof(ast_node));
	node->name = name;
	node->terminal = terminal;
	node->child = NULL;
	node->sibling = NULL;
	node->size = terminal ? 1 : 0;
	return node;
}



void add_child(ast_node* parent, ast_node* child) {
	if (DEBUG_PARSER) {
		printf("ADDING CHILD %s (%s) TO PARENT %s (%s).\n",
			lookup_ast_as_name(child->name),
			child->terminal? child->terminal->lexeme : "AST_NODE",
			lookup_ast_as_name(parent->name),
			parent->terminal ? parent->terminal->lexeme : "AST_NODE");
	}
	// If parent has no children, add as child.
	if (parent->child == NULL) {
		parent->child = child;
	} else {
		// If parent has children, add as sibling to children.
		ast_node* current_node = parent->child;
		while (current_node->sibling != NULL) {
			current_node = current_node->sibling;
		}
		current_node->sibling = child;
	}
	parent->size += child->size;
}

struct ast_node *match_binop(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH BINOP.\n"); }
	ast_node* bop;
	token_name name = (*tokens)->name;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	switch (name) {
		case TOKEN_ADD:
		case TOKEN_SUB:
		case TOKEN_MUL:
		case TOKEN_UDIV:
		case TOKEN_SDIV:
		case TOKEN_UREM:
		case TOKEN_SREM:
		case TOKEN_SHL:
		case TOKEN_LSHR:
		case TOKEN_ASHR:
		case TOKEN_AND:
		case TOKEN_OR:
		case TOKEN_XOR:
			bop = create_ast_node(AST_BINOP, 0);
			add_child(bop, create_ast_node(AST_TERMINAL, *tokens));
			break;
		default:
			bop = NULL;
			break;
	};
	return bop;
}

struct ast_node *match_fbop(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH FBOP.\n"); }
	ast_node* fbop;
	token_name name = (*tokens)->name;
	switch (name) {
		case TOKEN_FADD:
		case TOKEN_FSUB:
		case TOKEN_FMUL:
		case TOKEN_FDIV:
		case TOKEN_FREM:
			fbop = create_ast_node(AST_FBOP, 0);
			add_child(fbop, create_ast_node(AST_TERMINAL, *tokens));
			break;
		default:
			fbop = NULL;
			break;
	}
	return fbop;
}


struct ast_node* match_typ(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH TYP.\n"); }
	ast_node* typ;
	token_name name = (*tokens)->name;

	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	while (true) {

		switch (state) {

		case ENCOUNTERED_:

			switch ((*tk)->name) {

				case TOKEN_INTEGERTYPE:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TYP;
					continue;

					// case TOKEN_FP:

				case TOKEN_VOID:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TYP;
					continue;

				case TOKEN_OPEN_BRACKET:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_OPENBRACKET;
					continue;

				case TOKEN_OPEN_CURLY_BRACKET:
					continue;

					// CASE TYP TYP

				case TOKEN_IDENTIFIER:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TYP;
					continue;

				default:
					return NULL;

			}
			continue;

		case ENCOUNTERED_OPENBRACKET:
			if ((*tk)->name == TOKEN_SZ) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_OPENBRACKET_SZ;
			}
			continue;

		case ENCOUNTERED_OPENBRACKET_SZ:
			if ((*tk)->name == TOKEN_X) {
				tk++; tokens_skipped++;
				state = ENCOUNTERED_OPENBRACKET_SZ_X;
			}
			continue;

		case ENCOUNTERED_OPENBRACKET_SZ_X:
			if (stack[si] = match_typ(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_OPENBRACKET_SZ_X_TYP;
			}
			continue;

		case ENCOUNTERED_OPENBRACKET_SZ_X_TYP:
			if ((*tk)->name == TOKEN_CLOSE_BRACKET) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_TYP;
			}
			continue;

		case ENCOUNTERED_TYP:

			switch ((*tk)->name) {

				case TOKEN_PTR:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					continue;

				default:
					typ = create_ast_node(AST_TYP, 0);
					for (int i = 0; i < si; i++) {
						add_child(typ, stack[i]);
					}
					typ->size += tokens_skipped;
					
					return typ;
			}
			continue;

		default:
			return NULL;

		}
	}
}

struct ast_node* match_constant(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH CONSTANT.\n"); }

	ast_node* constant;

	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;
	
	int tokens_skipped = 0;
	token** tk = tokens;

	while (true) {

		switch (state) {

			case ENCOUNTERED_:

				switch ((*tk)->name) {

					case TOKEN_INTEGERTYPE:
						if ((*(tk + 1))->name == TOKEN_SZ) {
							stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
							stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
							state = ENCOUNTERED_CONSTANT;
							continue;
						}
						break;

					//case TOKEN_FP:
					//case TOKEN_OPEN_CURLY_BRACKET:
						// TODO:
				}

				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_TYP;
					continue;
				}
				


			case ENCOUNTERED_TYP:
			
				if ((*(tk - 1))->name == TOKEN_PTR) {
					state = ENCOUNTERED_TYP_PTR;
					continue;
				}
				continue;


			case ENCOUNTERED_TYP_PTR:

				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				continue;



			case ENCOUNTERED_INTEGERTYPE:
				if ((*tk)->name == TOKEN_SZ) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_CONSTANT;
				}
				continue;
		
			case ENCOUNTERED_CONSTANT:
				constant = create_ast_node(AST_CONSTANT, 0);
				for (int i = 0; i < si; i++) {
					add_child(constant, stack[i]);
				}
				constant->size += tokens_skipped;
				return constant;

			default:
				return NULL;

		}
	}
}



struct ast_node* match_val(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH VAL.\n"); }
	ast_node* bop;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	parsing_state state = ENCOUNTERED_;

	int tokens_skipped = 0;
	token** tk = tokens;

	ast_node* val = NULL;
	
	while (true) {

		switch (state) {

			case ENCOUNTERED_:

				switch ((*tk)->name) {

					case TOKEN_SZ:
					case TOKEN_IDENTIFIER:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_VAL;
						continue;
					case TOKEN_GETELEMENTPTR:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_GETELEMENTPTR;
						continue;
					default:
						if (stack[si] = match_constant(tk)) {
							tk += stack[si]->size; si++;
							state = ENCOUNTERED_VAL;
							continue;
						}
				}
				continue;

			case ENCOUNTERED_GETELEMENTPTR:
				if ((*tk)->name == TOKEN_INBOUNDS) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}

				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_GETELEMENTPTR_TYP;

					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
				}
				continue;

			case ENCOUNTERED_GETELEMENTPTR_TYP:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR;
				}
				continue;

			case ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR:
				if (stack[si] = match_val(tk)) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL;
				}
				continue;

			case ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL_TYP;
				} 
				continue;


			case ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL_TYP_VAL;
				}
				continue;

			case ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL_TYP_VAL:
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
					state = ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_VAL;
					continue;
				}
				else if ((*tk)->name == TOKEN_CLOSE_PAREN) {
					tk++; tokens_skipped++;
					state = ENCOUNTERED_VAL;
					continue;
				}
				continue;

			case ENCOUNTERED_VAL:
				val = create_ast_node(AST_VAL, 0);
				for (int i = 0; i < si; i++) {
					add_child(val, stack[i]);
				}
				val->size += tokens_skipped;
				return val;


			default:
				val = NULL;
				continue;
		}
	}
	return val;
}

struct ast_node* match_condition(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH CONDITION.\n"); }

	ast_node* condition;

	switch ((*tokens)->name) {
		/* Not yet implemented 
		case TOKEN_EQ:
		case TOKEN_NE:
		case TOKEN_UGT:
		case TOKEN_ULT:
		case TOKEN_ULE
		case TOKEN_SGT:
		case TOKEN_SGE: */
		case TOKEN_SLT:
		case TOKEN_SLE:
			condition = create_ast_node(AST_CONDITION, NULL);
			add_child(condition, create_ast_node(AST_TERMINAL, *tokens));
			break;
		default: 
			condition = NULL;
			break;
	}
	return condition;

}


struct ast_node *match_command(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH COMMAND.\n"); }
	ast_node* command; 
	parsing_state state = ENCOUNTERED_;
	
	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token **tk = tokens;

	bool matched = false;

	while (true) {

		switch (state) {

			case ENCOUNTERED_:
				switch ((*tk)->name) {
					case TOKEN_IDENTIFIER:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ID;
						continue;
					case TOKEN_STORE:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_STORE;
						continue;
					default:
						return NULL;
				}
				continue;

			case ENCOUNTERED_ID:
				switch ((*tk)->name) {
					case TOKEN_EQUALS:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ID_EQ;
						continue;
					default:
						return NULL;
				}
				continue;

			case ENCOUNTERED_ID_EQ:
				switch ((*tk)->name) {
					case TOKEN_ICMP:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ID_EQ_ICMP;
						continue;
					case TOKEN_LOAD:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ID_EQ_LOAD;
						continue;
					case TOKEN_ALLOCA:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ID_EQ_ALLOCA;
						continue;
					case TOKEN_GETELEMENTPTR:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ID_EQ_GETELEMENTPTR;
						continue;
				}

				if (stack[si] = match_binop(tk)) {
					si++; tk++;
					state = ENCOUNTERED_ID_EQ_BOP;
					continue;
				}

				else if (stack[si] = match_fbop(tk)) {
					si++; tk++;

					if ((*tk)->name == TOKEN_NSW) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						continue;
					}
					
					if ((*tk)->name = TOKEN_INTEGERTYPE
						&& (*(tk+1))->name == TOKEN_IDENTIFIER
						&& (*(tk+2))->name == TOKEN_COMMA
						&& (*(tk+3))->name == TOKEN_IDENTIFIER) {

						stack[si++] = create_ast_node(AST_TERMINAL, *tk++); // isz
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++); // id
						tk++; tokens_skipped++;								// comma
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++); // id
						state = ENCOUNTERED_ID_EQ_FBOP_INTEGERTYPE_ID_COMMA_ID;
						continue;
					}
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_GETELEMENTPTR:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++; 
					state = ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP;
				}
				continue;

			case ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP_VAL;
				}
				continue;

			case ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_VAL;
				}
				continue;


			case ENCOUNTERED_ID_EQ_BOP:
				if ((*tk)->name == TOKEN_NSW) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if ((*tk)->name == TOKEN_INTEGERTYPE) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE;
				}
				continue;

			case ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_VAL;
					}
				}
				continue;

			case ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COMMAND;
				}
				continue;

			case ENCOUNTERED_ID_EQ_ICMP:
				if (stack[si] = match_condition(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_ICMP_COND;
				}
				continue;


			case ENCOUNTERED_ID_EQ_ICMP_COND:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_ICMP_COND_TYP;
				}
				continue;

			case ENCOUNTERED_ID_EQ_ICMP_COND_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_ID_EQ_ICMP_COND_TYP_VAL;
					}
				}
				continue;

			case ENCOUNTERED_ID_EQ_ICMP_COND_TYP_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COMMAND;
				}
				continue;

			case ENCOUNTERED_ID_EQ_LOAD:

				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_ID_EQ_LOAD_TYP;
				}
				continue;

			case ENCOUNTERED_ID_EQ_LOAD_TYP:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_LOAD_TYP_TYP;
				}
				continue;

			case ENCOUNTERED_ID_EQ_LOAD_TYP_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_ID_EQ_LOAD_TYP_TYP_ID;

					}
				}
				continue;

			case ENCOUNTERED_ID_EQ_LOAD_TYP_TYP_ID:
				if ((*tk)->name == TOKEN_ALIGN) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN;
				}
				continue;

			case ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN:
				if ((*tk)->name == TOKEN_SZ) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_COMMAND;
				}
				continue;

			case ENCOUNTERED_ID_EQ_ALLOCA:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_ALLOCA_TYP;
					continue;
				}
				return NULL;;

			case ENCOUNTERED_ID_EQ_ALLOCA_TYP:
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
				}
				if ((*tk)->name == TOKEN_ALIGN) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if ((*tk)->name == TOKEN_SZ) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				state = ENCOUNTERED_COMMAND;
				continue;

			case ENCOUNTERED_STORE:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_STORE_TYP;
					continue;
				}
				continue;

			case ENCOUNTERED_STORE_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_STORE_TYP_VAL;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
				}
				continue;

			case ENCOUNTERED_STORE_TYP_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_STORE_TYP_VAL_VAL;
				}
				continue;


			case ENCOUNTERED_STORE_TYP_VAL_VAL:
				if ((*tk)->name == TOKEN_COMMA
					&& (*(tk+1))->name == TOKEN_ALIGN
					&& (*(tk + 2))->name == TOKEN_SZ) {

					tokens_skipped++; tk++;
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				continue;


			case ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN_SZ:
			case ENCOUNTERED_ID_EQ_FBOP_INTEGERTYPE_ID_COMMA_ID:
			case ENCOUNTERED_COMMAND: 
				command = create_ast_node(AST_COMMAND, 0);
				for (int i = 0; i < si; i++) { 
					add_child(command, stack[i]);}
				command->size += tokens_skipped;
				return command;
				
		}
	} 
}

struct ast_node* match_terminator(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH TERMINATOR.\n"); }
	ast_node* terminator;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	bool matched = false;
	while (true) {

		switch (state) {

			case ENCOUNTERED_:
				switch ((*tk)->name) {
					case TOKEN_BR:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_BR;
						continue;
					case TOKEN_RET:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_RET;
						continue;
					default:
						return NULL;
				}

			case ENCOUNTERED_BR:
				switch ((*tk)->name) {
					case TOKEN_LABEL:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_BR_LABEL;
						continue;
					case TOKEN_RET:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_RET;
						continue;
					case TOKEN_UNREACHABLE:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_TERMINATOR;
						continue;
				}

				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_BR_TYP;
					continue;
				}
				continue;

			case ENCOUNTERED_BR_TYP:
				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					if ((*tk)->name == TOKEN_COMMA) {
						tk++;  tokens_skipped++;
						state = ENCOUNTERED_BR_TYP_ID;
					}
				}
				continue;

			case ENCOUNTERED_BR_TYP_ID:
				if ((*tk)->name == TOKEN_LABEL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_BR_TYP_ID_LABEL;
				}
				continue;


			case ENCOUNTERED_BR_TYP_ID_LABEL:
				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_BR_TYP_ID_LABEL_ID;
					}
				}
				continue;

			case ENCOUNTERED_BR_TYP_ID_LABEL_ID:
				if ((*tk)->name == TOKEN_LABEL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_BR_TYP_ID_LABEL_ID_LABEL;
				}
				continue;

			case ENCOUNTERED_BR_TYP_ID_LABEL_ID_LABEL:
				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TERMINATOR;
				}
				continue;

			case ENCOUNTERED_BR_LABEL:
				switch ((*tk)->name) {
					case TOKEN_IDENTIFIER:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_TERMINATOR;
						continue;
					default:
						return NULL;
				}

			case ENCOUNTERED_RET:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_RET_TYP;
					continue;
				}

			case ENCOUNTERED_RET_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_TERMINATOR;
					continue;
				}
				continue;

			case ENCOUNTERED_TERMINATOR:
				terminator = create_ast_node(AST_TERMINATOR, 0);
				for (int i = 0; i < si; i++) {
					add_child(terminator, stack[i]);
				}
				terminator->size += tokens_skipped;
				return terminator;
		}
	}
}

struct ast_node* match_blocks(token  **tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH BLOCKS.\n"); }
	ast_node* blocks;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	bool matched = false;
	while (true) {

		switch (state) {

			case ENCOUNTERED_:
				if ((*tk)->name == TOKEN_BLOCKLABEL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				state = ENCOUNTERED_BLOCKLABEL;
				continue;

			case ENCOUNTERED_BLOCKLABEL:
				if (stack[si] = match_command(tk)) {
					tk += stack[si]->size; si++;
					continue;
				} else if (stack[si] = match_terminator(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_BLOCKS;
					continue;
				} else {
					return NULL;
				}

			case ENCOUNTERED_BLOCKS:
				blocks = create_ast_node(AST_BLOCKS, 0);
				for (int i = 0; i < si; i++) {
					add_child(blocks, stack[i]);
				}				
				blocks->size += tokens_skipped;
				return blocks;
		}
	}
}


struct ast_node* match_args(token **tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH ARGS.\n"); }
	ast_node* args;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;


	while (true) {

		switch (state) {

			case ENCOUNTERED_:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_TYP;
					continue;
				}

			case ENCOUNTERED_TYP:
				switch ((*tk)->name) {
					case TOKEN_IDENTIFIER:
							stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
							state = ENCOUNTERED_TYP_ID;
							continue;
						case TOKEN_CLOSE_PAREN:
							state = ENCOUNTERED_ARGS;
							continue;
						default:
							return NULL;
				}

			case ENCOUNTERED_TYP_ID:
				switch ((*tk)->name) {
					case TOKEN_COMMA:
						tk++; tokens_skipped++;
						if (stack[si] = match_typ(tk)) {
							tk += stack[si]->size; si++;
							state = ENCOUNTERED_TYP;
							continue;
						}
					case TOKEN_CLOSE_PAREN:
						state = ENCOUNTERED_ARGS;
						continue;
					default:
						return NULL;
				}

			case ENCOUNTERED_ARGS:
				args = create_ast_node(AST_ARGS, 0);
				for (int i = 0; i < si; i++) {
					add_child(args, stack[i]);
				}
				args->size += tokens_skipped;
				return args;

		}
	}
}



struct ast_node* match_prod(token **tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH PROD.\n"); }

	ast_node* prod;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	while (true) {

		switch (state) {

			case ENCOUNTERED_:

				switch ((*tk)->name) {

					case TOKEN_DEFINE:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						if (stack[si] = match_typ(tk)) {
							tk += stack[si]->size; si++; 
							if ((*tk)->name == TOKEN_IDENTIFIER) {
								stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
								state = ENCOUNTERED_DEFINE_ID;
								continue;
							}
						}
				}

			case ENCOUNTERED_DEFINE_ID:
				// Checking for arguments 

				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tokens_skipped++; tk++; 
				}
				if (stack[si] = match_args(tk)) {
					tk += stack[si]->size; si++;
				}
				if ((*tk)->name == TOKEN_CLOSE_PAREN) {
					tokens_skipped++; tk++;
				}

				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

				state = ENCOUNTERED_DEFINE_ID_ARGS;
				continue;

			case ENCOUNTERED_DEFINE_ID_ARGS:
				if ((*tk)->name == TOKEN_OPEN_CURLY_BRACKET) {
					tokens_skipped++; tk++;
				}

				if (stack[si] = match_blocks(tk)) {
					tk += stack[si]->size; si++;
					continue;
				}

				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

			case ENCOUNTERED_DEFINE_ID_ARGS_BLOCKS:

				switch ((*tk)->name) {
					case TOKEN_CLOSE_CURLY_BRACKET:
						tokens_skipped++; tk++;
						state = ENCOUNTERED_PROD;
						continue;
					default:
						return NULL;
				}

			case ENCOUNTERED_PROD:
				prod = create_ast_node(AST_PROD, 0);
				for (int i = 0; i < si; i++) {
					add_child(prod, stack[i]);
				}
				prod->size += tokens_skipped;
				return prod;

			default:
				return NULL;
		}
	}

}

struct ast_node* match_namedt(token * *tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH NAMEDT.\n"); }

	return NULL;
}


struct ast_node* match_layout(token** tokens) {
	return NULL;
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH LAYOUT.\n"); }
	ast_node* layout = malloc(sizeof(ast_node));
	token_name name = (*tokens)->name;
	switch (name) {
		case TOKEN_BIGENDIAN:
		case TOKEN_LITTLEENDIAN:
		case TOKEN_PTR:
		case TOKEN_INTEGERTYPE:
		case TOKEN_FLOAT:
		case TOKEN_AGGR:
		case TOKEN_STACK:
		default:
			return NULL;
		}
}


struct ast_node* match_module(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH MODULE.\n"); }
	ast_node* module;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;
	
		while (true) {

			switch (state) {

				case ENCOUNTERED_:
					if (stack[si] = match_layout(tk)) {
						tk += stack[si]->size; si++;
					}
					state = ENCOUNTERED_LAYOUT;
					continue;

				case ENCOUNTERED_LAYOUT:
					if (stack[si] = match_namedt(tk)) {
						tk += stack[si]->size; si++;
					}
					state = ENCOUNTERED_LAYOUT_NAMEDT;
					continue;

				case ENCOUNTERED_LAYOUT_NAMEDT:
					if (stack[si] = match_prod(tk)) {
						tk += stack[si]->size; si++;
					}
					state = ENCOUNTERED_MODULE;
					continue;

				case ENCOUNTERED_MODULE:
					module = create_ast_node(AST_MODULE, 0);
					for (int i = 0; i < si; i++) {
						add_child(module, stack[i]);
					}
					module->size += tokens_skipped;
					return module;

				default:
					return NULL;
			}
	}
}

void print_ast_indent(ast_node* root, int indent) {
	if (root->name == AST_TERMINAL) {
		print_token(&(root->terminal));
	} else {
		printf("%s\n", lookup_ast_as_name(root->name));
	}

	ast_node* current_node = root->child;
	while (current_node != NULL) {
		printf("%*s", indent, "");
		print_ast_indent(current_node, indent + 8);
		current_node = current_node->sibling;
	}
}

void print_ast(ast_node* root) {
	print_ast_indent(root, 4);
}