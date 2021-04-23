#pragma once

#include "lexer.h"

#define parse(a) match_module(a)

#define PARSING_STACK_BUFFER (2<<12)

#define DEBUG_PARSER_MATCH false
#define DEBUG_PARSER_CREATION false
#define DEBUG_PARSER_CHILDREN false

/* Debugging options */
#define ENABLE_PANIC_MODE true

typedef enum ASTNodeNames ast_node_name;
typedef union ASTNodeChildren ast_node_children;

typedef struct ASTNode ast_node;

typedef enum  ErrorCode error_code;

char* error_msgs[];

typedef struct ASTNode {
	/* Regular attributes - used during normal execution					*/
	ast_node_name name;		/* Name for this AST Node.						*/
	token* terminal;		/* Terminal associated with this node if it is 
							   a nonterminal								*/
	ast_node* sibling;		/* Sibling next to this node					*/
	ast_node* child;		/* Child below this node						*/
	int size;				/* Number of tokens accounted for to make tree	*/	
	/* Irregular attributes - used during error recovery */
	bool contains_error;	/* Form of tree poisoning to show errors		*/
	error_code error_code;  /* The code associated with the error.			*/
	token** error_token;	/* Identifies where error is in terms of lexema */
} ast_node;

ast_node* create_ast_node(ast_node_name, token*);
void add_child(ast_node*, ast_node*);
struct ast_node* match_id(token **);
struct ast_node* match_id(token **);
struct ast_node* match_bop(token **);
struct ast_node* match_fbop(token **);
struct ast_node* match_cop(token **);
struct ast_node* match_trop(token **);
struct ast_node* match_eop(token **);
struct ast_node* match_typ(token **);
struct ast_node* match_constant(token **);
struct ast_node* match_val(token **);
struct ast_node* match_condition(token **);
struct ast_node* match_pnode(token **);
struct ast_node* match_command(token **);
struct ast_node* match_terminator(token **);
struct ast_node* match_block(token **);
struct ast_node* match_args(token **);
struct ast_node* match_prod(token **);
struct ast_node* match_namedt(token **);
struct ast_node* match_layout(token **);
struct ast_node* match_module(token **);
void free_tree(ast_node *);
void print_ast_indent(ast_node *, int);
void print_ast(ast_node *);
void fprint_token(token **, FILE *);
void fprint_ast_indent(ast_node *, int, FILE *);
void fprint_ast(ast_node *, FILE *);

