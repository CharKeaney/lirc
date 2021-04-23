#include "parser.h"

typedef enum ASTNodeNames {
	AST_ARGS,
	AST_BINOP,
	AST_BLOCKS,
	AST_CONDITION,
	AST_CONSTANT,
	AST_COP,
	AST_TROP,
	AST_EOP,
	AST_ERROR,
	AST_FBOP,
	AST_COMMAND,
	AST_MODULE,
	AST_TERMINAL,
	AST_TYP,
	AST_TERMINATOR,
	AST_IDENTIFIER,
	AST_VAL,
	AST_PROD
} ast_node_name;

char* ast_name_lookup[] = {
	[AST_ARGS] = "AST_ARGS",
	[AST_BINOP] = "AST_BINOP",
	[AST_BLOCKS] = "AST_BLOCKS",
	[AST_COP] = "AST_COP",
	[AST_EOP] = "AST_EOP",
	[AST_ERROR] = "AST_ERROR",
	[AST_FBOP] = "AST_FBOP",
	[AST_CONDITION] = "AST_CONDITION",
	[AST_CONSTANT] = "AST_CONSTANT",
	[AST_COMMAND] = "AST_COMMAND",
	[AST_IDENTIFIER] = "AST_ID",
	[AST_MODULE] = "AST_MODULE",
	[AST_TERMINAL] = "AST_TERMINAL",
	[AST_TERMINATOR] = "AST_TERMINATOR",
	[AST_TROP] = "AST_TROP",
	[AST_VAL] = "AST_VAL",
	[AST_TYP] = "AST_TYP",
	[AST_PROD] = "AST_PROD"
};

#define lookup_ast_as_name(a) ast_name_lookup[a]

typedef enum  ParsingStates {
	/* Error states : Do not relate to an encounter */
	FAILED_TO_MATCH,
	PANIC_MODE_LOOKING_FOR_TERMINATOR,
	/* Encounter states : Store what has already been seen by the parser. */
	ENCOUNTERED_,							
	ENCOUNTERED_ALIGN,
	ENCOUNTERED_ALIGN_SZ,
	ENCOUNTERED_ARGS,
	ENCOUNTERED_BLOCKLABEL,
	ENCOUNTERED_BLOCKLABEL_PNODE,
	ENCOUNTERED_BLOCKLABEL_PNODE_COMMAND,
	ENCOUNTERED_BLOCKS,
	ENCOUNTERED_BOP,
	ENCOUNTERED_BOP_CONSTANT,
	ENCOUNTERED_BR,
	ENCOUNTERED_BR_TYP,
	ENCOUNTERED_BR_TYP_ID,
	ENCOUNTERED_BR_TYP_ID_LABEL,
	ENCOUNTERED_BR_TYP_ID_LABEL_ID,
	ENCOUNTERED_BR_TYP_ID_LABEL_ID_LABEL,
	ENCOUNTERED_CALL,
	ENCOUNTERED_CALL_VOID,
	ENCOUNTERED_CALL_VOID_ASM,
	ENCOUNTERED_CALL_VOID_ASM_STRING,
	ENCOUNTERED_CALL_VOID_ASM_STRING_ID,
	ENCOUNTERED_CALL_VOID_ID,
	ENCOUNTERED_COMMAND,
	ENCOUNTERED_CONSTANT,
	ENCOUNTERED_COP,
	ENCOUNTERED_COP_CONSTANT,
	ENCOUNTERED_COP_CONSTANT_TO,
	ENCOUNTERED_DECLARE,
	ENCOUNTERED_DECLARE_TYP,
	ENCOUNTERED_DECLARE_TYP_ID,
	ENCOUNTERED_DECLARE_TYP_ID_TYP,
	ENCOUNTERED_DEFINE,
	ENCOUNTERED_DEFINE_ID,
	ENCOUNTERED_DEFINE_ID_ARGS,
	ENCOUNTERED_DEFINE_ID_ARGS_BLOCKS,
	ENCOUNTERED_FBOP,
	ENCOUNTERED_FENCE,
	ENCOUNTERED_FENCE_SYNCSCOPE,
	ENCOUNTERED_FENCE_SYNCSCOPE_STRING,
	ENCOUNTERED_GETELEMENTPTR,
	ENCOUNTERED_GETELEMENTPTR_TYP,
	ENCOUNTERED_GETELEMENTPTR_TYP_CONSTANT,
	ENCOUNTERED_GETELEMENTPTR_TYP_CONSTANT_CONSTANT,
	ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_CONSTANT_CONSTANT,
	ENCOUNTERED_INTEGERTYPE,
	ENCOUNTERED_ID,
	ENCOUNTERED_ID_EQ,
	ENCOUNTERED_ID_EQ_BOP,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_VAL,
	ENCOUNTERED_ID_EQ_CALL,
	ENCOUNTERED_ID_EQ_CALL_TYP,
	ENCOUNTERED_ID_EQ_CALL_TYP_ID,
	ENCOUNTERED_ID_EQ_CALL_TYP_ID_TYP,
	ENCOUNTERED_ID_EQ_CALL_TYP_ID_PARAM,
	ENCOUNTERED_ID_EQ_CMPXCHG,
	ENCOUNTERED_ID_EQ_CMPXCHG_VAL,
	ENCOUNTERED_ID_EQ_CMPXCHG_VAL_VAL,
	ENCOUNTERED_ID_EQ_CMPXCHG_VAL_VAL_VAL,
	ENCOUNTERED_ID_EQ_COP,
	ENCOUNTERED_ID_EQ_COP_TYP,
	ENCOUNTERED_ID_EQ_COP_TYP_VAL,
	ENCOUNTERED_ID_EQ_COP_TYP_VAL_TO,
	ENCOUNTERED_ID_EQ_EOP,
	ENCOUNTERED_ID_EQ_EOP_TYP,
	ENCOUNTERED_ID_EQ_EOP_TYP_VAL,
	ENCOUNTERED_ID_EQ_EOP_TYP_VAL_TO,
	ENCOUNTERED_ID_EQ_EXTRACTVALUE,
	ENCOUNTERED_ID_EQ_EXTRACTVALUE_TYP,
	ENCOUNTERED_ID_EQ_EXTRACTVALUE_TYP_VAL,
	ENCOUNTERED_ID_EQ_FBOP,
	ENCOUNTERED_ID_EQ_FBOP_VAL,
	ENCOUNTERED_ID_EQ_FCMP,
	ENCOUNTERED_ID_EQ_FCMP_COND,
	ENCOUNTERED_ID_EQ_FCMP_COND_VAL,
	ENCOUNTERED_ID_EQ_FNEG,
	ENCOUNTERED_ID_EQ_GETELEMENTPTR,
	ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP,
	ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP_VAL,
	ENCOUNTERED_ID_EQ_LANDINGPAD,
	ENCOUNTERED_ID_EQ_LANDINGPAD_ID,
	ENCOUNTERED_ID_EQ_LOAD,
	ENCOUNTERED_ID_EQ_LOAD_TYP,
	ENCOUNTERED_ID_EQ_LOAD_TYP_PTR,
	ENCOUNTERED_ID_EQ_LOAD_TYP_VAL,
	ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN,
	ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN_SZ,
	ENCOUNTERED_ID_EQ_PHI,
	ENCOUNTERED_ID_EQ_PHI_TYP,
	ENCOUNTERED_ID_EQ_TROP,
	ENCOUNTERED_ID_EQ_TROP_TYP,
	ENCOUNTERED_ID_EQ_TROP_TYP_VAL,
	ENCOUNTERED_ID_EQ_TROP_TYP_VAL_TO,
	ENCOUNTERED_ID_EQ_ALLOCA,
	ENCOUNTERED_ID_EQ_ICMP,
	ENCOUNTERED_ID_EQ_ICMP_COND,
	ENCOUNTERED_ID_EQ_ICMP_COND_VAL,
	ENCOUNTERED_ID_EQ_ICMP_COND_TYP_VAL,
	ENCOUNTERED_ID_EQ_SELECT,
	ENCOUNTERED_INVOKE,
	ENCOUNTERED_INVOKE_TYP,
	ENCOUNTERED_INVOKE_TYP_ID,
	ENCOUNTERED_INVOKE_TYP_ID_PARAMS,
	ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO,
	ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL,
	ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL_ID,
	ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL_ID_LABEL,
	ENCOUNTERED_OPEN_PAREN,
	ENCOUNTERED_OPEN_ANGLE_BRACKET,
	ENCOUNTERED_OPEN_BRACKET,
	ENCOUNTERED_OPEN_BRACKET_SZ,
	ENCOUNTERED_OPEN_BRACKET_SZ_X,
	ENCOUNTERED_OPEN_BRACKET_SZ_X_TYP,
	ENCOUNTERED_OPEN_CURLY_BRACKET,
	ENCOUNTERED_BR_LABEL,
	ENCOUNTERED_STORE,
	ENCOUNTERED_STORE_TYP,
	ENCOUNTERED_STORE_TYP_VAL,
	ENCOUNTERED_STORE_TYP_VAL_VAL,
	ENCOUNTERED_SWITCH,
	ENCOUNTERED_SWITCH_VAL,
	ENCOUNTERED_SWITCH_VAL_LABEL,
	ENCOUNTERED_SWITCH_VAL_LABEL_ID,
	ENCOUNTERED_SWITCH_VAL_LABEL_ID_OPEN_BRACKET,
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
	ENCOUNTERED_TROP,
	ENCOUNTERED_TROP_CONSTANT,
	ENCOUNTERED_TROP_CONSTANT_TO,
	ENCOUNTERED_PROD,
	ENCOUNTERED_PNODE,
	ENCOUNTERED_LAYOUT_NAMEDT,
	ENCOUNTERED_LAYOUT,
	ENCOUNTERED_MODULE
} parsing_state;

/* Represents an error code used to store the reason why an error occured.*/
typedef enum  ErrorCode {
	GENERIC_ERROR,
	EXPECTED_END_OF_MODULE,
	EXPECTED_TERMINATOR
} error_code;

/* Stores the error message associated with a given error code. */
char* error_msgs[] = {
	[GENERIC_ERROR] = 
		"Failed to parse the input after the indicated location.",
	[EXPECTED_END_OF_MODULE] = 
		"Expected end of module (program) but more input remaining.",
	[EXPECTED_TERMINATOR] = 
		"Expected end of block terminator but instead seen as follows."
};

/**
* Responsible for creating and initializing an ast node from a given 
* name and a terminal that consitutes the node. Not all nodes have
* associated terminals
* 
* @param name The name of the token to create (represents type of node)
* @param terminal The terminal to associate with the new AST node.
* 
* @see ast_node
*/
ast_node* create_ast_node(ast_node_name name, token* terminal) {
	if (DEBUG_PARSER_CREATION) {
		printf("CREATING %s.\n", lookup_ast_as_name(name));
	}
	ast_node *node = malloc(sizeof(ast_node));
	if (node == NULL) {
		printf("Fatal Error : Failed to allocate memory to create node.");
		return NULL;
	}
	/* Normal operating attributes*/
	node->name = name;
	node->terminal = terminal;
	node->child = NULL;
	node->sibling = NULL;
	node->size = terminal ? 1 : 0;
	/* Error recovery information*/
	node->contains_error = false;
	node->error_token = NULL;
	node->error_code = GENERIC_ERROR;
	return node;
}


/*
* Adds a given child ast node as a child to a given ast parent node.
* This also transfers upwardly inheritable state and adjusts sizing, 
* so it is vital that this is a function on its own and that children
* are never manually added.
* 
* @param parent The parent node to which the child node should be added to.
* @param child The new child of the given parent node.
*/
void add_child(ast_node* parent, ast_node* child) {
	if (DEBUG_PARSER_CHILDREN) {
		printf("ADDING CHILD %s (%s) TO PARENT %s (%s).\n",
			lookup_ast_as_name(child->name),
			child->terminal? child->terminal->lexeme : "AST_NODE",
			lookup_ast_as_name(parent->name),
			parent->terminal ? parent->terminal->lexeme : "AST_NODE");
	}
	/* Transferring state that is upwardly inheritable */
	if (child->contains_error) { /* Error recovery data */
		parent->contains_error = true;
		if (parent->error_token == NULL) {
			parent->error_token = child->error_token;
			parent->error_code = child->error_code;
		}
	}
	/* If parent has no children, add as child. */
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

/**
* Adds many children from a list to a given ast parent node.
* This also transfers upwardly inheritable stateand adjusts sizing,
* so it is vital that this is a function on its ownand that children
* are never manually added.
*
* @param parent The parent node to which the child node should be added to.
* @param child The new child of the given parent node..
*
* @see add_child
*/
void add_children(ast_node* parent, ast_node** children) {
	ast_node** current_child = children;
	while (*current_child != NULL) {
		add_child(parent, *current_child);
		current_child++;
	}
}

/**
* Creates an AST Node with children as described by the given name, terminal
* and list of children.
*
* @param parent The parent node to which the child node should be added to.
* @param child The new child of the given parent node..
*
* @see create_ast_node
* @see add_child
*/
ast_node* create_ast_node_with_children(
	ast_node_name name, token* terminal, ast_node** children) {
	
	ast_node* node = create_ast_node(name, terminal);
	add_children(node, children);
	return node;

}


/*
* Matches tokens into an abstract syntax tree for an id.
*
* @param tokens The stream of tokens to match into a id.
* @return Abstract Syntax Tree representing an id or NULL.
*/
struct ast_node* match_id(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH ID.\n"); }

	ast_node* id;
	token_name name = (*tokens)->name;

	token** tk = tokens;

	switch ((*tk)->name) {
		case TOKEN_IDENTIFIER:
		case TOKEN_SZ:
		case TOKEN_FLOAT_LITERAL:
		case TOKEN_TRUE:
		case TOKEN_FALSE:
		case TOKEN_NULL:
			id = create_ast_node(AST_IDENTIFIER, 0);
			add_child(id, create_ast_node(AST_TERMINAL, *tokens));
			return id;
		default:
			return NULL;
	}
	return NULL;
}

/*
* Matches tokens into an abstract syntax tree for a binary operator.
* 
* @param tokens The stream of tokens to match into a binary operator.
* @return Abstract Syntax Tree representing a binary operator or NULL.
*/
struct ast_node *match_bop(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH BINOP.\n"); }
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


/*
* Responsible for matching tokens into an abstract syntax tree for a floating 
* point binary operator.
*
* @param tokens The stream of tokens to match into a floating point binary op.
* @return Abstract Syntax Tree representing a floating point binary op or NULL.
*/
struct ast_node *match_fbop(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH FBOP.\n"); }
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


/*
* Matches tokens into an abstract syntax tree for a cast operation.
*
* @param tokens The stream of tokens to match into a binary operator.
* @return Abstract Syntax Tree representing a binary operator or NULL.
*/
struct ast_node* match_cop(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH COP.\n"); }
	ast_node* cop;
	token_name name = (*tokens)->name;
	switch (name) {		
		case TOKEN_BITCAST:
		case TOKEN_SITOFP:
		case TOKEN_PTRTOINT:
		case TOKEN_FPTOUI:
		case TOKEN_FPTOSI:
		case TOKEN_INTTOPTR:
		case TOKEN_UITOFP:
			cop = create_ast_node(AST_COP, 0);
			add_child(cop, create_ast_node(AST_TERMINAL, *tokens));
			break;
		default:
			cop = NULL;
			break;
	}
	return cop;
}


/*
* Matches tokens into an abstract syntax tree for a trunc operation.
*
* @param tokens The stream of tokens to match into a trunc operator.
* @return Abstract Syntax Tree representing a trunc operator or NULL.
*/
struct ast_node* match_trop(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH TROP.\n"); }
	ast_node* trop;
	token_name name = (*tokens)->name;
	switch (name) {
		case TOKEN_TRUNC:
			trop = create_ast_node(AST_TROP, 0);
			add_child(trop, create_ast_node(AST_TERMINAL, *tokens));
			break;
		default:
			trop = NULL;
			break;
	}
	return trop;
}

/*
* Matches tokens into an abstract syntax tree for a extension operator.
*
* @param tokens The stream of tokens to match into a extension operator.
* @return Abstract Syntax Tree representing a extension operator or NULL.
*/
struct ast_node* match_eop(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH EOP.\n"); }
	ast_node* eop;
	token_name name = (*tokens)->name;
	switch (name) {
		case TOKEN_ZEXT:
		case TOKEN_SEXT:
		case TOKEN_FPEXT:
			eop = create_ast_node(AST_EOP, 0);
			add_child(eop, create_ast_node(AST_TERMINAL, *tokens));
			break;
		default:
			eop = NULL;
			break;
	}
	return eop;
}


/*
* Matches tokens into an abstract syntax tree for a type node.
*
* @param tokens The stream of tokens to match into a typ node.
* @return Abstract Syntax Tree representing a typ node or NULL.
*/
struct ast_node* match_typ(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH TYP.\n"); }
	ast_node* typ;
	token_name name = (*tokens)->name;

	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	while (true) {

		if (DEBUG_PARSER_MATCH) {
			printf("MATCH_TYP READ TOKEN %s (%s) IN STATE %d\n",
				lookup_token_as_name((*tk)->name),
				(*tk)->lexeme, state);
		}

		switch (state) {

		case ENCOUNTERED_:

			switch ((*tk)->name) {

				case TOKEN_INTEGERTYPE:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					if ((*tk)->name == TOKEN_IMMARG) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					}
					state = ENCOUNTERED_TYP;
					continue;

				case TOKEN_DOUBLE:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TYP;
					continue;

				case TOKEN_FLOAT:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TYP;
					continue;

				case TOKEN_VOID:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TYP;
					continue;

				case TOKEN_OPEN_BRACKET:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_OPEN_BRACKET;
					continue;

				case TOKEN_OPEN_PAREN:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_OPEN_PAREN;
					continue;

				case TOKEN_OPEN_ANGLE_BRACKET:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_OPEN_ANGLE_BRACKET;
					continue;

				case TOKEN_OPEN_CURLY_BRACKET:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_OPEN_CURLY_BRACKET;
					continue;

				case TOKEN_IDENTIFIER:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TYP;
					continue;

				default:
					return NULL;

			}
			continue;

		case ENCOUNTERED_OPEN_PAREN:
			if (stack[si] = match_typ(tk)) {
				tk += stack[si]->size; si++;
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
				}
				continue;
			}
			else if ((*tk)->name == TOKEN_DOTDOTDOT) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
			}
			if ((*tk)->name == TOKEN_CLOSE_PAREN) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_TYP;
				continue;
			}
			return NULL;

		case ENCOUNTERED_OPEN_CURLY_BRACKET:
			if (stack[si] = match_typ(tk)) {
				tk += stack[si]->size; si++;
				if ((*tk)->name == TOKEN_COMMA) {
					tk++;  tokens_skipped++;
					continue;
				}
			} 
		   if ((*tk)->name == TOKEN_CLOSE_CURLY_BRACKET) {
			   stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_TYP;
				continue;
			}
			return NULL;

		case ENCOUNTERED_OPEN_ANGLE_BRACKET:
			if (stack[si] = match_typ(tk)) {
				tk += stack[si]->size; si++;
				if ((*tk)->name == TOKEN_CLOSE_ANGLE_BRACKET) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TYP;
					continue;
				}
			}
			return NULL;

		case ENCOUNTERED_OPEN_BRACKET:
			if ((*tk)->name == TOKEN_SZ) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_OPEN_BRACKET_SZ;
			}
			continue;

		case ENCOUNTERED_OPEN_BRACKET_SZ:
			if ((*tk)->name == TOKEN_X) {
				tk++; tokens_skipped++;
				state = ENCOUNTERED_OPEN_BRACKET_SZ_X;
			}
			continue;

		case ENCOUNTERED_OPEN_BRACKET_SZ_X:
			if (stack[si] = match_typ(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_OPEN_BRACKET_SZ_X_TYP;
			}
			continue;

		case ENCOUNTERED_OPEN_BRACKET_SZ_X_TYP:
			if ((*tk)->name == TOKEN_CLOSE_BRACKET) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_TYP;
			}
			continue;

		case ENCOUNTERED_TYP:

			switch ((*tk)->name) {

				case TOKEN_OPEN_PAREN: 
					// We haven't finished if this is a function typ.
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_OPEN_PAREN;
					continue;
					
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
			return NULL;

		default:
			return NULL;

		}
	}
}


/*
* Matches tokens into an abstract syntax tree for a constant.
*
* @param tokens The stream of tokens to match into a constant node.
* @return Abstract Syntax Tree representing a constant node or NULL.
*
*/
struct ast_node* match_constant(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH CONSTANT.\n"); }

	ast_node* constant;

	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;
	
	int tokens_skipped = 0;
	token** tk = tokens;

	while (true) {

		if (DEBUG_PARSER_MATCH) {
			printf("MATCH_CONSTANT READ TOKEN %s (%s) IN STATE %d.\n",
				lookup_token_as_name((*tk)->name), (*tk)->lexeme, state);
		}

		switch (state) {

			case ENCOUNTERED_:

				// Terminals
				switch ((*tk)->name) {

					case TOKEN_INTEGERTYPE:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_INTEGERTYPE;
						continue;

					case TOKEN_GETELEMENTPTR:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_GETELEMENTPTR;
						continue;

					case TOKEN_ALIGN:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ALIGN;
						continue;
				}

				// Nonterminals
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_TYP;
					continue;
				}
				if (stack[si] = match_bop(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_BOP;
					continue;
				}
				if (stack[si] = match_fbop(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_FBOP;
					continue;
				}
				if (stack[si] = match_cop(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COP;
					continue;
				}
				if (stack[si] = match_trop(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_TROP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_BOP:
				if ((*tk)->name == TOKEN_NUW
					|| (*tk)->name == TOKEN_NSW) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}
				if (stack[si] = match_constant(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_BOP_CONSTANT;
					continue;
				}
				return NULL;

			case ENCOUNTERED_BOP_CONSTANT:

				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}
				if (stack[si] = match_constant(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ALIGN:
				if ((*tk)->name == TOKEN_SZ) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ALIGN_SZ;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ALIGN_SZ:
				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				return NULL;

			case ENCOUNTERED_COP:
				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}
				if (stack[si] = match_constant(tk)) {
					tk += stack[si]->size; si++;

					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_COP_CONSTANT;
					continue;				
				}
				return NULL;

			case ENCOUNTERED_COP_CONSTANT:
				if ((*tk)->name == TOKEN_CLOSE_PAREN) {
					tk++; tokens_skipped++;
				}
				if ((*tk)->name == TOKEN_TO) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_COP_CONSTANT_TO;
					continue;
				}
				return NULL;

			case ENCOUNTERED_COP_CONSTANT_TO:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				return NULL;

			case ENCOUNTERED_TROP:
				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}
				if (stack[si] = match_constant(tk)) {
					tk += stack[si]->size; si++;

					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}					
					state = ENCOUNTERED_TROP_CONSTANT;
					continue;
				}
				return NULL;

			case ENCOUNTERED_TROP_CONSTANT:
				if ((*tk)->name == TOKEN_TO) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TROP_CONSTANT_TO;
					continue;
				}
				return NULL;

			case ENCOUNTERED_TROP_CONSTANT_TO:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				return NULL;

			case ENCOUNTERED_TYP:
				if ((*(tk - 1))->name == TOKEN_PTR) {
					state = ENCOUNTERED_TYP_PTR;
					continue;
				}
				else if (stack[si] = match_id(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_CONSTANT;
					continue;
				} else if ((*tk)->name == TOKEN_ZEROINITIALIZER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				return NULL;

			case ENCOUNTERED_TYP_PTR:
				if ((*tk)->name == TOKEN_NULL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_CONSTANT;
					continue;
				} 

				if ((*tk)->name == TOKEN_ALIGN
					&& (*(tk + 1))->name == TOKEN_SZ) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

				if (stack[si] = match_id(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				
				if (stack[si] = match_constant(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				return NULL;


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

					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					continue;
				}
				return NULL;

			case ENCOUNTERED_GETELEMENTPTR_TYP:
				
				if (stack[si] = match_constant(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_GETELEMENTPTR_TYP_CONSTANT;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					continue;
				}
				return NULL;
				
			case ENCOUNTERED_GETELEMENTPTR_TYP_CONSTANT:
				if (stack[si] = match_constant(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						continue;
					}
					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_CONSTANT;
					continue;
				}
				return NULL;

			case ENCOUNTERED_GETELEMENTPTR_TYP_TYPPTR_CONSTANT_CONSTANT:

				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
					state = ENCOUNTERED_GETELEMENTPTR_TYP_CONSTANT_CONSTANT;
					continue;
				} else if ((*tk)->name == TOKEN_CLOSE_PAREN) {
					tk++; tokens_skipped++;
					state = ENCOUNTERED_VAL;
					continue;
				}
				return NULL;

			case ENCOUNTERED_INTEGERTYPE:
				switch ((*tk)->name) {
					case TOKEN_IDENTIFIER:
					case TOKEN_SZ:
					case TOKEN_TRUE:
					case TOKEN_FALSE:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_CONSTANT;
						continue;
					case TOKEN_PTRTOINT:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_COP;
						continue;
				}
				if (match_trop(tk)
					|| match_bop(tk)
					|| match_cop(tk)) {
					if (stack[si] = match_constant(tk)) {
						tk += stack[si]->size; si++;
						state = ENCOUNTERED_CONSTANT;
						continue;
					}
				}
				// Backtrack to treating this as a typ as opposed
				// to the INTEGERTYPE terminal.
				tk--; si--;
				// Match typ.
				stack[si] = match_typ(tk);
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_TYP;
				continue;

				
			case ENCOUNTERED_CONSTANT:
				stack[si] = NULL;
				constant = create_ast_node_with_children(AST_CONSTANT, 0, stack);
				constant->size += tokens_skipped;
				return constant;

			default:
				return NULL;

		}
	}
}


/*
* Matches tokens into an abstract syntax tree for a val.
*
* @param tokens The stream of tokens to match into a val.
* @return Abstract Syntax Tree representing a val or NULL.
*/
struct ast_node* match_val(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH VAL.\n"); }
	ast_node* val;
	token** tk = tokens;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	parsing_state state = ENCOUNTERED_;

	while (true) {

		if (DEBUG_PARSER_MATCH) {
			printf("MATCH_VAL READ TOKEN %s (%s) IN STATE %d\n",
				lookup_token_as_name((*tk)->name), 
				(*tk)->lexeme, state);
		}

		switch (state) {

			case ENCOUNTERED_:

				if ((stack[si] = match_constant(tk)) 
					&& (*(tk + (stack[si]->size)))->name != TOKEN_EQUALS) {

					tk += stack[si]->size; si++;
					state = ENCOUNTERED_VAL;
					continue;
				} 
				if (stack[si] = match_id(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_VAL;
					continue;
				} 
				return NULL;

			case ENCOUNTERED_VAL:
				stack[si] = NULL;
				val = create_ast_node_with_children(AST_VAL, 0, stack);
				return val;

			default:
				val = NULL;
				continue;
		}
	}
	return val;
}


/*
* Matches tokens into an abstract syntax tree for a condition.
*
* @param tokens The stream of tokens to match into a condition node.
* @return Abstract Syntax Tree representing a condition node or NULL.
*
*/
struct ast_node* match_condition(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH CONDITION.\n"); }

	ast_node* condition;

	switch ((*tokens)->name) {
		case TOKEN_EQ:
		case TOKEN_NE:
		case TOKEN_OEQ:
		case TOKEN_OLE:
		case TOKEN_OLT:
		case TOKEN_UGT:
		case TOKEN_OGE:
		case TOKEN_OGT:
		case TOKEN_UGE:
		case TOKEN_ULT:
		case TOKEN_ULE:
		case TOKEN_UNE:
		case TOKEN_SGT:
		case TOKEN_SGE:
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


/*
* Matches tokens into an abstract syntax tree for a pnode.
*
* @param tokens The stream of tokens to match into a pnode.
* @return Abstract Syntax Tree representing a pnode or NULL.
*/
struct ast_node* match_pnode(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH PNODE.\n"); }
	ast_node* pnode;
	token** tk = tokens;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0, tokens_skipped = 0;

	parsing_state state = ENCOUNTERED_;

	while (true) {

		if (DEBUG_PARSER_MATCH) {
			printf("MATCH_PNODE READ TOKEN %s (%s) IN STATE %d\n",
				lookup_token_as_name((*tk)->name),
				(*tk)->lexeme, state);
		}

		switch (state) {

		case ENCOUNTERED_:
			if ((*tk)->name == TOKEN_IDENTIFIER) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_ID;
				continue;
			}
			return NULL;

		case ENCOUNTERED_ID:
			if ((*tk)->name == TOKEN_EQUALS) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_ID_EQ;
				continue;
			}
			return NULL;

		case ENCOUNTERED_ID_EQ:
			if ((*tk)->name == TOKEN_PHI) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_ID_EQ_PHI;
				continue;
			}
			return NULL;

		case ENCOUNTERED_ID_EQ_PHI:
			if (stack[si] = match_typ(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_ID_EQ_PHI_TYP;
				continue;
			}
			return NULL;
			

		case ENCOUNTERED_ID_EQ_PHI_TYP:
			if ((*tk)->name == TOKEN_OPEN_BRACKET) {
				tk++; tokens_skipped++;

				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}

					if (stack[si] = match_val(tk)) {
						tk += stack[si]->size; si++;
						
						if ((*tk)->name == TOKEN_CLOSE_BRACKET) {
							tk++; tokens_skipped++;

							if ((*tk)->name == TOKEN_COMMA) {
								tk++; tokens_skipped++;
							}
							continue;
						}
					}
				}
			}
			state = ENCOUNTERED_PNODE;
			continue;

		case ENCOUNTERED_PNODE:
			stack[si] = NULL;
			pnode = create_ast_node_with_children(AST_COMMAND, 0, stack);
			pnode->size += tokens_skipped;
			return pnode;

		default:
			pnode = NULL;
			return pnode;
		}
	}
}


/*
* Matches tokens into an abstract syntax tree for a command.
*
* @param tokens The stream of tokens to match into a command node.
* @return Abstract Syntax Tree representing a command node or NULL.
*
*/
struct ast_node *match_command(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH COMMAND.\n"); }
	ast_node* command; 
	parsing_state state = ENCOUNTERED_;
	
	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token **tk = tokens;

	bool matched = false;

	while (true) {

		if (DEBUG_PARSER_MATCH) {
			printf("MATCH_COMMAND READ TOKEN %s (%s) IN STATE %d\n",
				lookup_token_as_name((*tk)->name), (*tk)->lexeme, state);
		}

		switch (state) {

		case ENCOUNTERED_:

			switch ((*tk)->name) {

				case TOKEN_TAIL:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					if ((*tk)->name == TOKEN_CALL) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_CALL;
						continue;
					}

				case TOKEN_CALL:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_CALL;
					continue;

				case TOKEN_IDENTIFIER:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID;
					continue;

				case TOKEN_STORE:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_STORE;
					continue;

				case TOKEN_FENCE:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_FENCE;
					continue;

				default:
					command = NULL;
					break;
			}
			return NULL;

		case ENCOUNTERED_CALL:
			if (stack[si] = match_typ(tk)) {
				if ((*tk)->name == TOKEN_VOID && stack[si]->size > 1) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_CALL_TYP;
					continue;
				}
			} 
			if ((*tk)->name == TOKEN_VOID) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_CALL_VOID;
				continue;
			}
			return NULL;

		case ENCOUNTERED_CALL_VOID:
			if ((*tk)->name == TOKEN_ASM) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_CALL_VOID_ASM;
				continue;
			}
			if ((*tk)->name == TOKEN_IDENTIFIER) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}
				state = ENCOUNTERED_CALL_VOID_ID;
				continue;
			}
			return NULL;

		case ENCOUNTERED_CALL_VOID_ASM:
			if ((*tk)->name == TOKEN_STRING) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_CALL_VOID_ASM_STRING;
				continue;
			}
			return NULL;

		case ENCOUNTERED_CALL_VOID_ASM_STRING:
			if ((*tk)->name == TOKEN_COMMA) {
				tk++; tokens_skipped++;
			}
			if ((*tk)->name == TOKEN_STRING) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}
				state = ENCOUNTERED_CALL_VOID_ASM_STRING_ID;
				continue;
			}
			return NULL;

		case ENCOUNTERED_CALL_VOID_ASM_STRING_ID:
		case ENCOUNTERED_CALL_VOID_ID:

			if (stack[si] = match_val(tk)) {
				tk += stack[si]->size; si++;
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
				}
				continue;
			}
			else if ((*tk)->name == TOKEN_CLOSE_PAREN) {
				tk++; tokens_skipped++;
				if ((*tk)->name  == TOKEN_IDENTIFIER 
					&& (*(tk+1))->name != TOKEN_EQUALS) { 
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				state = ENCOUNTERED_COMMAND;
				continue;
			}
			return NULL;

		case ENCOUNTERED_FENCE:
			if ((*tk)->name == TOKEN_SYNCSCOPE) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_FENCE_SYNCSCOPE;
				continue;
			}
			return NULL;

		case ENCOUNTERED_FENCE_SYNCSCOPE:
			if ((*tk)->name == TOKEN_OPEN_PAREN) {
				tk++; tokens_skipped++;
			}
			if ((*tk)->name == TOKEN_STRING) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_FENCE_SYNCSCOPE_STRING;
						continue;
					}
			}
			return NULL;

		case ENCOUNTERED_FENCE_SYNCSCOPE_STRING:
			if ((*tk)->name == TOKEN_SEQ_CST) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_COMMAND;
				continue;
			}
			return NULL;
			
		case ENCOUNTERED_ID:
			switch ((*tk)->name) {
			case TOKEN_EQUALS:
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_ID_EQ;
				continue;
				// More cases to come
			}
			return NULL;

		case ENCOUNTERED_ID_EQ:
			switch ((*tk)->name) {

				case TOKEN_TAIL:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					if ((*tk)->name == TOKEN_CALL) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_ID_EQ_CALL;
						continue;
					}

				case TOKEN_CALL:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_CALL;
					continue;

				case TOKEN_CMPXCHG:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_CMPXCHG;
					continue;

				case TOKEN_ICMP:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_ICMP;
					continue;

				case TOKEN_FCMP:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_FCMP;
					continue;

				case TOKEN_FNEG:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_FNEG;
					continue;

				case TOKEN_LANDINGPAD:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_LANDINGPAD;
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

				case TOKEN_EXTRACTVALUE:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_EXTRACTVALUE;
					continue;

				case TOKEN_SELECT:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_SELECT;
					continue;
			}

			if (stack[si] = match_bop(tk)) {
				si++; tk++;
				state = ENCOUNTERED_ID_EQ_BOP;
				continue;
			}
			else if (stack[si] = match_fbop(tk)) {
				si++; tk++;
				state = ENCOUNTERED_ID_EQ_FBOP;
				continue;
			}
			else if (stack[si] = match_cop(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_ID_EQ_COP;
				continue;
			}
			else if (stack[si] = match_eop(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_ID_EQ_EOP;
				continue;
			}
			else if (stack[si] = match_trop(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_ID_EQ_TROP;
				continue;
			}
			return NULL;

		case ENCOUNTERED_ID_EQ_FBOP:
			if ((*tk)->name == TOKEN_NSW) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
			}

			if (stack[si] = match_val(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_ID_EQ_FBOP_VAL;
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
				}
				continue;
			}
			return NULL;

		case ENCOUNTERED_ID_EQ_FBOP_VAL:
			if (stack[si] = match_id(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_COMMAND;
				continue;
			}
			return NULL;
		
			case ENCOUNTERED_ID_EQ_COP:
				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_COP_TYP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_COP_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_COP_TYP_VAL;
					if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
					}
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_COP_TYP_VAL:
				if ((*tk)->name == TOKEN_TO) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_COP_TYP_VAL_TO;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_COP_TYP_VAL_TO:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_CMPXCHG:
				if ((*tk)->name == TOKEN_VOLATILE) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_CMPXCHG_VAL;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_CMPXCHG_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_CMPXCHG_VAL_VAL;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_CMPXCHG_VAL_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_CMPXCHG_VAL_VAL_VAL;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_CMPXCHG_VAL_VAL_VAL:
				if ((*tk)->name == TOKEN_SEQ_CST) {
					if ((*(tk+1))->name == TOKEN_SEQ_CST) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_COMMAND;
						continue;
					}
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_CALL:
				if ((*tk)->name == TOKEN_DSO_LOCAL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if ((*tk)->name == TOKEN_NOALIAS) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_CALL_TYP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_CALL_TYP:
				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_CALL_TYP_ID;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_CALL_TYP_ID:
				if ((*tk)->name == TOKEN_OPEN_PAREN) {
					tk++; tokens_skipped++;
				}
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						continue;
					}
				}
				if ((*tk)->name == TOKEN_CLOSE_PAREN) {
					tk++; tokens_skipped++;
					state = ENCOUNTERED_ID_EQ_CALL_TYP_ID_PARAM;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_CALL_TYP_ID_PARAM:
				if ((*tk)->name == TOKEN_IDENTIFIER
					&& *((*tk)->lexeme) == '#') {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_COMMAND;
					continue;
				} 
				state = ENCOUNTERED_COMMAND;
				continue;				
				
			case ENCOUNTERED_ID_EQ_EXTRACTVALUE:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_EXTRACTVALUE_TYP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_EXTRACTVALUE_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_ID_EQ_EXTRACTVALUE_TYP_VAL;
					continue;
				}
				return NULL;
				
			case ENCOUNTERED_ID_EQ_EXTRACTVALUE_TYP_VAL:
				if (stack[si] = match_id(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_GETELEMENTPTR:
				if ((*tk)->name == TOKEN_INBOUNDS) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++; 
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP_VAL;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_GETELEMENTPTR_TYP_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						continue;
					} else {
						state = ENCOUNTERED_COMMAND;
						continue;
					}
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_BOP:
				if ((*tk)->name == TOKEN_NSW || (*tk)->name == TOKEN_EXACT) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if ((*tk)->name == TOKEN_INTEGERTYPE) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_VAL;
						continue;
					}
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_ICMP:
				if (stack[si] = match_condition(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_ICMP_COND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_ICMP_COND:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_ID_EQ_ICMP_COND_VAL;
						continue;
					}
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_ICMP_COND_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_COMMAND;
					continue;
				} else if ((*tk)->name == TOKEN_SZ) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_FCMP:
				if (stack[si] = match_condition(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_FCMP_COND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_FCMP_COND:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_ID_EQ_FCMP_COND_VAL;
						continue;
					}
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_FCMP_COND_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				else if ((*tk)->name == TOKEN_SZ) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_FNEG:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_EOP:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_EOP_TYP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_EOP_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_EOP_TYP_VAL;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_EOP_TYP_VAL:
				if ((*tk)->name == TOKEN_TO) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_EOP_TYP_VAL_TO;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_EOP_TYP_VAL_TO:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_LANDINGPAD:
				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_LANDINGPAD_ID;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_LANDINGPAD_ID:
				if ((*tk)->name == TOKEN_CLEANUP) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_COMMAND;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_LOAD:
				if ((*tk)->name == TOKEN_VOLATILE) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_ID_EQ_LOAD_TYP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_LOAD_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_ID_EQ_LOAD_TYP_VAL;
					continue;
				}
				return NULL;


			case ENCOUNTERED_ID_EQ_LOAD_TYP_VAL:
				if ((*tk)->name == TOKEN_ALIGN) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}	
				state = ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN;
				continue;

			case ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN:
				if ((*tk)->name == TOKEN_SZ) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				state = ENCOUNTERED_COMMAND;
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

			case ENCOUNTERED_ID_EQ_TROP:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_TROP_TYP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_TROP_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_ID_EQ_TROP_TYP_VAL;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_TROP_TYP_VAL:
				if ((*tk)->name == TOKEN_TO) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID_EQ_TROP_TYP_VAL_TO;
					continue;
				}
				return NULL;

			case ENCOUNTERED_ID_EQ_TROP_TYP_VAL_TO:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_COMMAND;
					continue;
				}
			return NULL;

			case ENCOUNTERED_ID_EQ_SELECT:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						continue;
					}
				}
				state = ENCOUNTERED_COMMAND;
				continue;

			case ENCOUNTERED_STORE:
				if ((*tk)->name == TOKEN_VOLATILE) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;

					if ((*tk)->name == TOKEN_ZEROINITIALIZER) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						
						if ((*tk)->name == TOKEN_COMMA) {
							tk++; tokens_skipped++;
							state = ENCOUNTERED_STORE_TYP_VAL;
							continue;
						}
					}
					state = ENCOUNTERED_STORE_TYP;
					continue;
				}
				return NULL;


			case ENCOUNTERED_STORE_TYP:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_STORE_TYP_VAL;
					continue;
				}
				return NULL;


			case ENCOUNTERED_STORE_TYP_VAL:
				if (stack[si] = match_val(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
					}
					state = ENCOUNTERED_STORE_TYP_VAL_VAL;
					continue;
				}
				return NULL;

			case ENCOUNTERED_STORE_TYP_VAL_VAL:

				if ((*tk)->name == TOKEN_ALIGN
					&& (*(tk + 1))->name == TOKEN_SZ) {

					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				state = ENCOUNTERED_COMMAND;
				continue;

			case ENCOUNTERED_ID_EQ_LOAD_TYP_PTR_ID_ALIGN_SZ:
			case ENCOUNTERED_COMMAND: 
				stack[si] = NULL;
				command = create_ast_node_with_children(AST_COMMAND, 0, stack);
				command->size += tokens_skipped;
				return command;
				
		}
	} 
}

/*
* Matches tokens into an abstract syntax tree for a terminator.
*
* @param tokens The stream of tokens to match into a terminator node.
* @return Abstract Syntax Tree representing a terminator node or NULL.
*/
struct ast_node* match_terminator(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH TERMINATOR.\n"); }
	ast_node* terminator;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	bool matched = false;
	while (true) {

		if (DEBUG_PARSER_MATCH) {
			printf("MATCH_TERMINATOR READ TOKEN %s (%s) IN STATE %d\n",
				lookup_token_as_name((*tk)->name), (*tk)->lexeme, state);
		}

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
				case TOKEN_IDENTIFIER:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ID;
					continue;
				case TOKEN_INVOKE:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_INVOKE;
					continue;
				case TOKEN_UNREACHABLE:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TERMINATOR;
					continue;
				case TOKEN_SWITCH:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_SWITCH;
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

			if (stack[si] = match_val(tk)) {
				tk += stack[si]->size; si++;
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
				}
				state = ENCOUNTERED_BR_TYP_ID;
				continue;
			}
			return NULL;

		case ENCOUNTERED_BR_TYP:
			if ((*tk)->name == TOKEN_IDENTIFIER) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				if ((*tk)->name == TOKEN_COMMA) {
					tk++;  tokens_skipped++;
					state = ENCOUNTERED_BR_TYP_ID;
					continue;
				}
			}
			return NULL;

		case ENCOUNTERED_BR_TYP_ID:
			if ((*tk)->name == TOKEN_LABEL) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_BR_TYP_ID_LABEL;
				continue;
			}
			return NULL;


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
				continue;
			}
			return NULL;

		case ENCOUNTERED_BR_LABEL:
			switch ((*tk)->name) {
				case TOKEN_IDENTIFIER:
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TERMINATOR;
					continue;
				default:
					return NULL;
			}

		case ENCOUNTERED_ID:
			if ((*tk)->name == TOKEN_EQUALS) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_ID_EQ;
				continue;
			}
			return NULL;

		case ENCOUNTERED_ID_EQ:
			if ((*tk)->name == TOKEN_INVOKE) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_INVOKE;
				continue;
			}
			return NULL;

		case ENCOUNTERED_RET:
			if ((*tk)->name == TOKEN_VOID) {
				if (((*(tk + 1))->name == TOKEN_CLOSE_CURLY_BRACKET)
					|| ((*(tk + 1))->name == TOKEN_BLOCKLABEL)) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_TERMINATOR;
					continue;
				}
			}
			if (stack[si] = match_typ(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_RET_TYP;
				continue;
			}
			return NULL;

		case ENCOUNTERED_RET_TYP:
			if (stack[si] = match_val(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_TERMINATOR;
				continue;
			}
			return NULL;

		case ENCOUNTERED_INVOKE:
			if (stack[si] = match_typ(tk)) {
				tk += stack[si]->size; si++;
				state = ENCOUNTERED_INVOKE_TYP;
				continue;
			}
			return NULL;

		case ENCOUNTERED_INVOKE_TYP:
			if ((*tk)->name == TOKEN_IDENTIFIER) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_INVOKE_TYP_ID;
				continue;
			}
			return NULL;

		case ENCOUNTERED_INVOKE_TYP_ID:
			if ((*tk)->name == TOKEN_OPEN_PAREN) {
				tk++; tokens_skipped++;
			}
			if (stack[si] = match_val(tk)) {
				tk += stack[si]->size; si++;
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
					continue;
				} 
			}
			if ((*tk)->name == TOKEN_CLOSE_PAREN) {
				tk++; tokens_skipped++;
				state = ENCOUNTERED_INVOKE_TYP_ID_PARAMS;
				continue;
			}
			return NULL;

		case ENCOUNTERED_INVOKE_TYP_ID_PARAMS:
			if ((*tk)->name == TOKEN_TO) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO;
				continue;
			}
			return NULL;

		case ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO:
			if ((*tk)->name == TOKEN_LABEL) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL;
				continue;
			}
			return NULL;

		case ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL:
			if ((*tk)->name == TOKEN_IDENTIFIER) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL_ID;
				continue;
			}
			return NULL;

		case ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL_ID:
			if ((*tk)->name == TOKEN_UNWIND) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
			}
			if ((*tk)->name == TOKEN_LABEL) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL_ID_LABEL;
				continue;
			}
			return NULL;

		case ENCOUNTERED_INVOKE_TYP_ID_PARAMS_TO_LABEL_ID_LABEL:
			if ((*tk)->name == TOKEN_IDENTIFIER) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_TERMINATOR;
				continue;
			}
			return NULL;


		case ENCOUNTERED_SWITCH:
			if (stack[si] = match_val(tk)) {
				tk += stack[si]->size; si++;
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
				}
				state = ENCOUNTERED_SWITCH_VAL;
				continue;
			}
			return NULL;

		case ENCOUNTERED_SWITCH_VAL:
			if ((*tk)->name == TOKEN_LABEL) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_SWITCH_VAL_LABEL;
				continue;
			}
			return NULL;

		case ENCOUNTERED_SWITCH_VAL_LABEL:
			if ((*tk)->name == TOKEN_IDENTIFIER) {
				stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				state = ENCOUNTERED_SWITCH_VAL_LABEL_ID;
				continue;
			}
			return NULL;

		case ENCOUNTERED_SWITCH_VAL_LABEL_ID:
			if ((*tk)->name == TOKEN_OPEN_BRACKET) {
				tk++; tokens_skipped++;
				state = ENCOUNTERED_SWITCH_VAL_LABEL_ID_OPEN_BRACKET;
				continue;
			}
			return NULL;

		case ENCOUNTERED_SWITCH_VAL_LABEL_ID_OPEN_BRACKET:
			if (stack[si] = match_val(tk)) {
				tk += stack[si]->size; si++;
				if ((*tk)->name == TOKEN_COMMA) {
					tk++; tokens_skipped++;
					if ((*tk)->name == TOKEN_LABEL) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						if ((*tk)->name == TOKEN_IDENTIFIER) {
							stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
							continue;
						}
					}
				}
				return NULL;
			}
			if ((*tk)->name == TOKEN_CLOSE_BRACKET) {
				tk++; tokens_skipped++;
				state = ENCOUNTERED_TERMINATOR;
				continue;
			}
			return NULL;

		case ENCOUNTERED_TERMINATOR:
			stack[si] = NULL;
			terminator = create_ast_node_with_children(AST_TERMINATOR, 0, stack);
			terminator->size += tokens_skipped;
			return terminator;
		}
	}
}

/*
* Matches tokens into an abstract syntax tree for a block.
* * WARNING: WILL PANIC. This procedure prefers to return an eroneous
*		   node as opposed to giving up. Do not use it unless you are
*		   certain it is a 'Last Resort' match, i.e. neccessary to 
*		   avoid error.
*
* @param tokens The stream of tokens to match into a block node.
* @return Abstract Syntax Tree representing a block node or NULL.
*
*/
struct ast_node* match_block(token  **tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH BLOCKS.\n"); }
	ast_node* blocks;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	/* Used during error recovery */
	token* error_token = NULL;

	bool matched = false;
	while (true) {

		if (DEBUG_PARSER_MATCH) {
			printf("MATCH_BLOCKS READ TOKEN %s (%s) IN STATE %d\n",
				lookup_token_as_name((*tk)->name), (*tk)->lexeme, state);
		}

		switch (state) {

			case ENCOUNTERED_:
				if ((*tk)->name == TOKEN_BLOCKLABEL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					
				}		
				state = ENCOUNTERED_BLOCKLABEL;
				continue;

			case ENCOUNTERED_BLOCKLABEL:
				if (stack[si] = match_pnode(tk)) {
					tk += stack[si]->size; si++;
					continue;
				}
				state = ENCOUNTERED_BLOCKLABEL_PNODE;
				continue;

			case ENCOUNTERED_BLOCKLABEL_PNODE:
				if (stack[si] = match_command(tk)) {
					tk += stack[si]->size; si++;
					continue;
				}
				state = ENCOUNTERED_BLOCKLABEL_PNODE_COMMAND;
				continue;


			case ENCOUNTERED_BLOCKLABEL_PNODE_COMMAND:
				if (stack[si] = match_terminator(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_BLOCKS;
					continue;
				}
				/* If we got here, an error has occured, and it is 
				   time for panic mode, we don't give up */
				if (!ENABLE_PANIC_MODE) { return NULL;  }
				state = PANIC_MODE_LOOKING_FOR_TERMINATOR;
				continue;

			case PANIC_MODE_LOOKING_FOR_TERMINATOR:
				/* Keep looking for a terminator until we find one. */
				error_token = tk;
				while (!(stack[si] = match_terminator(tk))) {
					if (tk != NULL && *tk != NULL) { 
						tk++; tokens_skipped++;
					} else { 
						/* In this scenario, we reach the end of the input
						   before finishing a proper terminator */
						return NULL; 
					}
				}
				/* Now that we have found one, we need to record that this
				   terminator is eroneous and record it in stack */
				stack[si]->contains_error = true;
				stack[si]->error_code = EXPECTED_TERMINATOR;
				stack[si]->error_token = error_token;
				tk += stack[si]->size; si++;
				/* From now on we can treat it as non eroneous */
				state = ENCOUNTERED_BLOCKS;
				continue;

			case ENCOUNTERED_BLOCKS:
				stack[si] = NULL;
				blocks = create_ast_node_with_children(AST_BLOCKS, 0, stack);
				blocks->size += tokens_skipped;
				return blocks;
		}
	}
}

/*
* Matches tokens into an abstract syntax tree for args.
*
* @param tokens The stream of tokens to match into an args node.
* @return Abstract Syntax Tree representing a args node or NULL.
*
*/
struct ast_node* match_args(token **tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH ARGS.\n"); }
	ast_node* args;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	while (true) {

		if (DEBUG_PARSER_MATCH) {
			printf("MATCH_ARGS READ TOKEN %s (%s) IN STATE %d\n",
				lookup_token_as_name((*tk)->name), (*tk)->lexeme, state);
		}

		switch (state) {

			case ENCOUNTERED_:
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_TYP;
					continue;
				}
				if ((*tk)->name == TOKEN_DOTDOTDOT) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					state = ENCOUNTERED_ARGS;
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
						case TOKEN_COMMA:
							tk++; tokens_skipped++;
							state = ENCOUNTERED_;
							continue;
						default:
							return NULL;
				}

			case ENCOUNTERED_TYP_ID:
				switch ((*tk)->name) {
					case TOKEN_COMMA:
						tk++; tokens_skipped++;
						state = ENCOUNTERED_;
						continue;
					case TOKEN_CLOSE_PAREN:
						state = ENCOUNTERED_ARGS;
						continue;
					default:
						return NULL;
				}

			case ENCOUNTERED_ARGS:
				stack[si] = NULL;
				args = create_ast_node_with_children(AST_ARGS, 0, stack);
				args->size += tokens_skipped;
				return args;

		}
	}
}

/*
* Matches tokens into an abstract syntax tree for a prod.
* WARNING: WILL PANIC. This procedure prefers to return an eroneous
*		   node as opposed to giving up. Do not use it unless you are
*		   certain it is a 'Last Resort' match, i.e. neccessary to 
*		   avoid error.
*
* @param tokens The stream of tokens to match into a prod node.
* @return Abstract Syntax Tree representing a prod node or NULL.
*/
struct ast_node* match_prod(token **tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH PROD.\n"); }
	if (*tokens == NULL || tokens == NULL) { return NULL; }

	ast_node* prod;
	parsing_state state = ENCOUNTERED_;

	// Stack
	ast_node* stack[PARSING_STACK_BUFFER];
	int si = 0;

	int tokens_skipped = 0;
	token** tk = tokens;

	while (true) {

		if (DEBUG_PARSER_MATCH && state != ENCOUNTERED_PROD) {
			printf("MATCH_PRODUCTION READ TOKEN %s (%s) IN STATE %d\n",
				lookup_token_as_name((*tk)->name), (*tk)->lexeme, state);
		}

		switch (state) {

			case ENCOUNTERED_:

				switch ((*tk)->name) {

					case TOKEN_DEFINE:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_DEFINE;
						continue;
					case TOKEN_DECLARE:
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						state = ENCOUNTERED_DECLARE;
						continue;
				}
				return NULL;

			case ENCOUNTERED_DECLARE:

				if ((*tk)->name == TOKEN_DSO_LOCAL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if ((*tk)->name == TOKEN_NOALIAS) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if ((*tk)->name == TOKEN_DLLIMPORT) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					state = ENCOUNTERED_DECLARE_TYP;
					continue;
				}
				return NULL;

			case ENCOUNTERED_DECLARE_TYP:
				if (stack[si] = match_id(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_OPEN_PAREN) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_DECLARE_TYP_ID;
						continue;
					}
				}
				return NULL;

			case ENCOUNTERED_DECLARE_TYP_ID:

				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_NOALIAS) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					}
					if ((*tk)->name == TOKEN_NOCAPTURE) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					}
					if ((*tk)->name == TOKEN_WRITEONLY 
						|| (*tk)->name == TOKEN_READONLY) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					}
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						continue;
					} else if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_DECLARE_TYP_ID_TYP;
						continue;
					}
				}
				else if ((*tk)->name == TOKEN_DOTDOTDOT) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					if ((*tk)->name == TOKEN_COMMA) {
						tk++; tokens_skipped++;
						continue;
					}
				}
				
				if ((*tk)->name == TOKEN_CLOSE_PAREN) {
						tk++; tokens_skipped++;
						state = ENCOUNTERED_DECLARE_TYP_ID_TYP;
						continue;
				}
				return NULL;

			case ENCOUNTERED_DECLARE_TYP_ID_TYP:
				if ((*tk)->name == TOKEN_IDENTIFIER) {
					if (*((*tk)->lexeme) == '#') {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					}
				}
				state = ENCOUNTERED_PROD;
				continue;
				
			case ENCOUNTERED_DEFINE:
				if ((*tk)->name == TOKEN_INTERNAL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

				if ((*tk)->name == TOKEN_LINKONCE_ODR) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

				if ((*tk)->name == TOKEN_DSO_LOCAL) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
		
					if ((*tk)->name == TOKEN_VOID) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
						}

					if ((*tk)->name == TOKEN_IDENTIFIER) {
						stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
					}

					state = ENCOUNTERED_DEFINE_ID;
					continue;
				}
				return NULL;


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

				if ((*tk)->name == TOKEN_COMDAT) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

				if ((*tk)->name == TOKEN_PERSONALITY) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}

				if (stack[si] = match_typ(tk)) {
					tk += stack[si]->size; si++;
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

				/* Match block panics so we must be careful not to call it
				   when it is not to match. */
				if (stack[si] = match_block(tk)) {
					tk += stack[si]->size; si++;
					if ((*tk)->name == TOKEN_CLOSE_CURLY_BRACKET) {
						tokens_skipped++; tk++;
						state = ENCOUNTERED_PROD;
						continue;
					}
					continue;
				}

				if ((*tk)->name == TOKEN_IDENTIFIER) {
					stack[si++] = create_ast_node(AST_TERMINAL, *tk++);
				}
				state = ENCOUNTERED_DEFINE_ID_ARGS_BLOCKS;
				continue;

			case ENCOUNTERED_PROD:
				stack[si] = NULL;
				prod = create_ast_node_with_children(AST_PROD, 0, stack);
				prod->size += tokens_skipped;
				return prod;

			default:
				return NULL;
		}
	}

}


/*
* Matches tokens into an abstract syntax tree for a namedt.
*
* @param tokens The stream of tokens to match into a namedt node.
* @return Abstract Syntax Tree representing a namedt node or NULL.
*
*/
struct ast_node* match_namedt(token * *tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH NAMEDT.\n"); }

	return NULL;
}


/*
* Matches tokens into an abstract syntax tree for a layout.
*
* @param tokens The stream of tokens to match into a layout node.
* @return Abstract Syntax Tree representing a layout node or NULL.
*
*/
struct ast_node* match_layout(token** tokens) {
	return NULL;
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH LAYOUT.\n"); }
	token_name name = (*tokens)->name;
	switch (name) {
		default:
			return NULL;
		}
}


/*
* Matches tokens into an abstract syntax tree for a module.
*
* @param tokens The stream of tokens to match into a module node.
* @return Abstract Syntax Tree representing a mmodule node or NULL.
*
*/
struct ast_node* match_module(token** tokens) {
	if (DEBUG_PARSER_MATCH) { printf("ATTEMPTED TO MATCH MODULE.\n"); }
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
						continue;
					}
					state = ENCOUNTERED_MODULE;
					continue;

				case ENCOUNTERED_MODULE:	
					stack[si] = NULL;
					module = create_ast_node_with_children(AST_MODULE, 0, stack);
					module->size += tokens_skipped;
					/* A module should contain the entire input, 
					//so lets validate that. */
					if ((*tk) != NULL) {
						module->contains_error = true;
						module->error_code = EXPECTED_END_OF_MODULE;
						module->error_token = tk;
					}
					return module;

				default:
					return NULL;
			}
		}
}


void free_tree(ast_node* node) {
	// Free child.
	if (node->child != NULL) {
		free_tree(node->child);
	}
	// Free siblings.
	if (node->sibling != NULL) {
		free_tree(node->sibling);
	}
	// Free the node itself.
	free(node);
}

/*
* Matches tokens into an abstract syntax tree for a layout.
*
* @param root The root of the tree which will be printed
* @param indent The level of indentation that the tree starts on
*
*/
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

void fprint_token(token** tk, FILE* fp) {
	char* field_format = "%-24s\t%-16s\t%-8d\n";
	fprintf(fp,
		field_format,
		lookup_token_as_name((**tk).name),
		(**tk).lexeme,
		(**tk).value);
}

/*
* Matches tokens into an abstract syntax tree for a layout.
*
* @param root The root of the tree which will be printed
* @param indent The level of indentation that the tree starts on
*
*/
void fprint_ast_indent(ast_node* root, int indent, FILE* fp) {
	if (root->name == AST_TERMINAL) {
		fprint_token(&(root->terminal), fp);
	} else {
		fprintf(fp, "%s\n", lookup_ast_as_name(root->name));
	}

	ast_node* current_node = root->child;
	while (current_node != NULL) {
		fprintf(fp, "%*s", indent, "");
		fprint_ast_indent(current_node, indent + 8, fp);
		current_node = current_node->sibling;
	}
}

void fprint_ast(ast_node* root, FILE* fp) {
	fprint_ast_indent(root, 4, fp);
}
