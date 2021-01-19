#pragma once
#pragma once

#include "lexer.h"

#define DEBUG_PARSER false

typedef enum ASTNodeNames ast_node_name;
typedef union ASTNodeChildren ast_node_children;
typedef struct ASTNode ast_node;

struct ast_node* match_binop(token** tokens);
struct ast_node* match_command(token** tokens);
void print_ast_indent(ast_node* root, int indent);
void print_ast(ast_node* root);
