/* lexer.c - Peforms lexical analysis on given input after preprocessing */

#include "lexer.h"

/*
* token_name_lookup is used to associate a tokens internal name
* with a human-readable name. Used for human interfacing and also
* for testing purposes. Lexeme is the appropriate lexeme, or
* an example lexeme if more than one exists.
*/
char* token_name_lookup[][2] = {
	[TOKEN_ADD] = {"TOKEN_ADD", "add"},
	[TOKEN_AGGR] = {"TOKEN_AGGR", "aggr"},
	[TOKEN_ALIGN] = {"TOKEN_ALIGN", "align"},
	[TOKEN_ALLOCA] = {"TOKEN_ALLOCA", "alloca"},
	[TOKEN_AND] = {"TOKEN_AND", "and"},
	[TOKEN_ASHR] = {"TOKEN_ASHR", "ashr"},
	[TOKEN_BIGENDIAN] = {"TOKEN_BIGENDIAN", "bigendian"},
	[TOKEN_BITCAST] = {"TOKEN_BITCAST", "bitcast"},
	[TOKEN_BLOCKLABEL] = {"TOKEN_LABEL", "entry:"},
	[TOKEN_BR] = {"TOKEN_BR", "br"},
	[TOKEN_BR] = {"TOKEN_BR", "br"},
	[TOKEN_CALL] = {"TOKEN_CALL", "call"},
	[TOKEN_CLOSE_BRACKET] = {"TOKEN_CLOSE_BRACKET", "]"},
	[TOKEN_CLOSE_CURLY_BRACKET] = {"TOKEN_CLOSE_CURLY_BRACKET", "}"},
	[TOKEN_CLOSE_PAREN] = {"TOKEN_CLOSE_PAREN", "]"},
	[TOKEN_COMMA] = {"TOKEN_COMMA", ","},
	[TOKEN_DECLARE] = {"TOKEN_DECLARE", "declare"},
	[TOKEN_DEFINE] = {"TOKEN_DEFINE", "define"},
	[TOKEN_DOTDOTDOT] = {"TOKEN_DOTDOTDOT", "..."},
	[TOKEN_DOUBLE] = {"TOKEN_DOUBLE", "double"},
	[TOKEN_EQUALS] = {"TOKEN_EQUALS", "="},
	[TOKEN_FADD] = {"TOKEN_FADD", "fadd"},
	[TOKEN_FALSE] = {"TOKEN_FALSE", "false"},
	[TOKEN_FCMP] = {"TOKEN_FCMP", "fcmp"},
	[TOKEN_FDIV] = {"TOKEN_FDIV", "fdiv"},
	[TOKEN_FLOAT] = {"TOKEN_FLOAT", "float"},
	[TOKEN_FMUL] = {"TOKEN_FMUL", "fmul"},
	[TOKEN_FPEXT] = {"TOKEN_FPEXT", "fpext"},
	[TOKEN_FPTOUI] = {"TOKEN_FPTOUI", "fptoui"},
	[TOKEN_FREE] = {"TOKEN_FREE", "free"},
	[TOKEN_FREM] = {"TOKEN_FREM", "frem"},
	[TOKEN_FSUB] = {"TOKEN_FSUB", "fsub"},
	[TOKEN_GETELEMENTPTR] = {"TOKEN_GETELEMENTPTR", "getelementptr"},
	[TOKEN_GLOBAL] = {"TOKEN_GLOBAL", "global"},
	[TOKEN_ICMP] = {"TOKEN_ICMP", "icmp"},
	[TOKEN_IDENTIFIER] = {"TOKEN_IDENTIFIER", "%identifier"},
	[TOKEN_INBOUNDS] = {"TOKEN_INBOUNDS", "inbounds"},
	[TOKEN_INTEGERTYPE] = {"TOKEN_INTEGERTYPE", "i32"},
	[TOKEN_INTTOPTR] = {"TOKEN_INTTOPTR", "inttoptr"},
	[TOKEN_LITTLEENDIAN] = {"TOKEN_LITTLEENDIAN", "littleendian"},
	[TOKEN_LABEL] = {"TOKEN_LABEL", "label"},
	[TOKEN_LOAD] = {"TOKEN_LOAD", "load"},
	[TOKEN_LSHR] = {"TOKEN_LSHR", "lshr"},
	[TOKEN_MALLOC] = {"TOKEN_MALLOC", "malloc"},
	[TOKEN_MUL] = {"TOKEN_MUL", "mul"},
	[TOKEN_NULL] = {"TOKEN_NULL", "null"},
	[TOKEN_NSW] = {"TOKEN_NSW", "nsw"},
	[TOKEN_OPEN_CURLY_BRACKET] = {"TOKEN_OPEN_CURLY_BRACKET", "{"},
	[TOKEN_OPEN_BRACKET] = {"TOKEN_OPEN_BRACKET", "["},
	[TOKEN_OPEN_PAREN] = {"TOKEN_OPEN_PAREN", "("},
	[TOKEN_OR] = {"TOKEN_OR", "or"},
	[TOKEN_PHI] = {"TOKEN_PHI", "phi"},
	[TOKEN_PTR] = {"TOKEN_PTR", "*"},
	[TOKEN_PTRTOINT] = {"TOKEN_PTRTOINT", "ptrtoint"},
	[TOKEN_RET] = {"TOKEN_RET", "ret"},
	[TOKEN_SDIV] = {"TOKEN_SDIV", "sdiv"},
	[TOKEN_SELECT] = {"TOKEN_SELECT", "select"},
	[TOKEN_SEXT] = {"TOKEN_SEXT", "sext"},
	[TOKEN_SLE] = {"TOKEN_SLE", "sle"},
	[TOKEN_SLT] = {"TOKEN_SLT", "slt"},
	[TOKEN_SHL] = {"TOKEN_SHL", "shl"},
	[TOKEN_SREM] = {"TOKEN_SREM", "srem"},
	[TOKEN_STACK] = {"TOKEN_STACK", "stack"},
	[TOKEN_STORE] = {"TOKEN_STORE", "store"},
	[TOKEN_SUB] = {"TOKEN_SUB", "sub"},
	[TOKEN_SZ] = {"TOKEN_SZ", "1"},
	[TOKEN_TO] = {"TOKEN_TO", "to"},
	[TOKEN_TRUNC] = {"TOKEN_TRUNC", "trunc"},
	[TOKEN_TRUE] = {"TOKEN_TRUE", "true"},
	[TOKEN_TYP] = {"TOKEN_TYP", "typ"},
	[TOKEN_UDIV] = {"TOKEN_UDIV", "udiv"},
	[TOKEN_UNDEF] = {"TOKEN_UNDEF", "undef"},
	[TOKEN_UNREACHABLE] = {"TOKEN_UNREACHABLE", "unreachable"},
	[TOKEN_UREM] = {"TOKEN_UREM", "urem"},
	[TOKEN_VAL] = {"TOKEN_VAL", "val"},
	[TOKEN_VOID] = {"TOKEN_VOID", "void"},
	[TOKEN_VOID] = {"TOKEN_VOID", "void"},
	[TOKEN_X] = {"TOKEN_X", "x"},
	[TOKEN_XOR] = {"TOKEN_XOR", "xor"},
	[TOKEN_ZEROINITIALIZER] = {"TOKEN_ZEROINITIALIZER", "zeroinitializer"},
	[TOKEN_ZEXT] = {"TOKEN_ZEXT", "zext"},
};

/*
* make_token generates a token struct from given data.
* name means the internal name e.g. TOKEN_ADD.
* lexeme means the word that made the token, e.g. "%myvar"
* value means the value behind the token, e.g. 32
*/
struct Token *make_token(token_name name, char* lexeme, int value) {
	token *tk = malloc(sizeof(token));
	// Copying Name.
	tk->name = name;
	// Copying lexeme.
	//tk->lexeme = malloc(sizeof(char) * strlen(lexeme));
	//strcpy(lexeme, tk->lexeme);
	tk->lexeme = lexeme;
	// Copying value.
	tk->value = value;
	return tk;
}

/*
* match is used to make a final decision on if an input corresponds
* to a token. If the inputs match then it returns the given result
* state, otherwise it returns the error state.
*/
state match(char* input, char* expected, state result) {
	if (strcmp(input, expected) == 0) {
		return result;
	} else {
		return STATE_UNRECOGNISED;
	}
}

/*
* tokenize is used to return a pointer to an appropriately constructed
* token from a given preprocessed word. 
*/
struct Token *tokenize(char* input) {
	char* ci = input; /* ci represents the currently considered char */
	state current_state = BEGIN_STATE;
	while (current_state != END_STATE) {
		if (DEBUG_TOKENIZE) { 
			printf("CURRENT STATE: %d\n", current_state);
			printf("WORD REMAINING: %s\n", ci);
		}
		switch (current_state) {

			case BEGIN_STATE:
				switch (*ci++) {
				case 'a':
					current_state = STATE_A;
					continue;
				case 'b':
					current_state = STATE_B;
					continue;
				case 'c':
					current_state = match(
						ci, "all", STATE_CALL);
					continue;
				case 'd':
					current_state = STATE_D;
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
					current_state = match(ci, "r", STATE_OR);
					continue;
				case 'p':
					current_state = STATE_P;
					continue;
				case 'r':
					current_state = match(ci, "et", STATE_RET);
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
				case 'x':
					if (*ci == NULL) {
						current_state = STATE_X;
						continue;
					} 
					current_state = match(ci, "or", STATE_XOR);
					continue;
				case 'z':
					if (*ci++ == 'e') {
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
				case '@':
					current_state = STATE_IDENTIFIER;
					continue;
				case '#':
					current_state = STATE_IDENTIFIER;
					continue;
				case '=':
					current_state = match(ci, "", STATE_EQUALS);
					continue;
				case '(':
					current_state = match(ci, "", STATE_OPEN_PAREN);
					continue;
				case ')':
					current_state = match(ci, "", STATE_CLOSE_PAREN);
					continue;
				case '[':
					current_state = match(ci, "", STATE_OPEN_BRACKET);
					continue;
				case ']':
					current_state = match(ci, "", STATE_CLOSE_BRACKET);
					continue;
				case '{':
					current_state = match(ci, "", STATE_OPEN_CURLY_BRACKET);
					continue;
				case '}':
					current_state = match(ci, "", STATE_CLOSE_CURLY_BRACKET);
					continue;
				case ',':
					current_state = match(ci, "", STATE_COMMA);
					continue;
				case '.':
					current_state = match(ci, "..", STATE_DOTDOTDOT);
					continue;
				case '*':
					current_state = match(ci, "", STATE_PTR);
					continue;
				case '0': 
				case '1': case '2': case '3': 
				case '4': case '5': case '6': 
				case '7': case '8': case '9': 
					current_state = STATE_SZ;
					continue;
				default: 
					current_state = STATE_UNRECOGNISED;
					continue;
			}
			break;

			case STATE_UNRECOGNISED: 
				; char* c = input
				; bool is_label = true;
				// Check if it is a label
				for (; *(c + 1) != NULL; c++) {
					if (!isalpha(*c)) { printf("GOTHERE"); is_label = false; break; }
				}
				if (is_label && *c == ':') { current_state = STATE_BLOCK_LABEL; continue; }
				// If we get here it is not.
				return make_token(ERROR_TOKEN, input, NULL);

			case STATE_A: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "d", STATE_ADD);
						continue;
					case 'g':
						current_state = match(ci, "gr", STATE_AGGR);
						continue;
					case 'l':
						current_state = STATE_AL;
						continue;
					case 'n':
						current_state = match(ci, "d", STATE_AND);
						continue;
					case 's':
						current_state = match(ci, "hr", STATE_ASHR);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
				}
				break;

			case STATE_ADD: return make_token(TOKEN_ADD, input, NULL);
			case STATE_AGGR: return make_token(TOKEN_AGGR, input, NULL);

			case STATE_AL:
					switch (*ci++) {
					case 'i':
						current_state = match(ci, "gn", STATE_ALIGN);
						continue;
					case 'l':
						current_state = match(ci, "oca", STATE_ALLOCA);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_ALIGN: return make_token(TOKEN_ALIGN, input, NULL);
			case STATE_ALLOCA: return make_token(TOKEN_ALLOCA, input, NULL);
			case STATE_AND: return make_token(TOKEN_AND, input, NULL);
			case STATE_ASHR: return make_token(TOKEN_ASHR, input, NULL);

			case STATE_B: 
				switch (*ci++) {
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
				switch (*ci++) {
					case 'g':
						current_state = match(
							ci, "endian", STATE_BIGENDIAN);
						continue;
					case 't':
						current_state = match(
							ci, "cast", STATE_BITCAST);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				break;

			case STATE_BIGENDIAN: return make_token(TOKEN_BIGENDIAN, input, NULL);
			case STATE_BITCAST: return make_token(TOKEN_BITCAST, input, NULL);
			case STATE_BR: return make_token(TOKEN_BR, input, NULL);

			case STATE_CALL: return make_token(TOKEN_CALL, input, NULL);

			case STATE_CLOSE_CURLY_BRACKET: return make_token(TOKEN_CLOSE_CURLY_BRACKET, input, NULL);
			case STATE_CLOSE_BRACKET: return make_token(TOKEN_CLOSE_BRACKET, input, NULL);
			case STATE_CLOSE_PAREN: return make_token(TOKEN_CLOSE_PAREN, input, NULL);
			case STATE_COMMA: return make_token(TOKEN_COMMA, input, NULL);

			case STATE_D: 
				switch (*ci++) {
					case 'e':
						current_state = STATE_DE;
						continue;
					case 'o':
						current_state = match(
							ci, "uble", STATE_DOUBLE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				};

			case STATE_DE: 
				switch (*ci++) {
					case 'c':
						current_state = STATE_DECLARE;
						continue;
					case 'f':
						current_state = STATE_DEFINE;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_DECLARE: return make_token(TOKEN_DECLARE, input, NULL);
			case STATE_DEFINE:return make_token(TOKEN_DEFINE, input, NULL);
			case STATE_DOTDOTDOT: return make_token(TOKEN_DOTDOTDOT, input, NULL);
			case STATE_DOUBLE: return make_token(TOKEN_DOUBLE, input, NULL);
			case STATE_EQUALS: return make_token(TOKEN_EQUALS, input, NULL);;

			case STATE_F: 
				switch (*ci++) {
					case 'a': 
						current_state = STATE_FALSE;
						continue;
					case 'c':
						current_state = match(
							ci, "mp", STATE_FCMP);
						continue;
					case 'd':
						current_state = match(
							ci, "iv", STATE_FDIV);
						continue;
					case 'l':
						current_state = match(
							ci, "oat", STATE_FLOAT);
						continue;
					case 'm':
						current_state = match(
							ci, "ul", STATE_FMUL);
						continue;
					case 'p':
						current_state = STATE_FP;
						continue;
					case 'r':
						switch (*ci++) {
							case 'e':
								current_state = STATE_FRE;
								continue;
							default:
								current_state = STATE_UNRECOGNISED;
								continue;
						}
					case 's':
						current_state = match(ci, "ub", STATE_FSUB);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_FA:
				switch (*ci) {
					case 'd':
						current_state = match(ci, "d", STATE_FADD);
						continue;
					case 'l':
						current_state = match(ci, "se", STATE_FADD);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				current_state = STATE_UNRECOGNISED;
				continue;

			case STATE_FALSE: return make_token(TOKEN_FALSE, input, NULL);
			case STATE_FADD: return make_token(TOKEN_FADD, input, NULL);

			case STATE_FCMP: return make_token(TOKEN_FCMP, input, NULL);
			case STATE_FDIV: return make_token(TOKEN_FDIV, input, NULL);
			case STATE_FLOAT: return make_token(TOKEN_FLOAT, input, NULL);
			case STATE_FMUL: return make_token(TOKEN_FMUL, input, NULL);

			case STATE_FP:
				switch (*ci++) {
					case 'e':
						current_state = match(
							ci, "xt", STATE_FPEXT);
						continue;
					case 't':
						current_state = match(
							ci, "oui", STATE_FPTOUI);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_FPEXT: return  make_token(TOKEN_FPEXT, input, NULL);
			case STATE_FPTOUI: return  make_token(TOKEN_FPTOUI, input, NULL);


			case STATE_FRE: 
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "", STATE_FREE);
						continue;
					case 'm':
						current_state = match(ci, "", STATE_FREM);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_FREE: return make_token(TOKEN_FREE, input, NULL);
			case STATE_FREM: return make_token(TOKEN_FREM, input, NULL);
			case STATE_FSUB: return make_token(TOKEN_FSUB, input, NULL);

			case STATE_G: 
				switch (*ci++) {
					case 'e':
						current_state = match(
							ci, "telementptr", STATE_GETELEMENTPTR);
						continue;
					case 'l':
						current_state = match(
							ci, "obal", STATE_GLOBAL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
			case STATE_GETELEMENTPTR: return make_token(TOKEN_GETELEMENTPTR, input, NULL);
			case STATE_GLOBAL: return make_token(TOKEN_GLOBAL, input, NULL);

			case STATE_I: 
				switch (*ci++) {
					case 'c':
						current_state = match(
							ci, "mp", STATE_ICMP);
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
				switch (*ci++) {
					case 'b':
						current_state = match(ci, "ounds", STATE_INBOUNDS);
						continue;
					case 't':
						current_state = match(ci, "optr", STATE_INTTOPTR);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_ICMP: return make_token(TOKEN_ICMP, input, NULL);
			case STATE_IDENTIFIER: return make_token(TOKEN_IDENTIFIER, input, NULL);
			case STATE_INBOUNDS:  return make_token(TOKEN_INBOUNDS, input, NULL);

			case STATE_INTEGERTYPE:
				switch (*ci++) {
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						current_state = STATE_INTEGERTYPE;
						continue;
					case NULL:
						return make_token(TOKEN_INTEGERTYPE, input, atoi(input+1));
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_INTTOPTR: return make_token(TOKEN_INTTOPTR, input, NULL);;

			case STATE_L: 
				switch (*ci++) {
					case 'a':
						current_state = match(ci, "bel", STATE_LABEL);
						continue;
					case 'i':
						current_state = match(
							ci, "ttleendian", STATE_LITTLEENDIAN);
						continue;
					case 'o':
						current_state = match(
							ci, "ad", STATE_LOAD);
						continue;
					case 's':
						current_state = match(
							ci, "hr", STATE_LSHR);
						continue;
					default:
						current_state = ERROR_TOKEN;
						continue;
				}

			case STATE_BLOCK_LABEL: return make_token(TOKEN_BLOCKLABEL, input, NULL);
			case STATE_LITTLEENDIAN: return make_token(TOKEN_LITTLEENDIAN, input, NULL);
			case STATE_LABEL: return make_token(TOKEN_LABEL, input, NULL);
			case STATE_LOAD: return make_token(TOKEN_LOAD, input, NULL);
			case STATE_LSHR: return make_token(TOKEN_LSHR, input, NULL);

			case STATE_M: 
				switch (*ci++) {
					case 'a':
						current_state = match(
							ci, "lloc", STATE_MALLOC);
						continue;
					case 'u':
						current_state = match(
							ci, "l", STATE_MUL);
						continue;
					default:
						current_state = ERROR_TOKEN;
						continue;
				}

			case STATE_MALLOC: return make_token(TOKEN_MALLOC, input, NULL);
			case STATE_MUL: return make_token(TOKEN_MUL, input, NULL);

			case STATE_N: 
				switch (*ci++) {
					case 's':
						current_state = match(ci, "w", STATE_NSW);
						continue;
					case 'u':
						current_state = match(ci, "ll", STATE_NULL);
						continue;
					default:
						current_state = ERROR_TOKEN;
						continue;
				}

			case STATE_NSW: return make_token(TOKEN_NSW, input, NULL);
			case STATE_NULL:return make_token(TOKEN_NULL, input, NULL);

			case STATE_OPEN_CURLY_BRACKET: return make_token(TOKEN_OPEN_CURLY_BRACKET, input, NULL);
			case STATE_OPEN_BRACKET: return make_token(TOKEN_OPEN_BRACKET, input, NULL);
			case STATE_OPEN_PAREN: return make_token(TOKEN_OPEN_PAREN, input, NULL);
			case STATE_OR: return make_token(TOKEN_OR, input, NULL);

			case STATE_P: 
				switch (*ci++) {
				case 'h':
					current_state = match(ci, "i", STATE_PHI);
					continue;
				case 't':

					current_state = match(
						ci, "rtoint", STATE_PTRTOINT);
					continue;
				default:
					current_state = STATE_UNRECOGNISED;
					continue;
				}
				

			case STATE_PHI: return make_token(TOKEN_PHI, input, NULL);
			case STATE_PTR: return make_token(TOKEN_PTR, input, NULL);
			case STATE_PTRTOINT: return make_token(TOKEN_PTRTOINT, input, NULL);

			case STATE_RET: return make_token(TOKEN_RET, input, NULL);

			case STATE_S: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "iv", STATE_SDIV);
						continue;
					case 'e':
						current_state = STATE_SE;
						continue;
					case 'h':
						current_state = match(ci, "l", STATE_SHL);
						continue;
					case 'l':
						current_state = STATE_SL;
						continue;
					case 'r':
						current_state = match(ci, "em", STATE_SREM);
						continue;
					case 't':
						current_state = STATE_ST;
						continue;
					case 'u':
						current_state = match(ci, "b", STATE_SUB);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
			

			case STATE_SL:
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "", STATE_SLE);
						continue;
					case 't':
						current_state = match(ci, "", STATE_SLT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_SDIV: return make_token(TOKEN_SDIV, input, NULL);

			case STATE_SE: 
				switch (*ci++) {
				case 'l':
					current_state = match(ci, "ect", STATE_SELECT);
					continue;
				case 'x':
					current_state = match(ci, "t", STATE_SEXT);
					continue;
				default: 
					current_state = STATE_UNRECOGNISED;
					continue;
				};

			case STATE_SELECT: return make_token(TOKEN_SELECT, input, NULL);
			case STATE_SEXT: return make_token(TOKEN_SEXT, input, NULL);
			case STATE_SHL: return make_token(TOKEN_SHL, input, NULL);
			case STATE_SLT: return make_token(TOKEN_SLT, input, NULL);
			case STATE_SLE: return make_token(TOKEN_SLE, input, NULL);

			case STATE_SREM: return make_token(TOKEN_SREM, input, NULL);

			case STATE_ST: 
				switch (*ci++) {
				case 'a':
					current_state = match(ci, "ck", STATE_STACK);
					continue;
				case 'o':
					current_state = match(ci, "re", STATE_STORE);
					continue;
				default: 
					current_state = STATE_UNRECOGNISED;
					continue;
				}

			case STATE_STACK: return make_token(TOKEN_STACK, input, NULL);
			case STATE_STORE: return make_token(TOKEN_STORE, input, NULL);
			case STATE_SUB: return make_token(TOKEN_SUB, input, NULL);
			
			case STATE_SZ: 
				switch (*ci++) {
					case '0':
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						continue;
					case NULL:
						return make_token(TOKEN_SZ, input, atoi(input));
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_T: 
				switch (*ci++) {
				case 'o':
					current_state = match(ci, "", STATE_TO);
					continue;
				case 'r':
					current_state = match(ci, "unc", STATE_TRUNC);
					continue;
				case 'y':
					current_state = match(ci, "p", STATE_TYP);
					continue;
				default:
					current_state = STATE_UNRECOGNISED;
					continue;
				}

			case STATE_TO: return make_token(TOKEN_TO, input, NULL);

			case STATE_TR:
				switch(*ci++) {

					case 'u':
						switch (*ci++) {
							case 'n':
								current_state = match(ci, "c", STATE_TRUNC);
								continue;
							case 'e':
								current_state = match(ci, "", STATE_TRUE);
								continue;
						}

					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

				

			case STATE_TRUNC: return make_token(TOKEN_TRUNC, input, NULL);
			case STATE_TRUE: return make_token(TOKEN_TRUNC, input, NULL);

			case STATE_TYP: return make_token(TOKEN_TYP, input, NULL);

			case STATE_U: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "iv", STATE_UDIV);
						continue;
					case 'n':
						current_state = STATE_UN;
						continue;
					case 'r':
						current_state = match(ci, "em", STATE_UREM);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_UDIV: return make_token(TOKEN_UDIV, input, NULL);

			case STATE_UN: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "ef", STATE_UNDEF);
						continue;
					case 'r':
						current_state = match(ci, "eachable", STATE_UNREACHABLE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_UNDEF: return make_token(TOKEN_UNDEF, input, NULL);
			case STATE_UNREACHABLE: return make_token(TOKEN_UNREACHABLE, input, NULL);
			case STATE_UREM: return make_token(TOKEN_UREM, input, NULL);

			case STATE_V: 
				switch (*ci++) {
					case 'a':;
						current_state = match(ci, "l", STATE_VAL);
						continue;
					case 'o':;
						current_state = match(ci, "id", STATE_VOID);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_VAL: return make_token(TOKEN_VAL, input, NULL);
			case STATE_VOID: return make_token(TOKEN_VOID, input, NULL);
			case STATE_X: return make_token(TOKEN_X, input, NULL);
			case STATE_XOR: return make_token(TOKEN_XOR, input, NULL);

			case STATE_ZE:
				switch (*ci++) {
					case 'r':
						current_state = match(
							ci, "oinitializer", STATE_ZEROINITIALIZER);
							continue;
					case 'x':
						current_state = match(
							ci, "t", STATE_ZEXT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_ZEROINITIALIZER: return make_token(TOKEN_ZEROINITIALIZER, input, NULL);
			case STATE_ZEXT: return make_token(TOKEN_ZEXT, input, NULL);

			default: /* invalid state */
				current_state = STATE_UNRECOGNISED;
				continue;
		}
	}
}

/* tokenizer takes preprocessed input words and places the output
* in a array of pointers to tokens. It marks the end of the output 
* with NULL.
*/
void  tokenizer(char** input, token **output) {
	// For each word in input
	char** lexemes = input;
	char* lexeme = *lexemes++;
	token** dest = output;
	while (lexeme != NULL) {

		if (DEBUG_TOKENIZER) {
			printf("READ  : %-16s\t", lexeme);
		}

		token *tk = tokenize(lexeme);
		
		if (DEBUG_TOKENIZER) {
			printf("WROTE : %-10s\n",
				lookup_token_as_name((*tk).name));
		}
		lexeme = *lexemes++;
		*dest++ = tk;
	}
	// Finish the tokens off with NULL
	*dest = NULL;
}

/* is_whitespace determines if the preprocessor should ignore these
* input symbols. It differs from is_delimiter as whitespace is never
* part of the input, while a delimiter may be the start of a 
* new word.
*/
 bool is_whitespace(char chr) {
	switch (chr) {
		case ' ': case '\t': 
		case '\n': case EOF:
		case NULL:
			return true;
		default: 
			return false;
	}
}

/* is_delimiter Informs the preprocessor as to if the word should 
*	finish and restart anew including this given symbol.
*/
bool is_delimiter(char chr) {
	if (is_whitespace(chr)) { return true; }
	switch (chr) {
		case ' ': case '\t': 
		case '\n': case EOF:
		case '[': case ']': 
		case '(': case ')':
		case ',': case ';':
		case '*': case NULL:
			return true;
		default: 
			return false;
	}
}

/* preprocessor plits an input string into 
	substrings each containing a single word.
	Assuming a well-formed input, one token
	can be made from each word.
*/
void preprocessor(char* input, char **output) {
	char* buffer = malloc(PREPROCESSOR_WORD_SIZE);

	int oi = 0, bi = 0; // output and buffer index
	char* ci = input; // Character index
	// For each character in input
	bool in_comment = false;
	while (*ci != NULL && *ci != EOF) {
		if (DEBUG_PREPROCESSOR) {
			printf("CURRENT CHARACTER: '%c'\n", *ci);
		}
		// Case 0. We are in a comment.
		if (in_comment) {
			if (*ci++ == '\n') {
				in_comment = false;
				continue;
			}
			else {
				continue;
			}
		}
		// Case 0. We are entering a comment.
		if (*ci == ';') {
			in_comment = true;
			continue;
		}
	
		// Case 1. This is white-space.
		if (is_whitespace(*ci)) { ci++; continue; }
		
		// Case 2. This a valid character in word.
		buffer[bi++] = *ci;
		// Case 2.2. This is the last character in word.
		
		if (is_delimiter(*ci) || is_delimiter(*(ci + 1))) {
			// If so finish word.
			buffer[bi] = NULL;
			// Then add word to output.
			output[oi++] = buffer;
			if (DEBUG_PREPROCESSOR) {
				printf("SAVING WORD: %s\n", buffer);
			}
			// Then reset state.
			bi = 0;
			buffer = malloc(PREPROCESSOR_WORD_SIZE);

		}
		ci++;
	}
	// Now finish it off with a NULL.
	output[oi] = NULL;
}

/* test_tokenize_case tests an individual case of tokenize
* and prints a short report on the result.
*/
void test_tokenize_case(char* gave, token_name expected) {
	token *got = tokenize(gave);
	printf("Gave %-16s expected %-22s got %-22s\n", 
		gave, 
		lookup_token_as_name(expected), 
		lookup_token_as_name((*got).name));
}

/* test_tokenize tests many cases of tokenize and prints
*  a report on each result.
*/
void test_tokenize() {
	for (token_name i = TOKEN_ADD; i < TOKEN_ZEXT; i++) {
		test_tokenize_case(lookup_token_as_lexeme(i), i);
	}
}

void print_token(token** tk) {
	char* field_format = "%-24s\t%-16s\t%-8d\n";
	printf(field_format,
		lookup_token_as_name((**tk).name),
		(**tk).lexeme,
		(**tk).value);
}

/* print_token_list prints a given list of pointers to tokens, 
*  including all of their field contents.
*/
void print_token_list(token **tokens) {
	token **tk = tokens;
	char* header_format = "%-24s\t%-16s\t%-8s\n";
	printf(header_format, "Token Name", "Token Lexeme", "Token Value");
	printf(header_format, "----------", "------------", "-----------");
	while (*tk != NULL) {
		print_token(tk++);
	};
}

