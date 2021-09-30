#pragma once

#include "tokens.h"
#include "tree.h"

#include <stdio.h>
#include <stdbool.h>

#define parse(a) match_module(a)

#define PARSING_STACK_BUFFER (2<<12)

#define DEBUG_PARSER_MATCH false
#define DEBUG_PARSER_CREATION false
#define DEBUG_PARSER_CHILDREN false
#define DEBUG_PARSER_BACKTRACK false

/* Debugging options */
#define ENABLE_PANIC_MODE true

ast_node* create_ast_node(ast_node_name, token*);
void add_child(ast_node*, ast_node*);
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
struct ast_node* match_params(token **);
struct ast_node* match_prod(token **);
struct ast_node* match_globals(token **);
struct ast_node* match_layout(token **);
struct ast_node* match_module(token **);
void free_tree(ast_node *);
void print_ast_indent(ast_node *, int);
void print_ast(ast_node *);
void fprint_token(token **, FILE *);
void fprint_ast_indent(ast_node *, int, FILE *);
void fprint_ast(ast_node *, FILE *);