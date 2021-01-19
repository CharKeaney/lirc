#include "parser.h"

#define DEBUG_PARSER true

typedef enum ASTNodeNames {
	AST_BINOP,
	AST_ERROR,
	AST_FBOP,
	AST_COMMAND,
	AST_MODULE,
	AST_TERMINAL
} ast_node_name;

char* ast_name_lookup[] = {
	[AST_BINOP] = "AST_BINOP",
	[AST_ERROR] = "AST_ERROR",
	[AST_FBOP] = "AST_FBOP",
	[AST_COMMAND] = "AST_COMMAND",
	[AST_MODULE] = "AST_MODULE",
	[AST_TERMINAL] = "AST_TERMINAL"
};


#define lookup_ast_as_name(a) ast_name_lookup[a]

typedef enum  ParsingStates {
	ENCOUNTERED_,
	ENCOUNTERED_ID,
	ENCOUNTERED_ID_EQ,
	ENCOUNTERED_ID_EQ_BOP,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID_COMMA,
	ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID_COMMA_ID
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
		printf("ATTEMPTED TO CREATE %s.\n", lookup_ast_as_name(name));
	}
	ast_node* node = malloc(sizeof(ast_node));
	node->name = name;
	node->terminal = terminal;
	node->child = NULL;
	node->sibling = NULL;
	node->size = 1;
	return node;
}



void add_child(ast_node* parent, ast_node* child) {
	printf("ATTEMPTED TO ADD CHILD %s TO PARENT %s.\n",
		lookup_ast_as_name(child->name),
		lookup_ast_as_name(parent->name));
	// If parent has no children, add as child.
	if (parent->child == NULL) {
		parent->child = child;
	}
	else {
		// If parent has children, add as sibling to children.
		ast_node* current_node = parent->child;
		while (current_node->sibling != NULL) {
			current_node = current_node->sibling;
		}
		current_node->sibling = child;
	}
	parent->size += (child->size + 1);
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

struct ast_node* match_fbop(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH FBOP.\n"); }
	ast_node* fbop;
	token_name name = (*tokens)->name;
	switch (name) {
	
	case TOKEN_FADD:
	case TOKEN_FSUB:
	case TOKEN_FMUL:
	case TOKEN_FDIV:
	case TOKEN_FREM:
		fbop = malloc(sizeof(ast_node));
		fbop->name = AST_FBOP;
		fbop->terminal = *tokens;
		fbop->size = 1;
		break;
	default:
		fbop = NULL;
		break;
	}
	return fbop;
}

struct ast_node *match_command(token** tokens) {
	if (DEBUG_PARSER) { printf("ATTEMPTED TO MATCH COMMAND.\n"); }
	ast_node* command; 

	ast_node *matched;
	parsing_state state = ENCOUNTERED_;

	token **tk = tokens;
	while (true) {
		switch (state) {
			case ENCOUNTERED_:
				switch ((*tk++)->name) {
					case TOKEN_IDENTIFIER:
						state = ENCOUNTERED_ID;
						continue;
				}
			case ENCOUNTERED_ID:
				switch ((*tk++)->name) {
					case TOKEN_EQUALS:
						state = ENCOUNTERED_ID_EQ;
						continue;
				}
			case ENCOUNTERED_ID_EQ:
				if (matched = match_binop(tk++)
					&& (*tk++)->name == TOKEN_INTEGERTYPE
					&& (*tk++)->name == TOKEN_IDENTIFIER
					&& (*tk++)->name == TOKEN_COMMA
					&& (*tk++)->name == TOKEN_IDENTIFIER) {
					state = ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID_COMMA_ID;
				}
			case ENCOUNTERED_ID_EQ_BOP_INTEGERTYPE_ID_COMMA_ID:
				command = create_ast_node(AST_COMMAND, 0);
				add_child(command, create_ast_node(AST_TERMINAL, *tokens++));	// id
				add_child(command, create_ast_node(AST_TERMINAL, *tokens++));	// eq
				add_child(command, match_binop(tokens++));						// bop
				add_child(command, create_ast_node(AST_TERMINAL, *tokens++));	// inttype
				add_child(command, create_ast_node(AST_TERMINAL, *tokens++));	// id
				tokens++; command->size += 1;									// comma
				add_child(command, create_ast_node(AST_TERMINAL, *tokens++));	// id
				return command;
		}
	}
}

struct ast_node* match_prod(token * *tokens) {

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
