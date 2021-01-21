#include "parser.h"

#define DEBUG_PARSER true

typedef enum ASTNodeNames {
	AST_ARGS,
	AST_BINOP,
	AST_BLOCKS,
	AST_ERROR,
	AST_FBOP,
	AST_COMMAND,
	AST_MODULE,
	AST_TERMINAL,
	AST_TYP,
	AST_TERMINATOR,
	AST_PROD
} ast_node_name;

char* ast_name_lookup[] = {
	[AST_ARGS] = "AST_ARGS",
	[AST_BINOP] = "AST_BINOP",
	[AST_BLOCKS] = "AST_BLOCKS",
	[AST_ERROR] = "AST_ERROR",
	[AST_FBOP] = "AST_FBOP",
	[AST_COMMAND] = "AST_COMMAND",
	[AST_MODULE] = "AST_MODULE",
	[AST_TERMINAL] = "AST_TERMINAL",
	[AST_TERMINATOR] = "AST_TERMINATOR",
	[AST_TYP] = "AST_TYP",
	[AST_PROD] = "AST_PROD"
};

#define lookup_ast_as_name(a) ast_name_lookup[a]

typedef enum  ParsingStates {
	ENCOUNTERED_,
	ENCOUNTERED_ARGS,
	ENCOUNTERED_BLOCKS,
	ENCOUNTERED_COMMAND,
	ENCOUNTERED_DEFINE,
	ENCOUNTERED_DEFINE_ID,
	ENCOUNTERED_DEFINE_ID_ARGS,
	ENCOUNTERED_DEFINE_ID_ARGS_BLOCKS,
	ENCOUNTERED_ID,
	ENCOUNTERED_ID_EQ,
	ENCOUNTERED_ID_EQ_BOP,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID_COMMA,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID_COMMA_ID,
	ENCOUNTERED_ID_EQ_FBOP_INTEGERTYPE_ID_COMMA_ID,
	ENCOUNTERED_ID_EQ_LOAD,
	ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN_SZ,
	ENCOUNTERED_LABEL,
	ENCOUNTERED_TYP,
	ENCOUNTERED_TYP_ID,
	ENCOUNTERED_RET,
	ENCOUNTERED_RET_TYP,
	ENCOUNTERED_TERMINATOR,
	ENCOUNTERED_PROD
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
	printf("ADDING CHILD %s TO PARENT %s.\n",
		lookup_ast_as_name(child->name),
		lookup_ast_as_name(parent->name));
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
	switch (name) {

		case TOKEN_INTEGERTYPE:
			typ = create_ast_node(AST_TYP, 0);
			add_child(typ, create_ast_node(AST_TERMINAL, *tokens++));
			break;
		// case TOKEN_FP:
		case TOKEN_VOID:
			typ = create_ast_node(AST_TYP, 0);
			add_child(typ, create_ast_node(AST_TERMINAL, *tokens++));
			break;
		case TOKEN_OPEN_BRACKET:
		case TOKEN_OPEN_CURLY_BRACKET:
		// CASE TYP TYP
		case TOKEN_IDENTIFIER:
			typ = create_ast_node(AST_TYP, 0);
			add_child(typ, create_ast_node(AST_TERMINAL, *tokens++));
			break;
		default:
			return NULL;
	}
	if ((*tokens)->name == TOKEN_PTR) {
		ast_node* ptr = create_ast_node(AST_TERMINAL, *tokens);
		add_child(typ, ptr);
	}
	return typ;
}


#define PARSING_STACK_BUFFER (2<<8)

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
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);;
						state = ENCOUNTERED_ID;
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
					case TOKEN_LOAD:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ID_EQ_LOAD;
						continue;
				}

				if (stack[si] = match_binop(tk)) {
					si++; tk++;
					if ((*(tk))->name == TOKEN_INTEGERTYPE
						&& (*(tk+1))->name == TOKEN_IDENTIFIER
						&& (*(tk+2))->name == TOKEN_COMMA
						&& (*(tk+3))->name == TOKEN_IDENTIFIER) {

						stack[si++] = create_ast_node(AST_TERMINAL, *tk++); // isz
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++); // id
						tk++;  tokens_skipped++;							// comma
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++); // id
						
						state = ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID_COMMA_ID;
						continue;
					}
				}
				else if (stack[si] = match_fbop(tk)) {
					si++; tk++;
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
				break;

			case ENCOUNTERED_ID_EQ_LOAD:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					if ((*(tk - 1))->name == TOKEN_PTR
						&& (*tk)->name == TOKEN_IDENTIFIER
						&& (*(tk + 1))->name == TOKEN_COMMA
						&& (*(tk + 2))->name == TOKEN_ALIGN
						&& (*(tk + 3))->name == TOKEN_SZ) {

						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);	// id
						tk++; tokens_skipped++;	// comma
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);;	// align
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);;	// sz

						state = ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN_SZ;
						continue;
					}
				}
				break;

			case ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN_SZ:
			case ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID_COMMA_ID:
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
				case TOKEN_RET:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_RET;
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
			switch ((*tk)->name) {
			case TOKEN_IDENTIFIER:
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_TERMINATOR;
				continue;
			default:
				return NULL;
			}

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
				switch ((*tk)->name) {
					case TOKEN_LABEL:
						state = ENCOUNTERED_LABEL;
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						continue;
					default:
						return NULL;
					}
				continue;


			case ENCOUNTERED_LABEL:
				if (stack[si] = match_command(tk)) {
					tk += stack[si]->size; si++;
					continue;
				} else if (stack[si] = match_terminator(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_BLOCKS;
					continue;
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
				state = ENCOUNTERED_DEFINE_ID_ARGS;
				continue;

			case ENCOUNTERED_DEFINE_ID_ARGS:
				if ((*tk)->name == TOKEN_OPEN_CURLY_BRACKET) {
					tokens_skipped++; tk++;
				}

				if (stack[si] = match_blocks(tk)) {

					tk += stack[si]->size; si++;

					state = ENCOUNTERED_DEFINE_ID_ARGS_BLOCKS;
					continue;
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

}


struct ast_node* match_layout(token** tokens) {
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
	ast_node* layout = match_layout(tokens);
	tokens += layout->size;
	ast_node* namedt = match_namedt(tokens);
	tokens += namedt->size;
	ast_node* prod = match_prod(tokens);
	tokens += prod->size;

	ast_node* module = create_ast_node(AST_MODULE, 0);
	if (layout != NULL) { add_child(module, layout); }
	if (namedt != NULL) { add_child(module, namedt); }
	if (prod != NULL) {	add_child(module, prod); }
	module->name = AST_MODULE;
	return module;
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
