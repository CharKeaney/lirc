#pragma once

#include "codegen.h"

#define get_child(node) (node->child)
#define get_sibling(node) (node->sibling)

#define node_has_name(node, node_name) (node && node->name == (node_name))

ast_node* skip_to_node(ast_node* node, ast_node_name name) {
	printf("skip_to_node: skipping to node with name %s...\n", 
		lookup_ast_as_name(name));

	ast_node* child = node->sibling;
	while (child && child->name != name) {
		child = get_sibling(child);
	}
	if (!child) { 
		printf("skip_to_node: failed to find node with name %s.\n",
			lookup_ast_as_name(name));
	}
	return child;
}

token* get_first_terminal(ast_node* node) {
	if (node == NULL) { return NULL; }
	if (node->name == AST_TERMINAL) { return node->terminal; }
	else if (node->child) { return get_first_terminal(node->child); }
	return NULL;
}

generated_segment *construct_segment(char* repr) {
	generated_segment* seg = malloc(sizeof(generated_segment));
	seg->delim_prefix = DELIM_NODELIM;
	seg->delim_prefix_multiplier = 1;
	seg->delim_postfix = DELIM_GENERIC;
	seg->delim_postfix_multiplier = 1;
	seg->repr = repr;
	seg->next_segment = NULL;
	return seg;
}

generated_line* construct_line() {
	generated_line* line = malloc(sizeof(generated_line));
	line->segment = NULL;
	line->next_line = NULL;
	return line;
}

generated_block* construct_block() {
	generated_block* block = malloc(sizeof(generated_block));
	block->line = NULL;
	block->next_block = NULL;
	return block;
}

/* functions responsible for adding created structures to file. */

void inline add_segment(cursor *c, generated_segment *s) {
	if (DEBUG_ADD) { printf("ADDING SEGMENT (%s).\n", s->repr); }
	if (c->seg == NULL || c->line->segment == NULL) {
		c->line->segment = s;
		c->seg = s;
	} else {
		c->seg->next_segment = s;
		c->seg = c->seg->next_segment;
	}
}

#define MAX_RENAMED_ID_LEN 255

char* get_converted_id(char* id) {
	char* converted_id = NULL;
	bool is_num = true;
	bool first_letter = true;

	for (char* w = id + 1; *w != NULL; w++) { 
		if (!(*w >= '0' && *w <= '9')) { 
			is_num = false; break; 
		}
	}
	char* wrd = malloc(MAX_RENAMED_ID_LEN);
	char* w = wrd;

	if (is_num && ENABLE_RENAMED_IDS) {
		int val = atoi(id + 1);
		char start_of_alphabet = 'i';
		int num_letters_in_alphabet = 'z' - start_of_alphabet;
		int rem = val % num_letters_in_alphabet;

		do {
			*w = 'i' + rem;
			val /= num_letters_in_alphabet;
			rem = val % num_letters_in_alphabet;
			w++;
		} while (val != 0);

		*w = NULL;
		converted_id = wrd;

	} else {

		char* id_ptr = id + 1;
		do {
			if (!('a' <= *id_ptr <= 'z')
				&& !('A' <= *id_ptr <= 'Z')
				&& !(first_letter && ('0' <= *id_ptr <= '9' ))) { 
				id_ptr++;
			} else {
				*w++ = *id_ptr++;
				first_letter = false;
			}
		} while (*id_ptr != NULL);

		*w = NULL;
		converted_id = wrd;
	}
	printf("get_converted_id: converted %s to %s.\n", id, converted_id);
	return converted_id;
}

void inline add_line(cursor* c) {
	if (DEBUG_GEN) { printf("ADDING LINE.\n"); }
	if (c->seg) c->seg->delim_postfix = DELIM_NEWLINE;
	generated_line* l = construct_line();
	if (c->line == NULL) {
		c->block->line = l;		
		c->line = l;
	} else {
		c->line->next_line = l;
		c->line = c->line->next_line;
	}
}

void inline add_block(cursor* c, generated_block* b) {
	if (DEBUG_GEN) { printf("ADDING BLOCK.\n"); }
	if (c->block == NULL) {
		c->block = b;
	} else {
		c->block->next_block = b;
		c->block = c->block->next_block;
	}
}

/* general generation functions */

bool gen_terminal(cursor* c, char* chr) {

	if (DEBUG_GEN) {
		printf("GENERATING TERMINAL (%s).\n", chr);
	}
	add_segment(c, construct_segment(_strdup(chr), " "));
	return true;
}

/* functions responsible for adding new nonterminals to the file */

/**
* Generates the C representation for an id in this
* file, on this line, after these tokens (segments).
*/
bool gen_id(cursor *c, ast_node* id) {
	
	if (DEBUG_GEN) {
		printf("GENERATING ID (%s).\n",
			get_first_terminal(id) ? get_first_terminal(id)->lexeme : NULL);
	}

	if (id == NULL || id->name != AST_IDENTIFIER) {
		return NULL;
	}
	ast_node* child = id->child;
	token* tk = child->terminal;
	switch (tk->name) {

		case TOKEN_CSTRING: {
			char* r = _strdup(tk->lexeme + 1);
			generated_segment* seg = construct_segment(r);
			add_segment(c, seg);
			break;
		}

		case TOKEN_IDENTIFIER: {
			/* Identifiers are represented by their lexeme,
			   minus the starting marker */
			char* r = get_converted_id(tk->lexeme);
			generated_segment* seg = construct_segment(r);
			add_segment(c, seg);
			break;
		}

		case TOKEN_SZ: {
			char* repr = malloc(10);
			sprintf_s(repr, 10, "%d", tk->val.ival);
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_FLOAT_LITERAL: {
			char* repr = malloc(10);
			sprintf_s(repr, 10, "%f", tk->val.fval);
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_TRUE: {
			// TODO;
			/* What if stdbool is not included? */
			char* repr = _strdup("true");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_FALSE: {
			// TODO;
			/* What if stdbool is not  included? */
			char* repr = _strdup("false");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}
						
		case TOKEN_NULL: {
			char* repr = _strdup("NULL");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		default:
			printf("gen_id: failed on (%s)", id->terminal);
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_ID;
				c->file->err->error_token = *get_first_terminal(id);
			}
			return false;
	}
	return true;
}

/**
* Generates the C representation for a binary operator in this
* file, on this line, after these tokens (segments).
*/
bool gen_bop(cursor *c, ast_node* bop) {
	if (DEBUG_GEN) {
		printf("GENERATING BOP (%s).\n",
			get_first_terminal(bop) ? get_first_terminal(bop)->lexeme: NULL);
	}

	if (bop == NULL || bop->name != AST_BINOP) {
		return NULL;
	}

	token* tk = bop->child->terminal;

	switch (tk->name) {
		
		case TOKEN_ADD: {
			char* repr = _strdup("+");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_SUB: {
			char* repr = _strdup("-");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_MUL: {
			char* repr = _strdup("*");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_UDIV: {
			char* repr = _strdup("/");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_SDIV: {
			char* repr = _strdup("/");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_UREM: {
			char* repr = _strdup("%");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_SREM: {
			char* repr = _strdup("%");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_SHL: {
			char* repr = _strdup("<<");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_LSHR: {
			char* repr = _strdup(">>");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_ASHR: {
			char* repr = _strdup(">>");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_AND: {
			char* repr = _strdup("&");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_OR: {
			char* repr = _strdup("|");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_XOR: {
			char* repr = _strdup("^");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}
					 
		default: {
			printf("gen_bop: failed on (%s).\n", bop->terminal);
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_BOP;
				c->file->err->error_token = *get_first_terminal(bop);
			}
			return false;
		}
	}
	return true;
}


/**
* Generates the C representation for a float binary operator in 
* this file, on this line, after these tokens (segments).
*/
bool gen_fbop(cursor* c, ast_node* fbop) {
	token* tk = fbop->terminal;

	if (DEBUG_GEN) {
		printf("GENERATING FBOP.\n");
	}

	if (fbop == NULL || fbop->name != AST_FBOP) { return NULL; }
	
	switch (tk->name) {

		case TOKEN_FADD: {
			char* repr = _strdup("+");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_FSUB: {
			char* repr = _strdup("-");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_FMUL: {
			char* repr = _strdup("*");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_FDIV: {
			char* repr = _strdup("/");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_FREM: {
			char* repr = _strdup("%");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		default:
			printf("gen_fbop: failed on (%s).\n", fbop->terminal);
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_FBOP;
				c->file->err->error_token = *get_first_terminal(fbop);
			}
			return false;
	}
	return true;
}


/**
* Generates the C representation for a cast operator in this
* file, on this line, after these tokens (segments).
*/
bool gen_cop(cursor* c, ast_node* cop) {
	if (DEBUG_GEN) {
		printf("GENERATING COP.\n");
	}

	if (cop == NULL || cop->name != AST_COP) {
		return NULL;
	}

	token* tk = cop->terminal;

	switch (tk->name) {
		case TOKEN_BITCAST:
		case TOKEN_SITOFP:
		case TOKEN_PTRTOINT:
		case TOKEN_FPTOUI:
		case TOKEN_FPTOSI:
		case TOKEN_INTTOPTR:
		case TOKEN_UITOFP:
			break;

		default:
			printf("gen_cop: failed on (%s)", cop->terminal);
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_COP;
				c->file->err->error_token = *get_first_terminal(cop);
			}
			return false;
	}
	return true;
}


/**
* Generates the C representation for a trop operator in this
* file, on this line, after these tokens (segments).
*/
bool gen_trop(cursor *c, ast_node* trop) {
	if (DEBUG_GEN) {
		printf("GENERATING TROP.\n");
	}

	if (trop->name != AST_TROP
		|| trop == NULL) {
		return NULL;
	}

	token* tk = trop->child->terminal;

	switch (tk->name) {
		case TOKEN_TRUNC:
			break;
		default:
			printf("gen_trop: failed on (%s).\n", trop->terminal);
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_TROP;
				c->file->err->error_token = *get_first_terminal(trop);
			}
			return false;
	}
	return true;
}

/* Quick mapping table to get the type of a value from the
   size of data stored in it. 
   Only designed for integer types  */
char *size_to_c_integer_type[] = {
	[1] = "bool",
	[8] = "char",
	[32] = "int",
	[48] = "long int",
	[64] = "long long int"
};

#define size_to_c_integer_type(typ) size_to_c_integer_type[(typ)]

/**
* Generates the C representation for a typ operator in this
* file, on this line, after these tokens (segments).
*/
bool gen_typ(cursor* c, ast_node* typ) {
	if (DEBUG_GEN) {
		printf("GENERATING TYP.\n");
	}

	if (typ == NULL || typ->name != AST_TYP) {
		return NULL;
	}

	ast_node* child = typ->child;
	token* tk = child->terminal;
	
	switch (typ->alt) {

		case INTEGERTYPE: {
			int sz = tk->val.ival;
			char* c_typ = size_to_c_integer_type(sz);
			char* repr = _strdup(c_typ);
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case DOUBLE: {
			char* repr = _strdup("double");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case FLOAT: {
			char* repr = _strdup("float");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TYP_PTR: {
			if (!gen_typ(c, child)) { return false; }
			char* repr = _strdup("*");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg); c->seg->delim_postfix = DELIM_NODELIM;
			break;
		}

		case VOID: {
			char* repr = _strdup("void");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case IDENTIFIER: {
			char* repr = _strdup(tk->lexeme + 1);
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case OPEN_PAREN_TYP_CLOSE_PAREN: 
			break;

		case OPEN_CURLY_BRACKET_TYP_CLOSE_CURLY_BRACKET: {
			child = get_sibling(child);		/* skip curly bracket ({)	*/
			ast_node* typ1 = child;

			gen_terminal(c, "struct");
			gen_terminal(c, "{");

			char repr[] = "a";				
			//gen_terminal(c, strdup("testattr"));

			for (; typ1->name == AST_TYP; typ1 = get_sibling(typ1)) {
				gen_typ(c, typ1);

				gen_terminal(c, _strdup(repr));				
				*repr += 1;

				if (typ1->sibling->name == AST_TYP) { 
					c->seg->delim_postfix = DELIM_NODELIM; 
					gen_terminal(c, ",");
				}
			}
			gen_terminal(c, "}");
			break;
		}

		case OPEN_ANGLE_BRACKET_TYP_CLOSE_ANGLE_BRACKET: 

		case OPEN_BRACKET_SZ_X_TYP_CLOSE_BRACKET: {
			child = get_sibling(child);		/* skip bracket ([)	*/
			ast_node* sz = child;
			child = get_sibling(child);
			ast_node* typ1 = child;
			child = get_sibling(child);
			
			gen_typ(c, typ1);
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, "[");
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, sz->terminal->lexeme);
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, "]");
			break;
		}

		default:
			printf("gen_typ: failed on (%s).\n", (typ->child->terminal) ? typ->child->terminal->lexeme : NULL);
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_TYP;
				c->file->err->error_token = *get_first_terminal(typ);
			}
			return false;
	}
	return true;
}

bool gen_constant(cursor *c, ast_node* constant) {
	if (DEBUG_GEN) {
		printf("GENERATING CONSTANT.\n");
	}
	ast_node* child = constant->child;

	switch(constant->alt) {
		
		case GETELEMENTPTR_TYP_CONSTANT_CONSTANT: {
			/* Collection */
			ast_node* typ = child;
			child = skip_to_node(child, AST_CONSTANT);
			ast_node* cnst1 = child;
			child = skip_to_node(child, AST_CONSTANT);
			ast_node* cnst2 = child;
			child = skip_to_node(child, AST_CONSTANT);
			ast_node* cnst3 = child;
			/* Generation */
			add_line(c);

			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_constant(c, cnst1)) return false;
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, "[");
			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_constant(c, cnst3)) return false;
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, "]");
			c->seg->delim_postfix = DELIM_NODELIM;
			return true;
		}

		case INTEGERTYPE_ID: {
			/* Collection */
			child = skip_to_node(child, AST_IDENTIFIER);
			ast_node* id = child;		
			/* Generation */
			if (!gen_id(c, id)) break;
			return true;
		}

		case TYP_PTR_ID: {
			/* Collection */
			ast_node* typ = child;			/* store typ */
			child = skip_to_node(child, AST_IDENTIFIER);
			ast_node* id = child;			/* store id */
			/* Generation */
			if (!gen_id(c, id)) break;
			return true;
		}
		
		case TYP_PTR_CONSTANT: {
			/* Collection */
			child = skip_to_node(child, AST_CONSTANT);
			ast_node* constant = child;		/* Skip CONSTANT	*/
			/* Generation */
			if (!gen_constant(c, constant)) break;
			return true;
		}
		
		default: break;
	}		
	printf("gen_constant: failed on (%s).\n", get_first_terminal(constant));
	if (!c->file->err->contains_error) {
		c->file->err->contains_error = true;
		c->file->err->error_code = INVALID_CONSTANT;
		c->file->err->error_token = *get_first_terminal(constant);
	}
	return false;
};

bool gen_val(cursor *c, ast_node* val) {
	if (DEBUG_GEN) {
		printf("GENERATING VAL (%s).\n", 
			get_first_terminal(val) ? get_first_terminal(val)->lexeme : NULL);
	}

	if (val == NULL || val->name != AST_VAL) {
		printf("EXPECTED VAL (%d).\n", AST_VAL);
		return NULL;
	}

	ast_node* child = val->child;
	switch (val->alt) {
		case CONSTANT:
			if (!gen_constant(c, child)) break;
			return true;
		case ID:
			if (!gen_id(c, child)) break;
			return true;
	}
	if (!c->file->err->contains_error) {
		c->file->err->contains_error = true;
		c->file->err->error_code = INVALID_VAL;
		c->file->err->error_token = *get_first_terminal(val);
	}
	return false;
};
 
bool gen_condition(cursor *c, ast_node* condition) {
	if (DEBUG_GEN) {
		printf("GENERATING CONDITION.\n");
	}
	switch (condition->child->terminal->name) {

		case TOKEN_OEQ:
		case TOKEN_EQ: {
			char* repr = _strdup("==");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}
		
		case TOKEN_UNE:
		case TOKEN_NE: {
			char* repr = _strdup("!=");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_OLT:
		case TOKEN_ULT:
		case TOKEN_SLT: {
			char* repr = _strdup("<");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_UGT:
		case TOKEN_OGT:
		case TOKEN_SGT: {
			char* repr = _strdup(">");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_OGE:
		case TOKEN_UGE:
		case TOKEN_SGE: {
			char* repr = _strdup(">=");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		case TOKEN_SLE: 
		case TOKEN_ULE:		
		case TOKEN_OLE: {
			char* repr = _strdup("<=");
			generated_segment* seg = construct_segment(repr);
			add_segment(c, seg);
			break;
		}

		default:
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_CONDITION;
				c->file->err->error_token = *get_first_terminal(condition);
			}
			return false;
	}
	return true;
};

bool gen_pnode(cursor *c, ast_node* pnode) {
	if (DEBUG_GEN) {
		printf("GENERATING PNODE.\n");
	}
	return true;
}

bool gen_conversion_op(cursor *c, ast_node *typ1, ast_node *typ2) {
	if (typ1->name != AST_TYP || typ2->name != AST_TYP) return false;

	switch (typ1->alt) {

		case TYP_PTR: {
		
			switch (typ2->alt) {

				case TYP_PTR:

					if (typ1->size > typ2->size) {
						gen_terminal(c, "*"); c->seg->delim_postfix = DELIM_NODELIM;
						if (!gen_conversion_op(c, typ1->child, typ2)) return false;
						return true;
					}
					else if (typ1->size < typ2->size) {
						gen_terminal(c, "&"); c->seg->delim_postfix = DELIM_NODELIM;
						if (!gen_conversion_op(c, typ1, typ2->child)) return false;
						return true;

					}
					else /* typ1->size == typ2->size */ {
						if (!gen_conversion_op(c, typ1->child, typ2->child)) return false;
						return true;
					}
					
				case IDENTIFIER:
				case INTEGERTYPE:
					gen_terminal(c, "*"); c->seg->delim_postfix = DELIM_NODELIM;
					if (!gen_conversion_op(c, typ1->child, typ2)) return false;
					return true;

				default: 
					return false;
			}
		}

		case IDENTIFIER:
		case INTEGERTYPE:

			switch (typ2->alt) {

				case IDENTIFIER:
				case INTEGERTYPE:
					return true;

				case TYP_PTR:
					//gen_terminal(c, "&"); c->seg->delim_postfix = DELIM_NODELIM;
					if (!gen_conversion_op(c, typ1, typ2->child)) return false;
					return true;

				default: 
					return false;
			}
			break;

		default: 
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_CONVERSION_OP;
				c->file->err->error_token = *get_first_terminal(typ1);
			}
			return false;
	}
}

ast_node* get_typ_of_struct_offset(ast_node* struct_node, int offset) {
	ast_node* child = struct_node->child;
	for (int i = 0; i < offset; i++) { child = child->sibling; }
	if (child == NULL || child->name != AST_TYP) { return NULL; }
	return child;
}

bool gen_params(cursor *c, ast_node* params) {
	if (DEBUG_GEN) {
		printf("GENERATING PARAMS.\n");
	}

	for (ast_node* param = params->child; 
		(param != NULL) && (param->name == AST_PARAM); 
		param = param->sibling) {

		ast_node* child = param->child;

		switch (param->alt) {
			
			case CONSTANT: {
				ast_node* cnst = child;
				if (!gen_constant(c, cnst)) return false;
				break;
			}

			case TYP_CONSTANT: {
				ast_node* typ = child;
				child = skip_to_node(child, AST_CONSTANT);
				ast_node* cnst = child;
				if (!gen_typ(c, typ)) return false;
				if (!gen_constant(c, cnst)) return false;
				break;
			}

			case TYP_ID: {
				ast_node* typ = child;
				child = skip_to_node(child, AST_IDENTIFIER);
				ast_node* id = child;
				if (!gen_typ(c, typ)) return false;
				if (!gen_id(c, id)) return false;
				break;
			}

			default:
				return false;
		}
		if (param->sibling != NULL) gen_terminal(c, ",");
	}
	return true;
}

bool gen_args(cursor* c, ast_node* params) {
	if (DEBUG_GEN) {
		printf("GENERATING PARAMS.\n");
	}

	for (ast_node* param = params->child;
		(param != NULL) && (param->name == AST_PARAM);
		param = param->sibling) {

		ast_node* child = param->child;

		switch (param->alt) {

			case CONSTANT: {
				ast_node* cnst = child;
				if (!gen_constant(c, cnst)) return false;
				return true;
			}
						 
			case TYP_CONSTANT: {
				child = skip_to_node(child, AST_IDENTIFIER);
				ast_node* cnst = child;
				if (!gen_constant(c, cnst)) return false;
				return true;
			}

			case TYP_ID: {
				child = skip_to_node(child, AST_IDENTIFIER);
				ast_node* id = child;
				if (!gen_id(c, id)) return false;
				return true;
			}

			default:
				return false;
		}
	}
	return true;
}

insert_var_into_symtab(char* lexeme, bool var_is_undeclared, bool var_is_primitive, symbol_table *sym) {
	symbol_entry* entry = create_entry_for_var(
		lexeme,
		var_is_undeclared,
		var_is_primitive);
	insert_symbol(sym, entry);
}


bool gen_command(cursor* c, ast_node* command, int nesting_level) {
	if (DEBUG_GEN) {
		printf("GENERATING COMMAND.\n");
	}
	if (c->sym == NULL) { printf("gen_command: sym not found.\n"); }

	generated_segment* starting_seg = NULL;

	bool encountered_error = false;

	ast_node* child = get_child(command);
	switch (command->alt) {

		case GENERIC_ALT: break;

		case ID_EQ_GETELEMENTPTR_TYP_VAL: {
			ast_node* id = child;
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;
			child = skip_to_node(child, AST_VAL);
			ast_node* val1 = child;
			child = skip_to_node(child, AST_VAL);
			ast_node* val2 = child;
			child = skip_to_node(child, AST_VAL);
			ast_node* val3 = child;
			
			char* id_lexeme = id->child->terminal->lexeme;
			char* conv_id = get_converted_id(id_lexeme);

			insert_var_into_symtab(
				get_converted_id(id->child->terminal->lexeme),
				true,
				false,
				c->sym);

			add_line(c);
   
			symbol_entry* entry = symbol_table_lookup(c->sym, conv_id);
			if (entry->var_is_undeclared) {
				gen_typ(c, typ);	
				starting_seg = c->seg;
				c->seg->delim_postfix = DELIM_NODELIM;
				gen_terminal(c, "*");
				entry->var_is_undeclared = false;
			}
			if (!gen_id(c, id)) { encountered_error = true; break; }
			if (!starting_seg) starting_seg = c->seg;

			if (!gen_terminal(c, "=")) { encountered_error = true; break; }

			gen_terminal(c, "&");
			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_val(c, val1)) { encountered_error = true; break; }
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, "[");
			c->seg->delim_postfix = DELIM_NODELIM;

			if (val3) {
				if (!gen_val(c, val3)) { encountered_error = true; break; }
			} else {
				if (!gen_val(c, val2)) { encountered_error = true; break; }
			}
			c->seg->delim_postfix = DELIM_NODELIM;

			gen_terminal(c, "]");
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, ";");
			add_line(c);
			return true;
		}

		case ID_EQ_ALLOCA_TYP_ALIGN_SZ: {
			/* Collection */
			ast_node* id = child;		/* Store ID		*/			
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;		/* Store TYP	*/
			
			/* Recording */
			insert_var_into_symtab(
				get_converted_id(id->child->terminal->lexeme),
				false, 
				true, 
				c->sym);

			/* Generation */
			add_line(c);

			if (!gen_typ(c, typ)) { encountered_error = true; break; }
			starting_seg = c->seg;
			
			if (typ->alt == OPEN_BRACKET_SZ_X_TYP_CLOSE_BRACKET) {			

				generated_segment* start_of_insert = c->line->segment;
				while (strcmp(start_of_insert->next_segment->repr, "[") != 0) {
					start_of_insert = start_of_insert->next_segment;
				}
				start_of_insert->delim_postfix = DELIM_SPACE;
				generated_segment* end_of_insert = start_of_insert;
				while (strcmp(end_of_insert->repr, "]") != 0) {
					end_of_insert = end_of_insert->next_segment;
				}

				if (!gen_id(c, id)) { encountered_error = true;  break; }
				c->seg->delim_postfix = DELIM_NODELIM;
				
				c->seg->next_segment = start_of_insert->next_segment;
				start_of_insert->next_segment = end_of_insert->next_segment;
				c->seg = end_of_insert;
			} else {
				if (!gen_id(c, id)) { encountered_error = true;  break; }
			}

			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, ";");
			add_line(c);
			return true;
		}

		case ID_EQ_LOAD_TYP_VAL_ALIGN_SZ: {
			/* Collection */
			ast_node* id = child;		/* Store ID		*/			
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;		/* Store TYPPTR	*/		
			child = skip_to_node(child, AST_VAL);
			ast_node* val = child;
			
			/* Recording */
			char* id_lexeme = id->child->terminal->lexeme;
			char* conv_id = get_converted_id(id_lexeme);
			insert_var_into_symtab(
				conv_id,
				true,
				true,
				c->sym);

			/* Generation */
			symbol_entry* entry = symbol_table_lookup(c->sym, conv_id);
			if (entry->var_is_undeclared) {
				gen_typ(c, typ);
				starting_seg = c->seg;
				entry->var_is_undeclared = false;
			}
			if (!gen_id(c, id)) { encountered_error = true;  break; }
			if (!starting_seg) starting_seg = c->seg;

			if (!gen_terminal(c, "=")) { encountered_error = true;  break; }
	
			ast_node* store_value_typ = typ;
			ast_node* store_location_typ = get_typ(val);
			
			if ((entry != NULL) && (entry->var_is_primitive)) {
				/* we deref the store location if it is a primitive var */
				if (!gen_conversion_op(c, store_value_typ, store_location_typ->child)) { 
					encountered_error = true;  break; 
				}
			}
			else {
				if (!gen_conversion_op(c, store_value_typ, store_location_typ)) { 
					encountered_error = true;  break; 
				}
			}

			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_val(c, val)) { encountered_error = true;  break; }
			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_terminal(c, ";")) { encountered_error = true;  break; }
			add_line(c);
			break;
		}
										
		case ID_EQ_BOP_INTEGERTYPE_VAL_VAL: {
			/* Collection */
			ast_node* id = child;				/* Store ID		*/		
			child = skip_to_node(child, AST_BINOP);
			ast_node* bop = child;				/* Store BOP	*/
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;				/* store typ	*/
			child = skip_to_node(child, AST_VAL);
			ast_node* val1 = child;				/* store val	*/
			child = skip_to_node(child, AST_VAL);
			ast_node* val2 = child;				/* store val */

			/* Recording */
			char* conv_id = get_converted_id(id->child->terminal->lexeme);
			insert_var_into_symtab(
				conv_id,
				true,
				true,
				c->sym);
			
			/* Generation */
			add_line(c);

			symbol_entry* entry = symbol_table_lookup(c->sym, conv_id);
			if (entry->var_is_undeclared) {
				gen_typ(c, typ);
				starting_seg = c->seg;
				entry->var_is_undeclared = false;
			}
			if (!gen_id(c, id)) { encountered_error = true; break; }
			if(!starting_seg) starting_seg = c->seg;
			if (!gen_terminal(c, "=")) { encountered_error = true;  break; }
			if (!gen_val(c, val1)) { encountered_error = true;  break; }
			if (!gen_bop(c, bop)) { encountered_error = true;  break; }
			if (!gen_val(c, val2)) { encountered_error = true;  break; }
			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_terminal(c, ";")) { encountered_error = true;  break; }
			add_line(c);
			break;
		}

		case ID_EQ_ICMP_COND_VAL_VAL: {
			/* Collection */
			ast_node* id = child;		/* Store ID		*/			
			child = skip_to_node(child, AST_CONDITION);

			ast_node* cond = child;		/* store cond	*/
			child = skip_to_node(child, AST_VAL);
			ast_node* val1 = child;		/* store val	*/
			child = skip_to_node(child, AST_VAL);
			ast_node* val2 = child;		/* store val	*/

			/* Recording */
			char* conv_id = get_converted_id(id->child->terminal->lexeme);
			insert_var_into_symtab(
				conv_id,
				true,
				true,
				c->sym);

			/* Generation */
			
			symbol_entry* entry = symbol_table_lookup(c->sym, conv_id);
			if (entry->var_is_undeclared) {
				gen_terminal(c, "bool");
				starting_seg = c->seg;
				entry->var_is_undeclared = false;
			}
			if (!gen_id(c, id)) { encountered_error = true;  break; }
			if (!starting_seg) starting_seg = c->seg;
			if (!gen_terminal(c, "=")) { encountered_error = true;  break; }
			if (!gen_val(c, val1)) { encountered_error = true;  break; }
			if (!gen_condition(c, cond)) { encountered_error = true;  break; }
			if (!gen_val(c, val2)) { encountered_error = true;  break; }
			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_terminal(c, ";")) { encountered_error = true;  break; }
			add_line(c);
			break;
		}

		case ID_EQ_COP_TYP_VAL_TO_TYP: {
			/* Collection */
			ast_node* id = child;		/* Store ID		*/			
			child = skip_to_node(child, AST_COP);
			ast_node* cop = child;		/* Store COP	*/			
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;		/* skip typ		*/
			child = skip_to_node(child, AST_VAL);
			ast_node* val = child;

			/* Recording */
			char* conv_id = get_converted_id(id->child->terminal->lexeme);
			insert_var_into_symtab(
				conv_id,
				true,
				true,
				c->sym);

			/* Generation */
			add_line(c);

			symbol_entry* entry = symbol_table_lookup(c->sym, conv_id);
			if (entry->var_is_undeclared) {
				if (!gen_typ(c, typ)) { encountered_error = true;  break; }
				entry->var_is_undeclared = false;
			}
			if (!gen_id(c, id)) { encountered_error = true;  break; }
			starting_seg = c->seg;
			if (!gen_terminal(c, "=")) { encountered_error = true;  break; }
			if (!gen_val(c, val)) { encountered_error = true;  break; }
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, ";");
			add_line(c);
			break;
		}

		case ID_EQ_COP_VAL_TO_TYP: {
			/* Collection */
			ast_node* id = child;			
			child = skip_to_node(child, AST_VAL);
			ast_node* val = child;			
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;

			insert_var_into_symtab(
				get_converted_id(id->child->terminal->lexeme),
				true,
				true,
				c->sym);

			/* Generation */
			gen_id(c, id);
			starting_seg = c->seg;
			gen_terminal(c, "=");
			gen_terminal(c, "(");
			gen_typ(c, typ);
			gen_terminal(c, ")");
			gen_val(c, val);
			gen_terminal(c, ";");
			break;
		}

		case ID_EQ_EOP_TYP_VAL_TO_TYP: {
			/* Collection */
			ast_node* id = child;
			child = skip_to_node(child, AST_TYP);
			ast_node* typ1 = child;			
			child = skip_to_node(child, AST_VAL);
			ast_node* val = child;			
			child = skip_to_node(child, AST_TYP);
			ast_node* typ2 = child;

			insert_var_into_symtab(
				get_converted_id(id->child->terminal->lexeme),
				true,
				true,
				c->sym);

			/* Generation */
			gen_id(c, id);
			starting_seg = c->seg;
			gen_terminal(c, "=");
			gen_terminal(c, "(");
			gen_typ(c, typ2);
			gen_terminal(c, ")");
			gen_val(c, val);
			gen_terminal(c, ";");
			c->seg->delim_postfix = DELIM_NODELIM;
			break;
		}

		case STORE_TYP_VAL_VAL: 
		case STORE_TYP_VAL_VAL_ALIGN_SZ: {
			/* Collection */			
			child = skip_to_node(child, AST_TYP);
			ast_node* val1_typ = child;	/* store TYP	*/
			child = skip_to_node(child, AST_VAL);
			ast_node* val1 = child;		/* store VAL1	*/
			child = skip_to_node(child, AST_VAL);
			ast_node* val2 = child;		/* store VAL2	*/

			/* Generation */
			add_line(c);

			char* lex = get_id(val1)->child->terminal->lexeme;
			char* conv_id = get_converted_id(lex);
			symbol_entry* entry = symbol_table_lookup(c->sym, conv_id);

			if ((entry != NULL) && (entry->var_is_undeclared == true)) {
				gen_typ(c, val1_typ);
				entry->var_is_undeclared = false;
			}

			ast_node* val2_typ = get_nonterminal(AST_TYP, val2);
			ast_node* val2_id = get_nonterminal(AST_IDENTIFIER, val2);
			char* val2_converted_id = get_converted_id(val2_id->child->terminal->lexeme);
			symbol_entry* val2_id_entry = symbol_table_lookup(c->sym, val2_converted_id);
		
			if (!gen_val(c, val2)) { encountered_error = true;  break; }
			starting_seg = c->seg;

			if (!gen_terminal(c, "=")) { encountered_error = true;  break; }

			if (val1_typ->alt == TYP_PTR) {
				gen_terminal(c, "*");
				c->seg->delim_postfix = DELIM_NODELIM;
			}
			if (!gen_val(c, val1)) { encountered_error = true;  break; }
			c->seg->delim_postfix = DELIM_NODELIM;

			if (!gen_terminal(c, ";")) { encountered_error = true;  break; }
			add_line(c);
			return true;
		}
		
		case ID_EQ_CALL_TYP_ID_PARAM: {
			/* Collection */
			ast_node* var_id = child;	/* store ID		*/		
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;		/* store typ	*/
			child = skip_to_node(child, AST_IDENTIFIER);
			ast_node* func_id = child;	/* store id		*/
			child = skip_to_node(child, AST_PARAMS);
			ast_node* args = child;		/* store param	*/

			insert_var_into_symtab(
				get_converted_id(var_id->child->terminal->lexeme),
				true,
				true,
				c->sym);

			/* Generation */
			add_line(c);
			
			if (!gen_typ(c, typ)) { encountered_error = true;  break; }
			starting_seg = c->seg;
			if (!gen_id(c, var_id)) { encountered_error = true;  break; }

			gen_terminal(c, "=");

			if (!gen_id(c, func_id)) { encountered_error = true;  break; }
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, "(");
			gen_args(c, args);
			gen_terminal(c, ")");
			gen_terminal(c, ";");
			add_line(c);
			break;
		}
	
		default: break;
	}
	if (encountered_error) {
		if (!c->file->err->contains_error) {
			c->file->err->contains_error = true;
			c->file->err->error_code = INVALID_COMMAND;
			c->file->err->error_token = *get_first_terminal(command);
		}
	} else {
		starting_seg->delim_prefix_multiplier = nesting_level;
		starting_seg->delim_prefix = DELIM_TAB;
	}
	return !encountered_error;
}

/**
* Generates the C representation for a id in this
* file, on this line, after these tokens (segments).
*/
bool gen_terminator(cursor *c, ast_node* terminator) {

	if (DEBUG_GEN) {
		printf("GENERATING TERMINATOR.\n");
	}
	ast_node* child = terminator->child;
	switch (terminator->alt) {

		case RET_TYP_VAL: {
			/* Collection */
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;		/* store TYP	*/
			child = skip_to_node(child, AST_VAL);
			ast_node* val = child;		/* store VAL	*/

			/* Generation */
			add_line(c);
			if (!gen_terminal(c, "return")) break;

			if (!gen_val(c, val)) break;
			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_terminal(c, ";")) break;
			add_line(c);
			return true;
		}

		case BR_TYP_ID_LABEL_ID_LABEL_ID: {
			/* Collection */
			child = skip_to_node(child, AST_VAL);
			ast_node* val = child;			
			child = skip_to_node(child, AST_IDENTIFIER);
			ast_node* id2 = child;		/* store ID		*/		
			child = skip_to_node(child, AST_IDENTIFIER);
			ast_node* id3 = child;		/* store ID		*/

			/* Generation */
			add_line(c);
			if (!gen_terminal(c, "if")) break;
			if (!gen_terminal(c, "(")) break;
			if (!gen_val(c, val)) break;
			if (!gen_terminal(c, ")")) break;
			return true;
		}
	}
	if (!c->file->err->contains_error) {
		c->file->err->contains_error = true;
		c->file->err->error_code = INVALID_TERMINATOR;
		c->file->err->error_token = *get_first_terminal(terminator);
	}
	return false;
}


void print_control_flow_graph(char *cfg[][MAX_NUMBER_BLOCKS_IN_PROD]) {
	printf("%6s    %4s\n", "Source", "Dest");
	printf("%6s    %4s\n", "------", "----");

	for (int i = 0; i < MAX_NUMBER_BLOCKS_IN_PROD; i++) {
		for (int j = 0; cfg[i][j] ; j++) {
			if (cfg[i][j]) printf("%6d -> %-s\n", i, cfg[i][j]);
		}
	}
}

void fill_control_flow_graph(
	symbol_table* sym,
	ast_node** blocks,	
	char* control_flow_graph[][MAX_NUMBER_BLOCKS_IN_PROD]) {

	for (int i = 0; i < MAX_NUMBER_BLOCKS_IN_PROD; i++) {
		for (int j = 0; j < MAX_NUMBER_BLOCKS_IN_PROD; j++) {
			control_flow_graph[i][j] = NULL;
		}
	}

	for (ast_node** b = blocks; *b != NULL; b++) {

		ast_node* child = get_child((*b));

		char* name = child->terminal->lexeme;
		int block_len = strlen(name) + 1;
		char* block_name = malloc(block_len);
		strcpy_s(block_name, block_len, name);
		block_name[block_len - 2] = NULL;

		int origin = symbol_table_lookup(sym, block_name)->block_number;
		
		child = skip_to_node(child, AST_TERMINATOR);

		if (node_has_name(child, AST_TERMINATOR)) {

			switch (child->alt) {

				case BR_LABEL_ID:
				case BR_TYP_ID_LABEL_ID_LABEL_ID: {
					int i = 0;
					for (ast_node* c = child->child;
						c != NULL;
						c = c->sibling) {

						if (c->name == AST_IDENTIFIER) {
							char* child_name = c->child->terminal->lexeme + 1;
							control_flow_graph[origin][i++] = child_name;
						}
					}
					control_flow_graph[origin][i] = NULL;
					break;
				}

				case INVOKE_TYP_ID_PARAMS_TO_LABEL_ID_LABEL_ID:
				case SWITCH_VAL_LABEL_ID_OPEN_BRACKET_CLOSE_BRACKET:
				case RET_VOID:
				case BR_UNREACHABLE:
				case UNREACHABLE:
				default:
					break;
			}
		}	
		free(block_name);
	}
}

bool gen_block(cursor* c, ast_node* block, int nesting_level) {
	if (DEBUG_GEN) {
		printf("GENERATING BLOCK.\n");
	}
	ast_node* child = get_child(block);

	if (node_has_name(child, AST_TERMINAL)) {
		//gen_terminal(c, child->terminal->lexeme);
		//c->seg->delim_postfix = DELIM_NEWLINE;
		child = get_sibling(child);
	}
	if (node_has_name(child, AST_PNODE)) {
		if (!gen_pnode(c, child)) return false;
		child = get_sibling(child);
	}
	while (node_has_name(child, AST_COMMAND)) {
		if (!gen_command(c, child, nesting_level)) {
			if (!c->file->err->contains_error) {
				c->file->err->contains_error = true;
				c->file->err->error_code = INVALID_COMMAND;
				c->file->err->error_token = *get_first_terminal(block);
			}
			return false;
		}
		child = get_sibling(child);
	}
	return true;
}

typedef enum control_flow_construct {
	CONSTRUCT_IF_ELSE,
	CONSTRUCT_WHILE,
	CONSTRUCT_FOR,
} control_flow_construct;

control_flow_construct get_construct_type(
	char* cfg[][MAX_NUMBER_BLOCKS_IN_PROD], 
	int origin,
	symbol_table* symtab) {
	
	/* condition */
	for (char** i = cfg[origin]; *i != NULL; i++) {
		symbol_entry* ie = symbol_table_lookup(symtab, *i); 
		int ii = ie->block_number;

		/* body	*/
		for (char** j = cfg[ii]; *j != NULL; j++) {
			symbol_entry* je = symbol_table_lookup(symtab, *j);
			int ji = je->block_number;

			if (ji == origin) {
				printf("get_construct_type: returning CONSTRUCT_WHILE.\n");
				return CONSTRUCT_WHILE;
			}

			for (char** k = cfg[ji]; *k != NULL; k++) {
				symbol_entry* ke = symbol_table_lookup(symtab, *k);
				int ki = ke->block_number;

				if (ki == origin) {
					printf("get_construct_type: returning CONSTRUCT_WHILE.\n");
					return CONSTRUCT_FOR;
				}
			}
		}
	}
	printf("get_construct_type: returning CONSTRUCT_IF_ELSE.\n");
	return CONSTRUCT_IF_ELSE;
}

bool gen_blocks_helper(
	cursor *c,
	ast_node **blocks,
	ast_node *block,
	char *cfg[][MAX_NUMBER_BLOCKS_IN_PROD],
	int nesting_level) {

	int num_edges_in_cfg = 0;

	char* name = block->child->terminal->lexeme;

	int block_name_len = strlen(name);
	char* block_name = malloc(block_name_len + 1);
	strcpy_s(block_name, block_name_len + 1, name);
	block_name[block_name_len - 1] = NULL;

	int origin = symbol_table_lookup(c->sym, block_name)->block_number;

	printf("gen_blocks_helper: generating block %d (%s).\n",
		origin, block_name);

	for (int i = 0; cfg[origin][i]; i++) {
		num_edges_in_cfg++;
	}

	printf("gen_blocks_helper: block (%s) flows to %d other blocks.\n",
		block_name, num_edges_in_cfg);

	switch (num_edges_in_cfg) {

		case 0: {

			if (!gen_block(c, block, nesting_level)) { return false; }

			add_line(c);
			gen_terminal(c, "return");
			c->seg->delim_prefix = DELIM_TAB;
			c->seg->delim_prefix_multiplier = nesting_level;

			ast_node* term = get_terminator(block);
			ast_node* val = get_val(term);
			if (!gen_val(c, val)) break;
			c->seg->delim_postfix = DELIM_NODELIM;
			gen_terminal(c, ";");
			c->seg->delim_postfix = DELIM_NEWLINE;
			return true;
		}

		case 1: {

			if (!gen_block(c, block, nesting_level)) { return false; }

			char* exit_name = cfg[origin][0];
			printf("gen_blocks_helper: flowing to %s.\n", exit_name);

			int exit_id = symbol_table_lookup(c->sym, exit_name)->block_number;

			if (!gen_blocks_helper(c, blocks, *(blocks + exit_id), cfg, c->sym, nesting_level)) {

				if (!c->file->err->contains_error) {
					c->file->err->contains_error = true;
					c->file->err->error_code = INVALID_BLOCK;
					c->file->err->error_token = *get_first_terminal(*(blocks + exit_id));
				}
				return false;
			}
			return true;
		}

		case 2: {

			if (!gen_block(c, block, nesting_level)) { return false; }

			char* exit1_name = cfg[origin][0];
			char* exit2_name = cfg[origin][1];
			printf("gen_blocks_helper: flowing to %s and %s.\n", exit1_name, exit2_name);

			/* Check what construct this is */
			control_flow_construct cfc = get_construct_type(cfg, origin, c->sym);

			switch (cfc) {

				case CONSTRUCT_IF_ELSE: {
					symbol_entry* sym_entry1 = symbol_table_lookup(c->sym, exit1_name);
					if (!sym_entry1) {
						printf("gen_blocks: failed to find first block.\n");
						if (!c->file->err->contains_error) {
							c->file->err->contains_error = true;
							c->file->err->error_code = INVALID_BLOCK;
							c->file->err->error_token = *get_first_terminal(block);
						}
						return false;
					}
					symbol_entry* sym_entry2 = symbol_table_lookup(c->sym, exit2_name);
					if (!sym_entry2) {
						printf("gen_blocks: failed to find second block.\n");
						if (!c->file->err->contains_error) {
							c->file->err->contains_error = true;
							c->file->err->error_code = INVALID_BLOCK;
							c->file->err->error_token = *get_first_terminal(block);
						}
						return false;
					}
					int exit1_id = sym_entry1->block_number;
					int exit2_id = sym_entry2->block_number;
					
					c->seg->delim_postfix = DELIM_NEWLINE;
					c->seg->delim_postfix_multiplier = 2;

					add_line(c);
					gen_terminal(c, "\n");
					gen_terminal(c, "if");
					c->seg->delim_prefix = DELIM_TAB;
					c->seg->delim_prefix_multiplier = nesting_level;
					gen_terminal(c, "(");

					ast_node* term = get_terminator(block);
					ast_node* val = get_val(term);
					if (!gen_val(c, val)) break;

					gen_terminal(c, ")");
					gen_terminal(c, "{");
					c->seg->delim_postfix = DELIM_NEWLINE;

					if (!gen_blocks_helper(c, blocks, *(blocks + exit1_id), cfg, nesting_level + 1)) {

						if (!c->file->err->contains_error) {
							c->file->err->contains_error = true;
							c->file->err->error_code = INVALID_BLOCK;
							c->file->err->error_token = *get_first_terminal(*(blocks + exit1_id));
						}
						return false;
					}
					add_line(c);
					gen_terminal(c, "}");
					c->seg->delim_prefix = DELIM_TAB;
					c->seg->delim_prefix_multiplier = nesting_level;
					c->seg->delim_postfix = DELIM_NEWLINE;

					//gen_terminal(c, "else");
					//gen_terminal(c, "{");

					//c->seg->delim_postfix = DELIM_NEWLINE;
					if (!gen_blocks_helper(c, blocks, *(blocks + exit2_id), cfg, nesting_level)) {

						if (!c->file->err->contains_error) {
							c->file->err->contains_error = true;
							c->file->err->error_code = INVALID_BLOCK;
							c->file->err->error_token = *get_first_terminal(*(blocks + exit2_id));
						}
						return false;
					}
					//gen_terminal(c, "}");
					/*
					c->seg->delim_postfix = DELIM_NEWLINE;
					c->seg->delim_prefix = DELIM_TAB;
					c->seg->delim_prefix_multiplier = nesting_level - 1;
					*/
					return true;
				}

				case CONSTRUCT_WHILE: {

					symbol_entry* sym_entry1 = symbol_table_lookup(c->sym, exit1_name);
					if (!sym_entry1) {
						printf("gen_blocks: failed to find first block.\n");
						if (!c->file->err->contains_error) {
							c->file->err->contains_error = true;
							c->file->err->error_code = INVALID_BLOCK;
							c->file->err->error_token = *get_first_terminal(block);
						}
						return false;
					}
					symbol_entry* sym_entry2 = symbol_table_lookup(c->sym, exit2_name);
					if (!sym_entry2) {
						printf("gen_blocks: failed to find second block.\n");
						if (!c->file->err->contains_error) {
							c->file->err->contains_error = true;
							c->file->err->error_code = INVALID_BLOCK;
							c->file->err->error_token = *get_first_terminal(block);
						}
						return false;
					}
					int exit1_id = sym_entry1->block_number;
					int exit2_id = sym_entry2->block_number;

					c->seg->delim_postfix = DELIM_NEWLINE;
					c->seg->delim_postfix_multiplier = 2;

					add_line(c);
					gen_terminal(c, "while");
					c->seg->delim_prefix = DELIM_TAB;
					c->seg->delim_prefix_multiplier = nesting_level;
					gen_terminal(c, "(");
					ast_node* term = get_terminator(block);
					ast_node* val = get_val(term);
					if (!gen_val(c, val)) break;
					gen_terminal(c, ")");
					gen_terminal(c, "{");
					c->seg->delim_postfix = DELIM_NEWLINE;

					if (DEBUG_INSERT_COMMENTS) {
						gen_terminal(c, "/* while body */");
						c->seg->delim_prefix = DELIM_TAB;
						c->seg->delim_prefix_multiplier = nesting_level + 1;
						c->seg->delim_postfix = DELIM_NEWLINE;
					}

					if (!gen_block(c, *(blocks + exit1_id), nesting_level + 1)) {

						if (!c->file->err->contains_error) {
							c->file->err->contains_error = true;
							c->file->err->error_code = INVALID_BLOCK;
							c->file->err->error_token = *get_first_terminal(*(blocks + exit1_id));
						}
						return false;
					}
					if (DEBUG_INSERT_COMMENTS) { 
						gen_terminal(c, "/* while condition */");
						c->seg->delim_prefix = DELIM_TAB;
						c->seg->delim_prefix_multiplier = nesting_level + 1;
						c->seg->delim_postfix = DELIM_NEWLINE;
					}
					if (!gen_block(c, block, nesting_level + 1)) { return false; }
					gen_terminal(c, "}");
					c->seg->delim_prefix = DELIM_TAB;
					c->seg->delim_prefix_multiplier = nesting_level;
					c->seg->delim_postfix = DELIM_NEWLINE;

					//gen_terminal(c, "else");
					//gen_terminal(c, "{");

					//c->seg->delim_postfix = DELIM_NEWLINE;
					if (!gen_blocks_helper(c, blocks, *(blocks + exit2_id), cfg, nesting_level)) {

						if (!c->file->err->contains_error) {
							c->file->err->contains_error = true;
							c->file->err->error_code = INVALID_BLOCK;
							c->file->err->error_token = *get_first_terminal(*(blocks + exit2_id));
						}
						return false;
					}
					return true;
				}

				case CONSTRUCT_FOR: {
					int for_cond_id = origin;

					char* for_body_name = cfg[for_cond_id][0];
					symbol_entry* for_body_entry = symbol_table_lookup(c->sym, for_body_name);
					int for_body_id = for_body_entry->block_number;

					char* for_end_name = cfg[for_cond_id][1];
					symbol_entry* for_end_entry = symbol_table_lookup(c->sym, for_end_name);
					int for_end_id = for_end_entry->block_number;

					char* for_inc_name = cfg[for_body_id][0];
					symbol_entry* for_inc_entry = symbol_table_lookup(c->sym, for_inc_name);
					int for_inc_id = for_inc_entry->block_number;
					
					if (DEBUG_INSERT_COMMENTS) {
						gen_terminal(c, "/* for cond */");
						c->seg->delim_prefix = DELIM_TAB;
						c->seg->delim_prefix_multiplier = nesting_level;
						c->seg->delim_postfix = DELIM_NEWLINE;
					}
					if (!gen_block(c, *(blocks + for_cond_id), nesting_level)) return false;

					gen_terminal(c, "for");
					c->seg->delim_prefix = DELIM_TAB;
					c->seg->delim_prefix_multiplier = nesting_level;
					gen_terminal(c, "(");
					gen_terminal(c, ";");
					ast_node* term1 = get_terminator(*(blocks + for_cond_id));
					ast_node* val1 = get_val(term1);
					if (!gen_val(c, val1)) break;
					gen_terminal(c, ";");
					gen_terminal(c, ")");
					gen_terminal(c, "{");
					c->seg->delim_postfix = DELIM_NEWLINE;

					if (DEBUG_INSERT_COMMENTS) {
						gen_terminal(c, "/* for body */");
						c->seg->delim_prefix = DELIM_TAB;
						c->seg->delim_prefix_multiplier = nesting_level + 1;
						c->seg->delim_postfix = DELIM_NEWLINE;
					}
					if (!gen_block(c, *(blocks + for_body_id), nesting_level + 1)) return false;

					if (DEBUG_INSERT_COMMENTS) {
						gen_terminal(c, "/* for inc */");
						c->seg->delim_prefix = DELIM_TAB;
						c->seg->delim_prefix_multiplier = nesting_level + 1;
						c->seg->delim_postfix = DELIM_NEWLINE;
					}
					if (!gen_block(c, *(blocks + for_inc_id), nesting_level + 1)) return false;

					if (DEBUG_INSERT_COMMENTS) {
						gen_terminal(c, "/* for cond */");
						c->seg->delim_prefix = DELIM_TAB;
						c->seg->delim_prefix_multiplier = nesting_level + 1;
						c->seg->delim_postfix = DELIM_NEWLINE;
					}
					if (!gen_block(c, *(blocks + for_cond_id), nesting_level + 1)) return false;

					gen_terminal(c, "}");
					c->seg->delim_postfix = DELIM_NEWLINE;
					c->seg->delim_prefix = DELIM_TAB;
					c->seg->delim_prefix_multiplier = nesting_level;
					if (!gen_blocks_helper(c, blocks, *(blocks + for_end_id), cfg, nesting_level)) return false;
					return true;
				}
			}

			case 3: {
				char* exit1_name = cfg[origin][0];
				char* exit2_name = cfg[origin][1];
				char* exit3_name = cfg[origin][2];
				int exit1_id = symbol_table_lookup(c->sym, exit1_name)->block_number;
				int exit2_id = symbol_table_lookup(c->sym, exit2_name)->block_number;
				int exit3_id = symbol_table_lookup(c->sym, exit3_name)->block_number;

				c->seg->delim_postfix = DELIM_NEWLINE;
				c->seg->delim_postfix_multiplier = 2;
				gen_terminal(c, "\n");
				gen_terminal(c, "if");
				c->seg->delim_prefix = DELIM_TAB;
				c->seg->delim_prefix_multiplier = nesting_level - 1;
				gen_terminal(c, "(");
				gen_terminal(c, ")");
				gen_terminal(c, "{");
				c->seg->delim_postfix = DELIM_NEWLINE;
				
				if (!gen_blocks_helper(c, blocks, *(blocks + exit1_id), cfg, nesting_level)) {

					if (!c->file->err->contains_error) {
						c->file->err->contains_error = true;
						c->file->err->error_code = INVALID_BLOCK;
						c->file->err->error_token = *get_first_terminal(*(blocks + exit1_id));
					}
					return false;
				}
				add_line(c);
				gen_terminal(c, "}");
				c->seg->delim_prefix = DELIM_TAB;
				c->seg->delim_prefix_multiplier = nesting_level - 1;
				gen_terminal(c, "else");
				
				gen_terminal(c, "if");
				gen_terminal(c, "(");

				ast_node* term = get_terminator(block);
				ast_node* val = get_val(term);
				gen_terminal(c, _strdup(val->child->terminal->lexeme));

				gen_terminal(c, ")");
				gen_terminal(c, "{");
				c->seg->delim_postfix = DELIM_NEWLINE;
				if (!gen_blocks_helper(c, blocks, *(blocks + exit2_id), cfg, nesting_level)) {

					if (!c->file->err->contains_error) {
						c->file->err->contains_error = true;
						c->file->err->error_code = INVALID_BLOCK;
						c->file->err->error_token = *get_first_terminal(*(blocks + exit2_id));
					}
					return false;
				}
				add_line(c);
				gen_terminal(c, "}");
				c->seg->delim_prefix = DELIM_TAB;
				c->seg->delim_prefix_multiplier = nesting_level - 1;
				gen_terminal(c, "else");
				gen_terminal(c, "{");
				c->seg->delim_postfix = DELIM_NEWLINE;
				if (!gen_blocks_helper(c, blocks, *(blocks + exit3_id), cfg, nesting_level)) {

					if (!c->file->err->contains_error) {
						c->file->err->contains_error = true;
						c->file->err->error_code = INVALID_BLOCK;
						c->file->err->error_token = *get_first_terminal(*(blocks + exit3_id));
					}
					return false;
				}
				gen_terminal(c, "}");
				c->seg->delim_postfix = DELIM_NEWLINE;
				return true;
			}

			default:
				if (!c->file->err->contains_error) {
					c->file->err->contains_error = true;
					c->file->err->error_code = INVALID_BLOCK;
					c->file->err->error_token = *get_first_terminal(block);
				}
				return false;
		}
		return false;
	}
}

bool gen_blocks(cursor* c, ast_node** blocks) {
	if (DEBUG_GEN) {
		printf("GENERATING BLOCK.\n");
	}

	for (ast_node** b = blocks; *b != NULL; b++) {
		char* name = (*b)->child->terminal->lexeme;

		int block_name_len = strlen(name) + 1;
		char* block_name = malloc(block_name_len);
		strcpy_s(block_name, block_name_len, name);
		block_name[block_name_len - 2] = NULL;
	
		int block_number = b - blocks;
		symbol_entry* entry = create_entry_for_block(block_name, block_number);
		insert_symbol(c->sym, entry);
	}

	char* cfg[MAX_NUMBER_BLOCKS_IN_PROD][MAX_NUMBER_BLOCKS_IN_PROD];
	fill_control_flow_graph(c->sym, blocks, cfg);
	print_control_flow_graph(cfg);

	int nesting_level = 1; 
	if (!gen_blocks_helper(c, blocks, *blocks, cfg, nesting_level)) {

		ast_node* block = *blocks;
		if (!c->file->err->contains_error) {
			c->file->err->contains_error = true;
			c->file->err->error_code = INVALID_BLOCK;
			c->file->err->error_token = *get_first_terminal(block);
		}
		return false;
	}
	print_symbol_table(c->sym);
	return true;
}

bool gen_prod(cursor *c, ast_node* prod) {
	if (DEBUG_GEN) {
		printf("GENERATING PROD.\n");
	}

	ast_node* child = get_child(prod);
	
	alternative alt = prod->alt;

	switch (alt) {
		
		case DECLARE_TYP_ID_TYP: {
			/* Collection */			
			child = skip_to_node(child, AST_TYP);
			ast_node* typ1 = child; /* Store TYP	*/
			child = skip_to_node(child, AST_IDENTIFIER);
			ast_node* id = child;	/* Store ID		*/
			child = skip_to_node(child, AST_TYP);
			ast_node* typ2 = child;	/* Store TYP	*/
			/* Generation */
			return;
		}

		case DECLARE_TYP_ID_TYP_ID:
			return;

		case DEFINE_ID_ARGS:
			return;

		case DEFINE_DEFDESC_TYP_ID_ARGS_BLOCKS: {
			/* Collection */			
			child = skip_to_node(child, AST_DEFDESCRIPTOR);
			ast_node* defdesc = child;  /* Store DEFDESC	*/		
			child = skip_to_node(child, AST_TYP);
			ast_node* typ = child;		/* Store TYP		*/
			child = skip_to_node(child, AST_IDENTIFIER);
			ast_node* id = child;		/* Store ID			*/			
			child = skip_to_node(child, AST_PARAMS);
			ast_node* params = child;	/* Store ARGS		*/
			child = skip_to_node(child, AST_BLOCKS);

			ast_node** blocks = malloc(
				sizeof(ast_node*) 
				* MAX_NUMBER_BLOCKS_IN_PROD);
			ast_node** block = blocks;
			*block++ = child;			/* Store BLOCKS		*/
			while (child = get_sibling(child)) {
				*block++ = child;
			}
			*block = NULL;

			/* Generation */
			add_block(c, construct_block());
			if (c->file->functions == NULL) { 
				c->file->functions = c->block; 
			}
			add_line(c);
			if (!gen_typ(c, typ)) break;
			if (!gen_id(c, id)) break;
			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_terminal(c, "(")) break;
			c->seg->delim_postfix = DELIM_NODELIM;
			if (!gen_params(c, params)) break;
			if (!gen_terminal(c, ")")) break;
			if (!gen_terminal(c, "{")) break;
			c->seg->delim_postfix = DELIM_NEWLINE;
			if (!gen_blocks(c, blocks)) break;
			if (!gen_terminal(c, "}")) break;
			return true;
		}
	} 
	if (!c->file->err->contains_error) {
		c->file->err->contains_error = true;
		c->file->err->error_code = INVALID_PROD;
		c->file->err->error_token = *get_first_terminal(prod);
	}
	return false;
}

bool gen_globals(cursor* c, ast_node* globals) {
	if (DEBUG_GEN) {
		printf("GENERATING GLOBALS.\n");
	}
	
	add_block(c, construct_block());
	c->file->globals = c->block;

	for (ast_node* child = get_child(globals);
		child && child->name == AST_GLOBAL;
		child = child->sibling) {

		switch (child->alt) {

			case ID_EQ_GLOBAL_TYP_CONST: {
				add_line(c);

				ast_node* an = child->child;
				while (an && (an->name != AST_IDENTIFIER)) { an = an->sibling; }
				ast_node* id = an;
				while (an && (an->name != AST_TYP)) { an = an->sibling; }
				ast_node* typ = an;

				ast_node* cstring = NULL;
				while (an && (an->name != AST_VAL)) { an = an->sibling; }
				cstring = an;

				gen_typ(c, typ);

				generated_segment *start_of_insert = NULL, *end_of_insert = NULL;

				if (typ->alt == OPEN_BRACKET_SZ_X_TYP_CLOSE_BRACKET) {

					start_of_insert = c->line->segment;
					char* start_of_insert_lexeme = start_of_insert->next_segment->repr;

					while (strcmp(start_of_insert_lexeme, "[") != 0) {
						start_of_insert = start_of_insert->next_segment;
						start_of_insert_lexeme = start_of_insert->repr;
					}
					start_of_insert->delim_postfix = DELIM_SPACE;
					end_of_insert = start_of_insert;
					while (strcmp(end_of_insert->repr, "]") != 0) {
						end_of_insert = end_of_insert->next_segment;
					}
				}

				if (!gen_id(c, id)) break;
				c->seg->delim_postfix = DELIM_NODELIM;
				
				if (typ->alt == OPEN_BRACKET_SZ_X_TYP_CLOSE_BRACKET) {
					c->seg->next_segment = start_of_insert->next_segment;
					start_of_insert->next_segment = end_of_insert->next_segment;
					c->seg = end_of_insert;
				}

				gen_terminal(c, _strdup("="));
				gen_val(c, cstring);
				gen_terminal(c, _strdup(";"));
				c->seg->delim_postfix = DELIM_NEWLINE;
				continue;
			}

			case ID_EQ_TYPE_TYP: {
				
				add_line(c);
				ast_node* id = get_nonterminal(AST_IDENTIFIER, child);
				ast_node* typ = get_nonterminal(AST_TYP, child);

				switch (typ->alt) {

					case OPEN_BRACKET_SZ_X_TYP_CLOSE_BRACKET: {
						gen_terminal(c, "typedef");
						gen_terminal(c, "struct");
						if (!gen_id(c, id)) return false;
						gen_terminal(c, "{");

						c->seg->delim_postfix = DELIM_NEWLINE;
						add_line(c);

						for (ast_node* attrs = typ->child;
							attrs != NULL;
							attrs = attrs->sibling) {

							if (attrs->name == AST_TYP) {
								if (!gen_typ(c, attrs)) return false;
								c->seg->delim_prefix = DELIM_TAB;
								c->seg->delim_postfix = DELIM_NODELIM;
								gen_terminal(c, ";");
								c->seg->delim_postfix = DELIM_NEWLINE;
								add_line(c);
							}
						}
						gen_terminal(c, "}");
						if (!gen_id(c, id)) return false;
						c->seg->delim_postfix = DELIM_NODELIM;
						gen_terminal(c, ";");
						c->seg->delim_postfix = DELIM_NEWLINE;
						add_line(c);
						continue;
					}

					default: {
						/* Recording */
						bool var_is_undeclared = false;
						bool var_is_primitive = false;

						symbol_entry* entry = create_entry_for_var(
							get_converted_id(id->child->terminal->lexeme),
							var_is_undeclared,
							var_is_primitive);
						insert_symbol(c->sym, entry);

						if (!gen_typ(c, typ)) return false;
						if (!gen_id(c, id)) return false;
						c->seg->delim_postfix = DELIM_NODELIM;
						gen_terminal(c, ";");
						c->seg->delim_postfix = DELIM_NEWLINE;
						add_line(c);
						continue;
					}
				}
			}

			case ID_EQ_TYPE_OPAQUE:
			case ID_EQ_COMDAT: { // [6 x i8] c"hello\00"

				ast_node* an = child->child;
				while (an && (an->name != AST_IDENTIFIER)) { an = an->sibling; }
				ast_node* id = an;
 				while (an && (an->name != AST_TYP)) { an = an->sibling; }
				ast_node* typ = an;

				ast_node* cstring = NULL;
				while (an && !(an->name == AST_TERMINAL && an->terminal->name == TOKEN_STRING)) { an = an->sibling; }
				cstring = an;
				gen_typ(c, typ);
				gen_id(c, id);
				gen_terminal(c, _strdup("="));
				if (cstring) gen_terminal(c, cstring);
				gen_terminal(c, _strdup(";"));
				add_line(c);
				continue;
			}

			case ID_EQ_COMDAT_ANY:
				continue;

			default: {
				printf("UNIMPLEMENTED");
				return false;
			}
		}
	}		
	return true;
}

bool gen_layout(cursor *c, ast_node* layout) {
	if (DEBUG_GEN) {
		printf("GENERATING LAYOUT.\n");
	}
	return 1;
}

bool gen_metadata(cursor* c, ast_node* layout) {
	if (DEBUG_GEN) {
		printf("GENERATING METADATA.\n");
	}
	return 1;
}

generated_file *gen_module(ast_node* module) {
	if (DEBUG_GEN) {
		printf("GENERATING MODULE.\n");
	}
	/* cursor doesn't point to anywhere yet */
	cursor* c = malloc(sizeof(cursor));
	c->block = NULL;
	c->file = NULL;
	c->line = NULL;
	c->seg = NULL;
	c->sym = construct_symbol_table();

	generated_file* file = malloc(sizeof(generated_file));
	file->functions = NULL;
	file->globals = NULL;
	file->includes = NULL;
	file->err = malloc(sizeof(error_info));
	file->err->contains_error = false;
	c->file = file;

	ast_node* child = get_child(module);

	if (node_has_name(child, AST_LAYOUT)) {
		if (!gen_layout(c, child)) return file;
		child = get_sibling(child);
	}

	if (node_has_name(child, AST_GLOBALS)) {
		if (!gen_globals(c, child)) return file;
		child = get_sibling(child);
	}

	while (node_has_name(child, AST_PROD)) {
		if (!gen_prod(c, child)) return file;
		c->seg->delim_postfix = DELIM_NEWLINE;
		c->seg->delim_postfix_multiplier = 2;
		child = get_sibling(child);
	}

	if (node_has_name(child, AST_METADATA)) {
		if (!gen_metadata(c, child)) return file;
		child = get_sibling(child);
	}
	return file;
}

char* output_delim(char* output, delim_type delim, char multiplier) {
	for (char i = 0; i < multiplier; i++) {
		switch (delim) {
			case DELIM_GENERIC: *output++ = ' '; break;
			case DELIM_SPACE: *output++ = ' '; break;
			case DELIM_NEWLINE: *output++ = '\n'; break;
			case DELIM_TAB: for (int i = 0; i < 4; i++) { *output++ = ' '; } break;
			case DELIM_NODELIM: break;
			default: break;
		}
	}
	return output;
}

/* functions responsible for generating target code */

char* output_target_from_segment(generated_segment* segment, char* output) {
	delim_type prefix = segment->delim_prefix;
	char multiplier = segment->delim_prefix_multiplier;
	output = output_delim(output, prefix, multiplier);
	for (char* r = segment->repr; *r != NULL; r++, output++ ) {
		*output = *r;
	}
	delim_type postfix = segment->delim_postfix;
	output = output_delim(output, postfix, 1);
	return output;
}

char* output_target_from_line(generated_line* line, char* output) {
	for (generated_segment* seg = line->segment;
		 seg != NULL; 
		 seg = seg->next_segment) {
		output = output_target_from_segment(seg, output);
	}
	return output;
}

char* output_target_from_block(generated_block* block, char* output) {
	for (generated_line* line = block->line;
		 line != NULL;
		 line = line->next_line) {
		output = output_target_from_line(line, output);
	}
	return output;
}

void output_target_from_file(generated_file* file, char* output) {

	for (generated_block* block = file->includes;
		block != NULL;
		block = block->next_block) {
		output = output_target_from_block(block, output);
	}
	for (generated_block* block = file->globals;
		block != NULL;
		block = block->next_block) {
		output = output_target_from_block(block, output);
	}
	for (generated_block* block = file->functions;
		block != NULL;
		block = block->next_block) {
		output = output_target_from_block(block, output);
	}
	*output = NULL;
}