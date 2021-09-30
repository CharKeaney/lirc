/* lexer.c - Peforms lexical analysis on given input after preprocessing */

#pragma once

#include "lexer.h"
#include "preprocessor.h"

/**
* Responsible for creating a token struct from given data.
* name means the internal name e.g. TOKEN_ADD.
* lexeme means the word that made the token, e.g. "%myvar"
* value means the value behind the token, e.g. 32
* 
* @param name The name of the new token
* @param lexeme The lexeme (text) behind the new token
* @param val The value of the new token (not neccessarily meaningful)
*/
struct Token* make_token(
	token_name name, preprocessed_word word, value val) {
	token *tk = malloc(sizeof(token));
	// Copying Name.
	tk->name = name;
	// Copying lexeme.
	tk->lexeme = word.word;;
	// Copying value.
	if (name == TOKEN_FLOAT_LITERAL) { 
		tk->val.fval = val.fval;
	} else { 
		tk->val.ival = val.ival;
	}
	// Copying position
	tk->pos = word.pos;
	return tk;
}

/**
* Responsible for making final decision on if an input corresponds
* to a token. If the inputs match then it returns the given result
* state, otherwise it returns the error state.
* 
* @param input The input word which should be matched into a token.
* @param expected The word we expect the input to be for us to go to state
* @param result The result state we go to if we see a match.
*/
state match(char* word, char* expected, state result) {
	if (strcmp(word, expected) == 0) {
		return result;
	} else {
		return STATE_UNRECOGNISED;
	}
}

/**
* Responsible for returning a pointer to an appropriately constructed
* token from a given preprocessed word. 
* 
* @param input The input to tokenize.
*/
struct Token *tokenize(preprocessed_word word) {
	char* lexeme = word.word; /* ci represents the currently considered char */
	value val; val.ival = 0;
	state current_state = BEGIN_STATE;
	while (current_state != END_STATE) {
		if (DEBUG_TOKENIZE) { 
			printf("CURRENT STATE: %d\n", current_state);
			printf("WORD REMAINING: %s\n", lexeme);
		}
		switch (current_state) {

			case BEGIN_STATE:
				switch (*lexeme++) {
					case 'a':
						current_state = STATE_A;
						continue;
					case 'b':
						current_state = STATE_B;
						continue;
					case 'c':
						current_state = STATE_C;
						continue;
					case 'd':
						current_state = STATE_D;
						continue;
					case 'e':
						current_state = STATE_E;
						continue;
					case 'f':
						current_state = STATE_F;
						continue;
					case 'g':
						current_state = STATE_G;
						continue;
					case 'i':
						current_state = STATE_I;
						continue;
					case 'l':
						current_state = STATE_L;
						continue;
					case 'm':
						current_state = STATE_M;
						continue;
					case 'n':
						current_state = STATE_N;
						continue;
					case 'o':
						current_state = STATE_O;
						continue;
					case 'p':
						current_state = STATE_P;
						continue;
					case 'r':
						if (*lexeme++ == 'e') {
							current_state = STATE_RE;
							continue;
						}
						current_state = STATE_UNRECOGNISED;
						continue;
					case 's':
						current_state = STATE_S;
						continue;
					case 't':
						current_state = STATE_T;
						continue;
					case 'u':
						current_state = STATE_U;
						continue;
					case 'v':
						current_state = STATE_V;
						continue;
					case 'w':
						current_state = STATE_W;
						continue;
					case 'x':
						if (*lexeme == NULL) {
							current_state = STATE_X;
							continue;
						} 
						current_state = match(lexeme, "or", STATE_XOR);
						continue;
					case 'z':
						if (*lexeme++ == 'e') {
							current_state = STATE_ZE;
							continue;
						}
						else {
							current_state = STATE_UNRECOGNISED;
							continue;
						}
					case '%':
						current_state = STATE_IDENTIFIER;
						continue;
					case '-':
						current_state = STATE_MINUS;
						continue;
					case '@':
						current_state = STATE_IDENTIFIER;
						continue;
					case '#':
						current_state = STATE_IDENTIFIER;
						continue;
					case '$':
						current_state = STATE_IDENTIFIER;
						continue;
					case '=':
						current_state = match(lexeme, "", STATE_EQUALS);
						continue;
					case '(':
						current_state = match(lexeme, "", STATE_OPEN_PAREN);
						continue;
					case ')':
						current_state = match(lexeme, "", STATE_CLOSE_PAREN);
						continue;
					case '[':
						current_state = match(lexeme, "", STATE_OPEN_BRACKET);
						continue;
					case ']':
						current_state = match(lexeme, "", STATE_CLOSE_BRACKET);
						continue;
					case '{':
						current_state = match(
							lexeme, "", STATE_OPEN_CURLY_BRACKET);
						continue;
					case '}':
						current_state = match(
							lexeme, "", STATE_CLOSE_CURLY_BRACKET);
						continue;
					case '<':
						current_state = match(
							lexeme, "", STATE_OPEN_ANGLE_BRACKET);
						continue;
					case '>':
						current_state = match(
							lexeme, "", STATE_CLOSE_ANGLE_BRACKET);
						continue;
					case ',':
						current_state = match(lexeme, "", STATE_COMMA);
						continue;
					case '.':
						current_state = match(lexeme, "..", STATE_DOTDOTDOT);
						continue;
					case '!':
						current_state = match(lexeme, "", STATE_EXCLAMATION_MARK);
						continue;
					case '"':
						while (*lexeme++ != '"') {
							if (*lexeme == NULL) {
								current_state = STATE_UNRECOGNISED;
								continue;
							}
						}
						if (*lexeme == NULL) {
							current_state = STATE_UNRECOGNISED;
						}
						current_state = STATE_STRING;
						continue;
					case '*':
						current_state = match(lexeme, "", STATE_PTR);
						continue;
					case '0': 
						if (*lexeme == 'x') {
							lexeme++;
							current_state = STATE_OX;
							continue;
						}
					case '1': case '2': case '3': 
					case '4': case '5': case '6': 
					case '7': case '8': case '9': 
						current_state = STATE_DIGITS;
						continue;
					case '\0':
						return make_token(TOKEN_EOF, word, val);
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
					
			}
			break;

			case STATE_EX:
				switch (*lexeme++) {
					case 't':
						current_state = match(
							lexeme, "ractvalue", STATE_EXTRACTVALUE);
						continue;
					case 'a':
						current_state = match(lexeme, "ct", STATE_EXACT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				break;

			case STATE_ERROR_ENCOUNTERED:	
				return make_token(ERROR_TOKEN, word, val);
			case STATE_EXACT: 
				return make_token(TOKEN_EXACT, word, val);
			case STATE_EXCLAMATION_MARK:
				return make_token(TOKEN_EXCLAMATION_MARK, word, val);
			case STATE_UGT: 
				return make_token(TOKEN_UGT, word, val);

			case STATE_UL: 
				switch (*lexeme++) {
					case 't':
						current_state = match(lexeme, "", STATE_ULT);
						continue;
					case 'e':
						current_state = match(lexeme, "", STATE_ULE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				break;
			
			case STATE_ULT: 
				return make_token(TOKEN_ULT, word, val);
			case STATE_ULE: 
				return make_token(TOKEN_ULE, word, val);

			case STATE_UNRECOGNISED: 
				; char* c = word.word
				; bool is_label = true;
				// Check if it is a label
				for (; *(c + 1) != NULL; c++) {
					if (!isdigit(*c) && !isalpha(*c) && !ispunct(*c)) {
						is_label = false; break; 
					}
				}
				if (is_label && *c == ':') { 
					current_state = STATE_BLOCK_LABEL; 
					continue; 
				}
				// If we get here it is not.
				current_state = STATE_ERROR_ENCOUNTERED;
				continue;

			case STATE_A: 
				switch (*lexeme++) {
					case 'd':
						current_state = match(lexeme, "d", STATE_ADD);
						continue;
					case 'g':
						current_state = match(lexeme, "gr", STATE_AGGR);
						continue;
					case 'l':
						current_state = STATE_AL;
						continue;
					case 'n':
						current_state = STATE_AN;
						continue;
					case 'r':
						current_state = match(lexeme, "gmemonly", STATE_ARGMEMONLY);
						continue;
					case 's':
						current_state = STATE_AS;
						continue;
					case 't':
						current_state = match(lexeme, "tributes", STATE_ATTRIBUTES);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
				}
				break;

			case STATE_ADD: 
				return make_token(TOKEN_ADD, word, val);
			case STATE_AGGR: 
				return make_token(TOKEN_AGGR, word, val);

			case STATE_AL:
				switch (*lexeme++) {
					case 'i':
						current_state = match(lexeme, "gn", STATE_ALIGN);
						continue;
					case 'l':
						current_state = match(lexeme, "oca", STATE_ALLOCA);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_ALIGN: 
				return make_token(TOKEN_ALIGN, word, val);
			case STATE_ALLOCA: 
				return make_token(TOKEN_ALLOCA, word, val);

			case STATE_AN:
				switch (*lexeme++) {
					case 'd':
						current_state = match(lexeme, "", STATE_AND);
						continue;
					case 'y':
						current_state = match(lexeme, "", STATE_ANY);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				;}
			case STATE_ANY:
				return make_token(TOKEN_ANY, word, val);
			case STATE_AND: 
				return make_token(TOKEN_AND, word, val);

			case STATE_AS:
				switch (*lexeme++) {
					case 'm':
						current_state = match(lexeme, "", STATE_ASM);
						continue;
					case 'h':
						current_state = match(lexeme, "r", STATE_ASHR);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_ASM: 
				return make_token(TOKEN_ASM, word, val);
			case STATE_ATTRIBUTES:
				return make_token(TOKEN_ATTRIBUTES, word, val);
			case STATE_ARGMEMONLY:
				return make_token(TOKEN_ARGMEMONLY, word, val);
			case STATE_ASHR: 
				return make_token(TOKEN_ASHR, word, val);

			case STATE_B: 
				switch (*lexeme++) {
					case 'i': 
						current_state = STATE_BI;
						continue;
					case 'r': 
						current_state = STATE_BR;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				break;

			case STATE_BI: 
				switch (*lexeme++) {
					case 'g':
						current_state = match(
							lexeme, "endian", STATE_BIGENDIAN);
						continue;
					case 't':
						current_state = match(
							lexeme, "cast", STATE_BITCAST);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_BIGENDIAN: 
				return make_token(TOKEN_BIGENDIAN, word, val);
			case STATE_BITCAST: 
				return make_token(TOKEN_BITCAST, word, val);
			case STATE_BR: 
				return make_token(TOKEN_BR, word, val);

			case STATE_C:
				switch (*lexeme++) {
					case 'a':
						current_state = match(lexeme, "ll", STATE_CALL);
						continue;
					case 'l':
						current_state = match(lexeme, "eanup", STATE_CLEANUP);
						continue;
					case 'm':
						current_state = match(lexeme, "pxchg", STATE_CMPXCHG);
						continue;
					case 'o':
						current_state = STATE_CO;
						continue;
					case '"':
						while (*lexeme++ != '"') {
							if (*lexeme == NULL) {
								current_state = STATE_ERROR_ENCOUNTERED;
								continue;
							}
						}
						current_state = STATE_CSTRING;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_CALL: 
				return make_token(TOKEN_CALL, word, val);
			case STATE_CLEANUP: 
				return make_token(TOKEN_CLEANUP, word, val);
			case STATE_CLOSE_ANGLE_BRACKET: 
				return make_token(TOKEN_CLOSE_ANGLE_BRACKET, word, val);
			case STATE_CLOSE_CURLY_BRACKET: 
				return make_token(TOKEN_CLOSE_CURLY_BRACKET, word, val);
			case STATE_CLOSE_BRACKET: 
				return make_token(TOKEN_CLOSE_BRACKET, word, val);
			case STATE_CLOSE_PAREN: 
				return make_token(TOKEN_CLOSE_PAREN, word, val);
			case STATE_CMPXCHG: 
				return make_token(TOKEN_CMPXCHG, word, val);

			case STATE_CO:
				switch (*lexeme++) {
					case 'n':
						current_state = match(lexeme, "stant", STATE_CONSTANT);
						continue;
					case 'm':
						current_state = match(lexeme, "dat", STATE_COMDAT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_CONSTANT:
				return make_token(TOKEN_CONSTANT, word, val);
			case STATE_COMDAT: 
				return make_token(TOKEN_COMDAT, word, val);
			case STATE_COMMA: 
				return make_token(TOKEN_COMMA, word, val);

			case STATE_D: 
				switch (*lexeme++) {
					case 'a':
						current_state = match(
							lexeme, "talayout", STATE_DATALAYOUT);
						continue;
					case 'e':
						current_state = STATE_DE;
						continue;
					case 'l':
						current_state = match(
							lexeme, "limport", STATE_DLLIMPORT);
						continue;
					case 'o':
						current_state = match(
							lexeme, "uble", STATE_DOUBLE);
						continue;
					case 's':
						current_state = match(
							lexeme, "o_local", STATE_DSO_LOCAL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				};

			case STATE_CSTRING:
				return make_token(TOKEN_CSTRING, word, val);

			case STATE_DATALAYOUT:
				return make_token(TOKEN_DATALAYOUT, word, val);

			case STATE_DE: 
				switch (*lexeme++) {
					case 'c':
						current_state = match(lexeme, "lare", STATE_DECLARE);
						continue;
					case 'f':
						current_state = match(lexeme, "ine", STATE_DEFINE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_DECLARE: 
				return make_token(TOKEN_DECLARE, word, val);
			case STATE_DEFINE:
				return make_token(TOKEN_DEFINE, word, val);
			case STATE_DLLIMPORT: 
				return make_token(TOKEN_DLLIMPORT, word, val);
			case STATE_DOTDOTDOT: 
				return make_token(TOKEN_DOTDOTDOT, word, val);
			case STATE_DOUBLE: 
				return make_token(TOKEN_DOUBLE, word, val);
			case STATE_DSO_LOCAL: 
				return make_token(TOKEN_DSO_LOCAL, word, val);
			
			case STATE_E:
				switch (*lexeme++) {
					case 'q':
						current_state = STATE_EQ;
						continue;
					case 'x':
						current_state = STATE_EX;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_EQ: 
				return make_token(TOKEN_EQ, word, val);
			case STATE_EQUALS: 
				return make_token(TOKEN_EQUALS, word, val);

			case STATE_F: 
				switch (*lexeme++) {
					case 'a': 
						current_state = STATE_FA;
						continue;
					case 'c':
						current_state = match(
							lexeme, "mp", STATE_FCMP);
						continue;
					case 'd':
						current_state = match(
							lexeme, "iv", STATE_FDIV);
						continue;
					case 'e':
						current_state = match(
							lexeme, "nce", STATE_FENCE);
						continue;
					case 'l':
						current_state = match(
							lexeme, "oat", STATE_FLOAT);
						continue;
					case 'm':
						current_state = match(
							lexeme, "ul", STATE_FMUL);
						continue;
					case 'n':
						current_state = match(
							lexeme, "eg", STATE_FNEG);
						continue;
					case 'p':
						current_state = STATE_FP;
						continue;
					case 'r':
						switch (*lexeme++) {
							case 'e':
								current_state = STATE_FRE;
								continue;
							default:
								current_state = STATE_UNRECOGNISED;
								continue;
						}
					case 's':
						current_state = match(lexeme, "ub", STATE_FSUB);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_FA:
				switch (*lexeme++) {
					case 'd':
						current_state = match(lexeme, "d", STATE_FADD);
						continue;
					case 'l':
						current_state = match(lexeme, "se", STATE_FALSE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				current_state = STATE_UNRECOGNISED;
				continue;

			case STATE_FALSE: 
				return make_token(TOKEN_FALSE, word, val);
			case STATE_FADD:
				return make_token(TOKEN_FADD, word, val);
			case STATE_FCMP: 
				return make_token(TOKEN_FCMP, word, val);
			case STATE_FDIV: 
				return make_token(TOKEN_FDIV, word, val);
			case STATE_FENCE: 
				return make_token(TOKEN_FENCE, word, val);
			case STATE_FLOAT: 
				return make_token(TOKEN_FLOAT, word, val);
			case STATE_FMUL: 
				return make_token(TOKEN_FMUL, word, val);
			case STATE_FNEG: 
				return make_token(TOKEN_FNEG, word, val);

			case STATE_FP:
				switch (*lexeme++) {
					case 'e':
						current_state = match(
							lexeme, "xt", STATE_FPEXT);
						continue;
					case 't':
						if (*(lexeme++) == 'o') {
							current_state = STATE_FPTO;
						}
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_FPEXT: 
				return  make_token(TOKEN_FPEXT, word, val);
				
			case STATE_FPTO:
				switch (*lexeme++) {
					case 'u':
						current_state = match(lexeme, "i", STATE_FPTOUI);
						continue;
					case 's':
						current_state = match(lexeme, "i", STATE_FPTOSI);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
						
				}
				return NULL;

			case STATE_FPTOUI: 
				return make_token(TOKEN_FPTOUI, word, val);
			case STATE_FPTOSI: 
				return make_token(TOKEN_FPTOSI, word, val);

			case STATE_FRE: 
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "", STATE_FREE);
						continue;
					case 'm':
						current_state = match(lexeme, "", STATE_FREM);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_FREE: 
				return make_token(TOKEN_FREE, word, val);
			case STATE_FREM: 
				return make_token(TOKEN_FREM, word, val);
			case STATE_FSUB: 
				return make_token(TOKEN_FSUB, word, val);

			case STATE_G: 
				switch (*lexeme++) {
					case 'e':
						current_state = match(
							lexeme, "telementptr", STATE_GETELEMENTPTR);
						continue;
					case 'l':
						current_state = match(
							lexeme, "obal", STATE_GLOBAL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_GETELEMENTPTR: 
				return make_token(TOKEN_GETELEMENTPTR, word, val);
			case STATE_GLOBAL: 
				return make_token(TOKEN_GLOBAL, word, val);

			case STATE_I: 
				switch (*lexeme++) {
					case 'c':
						current_state = match(
							lexeme, "mp", STATE_ICMP);
						continue;
					case 'm':
						current_state = match(
							lexeme, "marg", STATE_IMMARG);
						continue;
					case 'n':
						current_state = STATE_IN;
						continue;
					case '0': 
					case '1': case '2': case '3':
					case '4': case '5': case '6': 
					case '7': case '8': case '9':
						current_state = STATE_INTEGERTYPE;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;;
						continue;
				}

			case STATE_IN:
				switch (*lexeme++) {
					case 'b':
						current_state = match(lexeme, "ounds", STATE_INBOUNDS);
						continue;
					case 't':
						current_state = STATE_INT;
						continue;
					case 'v':
						current_state = match(lexeme, "oke", STATE_INVOKE);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_ICMP: 
				return make_token(TOKEN_ICMP, word, val);
			case STATE_IDENTIFIER: 
				return make_token(TOKEN_IDENTIFIER, word, val);
			case STATE_IMMARG: 
				return make_token(TOKEN_IMMARG, word, val);
			case STATE_INBOUNDS:  
				return make_token(TOKEN_INBOUNDS, word, val);
			case STATE_INVOKE: 
				return make_token(TOKEN_INVOKE, word, val);

			case STATE_INTEGERTYPE:
				switch (*lexeme++) {
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						current_state = STATE_INTEGERTYPE;
						continue;
					case NULL:
						val.ival = atoi(word.word + 1);
						return make_token(
							TOKEN_INTEGERTYPE, word, val);
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_INT:
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "rnal", STATE_INTERNAL);
						continue;
					case 't':
						current_state = match(lexeme, "optr", STATE_INTTOPTR);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_INTTOPTR: 
				return make_token(TOKEN_INTTOPTR, word, val);
			case STATE_INTERNAL: 
				return make_token(TOKEN_INTERNAL, word, val);

			case STATE_EXTRACTVALUE: 
				return make_token(TOKEN_EXTRACTVALUE, word, val);

			case STATE_L: 
				switch (*lexeme++) {
					case 'a':
						current_state = STATE_LA;
						continue;
					case 'i':
						current_state = STATE_LI;
						continue;
					case 'l':
						if (*lexeme++ == 'v'
							&& *lexeme++ =='m'
							&& *lexeme++ == '.') {
							current_state = STATE_LLVM_;
							continue;
						}
						current_state = STATE_UNRECOGNISED;
					case 'o':
						current_state = match(
							lexeme, "ad", STATE_LOAD);
						continue;
					case 's':
						current_state = match(
							lexeme, "hr", STATE_LSHR);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_LA:
				switch (*lexeme++) {
					case 'n':
						current_state = match(lexeme, "dingpad", STATE_LANDINGPAD);
						continue;
					case 'b':
						current_state = match(lexeme, "el", STATE_LABEL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_LI:
				switch (*lexeme++) {
					case 'n':
						current_state = match(
							lexeme, "konce_odr", STATE_LINKONCE_ODR);
						continue;
					case 't':
						current_state = match(
							lexeme, "tleendian", STATE_LITTLEENDIAN);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_LINKONCE_ODR:
				return make_token(TOKEN_LINKONCE_ODR, word, val);

			case STATE_LLVM_:
				switch (*lexeme++) {
					case 'i':
						current_state = match(lexeme, "dent", STATE_LLVM_IDENT);
						continue;
					case 'm':
						current_state = match(lexeme, "odule.flags", STATE_LLVM_MODULE_FLAGS);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_LLVM_IDENT:
				return make_token(TOKEN_LLVM_IDENT, word, val);
			case STATE_LLVM_MODULE_FLAGS:
				return make_token(TOKEN_LLVM_MODULE_FLAGS, word, val);

			case STATE_BLOCK_LABEL: 
				return make_token(TOKEN_BLOCKLABEL, word, val);
			case STATE_LITTLEENDIAN: 
				return make_token(TOKEN_LITTLEENDIAN, word, val);
			case STATE_LANDINGPAD: 
				return make_token(TOKEN_LANDINGPAD, word, val);
			case STATE_LABEL:
				return make_token(TOKEN_LABEL, word, val);
			case STATE_LOAD: 
				return make_token(TOKEN_LOAD, word, val);
			case STATE_LSHR: 
				return make_token(TOKEN_LSHR, word, val);

			case STATE_M: 
				switch (*lexeme++) {
					case 'a':
						current_state = match(
							lexeme, "lloc", STATE_MALLOC);
						continue;
					case 'u':
						current_state = match(
							lexeme, "l", STATE_MUL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_MALLOC: 
				return make_token(TOKEN_MALLOC, word, val);

			case STATE_MINUS: 
				switch (*lexeme++) {
					case '0':
					case '1': case '2': case '3': 
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						current_state = STATE_DIGITS;
						continue;
				} 
				return NULL;

			case STATE_MUL: return make_token(TOKEN_MUL, word, val);

			case STATE_N: 
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "", STATE_NE);
						continue;
					case 'o':
						current_state = STATE_NO;
						continue;
					case 's':
						current_state = match(lexeme, "w", STATE_NSW);
						continue;
					case 'u':
						current_state = STATE_NU;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_NE: 
				return make_token(TOKEN_NE, word, val);

			case STATE_NU:
				switch (*lexeme++) {
					case 'w':
						current_state = STATE_NUW;
						continue;
					case 'l':
						current_state = match(lexeme, "l", STATE_NULL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				

			case STATE_NO:
				switch (*lexeme++) {	
					case 'a':
						current_state = match(lexeme, "lias", STATE_NOALIAS);
						continue;
					case 'b':
						current_state = match(lexeme, "uiltin", STATE_NOBUILTIN);
						continue;
					case 'c':
						current_state = match(lexeme, "apture", STATE_NOCAPTURE);
						continue;
					case 'i':
						current_state = match(lexeme, "nline", STATE_NOINLINE);
						continue;
					case 'u':
						current_state = match(lexeme, "nwind", STATE_NOUNWIND);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
			
			case STATE_NOALIAS: 
				return make_token(TOKEN_NOALIAS, word, val);
			case STATE_NOBUILTIN:
				return make_token(TOKEN_NOBUILTIN, word, val);
			case STATE_NOCAPTURE: 
				return make_token(TOKEN_NOCAPTURE, word, val);
			case STATE_NOINLINE:
				return make_token(TOKEN_NOINLINE, word, val);
			case STATE_NOUNWIND:
				return make_token(TOKEN_NOUNWIND, word, val);
			case STATE_NUW: 
				return make_token(TOKEN_NUW, word, val);
			case STATE_NSW: 
				return make_token(TOKEN_NSW, word, val);
			case STATE_NULL: 
				return make_token(TOKEN_NULL, word, val);

			case STATE_O:
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "q", STATE_OEQ);
						continue;
					case 'l':
						current_state = STATE_OL;
						continue;
					case 'p':
						current_state = STATE_OP;
						continue;
					case 'r':
						current_state = STATE_OR;
						continue;
					case 'g':
						current_state = STATE_OG;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_OEQ: 
				return make_token(TOKEN_OEQ, word, val);

			case STATE_OG:
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "", STATE_OGE);;
						continue;
					case 't':
						current_state = match(lexeme, "", STATE_OGT);;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_OL:
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "", STATE_OLE);;
						continue;
					case 't':
						current_state = match(lexeme, "", STATE_OLT);;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_OP:
				switch (*lexeme++) {
					case 'a':
						current_state = match(lexeme, "que", STATE_OPAQUE);
						continue;
					case 't':
						current_state = match(lexeme, "none", STATE_OPTNONE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_OPAQUE:
				return make_token(TOKEN_OPAQUE, word, val);
			case STATE_OPTNONE:
				return make_token(TOKEN_OPTNONE, word, val);
			case STATE_OPEN_ANGLE_BRACKET: 
				return make_token(TOKEN_OPEN_ANGLE_BRACKET, word, val);
			case STATE_OPEN_CURLY_BRACKET: 
				return make_token(TOKEN_OPEN_CURLY_BRACKET, word, val);
			case STATE_OPEN_BRACKET: 
				return make_token(TOKEN_OPEN_BRACKET, word, val);
			case STATE_OPEN_PAREN: 
				return make_token(TOKEN_OPEN_PAREN, word, val);
			case STATE_OGE: 
				return make_token(TOKEN_OGE, word, val);
			case STATE_OGT: 
				return make_token(TOKEN_OGT, word, val);
			case STATE_OLE: 
				return make_token(TOKEN_OLE, word, val);
			case STATE_OLT: 
				return make_token(TOKEN_OLT, word, val);
			case STATE_OR: 
				return make_token(TOKEN_OR, word, val);

			case STATE_OX:
				switch (*lexeme++) {
					case '0': 
					case '1': case '2': case '3': 
					case '4': case '5': case '6': 
					case '7': case '8': case '9':
					case 'A': case 'B': case 'C': 
					case 'D': case 'E': case 'F':
						current_state = STATE_OX_DIGITS;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_OX_DIGITS:
				switch (*lexeme++) {
					case '0': case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9':
					case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
						continue;
					case NULL:
						val.ival = strtol(word.word, NULL, 0);
						return make_token(TOKEN_SZ, word, val);
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_P: 
				switch (*lexeme++) {
					case 'e':
						current_state = match(
							lexeme, "rsonality", STATE_PERSONALITY);
						continue;
					case 'h':
						current_state = match(
							lexeme, "i", STATE_PHI);
						continue;
					case 'r':
						current_state = match(
							lexeme, "ivate", STATE_PRIVATE
						);
						continue;
					case 't':
						current_state = match(
							lexeme, "rtoint", STATE_PTRTOINT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				
			case STATE_PERSONALITY: 
				return make_token(TOKEN_PERSONALITY, word, val);
			case STATE_PHI: 
				return make_token(TOKEN_PHI, word, val);
			case STATE_PRIVATE:
				return make_token(TOKEN_PRIVATE, word, val);
			case STATE_PTR: 
				return make_token(TOKEN_PTR, word, val);
			case STATE_PTRTOINT: 
				return make_token(TOKEN_PTRTOINT, word, val);

			case STATE_RE:
				switch (*lexeme++) {
					case 't':
						current_state = match(lexeme, "", STATE_RET);
						continue;
					case 'a':
						if (*lexeme++ == 'd') {
							current_state = STATE_READ;
							continue;
						}
						current_state = STATE_UNRECOGNISED;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_RET: 
				return make_token(TOKEN_RET, word, val);

			case STATE_READ:
				switch (*lexeme++) {
					case 'n':
						current_state = match(lexeme, "one", STATE_READNONE);
						continue;
					case 'o':
						current_state = match(lexeme, "nly", STATE_READONLY);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_READNONE:
				return make_token(TOKEN_READNONE, word, val);
			case STATE_READONLY: 
				return make_token(TOKEN_READONLY, word, val);

			case STATE_S: 
				switch (*lexeme++) {
					case 'd':
						current_state = match(lexeme, "iv", STATE_SDIV);
						continue;
					case 'e':
						current_state = STATE_SE;
						continue;
					case 'i':
						current_state = match(lexeme, "tofp", STATE_SITOFP);
						continue;
					case 'o':
						current_state = match(lexeme, "urce_filename", STATE_SOURCE_FILENAME);
						continue;
					case 'g':
						current_state = STATE_SG;
						continue;
					case 'h':
						current_state = match(lexeme, "l", STATE_SHL);
						continue;
					case 'l':
						current_state = STATE_SL;
						continue;
					case 'p':
						current_state = match(lexeme, "eculatable", STATE_SPECULATABLE);
						continue;
					case 'r':
						if (*lexeme++ == 'e') {
							current_state = STATE_SRE;
							continue;
						}
						current_state = STATE_UNRECOGNISED;
						continue;
						
					case 't':
						current_state = STATE_ST;
						continue;
					case 'u':
						current_state = match(lexeme, "b", STATE_SUB);
						continue;
					case 'w':
						current_state = match(lexeme, "itch", STATE_SWITCH);
						continue;
					case 'y':
						current_state = match(lexeme, "ncscope", STATE_SYNCSCOPE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
			
			case STATE_SITOFP: 
				return make_token(TOKEN_SITOFP, word, val);

			case STATE_SL:
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "", STATE_SLE);
						continue;
					case 't':
						current_state = match(lexeme, "", STATE_SLT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_SDIV: 
				return make_token(TOKEN_SDIV, word, val);

			case STATE_SE: 
				switch (*lexeme++) {
					case 'l':
						current_state = match(lexeme, "ect", STATE_SELECT);
						continue;
					case 'x':
						current_state = match(lexeme, "t", STATE_SEXT);
						continue;
					case 'q':
						current_state = match(lexeme, "_cst", STATE_SEQ_CST);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				};

			case STATE_SELECT: 
				return make_token(TOKEN_SELECT, word, val);
			case STATE_SEQ_CST: 
				return make_token(TOKEN_SEQ_CST, word, val);
			case STATE_SEXT: 
				return make_token(TOKEN_SEXT, word, val);

			case STATE_SG:
				switch (*lexeme++) {
				case 't':
					current_state = STATE_SGT;
					continue;
				case 'e': 
					current_state = STATE_SGE;
					continue;
				default:
					current_state = STATE_UNRECOGNISED;
					continue;
				}
				break;

			case STATE_SGT: 
				return make_token(TOKEN_SGT, word, val);
			case STATE_SGE: 
				return make_token(TOKEN_SGE, word, val);
			case STATE_SHL: 
				return make_token(TOKEN_SHL, word, val);
			case STATE_SLT: 
				return make_token(TOKEN_SLT, word, val);
			case STATE_SLE: 
				return make_token(TOKEN_SLE, word, val);
			case STATE_SOURCE_FILENAME:
				return make_token(TOKEN_SOURCE_FILENAME, word, val);
			case STATE_SPECULATABLE:
				return make_token(TOKEN_SPECULATABLE, word, val);
			case STATE_SRE:
				switch (*lexeme++) {
					case 't':
						current_state = match(lexeme, "", STATE_SRET);
						continue;
					case 'm':
						current_state = match(lexeme, "", STATE_SREM);
						continue;
				default:
					current_state = STATE_UNRECOGNISED;
					continue;
				}
				return NULL;
				

		    case STATE_SREM: 
				return make_token(TOKEN_SREM, word, val);

			case STATE_SRET:
				return make_token(TOKEN_SRET, word, val);

			case STATE_ST: 
				switch (*lexeme++) {
				case 'a':
					current_state = match(lexeme, "ck", STATE_STACK);
					continue;
				case 'o':
					current_state = match(lexeme, "re", STATE_STORE);
					continue;
				default: 
					current_state = STATE_UNRECOGNISED;
					continue;
				}

			case STATE_STACK: 
				return make_token(TOKEN_STACK, word, val);
			case STATE_STORE: 
				return make_token(TOKEN_STORE, word, val);
			case STATE_STRING: 
				return make_token(TOKEN_STRING, word, val);
			case STATE_SUB: 
				return make_token(TOKEN_SUB, word, val);
			case STATE_SWITCH: 
				return make_token(TOKEN_SWITCH, word, val);
			case STATE_SYNCSCOPE: 
				return make_token(TOKEN_SYNCSCOPE, word, val);
			
			case STATE_DIGITS: 
				switch (*lexeme++) {
					case '0':
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						continue;
					case '.':
						current_state = STATE_DIGITS_DOT;
						continue;
					case NULL:
						val.ival = atoi(word.word);
						return make_token(TOKEN_SZ, word, val);
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_DIGITS_DOT:
				switch (*lexeme++) {
					case '0':
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						current_state = STATE_DIGITS_DOT_DIGITS;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;
			
			case STATE_DIGITS_DOT_DIGITS:
				switch (*lexeme++) {
					case '0':
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						continue;
					case 'e':
						current_state = STATE_DIGITS_DOT_DIGITS_E;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_DIGITS_DOT_DIGITS_E:
				switch (*lexeme++) {
					case '+': case '-':
						current_state = STATE_DIGITS_DOT_DIGITS_E_SIGN;
						continue;
					default:
						return NULL;
				}
				return NULL;

			case STATE_DIGITS_DOT_DIGITS_E_SIGN:
				switch (*lexeme++) {
					case '0':
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						current_state = STATE_DIGITS_DOT_DIGITS_E_SIGN_DIGITS;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_DIGITS_DOT_DIGITS_E_SIGN_DIGITS:
				switch (*lexeme++) {
					case '0':
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						continue;
					case NULL:
						current_state = STATE_FLOAT_LITERAL;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_FLOAT_LITERAL:
				val.fval = atol(word.word);
				return make_token(TOKEN_FLOAT_LITERAL, word, val);

			case STATE_T: 
				switch (*lexeme++) {
					case 'a':
						current_state = STATE_TA;
						continue;
					case 'o':
						current_state = match(lexeme, "", STATE_TO);
						continue;
					case 'r':
						current_state = STATE_TR;
						continue;
					case 'y':
						current_state = match(lexeme, "pe", STATE_TYPE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_TA:
				switch (*lexeme++) {
					case 'i':
						current_state = match(lexeme, "l", STATE_TAIL);
						continue;
					case 'r':
						current_state = match(lexeme, "get", STATE_TARGET);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_TAIL:
				return make_token(TOKEN_TAIL, word, val);
			case STATE_TARGET:
				return make_token(TOKEN_TARGET, word, val);

			case STATE_TO:
				return make_token(TOKEN_TO, word, val);

			case STATE_TR:
				switch(*lexeme++) {

					case 'i':
						current_state = match(lexeme, "ple", STATE_TRIPLE);
						continue;

					case 'u':
						switch (*lexeme++) {
							case 'n':
								current_state = match(lexeme, "c", STATE_TRUNC);
								continue;
							case 'e':
								current_state = match(lexeme, "", STATE_TRUE);
								continue;
						}

					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_TRIPLE:
				return make_token(TOKEN_TRIPLE, word, val);
			case STATE_TRUNC: 
				return make_token(TOKEN_TRUNC, word, val);
			case STATE_TRUE: 
				return make_token(TOKEN_TRUE, word, val);
			case STATE_TYPE: 
				return make_token(TOKEN_TYPE, word, val);

			case STATE_U: 
				switch (*lexeme++) {
					case 'd':
						current_state = match(lexeme, "iv", STATE_UDIV);
						continue;
					case 'g':
						current_state = STATE_UG;
						continue;
					case 'i':
						current_state = match(lexeme, "tofp", STATE_UITOFP);
						continue;
					case 'l':
						current_state = STATE_UL;
						continue;
					case 'n':
						current_state = STATE_UN;
						continue;
					case 'r':
						current_state = match(lexeme, "em", STATE_UREM);
						continue;
					case 'w':
						current_state = match(lexeme, "table", STATE_UWTABLE);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_UITOFP: 
				return make_token(TOKEN_UITOFP, word, val);

			case STATE_UG:
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "", STATE_UGE);
						continue;
					case 't':
						current_state = match(lexeme, "", STATE_UGT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_UGE: 
				return make_token(TOKEN_UGE, word, val);
			case STATE_UDIV: 
				return make_token(TOKEN_UDIV, word, val);

			case STATE_UN: 
				switch (*lexeme++) {
					case 'n':
						current_state = match(lexeme, "amed_addr", STATE_UNNAMED_ADDR);
						continue;
					case 'd':
						current_state = match(lexeme, "ef", STATE_UNDEF);
						continue;
					case 'e':
						current_state = match(lexeme, "", STATE_UNE);
						continue;
					case 'r':
						current_state = match(
							lexeme, "eachable", STATE_UNREACHABLE);
						continue;
					case 'w':
						current_state = match(lexeme, "ind", STATE_UNWIND);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_UNNAMED_ADDR:
				return make_token(TOKEN_UNNAMED_ADDR, word, val);
			case STATE_UNE: 
				return make_token(TOKEN_UNE, word, val);
			case STATE_UNDEF: 
				return make_token(TOKEN_UNDEF, word, val);
			case STATE_UNREACHABLE: 
				return make_token(TOKEN_UNREACHABLE, word, val);
			case STATE_UNWIND: 
				return make_token(TOKEN_UNWIND, word, val);
			case STATE_UREM: 
				return make_token(TOKEN_UREM, word, val);
			case STATE_UWTABLE:
				return make_token(TOKEN_UWTABLE, word, val);

			case STATE_V: 
				switch (*lexeme++) {
					case 'a':;
						current_state = match(lexeme, "l", STATE_VAL);
						continue;
					case 'o':;
						current_state = STATE_VO;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_VAL: return make_token(TOKEN_VAL, word, val);

			case STATE_VO:
				switch (*lexeme++) {
					case 'l':
						current_state = match(lexeme, "atile", STATE_VOLATILE);
						continue;
					case 'i':
						current_state = match(lexeme, "d", STATE_VOID);;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_VOLATILE: 
				return make_token(TOKEN_VOLATILE, word, val);
			case STATE_VOID: 
				return make_token(TOKEN_VOID, word, val);
			
			case STATE_X: 
				return make_token(TOKEN_X, word, val);
			case STATE_XOR: 
				return make_token(TOKEN_XOR, word, val);
			
			case STATE_W:
				switch (*lexeme++) {
				case 'i':
					current_state = match(lexeme, "llreturn", STATE_WILLRETURN);
					continue;
				case 'r':
					current_state = match(lexeme, "iteonly", STATE_WRITEONLY);
					continue;
				default:
					current_state = STATE_UNRECOGNISED;
					continue;
				}
				return NULL;

			case STATE_WILLRETURN:
				return make_token(TOKEN_WILLRETURN, word, val);
			case STATE_WRITEONLY: 
				return make_token(TOKEN_WRITEONLY, word, val);

			case STATE_ZE:
				switch (*lexeme++) {
					case 'r':
						if (*lexeme++ == 'o') {
							current_state = STATE_ZERO;
							continue;
						}
						current_state = STATE_UNRECOGNISED;
						continue;
					case 'x':
						current_state = match(
							lexeme, "t", STATE_ZEXT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_ZERO:
				switch (*lexeme++) {
					case 'e':
						current_state = match(lexeme, "xt", STATE_ZEROEXT);
						continue;
					case 'i':
						current_state = match(lexeme, "nitializer", STATE_ZEROINITIALIZER);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_ZEROINITIALIZER: 
				return make_token(TOKEN_ZEROINITIALIZER, word, val);
			case STATE_ZEROEXT:
				return make_token(TOKEN_ZEROEXT, word, val);
			case STATE_ZEXT: 
				return make_token(TOKEN_ZEXT, word, val);

			default: /* invalid state */
				current_state = STATE_UNRECOGNISED;
				continue;
		}
	}
}

/** 
* Responsible for taking preprocessed input words and placing the output
* in a array of pointers to tokens. It marks the end of the output 
* with NULL.
* 
* @param input The preprocessed input which will be converted to tokens
* @param output The location to place the new tokens created from the input.
* 
* @see tokenize
*/
void  tokenizer(preprocessed_word** input, token **output) {
	// For each word in input
	preprocessed_word** words = input;
	preprocessed_word* word = *words++;
	token** dest = output;
	while (word != NULL) {

		if (DEBUG_TOKENIZER) {
			printf("READ  : %-32s %8s", word->word, " ");
		}

		token *tk = tokenize(*word);
		
		if (DEBUG_TOKENIZER) {
			printf("WROTE : %-32s\n",
				lookup_token_as_name((*tk).name));
		}
		word = *words++;
		*dest++ = tk;
	}
	// Finish the tokens off with NULL
	*dest = NULL;
}