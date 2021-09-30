#pragma once

#include "error.h"

#include <stdlib.h>
#include <stdio.h>

typedef enum ASTNodeNames ast_node_name;
typedef union ASTNodeChildren ast_node_children;

typedef enum alternative {
	GENERIC_ALT,
	BR_TYP_ID_LABEL_ID_LABEL_ID,
	/* Typ alts */
	INTEGERTYPE,
	DOUBLE,
	FLOAT,
	VOID,
	IDENTIFIER,
	OPEN_PAREN_TYP_CLOSE_PAREN,
	OPEN_CURLY_BRACKET_TYP_CLOSE_CURLY_BRACKET,
	OPEN_ANGLE_BRACKET_TYP_CLOSE_ANGLE_BRACKET,
	OPEN_BRACKET_SZ_X_TYP_CLOSE_BRACKET,
	/* Terminator alts */
	BR_LABEL_ID,
	BR_UNREACHABLE,
	SWITCH_VAL_LABEL_ID_OPEN_BRACKET_CLOSE_BRACKET,
	INVOKE_TYP_ID_PARAMS_TO_LABEL_ID_LABEL_ID,
	RET_VOID,
	UNREACHABLE,
	/* other */
	CONSTANT,
	DECLARE_TYP_ID_TYP,
	DECLARE_TYP_ID_TYP_ID,
	DEFINE_ID_ARGS,
	DEFINE_DEFDESC_TYP_ID_ARGS_BLOCKS,
	ID,
	ID_EQ_ALLOCA_TYP_ALIGN_SZ,
	ID_EQ_BOP_INTEGERTYPE_VAL_VAL,
	ID_EQ_COP_TYP_VAL_TO_TYP,
	ID_EQ_ICMP_COND_VAL_VAL,
	ID_EQ_LOAD_TYP_VAL_ALIGN_SZ,
	ID_EQ_COP_VAL_TO_TYP,
	ID_EQ_EOP_TYP_VAL_TO_TYP,
	INTEGERTYPE_ID,
	STORE_TYP_VAL_VAL_ALIGN_SZ,
	STORE_TYP_VAL_VAL,
	RET_TYP_VAL,
	TYP_PTR,
	TYP_PTR_CONSTANT,
	TYP_PTR_ID,
	GETELEMENTPTR_TYP_CONSTANT_CONSTANT,
	ID_EQ_GETELEMENTPTR_TYP_VAL,
	ID_EQ_TYPE_TYP,
	ID_EQ_TYPE_OPAQUE,
	ID_EQ_COMDAT,
	ID_EQ_COMDAT_ANY,
	ID_EQ_CALL_TYP_ID_PARAM,
	ID_EQ_GLOBAL_TYP_CONST,
	TYP_CONSTANT,
	TYP_ID
} alternative;

typedef struct ast_node {
	/* Regular attributes - used during normal execution					*/
	ast_node_name name;		/* Name for this AST Node.						*/
	token* terminal;		/* Terminal associated with this node if it is
							   a nonterminal								*/
	struct ast_node* sibling;		/* Sibling next to this node					*/
	struct ast_node* child;		/* Child below this node						*/
	int size;				/* Number of tokens passed to make tree			*/
	alternative alt;		/* Alternative used to make tree				*/
	/* Irregular attributes - used during error recovery					*/
	error_info err_info;
} ast_node;


typedef enum ASTNodeNames {
	AST_PARAM,
	AST_PARAMS,
	AST_BINOP,
	AST_BLOCKS,
	AST_CONDITION,
	AST_CONSTANT,
	AST_COMMAND,
	AST_COP,
	AST_DECLDESCRIPTOR,
	AST_DEFDESCRIPTOR,
	AST_TROP,
	AST_EOP,
	AST_ERROR,
	AST_FBOP,
	AST_GLOBAL,
	AST_GLOBALS,
	AST_LAYOUT,
	AST_METADATA,
	AST_MODULE,
	AST_TERMINAL,
	AST_TYP,
	AST_TERMINATOR,
	AST_IDENTIFIER,
	AST_VAL,
	AST_PNODE,
	AST_PROD
} ast_node_name;

static char* ast_name_lookup[] = {
	[AST_PARAM] = "AST_PARAM",
	[AST_PARAMS] = "AST_PARAMS",
	[AST_BINOP] = "AST_BINOP",
	[AST_BLOCKS] = "AST_BLOCKS",
	[AST_COP] = "AST_COP",
	[AST_EOP] = "AST_EOP",
	[AST_ERROR] = "AST_ERROR",
	[AST_FBOP] = "AST_FBOP",
	[AST_CONDITION] = "AST_CONDITION",
	[AST_CONSTANT] = "AST_CONSTANT",
	[AST_COMMAND] = "AST_COMMAND",
	[AST_DECLDESCRIPTOR] = "AST_DECLDESCRIPTOR",
	[AST_DEFDESCRIPTOR] = "AST_DEFDESCRIPTOR",
	[AST_IDENTIFIER] = "AST_ID",
	[AST_MODULE] = "AST_MODULE",
	[AST_TERMINAL] = "AST_TERMINAL",
	[AST_TERMINATOR] = "AST_TERMINATOR",
	[AST_TROP] = "AST_TROP",
	[AST_VAL] = "AST_VAL",
	[AST_TYP] = "AST_TYP",
	[AST_PNODE] = "AST_PNODE",
	[AST_PROD] = "AST_PROD"
};

#define lookup_ast_as_name(a) ast_name_lookup[a]

void inline free_tree(ast_node* node) {
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
void inline print_ast_indent(ast_node* root, int indent) {
	if (root->name == AST_TERMINAL) {
		print_token(&(root->terminal));
	}
	else {
		printf("%s\n", lookup_ast_as_name(root->name));
	}

	ast_node* current_node = root->child;
	while (current_node != NULL) {
		printf("%*s", indent, "");
		print_ast_indent(current_node, indent + 8);
		current_node = current_node->sibling;
	}
}

void inline print_ast(ast_node* root) {
	print_ast_indent(root, 4);
}

void inline fprint_token(token** tk, FILE* fp) {
	char* field_format = "%-24s\t%-16s\t%-8d\n";
	fprintf(fp,
		field_format,
		lookup_token_as_name((**tk).name),
		(**tk).lexeme,
		(**tk).val);
}

/*
* Matches tokens into an abstract syntax tree for a layout.
*
* @param root The root of the tree which will be printed
* @param indent The level of indentation that the tree starts on
*
*/
void inline fprint_ast_indent(ast_node* root, int indent, FILE* fp) {
	if (root->name == AST_TERMINAL) {
		fprint_token(&(root->terminal), fp);
	}
	else {
		fprintf(fp, "%s\n", lookup_ast_as_name(root->name));
	}

	ast_node* current_node = root->child;
	while (current_node != NULL) {
		fprintf(fp, "%*s", indent, "");
		fprint_ast_indent(current_node, indent + 8, fp);
		current_node = current_node->sibling;
	}
}

void inline fprint_ast(ast_node* root, FILE* fp) {
	fprint_ast_indent(root, 4, fp);
}