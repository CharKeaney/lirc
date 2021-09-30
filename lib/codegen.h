#pragma once

#include "tree.h"
#include "tokens.h"
#include "semantics.h"

#define DEBUG_ADD false
#define DEBUG_GEN false

#define DEBUG_INSERT_COMMENTS true
#define ENABLE_RENAMED_IDS true

#define MAX_NUMBER_BLOCKS_IN_PROD (1 << 8)

inline ast_node* get_nonterminal(ast_node_name nonterminal_name, ast_node* parent) {

	ast_node* nonterminal = parent->child;

	if (nonterminal == NULL) { return NULL; }


	while (nonterminal->name != nonterminal_name) {

		ast_node* nt = get_nonterminal(nonterminal_name, nonterminal);
		if (nt) { return nt; }

		if (nonterminal->sibling == NULL) {
			return NULL;
		}
		else {
			nonterminal = nonterminal->sibling;
			continue;
		}
	}
	return nonterminal;
}

#define get_id(a) get_nonterminal(AST_IDENTIFIER, a)
#define get_typ(a) get_nonterminal(AST_TYP, a)
#define get_terminator(a) get_nonterminal(AST_TERMINATOR, a)
#define get_val(a) get_nonterminal(AST_VAL, a)

typedef enum delim_type {
	DELIM_GENERIC,
	DELIM_SPACE,
	DELIM_NEWLINE,
	DELIM_NODELIM,
	DELIM_TAB
} delim_type;

typedef struct generated_segment {
	char *repr;
	delim_type delim_prefix;
	char delim_prefix_multiplier;
	delim_type delim_postfix;
	char delim_postfix_multiplier;

	struct generated_segment *next_segment;
} generated_segment;

typedef struct generated_line {
	generated_segment *segment;
	struct generated_line *next_line;
} generated_line;

typedef struct generated_block {
	generated_line* line;
	struct generated_block *next_block;
} generated_block;

typedef struct generated_file {
	generated_block *includes;
	generated_block *globals;
	generated_block* functions;
	error_info *err;
} generated_file;

typedef struct cursor {
	generated_segment *seg;
	generated_line *line;
	generated_block *block;
	generated_file *file;
	symbol_table *sym;
} cursor;