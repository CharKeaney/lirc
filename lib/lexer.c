/* lexer.c - Peforms lexical analysis on given input after preprocessing */

#include "lexer.h"

/*
* token_name_lookup is used to associate a tokens internal name
* with a human-readable name. Used for human interfacing and also
* for testing purposes. Lexeme is the appropriate lexeme, or
* an example lexeme if more than one exists.
*/
char* token_name_lookup[][2] = {
	[ERROR_TOKEN] = {"TOKEN_ERROR", ""},
	[TOKEN_ADD] = {"TOKEN_ADD", "add"},
	[TOKEN_ASM] = {"TOKEN_ASM", "asm"},
	[TOKEN_AGGR] = {"TOKEN_AGGR", "aggr"},
	[TOKEN_ALIGN] = {"TOKEN_ALIGN", "align"},
	[TOKEN_ALLOCA] = {"TOKEN_ALLOCA", "alloca"},
	[TOKEN_AND] = {"TOKEN_AND", "and"},
	[TOKEN_ASHR] = {"TOKEN_ASHR", "ashr"},
	[TOKEN_BIGENDIAN] = {"TOKEN_BIGENDIAN", "bigendian"},
	[TOKEN_BITCAST] = {"TOKEN_BITCAST", "bitcast"},
	[TOKEN_BLOCKLABEL] = {"TOKEN_BLOCKLABEL", "entry:"},
	[TOKEN_BR] = {"TOKEN_BR", "br"},
	[TOKEN_CALL] = {"TOKEN_CALL", "call"},
	[TOKEN_CLEANUP] = {"TOKEN_CLEANUP", "cleanup"},
	[TOKEN_CLOSE_ANGLE_BRACKET] = {"TOKEN_CLOSE_ANGLE_BRACKET", ">"},
	[TOKEN_CLOSE_BRACKET] = {"TOKEN_CLOSE_BRACKET", "]"},
	[TOKEN_CLOSE_CURLY_BRACKET] = {"TOKEN_CLOSE_CURLY_BRACKET", "}"},
	[TOKEN_CLOSE_PAREN] = {"TOKEN_CLOSE_PAREN", "]"},
	[TOKEN_CMPXCHG] = {"TOKEN_CMPXCHG", "cmpxchg"},
	[TOKEN_COMMA] = {"TOKEN_COMMA", ","},
	[TOKEN_COMDAT] = {"TOKEN_COMDAT", "comdat"},
	[TOKEN_DECLARE] = {"TOKEN_DECLARE", "declare"},
	[TOKEN_DEFINE] = {"TOKEN_DEFINE", "define"},
	[TOKEN_DOTDOTDOT] = {"TOKEN_DOTDOTDOT", "..."},
	[TOKEN_DOUBLE] = {"TOKEN_DOUBLE", "double"},
	[TOKEN_DSO_LOCAL] = {"TOKEN_DSO_LOCAL", "dso_local"},
	[TOKEN_DLLIMPORT] = {"TOKEN_DLL_IMPORT", "dllimport"},
	[TOKEN_EQUALS] = {"TOKEN_EQUALS", "="},
	[TOKEN_EQ] = {"TOKEN_EQ", "eq"},
	[TOKEN_EXACT] = {"TOKEN_EXACT", "exact"},
	[TOKEN_EXTRACTVALUE] = {"TOKEN_EXTRACTVALUE", "extractvalue"},
	[TOKEN_FADD] = {"TOKEN_FADD", "fadd"},
	[TOKEN_FALSE] = {"TOKEN_FALSE", "false"},
	[TOKEN_FCMP] = {"TOKEN_FCMP", "fcmp"},
	[TOKEN_FDIV] = {"TOKEN_FDIV", "fdiv"},
	[TOKEN_FENCE] = {"TOKEN_FENCE", "fence"},
	[TOKEN_FLOAT] = {"TOKEN_FLOAT", "float"},
	[TOKEN_FLOAT_LITERAL] = {"TOKEN_FLOAT_LITERAL", "1.1e+1"},
	[TOKEN_FMUL] = {"TOKEN_FMUL", "fmul"},
	[TOKEN_FPEXT] = {"TOKEN_FPEXT", "fpext"},
	[TOKEN_FPTOUI] = {"TOKEN_FPTOUI", "fptoui"},
	[TOKEN_FPTOSI] = {"TOKEN_FPTOSI", "fptosi"},
	[TOKEN_FREE] = {"TOKEN_FREE", "free"},
	[TOKEN_FREM] = {"TOKEN_FREM", "frem"},
	[TOKEN_FNEG] = {"TOKEN_FNEG", "fneg"},
	[TOKEN_FSUB] = {"TOKEN_FSUB", "fsub"},
	[TOKEN_GETELEMENTPTR] = {"TOKEN_GETELEMENTPTR", "getelementptr"},
	[TOKEN_GLOBAL] = {"TOKEN_GLOBAL", "global"},
	[TOKEN_ICMP] = {"TOKEN_ICMP", "icmp"},
	[TOKEN_IDENTIFIER] = {"TOKEN_IDENTIFIER", "%identifier"},
	[TOKEN_IMMARG] = {"TOKEN_IMMARGS", "immargs"},
	[TOKEN_INBOUNDS] = {"TOKEN_INBOUNDS", "inbounds"},
	[TOKEN_INTEGERTYPE] = {"TOKEN_INTEGERTYPE", "i32"},
	[TOKEN_INTERNAL] = {"TOKEN_INTERNAL", "internal"},
	[TOKEN_INTTOPTR] = {"TOKEN_INTTOPTR", "inttoptr"},
	[TOKEN_INVOKE] = {"TOKEN_INVOKE", "invoke"},
	[TOKEN_LITTLEENDIAN] = {"TOKEN_LITTLEENDIAN", "littleendian"},
	[TOKEN_LINKONCE_ODR] = {"TOKEN_LINKONCE", "linkonce_odr"},
	[TOKEN_LABEL] = {"TOKEN_LABEL", "label"},
	[TOKEN_LANDINGPAD] = {"TOKEN_LANDINGPAD", "Landingpad"},
	[TOKEN_LOAD] = {"TOKEN_LOAD", "load"},
	[TOKEN_LSHR] = {"TOKEN_LSHR", "lshr"},
	[TOKEN_MALLOC] = {"TOKEN_MALLOC", "malloc"},
	[TOKEN_MUL] = {"TOKEN_MUL", "mul"},
	[TOKEN_NE] = {"TOKEN_NE", "ne"},
	[TOKEN_NOALIAS] = {"TOKEN_NOALIAS", "noalias"},
	[TOKEN_NOCAPTURE] = {"TOKEN_NOCAPTURE", "nocapture"},
	[TOKEN_NULL] = {"TOKEN_NULL", "null"},
	[TOKEN_NUW] = {"TOKEN_NUW", "nuw"},
	[TOKEN_NSW] = {"TOKEN_NSW", "nsw"},
	[TOKEN_OEQ] = {"TOKEN_OEQ", "oeq"},
	[TOKEN_OPEN_CURLY_BRACKET] = {"TOKEN_OPEN_CURLY_BRACKET", "{"},
	[TOKEN_OPEN_ANGLE_BRACKET] = {"TOKEN_OPEN_ANGLE_BRACKET", "<"},
	[TOKEN_OPEN_BRACKET] = {"TOKEN_OPEN_BRACKET", "["},
	[TOKEN_OPEN_PAREN] = {"TOKEN_OPEN_PAREN", "("},
	[TOKEN_OR] = {"TOKEN_OR", "or"},
	[TOKEN_OGE] = {"TOKEN_OGE", "oge"},	
	[TOKEN_OGT] = {"TOKEN_OGT", "ogt"},
	[TOKEN_OLE] = {"TOKEN_OLE", "ole"},
	[TOKEN_OLT] = {"TOKEN_OLT", "olt"},
	[TOKEN_PERSONALITY] = {"TOKEN_PERSONALITY", "personality"},
	[TOKEN_PHI] = {"TOKEN_PHI", "phi"},
	[TOKEN_PTR] = {"TOKEN_PTR", "*"},
	[TOKEN_PTRTOINT] = {"TOKEN_PTRTOINT", "ptrtoint"},
	[TOKEN_READONLY] = {"TOKEN_READONLY", "readonly"},
	[TOKEN_RET] = {"TOKEN_RET", "ret"},
	[TOKEN_SDIV] = {"TOKEN_SDIV", "sdiv"},
	[TOKEN_SELECT] = {"TOKEN_SELECT", "select"},
	[TOKEN_SEXT] = {"TOKEN_SEXT", "sext"},
	[TOKEN_STRING] = {"TOKEN_STRING", "\"string\""},
	[TOKEN_SEQ_CST] = {"TOKEN_SEQ_CST", "seq_cst"},
	[TOKEN_SITOFP] = {"TOKEN_SITOFP", "sitofp"},
	[TOKEN_SGT] = {"TOKEN_SGT", "sgt"},
	[TOKEN_SGE] = {"TOKEN_SGE", "sge"},
	[TOKEN_SLE] = {"TOKEN_SLE", "sle"},
	[TOKEN_SLT] = {"TOKEN_SLT", "slt"},
	[TOKEN_SHL] = {"TOKEN_SHL", "shl"},
	[TOKEN_STRING] = {"TOKEN_STRING", "\"string\""},
	[TOKEN_SREM] = {"TOKEN_SREM", "srem"},
	[TOKEN_STACK] = {"TOKEN_STACK", "stack"},
	[TOKEN_STORE] = {"TOKEN_STORE", "store"},
	[TOKEN_SUB] = {"TOKEN_SUB", "sub"},
	[TOKEN_SWITCH] = {"TOKEN_SWITCH", "switch"},
	[TOKEN_SYNCSCOPE] = {"TOKEN_SYNCSCOPE", "syncscope"},
	[TOKEN_SZ] = {"TOKEN_SZ", "1"},
	[TOKEN_TAIL] = {"TOKEN_TAIL", "tail"},
	[TOKEN_TO] = {"TOKEN_TO", "to"},
	[TOKEN_TRUNC] = {"TOKEN_TRUNC", "trunc"},
	[TOKEN_TRUE] = {"TOKEN_TRUE", "true"},
	[TOKEN_TYP] = {"TOKEN_TYP", "typ"},
	[TOKEN_UDIV] = {"TOKEN_UDIV", "udiv"},
	[TOKEN_UITOFP] = {"TOKEN_UITOFP", "uitofp"},
	[TOKEN_UGE] = {"TOKEN_UGE", "uge"},
	[TOKEN_UGT] = {"TOKEN_UGT", "ugt"},
	[TOKEN_ULE] = {"TOKEN_ULE", "ule"},
	[TOKEN_UNE] = {"TOKEN_UNE", "une"},
	[TOKEN_ULT] = {"TOKEN_ULT", "ult"},
	[TOKEN_UNDEF] = {"TOKEN_UNDEF", "undef"},
	[TOKEN_UNWIND] = {"TOKEN_UNWIND", "unwind"},
	[TOKEN_UNREACHABLE] = {"TOKEN_UNREACHABLE", "unreachable"},
	[TOKEN_UREM] = {"TOKEN_UREM", "urem"},
	[TOKEN_VAL] = {"TOKEN_VAL", "val"},
	[TOKEN_VOID] = {"TOKEN_VOID", "void"},
	[TOKEN_VOID] = {"TOKEN_VOID", "void"},
	[TOKEN_VOLATILE] = {"TOKEN_VOLATILE", "volatile"},
	[TOKEN_X] = {"TOKEN_X", "x"},
	[TOKEN_XOR] = {"TOKEN_XOR", "xor"},
	[TOKEN_WRITEONLY] = {"TOKEN_WRITEONLY", "writeonly"},
	[TOKEN_ZEROINITIALIZER] = {"TOKEN_ZEROINITIALIZER", "zeroinitializer"},
	[TOKEN_ZEXT] = {"TOKEN_ZEXT", "zext"},
};

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
struct Token* make_token(token_name name, char* lexeme, int val) {
	token *tk = malloc(sizeof(token));
	// Copying Name.
	tk->name = name;
	// Copying lexeme.
	tk->lexeme = lexeme;
	// Copying value.
	tk->value = val;
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
state match(char* input, char* expected, state result) {
	if (strcmp(input, expected) == 0) {
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
						if (*ci++ == 'e') {
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
						current_state = match(ci, "riteonly", STATE_WRITEONLY);
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
					case '-':
						current_state = STATE_MINUS;
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
						current_state = match(
							ci, "", STATE_OPEN_CURLY_BRACKET);
						continue;
					case '}':
						current_state = match(
							ci, "", STATE_CLOSE_CURLY_BRACKET);
						continue;
					case '<':
						current_state = match(
							ci, "", STATE_OPEN_ANGLE_BRACKET);
						continue;
					case '>':
						current_state = match(
							ci, "", STATE_CLOSE_ANGLE_BRACKET);
						continue;
					case ',':
						current_state = match(ci, "", STATE_COMMA);
						continue;
					case '.':
						current_state = match(ci, "..", STATE_DOTDOTDOT);
						continue;
					case '"':
						while (*ci++ != '"') {
							if (*ci == NULL) {
								current_state = STATE_UNRECOGNISED;
								continue;
							}
						}
						if (*ci == NULL) {
							current_state = STATE_UNRECOGNISED;
						}
						current_state = STATE_STRING;
						continue;
					case '*':
						current_state = match(ci, "", STATE_PTR);
						continue;
					case '0': 
						if (*ci == 'x') {
							ci++;
							current_state = STATE_OX;
							continue;
						}
					case '1': case '2': case '3': 
					case '4': case '5': case '6': 
					case '7': case '8': case '9': 
						current_state = STATE_DIGITS;
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
			}
			break;

			case STATE_EX:
				switch (*ci++) {
					case 't':
						current_state = match(
							ci, "ractvalue", STATE_EXTRACTVALUE);
						continue;
					case 'a':
						current_state = match(ci, "ct", STATE_EXACT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				break;

			case STATE_ERROR_ENCOUNTERED:	
				return make_token(ERROR_TOKEN, input, NULL);
			case STATE_EXACT: 
				return make_token(TOKEN_EXACT, input, NULL);

			case STATE_UGT: 
				return make_token(TOKEN_UGT, input, NULL);

			case STATE_UL: 
				switch (*ci++) {
					case 't':
						current_state = match(ci, "", STATE_ULT);
						continue;
					case 'e':
						current_state = match(ci, "", STATE_ULE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				break;
			
			case STATE_ULT: 
				return make_token(TOKEN_ULT, input, NULL);
			case STATE_ULE: 
				return make_token(TOKEN_ULE, input, NULL);

			case STATE_UNRECOGNISED: 
				; char* c = input
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
						current_state = STATE_AS;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
				}
				break;

			case STATE_ADD: 
				return make_token(TOKEN_ADD, input, NULL);
			case STATE_AGGR: 
				return make_token(TOKEN_AGGR, input, NULL);

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

			case STATE_ALIGN: 
				return make_token(TOKEN_ALIGN, input, NULL);
			case STATE_ALLOCA: 
				return make_token(TOKEN_ALLOCA, input, NULL);
			case STATE_AND: 
				return make_token(TOKEN_AND, input, NULL);

			case STATE_AS:
				switch (*ci++) {
					case 'm':
						current_state = match(ci, "", STATE_ASM);
						continue;
					case 'h':
						current_state = match(ci, "r", STATE_ASHR);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_ASM: 
				return make_token(TOKEN_ASM, input, NULL);
			case STATE_ASHR: 
				return make_token(TOKEN_ASHR, input, NULL);

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
				return NULL;

			case STATE_BIGENDIAN: 
				return make_token(TOKEN_BIGENDIAN, input, NULL);
			case STATE_BITCAST: 
				return make_token(TOKEN_BITCAST, input, NULL);
			case STATE_BR: 
				return make_token(TOKEN_BR, input, NULL);

			case STATE_C:
				switch (*ci++) {
					case 'a':
						current_state = match(ci, "ll", STATE_CALL);
						continue;
					case 'l':
						current_state = match(ci, "eanup", STATE_CLEANUP);
						continue;
					case 'm':
						current_state = match(ci, "pxchg", STATE_CMPXCHG);
						continue;
					case 'o':
						current_state = match(ci, "mdat", STATE_COMDAT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_CALL: 
				return make_token(TOKEN_CALL, input, NULL);
			case STATE_CLEANUP: 
				return make_token(TOKEN_CLEANUP, input, NULL);
			case STATE_CLOSE_ANGLE_BRACKET: 
				return make_token(TOKEN_CLOSE_ANGLE_BRACKET, input, NULL);
			case STATE_CLOSE_CURLY_BRACKET: 
				return make_token(TOKEN_CLOSE_CURLY_BRACKET, input, NULL);
			case STATE_CLOSE_BRACKET: 
				return make_token(TOKEN_CLOSE_BRACKET, input, NULL);
			case STATE_CLOSE_PAREN: 
				return make_token(TOKEN_CLOSE_PAREN, input, NULL);
			case STATE_CMPXCHG: 
				return make_token(TOKEN_CMPXCHG, input, NULL);
			case STATE_COMDAT: 
				return make_token(TOKEN_COMDAT, input, NULL);
			case STATE_COMMA: 
				return make_token(TOKEN_COMMA, input, NULL);

			case STATE_D: 
				switch (*ci++) {
					case 'e':
						current_state = STATE_DE;
						continue;
					case 'l':
						current_state = match(
							ci, "limport", STATE_DLLIMPORT);
						continue;
					case 'o':
						current_state = match(
							ci, "uble", STATE_DOUBLE);
						continue;
					case 's':
						current_state = match(
							ci, "o_local", STATE_DSO_LOCAL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				};

			case STATE_DE: 
				switch (*ci++) {
					case 'c':
						current_state = match(ci, "lare", STATE_DECLARE);
						continue;
					case 'f':
						current_state = match(ci, "ine", STATE_DEFINE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_DECLARE: 
				return make_token(TOKEN_DECLARE, input, NULL);
			case STATE_DEFINE:
				return make_token(TOKEN_DEFINE, input, NULL);
			case STATE_DLLIMPORT: 
				return make_token(TOKEN_DLLIMPORT, input, NULL);
			case STATE_DOTDOTDOT: 
				return make_token(TOKEN_DOTDOTDOT, input, NULL);
			case STATE_DOUBLE: 
				return make_token(TOKEN_DOUBLE, input, NULL);
			case STATE_DSO_LOCAL: 
				return make_token(TOKEN_DSO_LOCAL, input, NULL);
			
			case STATE_E:
				switch (*ci++) {
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
				return make_token(TOKEN_EQ, input, NULL);
			case STATE_EQUALS: 
				return make_token(TOKEN_EQUALS, input, NULL);

			case STATE_F: 
				switch (*ci++) {
					case 'a': 
						current_state = STATE_FA;
						continue;
					case 'c':
						current_state = match(
							ci, "mp", STATE_FCMP);
						continue;
					case 'd':
						current_state = match(
							ci, "iv", STATE_FDIV);
						continue;
					case 'e':
						current_state = match(
							ci, "nce", STATE_FENCE);
						continue;
					case 'l':
						current_state = match(
							ci, "oat", STATE_FLOAT);
						continue;
					case 'm':
						current_state = match(
							ci, "ul", STATE_FMUL);
						continue;
					case 'n':
						current_state = match(
							ci, "eg", STATE_FNEG);
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
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "d", STATE_FADD);
						continue;
					case 'l':
						current_state = match(ci, "se", STATE_FALSE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				current_state = STATE_UNRECOGNISED;
				continue;

			case STATE_FALSE: 
				return make_token(TOKEN_FALSE, input, NULL);
			case STATE_FADD:
				return make_token(TOKEN_FADD, input, NULL);
			case STATE_FCMP: 
				return make_token(TOKEN_FCMP, input, NULL);
			case STATE_FDIV: 
				return make_token(TOKEN_FDIV, input, NULL);
			case STATE_FENCE: 
				return make_token(TOKEN_FENCE, input, NULL);
			case STATE_FLOAT: 
				return make_token(TOKEN_FLOAT, input, NULL);
			case STATE_FMUL: 
				return make_token(TOKEN_FMUL, input, NULL);
			case STATE_FNEG: 
				return make_token(TOKEN_FNEG, input, NULL);

			case STATE_FP:
				switch (*ci++) {
					case 'e':
						current_state = match(
							ci, "xt", STATE_FPEXT);
						continue;
					case 't':
						if (*(ci++) == 'o') {
							current_state = STATE_FPTO;
						}
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_FPEXT: 
				return  make_token(TOKEN_FPEXT, input, NULL);
				
			case STATE_FPTO:
				switch (*ci++) {
					case 'u':
						current_state = match(ci, "i", STATE_FPTOUI);
						continue;
					case 's':
						current_state = match(ci, "i", STATE_FPTOSI);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
						
				}
				return NULL;

			case STATE_FPTOUI: 
				return make_token(TOKEN_FPTOUI, input, NULL);
			case STATE_FPTOSI: 
				return make_token(TOKEN_FPTOSI, input, NULL);

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

			case STATE_FREE: 
				return make_token(TOKEN_FREE, input, NULL);
			case STATE_FREM: 
				return make_token(TOKEN_FREM, input, NULL);
			case STATE_FSUB: 
				return make_token(TOKEN_FSUB, input, NULL);

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

			case STATE_GETELEMENTPTR: 
				return make_token(TOKEN_GETELEMENTPTR, input, NULL);
			case STATE_GLOBAL: 
				return make_token(TOKEN_GLOBAL, input, NULL);

			case STATE_I: 
				switch (*ci++) {
					case 'c':
						current_state = match(
							ci, "mp", STATE_ICMP);
						continue;
					case 'm':
						current_state = match(
							ci, "marg", STATE_IMMARG);
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
						current_state = STATE_INT;
						continue;
					case 'v':
						current_state = match(ci, "oke", STATE_INVOKE);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_ICMP: 
				return make_token(TOKEN_ICMP, input, NULL);
			case STATE_IDENTIFIER: 
				return make_token(TOKEN_IDENTIFIER, input, NULL);
			case STATE_IMMARG: 
				return make_token(TOKEN_IMMARG, input, NULL);
			case STATE_INBOUNDS:  
				return make_token(TOKEN_INBOUNDS, input, NULL);
			case STATE_INVOKE: 
				return make_token(TOKEN_INVOKE, input, NULL);

			case STATE_INTEGERTYPE:
				switch (*ci++) {
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						current_state = STATE_INTEGERTYPE;
						continue;
					case NULL:
						return make_token(
							TOKEN_INTEGERTYPE, input, atoi(input+1));
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_INT:
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "rnal", STATE_INTERNAL);
						continue;
					case 't':
						current_state = match(ci, "optr", STATE_INTTOPTR);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_INTTOPTR: 
				return make_token(TOKEN_INTTOPTR, input, NULL);
			case STATE_INTERNAL: 
				return make_token(TOKEN_INTERNAL, input, NULL);

			case STATE_EXTRACTVALUE: 
				return make_token(TOKEN_EXTRACTVALUE, input, NULL);

			case STATE_L: 
				switch (*ci++) {
					case 'a':
						current_state = STATE_LA;
						continue;
					case 'i':
						current_state = STATE_LI;
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
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_LA:
				switch (*ci++) {
					case 'n':
						current_state = match(ci, "dingpad", STATE_LANDINGPAD);
						continue;
					case 'b':
						current_state = match(ci, "el", STATE_LABEL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_LI:
				switch (*ci++) {
					case 'n':
						current_state = match(
							ci, "konce_odr", STATE_LINKONCE_ODR);
						continue;
					case 't':
						current_state = match(
							ci, "tleendian", STATE_LITTLEENDIAN);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_LINKONCE_ODR:
				return make_token(TOKEN_LINKONCE_ODR, input, NULL);

			case STATE_BLOCK_LABEL: 
				return make_token(TOKEN_BLOCKLABEL, input, NULL);
			case STATE_LITTLEENDIAN: 
				return make_token(TOKEN_LITTLEENDIAN, input, NULL);
			case STATE_LANDINGPAD: 
				return make_token(TOKEN_LANDINGPAD, input, NULL);
			case STATE_LABEL:
				return make_token(TOKEN_LABEL, input, NULL);
			case STATE_LOAD: 
				return make_token(TOKEN_LOAD, input, NULL);
			case STATE_LSHR: 
				return make_token(TOKEN_LSHR, input, NULL);

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
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_MALLOC: 
				return make_token(TOKEN_MALLOC, input, NULL);

			case STATE_MINUS: 
				switch (*ci++) {
					case '0':
					case '1': case '2': case '3': 
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						current_state = STATE_DIGITS;
						continue;
				} 
				return NULL;

			case STATE_MUL: return make_token(TOKEN_MUL, input, NULL);

			case STATE_N: 
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "", STATE_NE);
						continue;
					case 'o':
						current_state = STATE_NO;
						continue;
					case 's':
						current_state = match(ci, "w", STATE_NSW);
						continue;
					case 'u':
						current_state = STATE_NU;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_NE: 
				return make_token(TOKEN_NE, input, NULL);

			case STATE_NU:
				switch (*ci++) {
					case 'w':
						current_state = STATE_NUW;
						continue;
					case 'l':
						current_state = match(ci, "l", STATE_NULL);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				

			case STATE_NO:
				switch (*ci++) {
					case 'c':
						current_state = match(ci, "apture", STATE_NOCAPTURE);
						continue;
					case 'a':
						current_state = match(ci, "lias", STATE_NOALIAS);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
			
			case STATE_NOALIAS: 
				return make_token(TOKEN_NOALIAS, input, NULL);
			case STATE_NOCAPTURE: 
				return make_token(TOKEN_NOCAPTURE, input, NULL);
			case STATE_NUW: 
				return make_token(TOKEN_NUW, input, NULL);
			case STATE_NSW: 
				return make_token(TOKEN_NSW, input, NULL);
			case STATE_NULL: 
				return make_token(TOKEN_NULL, input, NULL);

			case STATE_O:
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "q", STATE_OEQ);
						continue;
					case 'l':
						current_state = STATE_OL;
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
				return make_token(TOKEN_OEQ, input, NULL);

			case STATE_OG:
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "", STATE_OGE);;
						continue;
					case 't':
						current_state = match(ci, "", STATE_OGT);;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_OL:
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "", STATE_OLE);;
						continue;
					case 't':
						current_state = match(ci, "", STATE_OLT);;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_OPEN_ANGLE_BRACKET: 
				return make_token(TOKEN_OPEN_ANGLE_BRACKET, input, NULL);
			case STATE_OPEN_CURLY_BRACKET: 
				return make_token(TOKEN_OPEN_CURLY_BRACKET, input, NULL);
			case STATE_OPEN_BRACKET: 
				return make_token(TOKEN_OPEN_BRACKET, input, NULL);
			case STATE_OPEN_PAREN: 
				return make_token(TOKEN_OPEN_PAREN, input, NULL);
			case STATE_OGE: 
				return make_token(TOKEN_OGE, input, NULL);
			case STATE_OGT: 
				return make_token(TOKEN_OGT, input, NULL);
			case STATE_OLE: 
				return make_token(TOKEN_OLE, input, NULL);
			case STATE_OLT: 
				return make_token(TOKEN_OLT, input, NULL);
			case STATE_OR: 
				return make_token(TOKEN_OR, input, NULL);

			case STATE_OX:
				switch (*ci++) {
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
				switch (*ci++) {
					case '0': case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9':
					case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
						continue;
					case NULL:
						return make_token(TOKEN_SZ, input, strtol(input, NULL, 0));
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_P: 
				switch (*ci++) {
					case 'e':
						current_state = match(
							ci, "rsonality", STATE_PERSONALITY);
						continue;
					case 'h':
						current_state = match(
							ci, "i", STATE_PHI);
						continue;
					case 't':
						current_state = match(
							ci, "rtoint", STATE_PTRTOINT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				
			case STATE_PERSONALITY: 
				return make_token(TOKEN_PERSONALITY, input, NULL);
			case STATE_PHI: 
				return make_token(TOKEN_PHI, input, NULL);
			case STATE_PTR: 
				return make_token(TOKEN_PTR, input, NULL);
			case STATE_PTRTOINT: 
				return make_token(TOKEN_PTRTOINT, input, NULL);

			case STATE_RE:
				switch (*ci++) {
					case 't':
						current_state = match(ci, "", STATE_RET);
						continue;
					case 'a':
						current_state = match(ci, "donly", STATE_READONLY);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_RET: 
				return make_token(TOKEN_RET, input, NULL);
			case STATE_READONLY: 
				return make_token(TOKEN_READONLY, input, NULL);

			case STATE_S: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "iv", STATE_SDIV);
						continue;
					case 'e':
						current_state = STATE_SE;
						continue;
					case 'i':
						current_state = match(ci, "tofp", STATE_SITOFP);
						continue;
					case 'g':
						current_state = STATE_SG;
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
					case 'w':
						current_state = match(ci, "itch", STATE_SWITCH);
						continue;
					case 'y':
						current_state = match(ci, "ncscope", STATE_SYNCSCOPE);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
			
			case STATE_SITOFP: 
				return make_token(TOKEN_SITOFP, input, NULL);

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

			case STATE_SDIV: 
				return make_token(TOKEN_SDIV, input, NULL);

			case STATE_SE: 
				switch (*ci++) {
					case 'l':
						current_state = match(ci, "ect", STATE_SELECT);
						continue;
					case 'x':
						current_state = match(ci, "t", STATE_SEXT);
						continue;
					case 'q':
						current_state = match(ci, "_cst", STATE_SEQ_CST);
						continue;
					default: 
						current_state = STATE_UNRECOGNISED;
						continue;
				};

			case STATE_SELECT: 
				return make_token(TOKEN_SELECT, input, NULL);
			case STATE_SEQ_CST: 
				return make_token(TOKEN_SEQ_CST, input, NULL);
			case STATE_SEXT: 
				return make_token(TOKEN_SEXT, input, NULL);

			case STATE_SG:
				switch (*ci++) {
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
				return make_token(TOKEN_SGT, input, NULL);
			case STATE_SGE: 
				return make_token(TOKEN_SGE, input, NULL);
			case STATE_SHL: 
				return make_token(TOKEN_SHL, input, NULL);
			case STATE_SLT: 
				return make_token(TOKEN_SLT, input, NULL);
			case STATE_SLE: 
				return make_token(TOKEN_SLE, input, NULL);
		    case STATE_SREM: 
				return make_token(TOKEN_SREM, input, NULL);

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

			case STATE_STACK: 
				return make_token(TOKEN_STACK, input, NULL);
			case STATE_STORE: 
				return make_token(TOKEN_STORE, input, NULL);
			case STATE_STRING: 
				return make_token(TOKEN_STRING, input, NULL);
			case STATE_SUB: 
				return make_token(TOKEN_SUB, input, NULL);
			case STATE_SWITCH: 
				return make_token(TOKEN_SWITCH, input, NULL);
			case STATE_SYNCSCOPE: 
				return make_token(TOKEN_SYNCSCOPE, input, NULL);
			
			case STATE_DIGITS: 
				switch (*ci++) {
					case '0':
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						continue;
					case '.':
						current_state = STATE_DIGITS_DOT;
						continue;
					case NULL:
						return make_token(TOKEN_SZ, input, atoi(input));
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_DIGITS_DOT:
				switch (*ci++) {
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
				switch (*ci++) {
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
				switch (*ci++) {
					case '+': case '-':
						current_state = STATE_DIGITS_DOT_DIGITS_E_SIGN;
						continue;
					default:
						return NULL;
				}
				return NULL;

			case STATE_DIGITS_DOT_DIGITS_E_SIGN:
				switch (*ci++) {
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
				switch (*ci++) {
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
				return make_token(TOKEN_FLOAT_LITERAL, input, 0XDEADBEEF);

			case STATE_T: 
				switch (*ci++) {
					case 'a':
						current_state = match(ci, "il", STATE_TAIL);
						continue;
					case 'o':
						current_state = match(ci, "", STATE_TO);
						continue;
					case 'r':
						current_state = STATE_TR;
						continue;
					case 'y':
						current_state = match(ci, "p", STATE_TYP);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_TAIL:
				return make_token(TOKEN_TAIL, input, NULL);
			case STATE_TO:
				return make_token(TOKEN_TO, input, NULL);

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

			case STATE_TRUNC: 
				return make_token(TOKEN_TRUNC, input, NULL);
			case STATE_TRUE: 
				return make_token(TOKEN_TRUE, input, NULL);
			case STATE_TYP: 
				return make_token(TOKEN_TYP, input, NULL);

			case STATE_U: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "iv", STATE_UDIV);
						continue;
					case 'g':
						current_state = STATE_UG;
						continue;
					case 'i':
						current_state = match(ci, "tofp", STATE_UITOFP);
						continue;
					case 'l':
						current_state = STATE_UL;
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
				return NULL;

			case STATE_UITOFP: 
				return make_token(TOKEN_UITOFP, input, NULL);

			case STATE_UG:
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "", STATE_UGE);
						continue;
					case 't':
						current_state = match(ci, "", STATE_UGT);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_UGE: 
				return make_token(TOKEN_UGE, input, NULL);
			case STATE_UDIV: 
				return make_token(TOKEN_UDIV, input, NULL);

			case STATE_UN: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "ef", STATE_UNDEF);
						continue;
					case 'e':
						current_state = match(ci, "", STATE_UNE);
						continue;
					case 'r':
						current_state = match(
							ci, "eachable", STATE_UNREACHABLE);
						continue;
					case 'w':
						current_state = match(ci, "ind", STATE_UNWIND);
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_UNE: 
				return make_token(TOKEN_UNE, input, NULL);
			case STATE_UNDEF: 
				return make_token(TOKEN_UNDEF, input, NULL);
			case STATE_UNREACHABLE: 
				return make_token(TOKEN_UNREACHABLE, input, NULL);
			case STATE_UNWIND: 
				return make_token(TOKEN_UNWIND, input, NULL);
			case STATE_UREM: 
				return make_token(TOKEN_UREM, input, NULL);

			case STATE_V: 
				switch (*ci++) {
					case 'a':;
						current_state = match(ci, "l", STATE_VAL);
						continue;
					case 'o':;
						current_state = STATE_VO;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}

			case STATE_VAL: return make_token(TOKEN_VAL, input, NULL);

			case STATE_VO:
				switch (*ci++) {
					case 'l':
						current_state = match(ci, "atile", STATE_VOLATILE);
						continue;
					case 'i':
						current_state = match(ci, "d", STATE_VOID);;
						continue;
					default:
						current_state = STATE_UNRECOGNISED;
						continue;
				}
				return NULL;

			case STATE_VOLATILE: 
				return make_token(TOKEN_VOLATILE, input, NULL);
			case STATE_VOID: 
				return make_token(TOKEN_VOID, input, NULL);
			
			case STATE_X: 
				return make_token(TOKEN_X, input, NULL);
			case STATE_XOR: 
				return make_token(TOKEN_XOR, input, NULL);
			
			case STATE_WRITEONLY: 
				return make_token(TOKEN_WRITEONLY, input, NULL);

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

			case STATE_ZEROINITIALIZER: 
				return make_token(TOKEN_ZEROINITIALIZER, input, NULL);
			case STATE_ZEXT: 
				return make_token(TOKEN_ZEXT, input, NULL);

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
void  tokenizer(char** input, token **output) {
	// For each word in input
	char** lexemes = input;
	char* lexeme = *lexemes++;
	token** dest = output;
	while (lexeme != NULL) {

		if (DEBUG_TOKENIZER) {
			printf("READ  : %-32s %8s", lexeme, " ");
		}

		token *tk = tokenize(lexeme);
		
		if (DEBUG_TOKENIZER) {
			printf("WROTE : %-32s\n",
				lookup_token_as_name((*tk).name));
		}
		lexeme = *lexemes++;
		*dest++ = tk;
	}
	// Finish the tokens off with NULL
	*dest = NULL;
}

/** 
* Responsible for determining if the preprocessor should ignore these
* input symbols. It differs from is_delimiter as whitespace is never
* used in the output, while a delimiter may be the start of a 
* new word and while constitute the next word.
* 
* @param chr The character which we validate as whitespace.
*/
 bool is_whitespace(char chr) {
	switch (chr) {
		case ' ': case '\t': 
		case '\n': case '\r':
		case '\v': case '\f':
		case EOF: case NULL:
			return true;
		default: 
			return false;
	}
}

/**
* Responsible for informing the preprocessor as to if the word should 
* finish and restart anew including this given symbol.
*/
bool is_delimiter(char chr) {
	switch (chr) {
		case '[': case ']': 
		case '(': case ')':
		case '<': case '>':
		case '{': case '}':
		case ',': case ';': 
		case '*':
			return true;
		default: 
			return false;
	}
}

/* The various states used by the preprocessor */
typedef enum preprocessor_state {
	EXPECTING_WORD,		/* We expect a word to start (not started yet) */
	IN_WORD,			/* A new word has already started				*/
	IN_COMMENT,			/* We are in the middle of a comment			*/
	ENCOUNTERED_WORD,	/* We have already encountered the full word	*/
} preprocessor_state;

/** 
* Responsible for splitting an input string into 
* substrings each containing a single word.
* Assuming a well-formed input, one token
* can be made from each word.
* 
* @param input The input to preprocess.
* @param output The location to store the preprocessed output.
* 
* @see is_whitespace
* @see is_delimiter
*/
void preprocessor(char* input, char **output) {
	char buffer[PREPROCESSOR_WORD_SIZE];

	long oi = 0, bi = 0; // output and buffer index
	char* ci = input; // Character index
	// For each character in input
	bool in_identifier = false, in_whitespace = false;

	preprocessor_state state = EXPECTING_WORD;

	while ((*ci != NULL && *ci != EOF && ci != NULL) 
		   || state != EXPECTING_WORD) {
		
		if (DEBUG_PREPROCESSOR) {
			char* debug_msg = "Preprocessor : "
				"Output Index = %d, "
				"Buffer Index = %-3d, "
				"Character Index = %-3d, "
				"State = %d, "
				"Character = %c\n";
			printf(debug_msg, oi, bi, ci, state, *ci);
		}

		switch (state) {

			case EXPECTING_WORD:

				// Comments have highest precendence
				if (*ci == ';') { 
					ci++;
					state = IN_COMMENT;
					continue;
				}
				
				// Identifiers have lower precedence than comments
				if (*ci == '"') { 
					in_identifier = true;
					buffer[bi++] = *ci++;
					state = IN_WORD;
					continue;
				}

				if (is_whitespace(*ci)) {
					ci++;
					continue;
				}

				if (is_delimiter(*ci)) {
					buffer[bi++] = *ci++;
					state = ENCOUNTERED_WORD;
					continue;
				}

				buffer[bi++] = *ci++;
				state = IN_WORD;
				continue;

			case IN_WORD:

				if (in_identifier) {

					if (*ci == '"') {
						in_identifier = false;
					}
					buffer[bi++] = *ci++;
					continue;
				}

				if (*ci == '"') {
					in_identifier = true;
					buffer[bi++] = *ci++;
					continue;
				}
				
				if (*ci == ';') {
					state = ENCOUNTERED_WORD;
					continue;
				}

				if (is_whitespace(*ci)) {
					state = ENCOUNTERED_WORD;
					continue;
				}

				if (is_delimiter(*ci)) {
					state = ENCOUNTERED_WORD;
					continue;
				}

				buffer[bi++] = *ci++;
				continue;

			case IN_COMMENT:
				if (*ci == '\n') {
					state = EXPECTING_WORD;
				}
				ci++;
				continue;

			case ENCOUNTERED_WORD:
				/* Copy the word as encountered so far into the output */
				buffer[bi] = NULL;
				int final_word_len = strlen(buffer) + 1;
				char *final_word = malloc(final_word_len);
				if (final_word == NULL) {
					printf(
						"Fatal Error : "
						"Failed to allocate memory to store word %s.\n",
						buffer);
					return NULL;
				}
				strcpy_s(final_word, final_word_len, buffer);
				final_word[final_word_len-1] = NULL;

				output[oi++] = final_word;
				bi = 0;
				
				state = EXPECTING_WORD;
				continue;
		}

	}
	/* Now we mark the end of the output with a NULL. */
	output[oi] = NULL;
}

/** 
* Reponsible for testing an individual case of tokenize and printing a short
* report on the result.
* 
* @param gave The input we give the tokenize function.
* @param expected The output we expect in terms of the tokens name.
* 
* @see tokenize
*/
void test_tokenize_case(char* gave, token_name expected) {
	token *got = tokenize(gave);
	printf("Gave %-16s expected %-22s got %-22s\n", 
		gave, 
		lookup_token_as_name(expected), 
		lookup_token_as_name((*got).name));
}

/** 
* Reponsible for testing many cases of tokenize and prints
* a report on each result.
*/
void test_tokenize() {
	for (token_name i = TOKEN_ADD; i < TOKEN_ZEXT; i++) {
		test_tokenize_case(lookup_token_as_lexeme(i), i);
	}
}

/**
* Reponsible for printing a token and its contents.
* 
* @param tk the token to print
*/
void print_token(token** tk) {
	char* field_format = "%-24s\t%-16s\t%-8d\n";
	printf(field_format,
		lookup_token_as_name((**tk).name),
		(**tk).lexeme,
		(**tk).value);
}

/** 
* Responsible for printing a given list of pointers to tokens, 
* including all of their field contents.
* 
* @param tokens 
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