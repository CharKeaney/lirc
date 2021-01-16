#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define PREPROCESSOR_BUFFER_SIZE (1 << 8)
#define PREPROCESSOR_WORD_SIZE (1 << 4)
#define TOKENS_BUFFER_SIZE (1 << 8)

#define DEBUG_PREPROCESSOR false
#define DEBUG_TOKENIZE false
#define DEBUG_TOKENIZER false
#define DEBUG_TEST_TOKENIZER true

#define lookup_token_as_name(a) token_name_lookup[a][0]
#define lookup_token_as_lexeme(a) token_name_lookup[a][1]

// A list of tokens that can be recognised.
typedef enum {
	ERROR_TOKEN,
	TOKEN_ADD,
	TOKEN_AGGR,
	TOKEN_ALLOCA,
	TOKEN_AND,
	TOKEN_ASHR,
	TOKEN_BIGENDIAN,
	TOKEN_BITCAST,
	TOKEN_BR,
	TOKEN_CALL,
	TOKEN_COMMA,
	TOKEN_DECLARE,
	TOKEN_DEFINE,
	TOKEN_DOUBLE,
	TOKEN_EQUALS,
	TOKEN_FADD,
	TOKEN_FCMP,
	TOKEN_FDIV,
	TOKEN_FLOAT,
	TOKEN_FMUL,
	TOKEN_FPEXT,
	TOKEN_FPTOUI,
	TOKEN_FREE,
	TOKEN_FREM,
	TOKEN_FSUB,
	TOKEN_GETELEMENTPTR,
	TOKEN_GLOBAL,
	TOKEN_ICMP,
	TOKEN_IDENTIFIER,
	TOKEN_INTEGERTYPE,
	TOKEN_INTTOPTR,
	TOKEN_LITTLEENDIAN,
	TOKEN_LOAD,
	TOKEN_LSHR,
	TOKEN_MALLOC,
	TOKEN_MUL,
	TOKEN_NULL,
	TOKEN_OR,
	TOKEN_PHI,
	TOKEN_PTRTOINT,
	TOKEN_RET,
	TOKEN_SDIV,
	TOKEN_SELECT,
	TOKEN_SEXT,
	TOKEN_SHL,
	TOKEN_SREM,
	TOKEN_STACK,
	TOKEN_STORE,
	TOKEN_SUB,
	TOKEN_TO,
	TOKEN_TRUNC,
	TOKEN_TYP,
	TOKEN_UDIV,
	TOKEN_UNDEF,
	TOKEN_UNREACHABLE,
	TOKEN_UREM,
	TOKEN_VAL,
	TOKEN_VOID,
	TOKEN_XOR,
	TOKEN_ZEROINITIALIZER,
	TOKEN_ZEXT,
} token;

char* token_name_lookup[][2] = {
	[TOKEN_ADD] = {"TOKEN_ADD", "add"},
	[TOKEN_AGGR] = {"TOKEN_AGGR", "aggr"},
	[TOKEN_ALLOCA] = {"TOKEN_ALLOCA", "alloca"},
	[TOKEN_AND] = {"TOKEN_AND", "and"},
	[TOKEN_ASHR] = {"TOKEN_ASHR", "ashr"},
	[TOKEN_BIGENDIAN] = {"TOKEN_BIGENDIAN", "bigendian"},
	[TOKEN_BITCAST] = {"TOKEN_BITCAST", "bitcast"},
	[TOKEN_BR] = {"TOKEN_BR", "br"},
	[TOKEN_BR] = {"TOKEN_BR", "br"},
	[TOKEN_CALL] = {"TOKEN_CALL", "call"},
	[TOKEN_COMMA] = {"TOKEN_COMMA", ","},
	[TOKEN_DECLARE] = {"TOKEN_DECLARE", "declare"},
	[TOKEN_DEFINE] = {"TOKEN_DEFINE", "define"},
	[TOKEN_DOUBLE] = {"TOKEN_DOUBLE", "double"},
	[TOKEN_EQUALS] = {"TOKEN_EQUALS", "="},
	[TOKEN_FADD] = {"TOKEN_FADD", "fadd"},
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
	[TOKEN_INTEGERTYPE] = {"TOKEN_INTEGERTYPE", "i32"},
	[TOKEN_INTTOPTR] = {"TOKEN_INTTOPTR", "inttoptr"},
	[TOKEN_LITTLEENDIAN] = {"TOKEN_LITTLEENDIAN", "littleendian"},
	[TOKEN_LOAD] = {"TOKEN_LOAD", "load"},
	[TOKEN_LSHR] = {"TOKEN_LSHR", "lshr"},
	[TOKEN_MALLOC] = {"TOKEN_MALLOC", "malloc"},
	[TOKEN_MUL] = {"TOKEN_MUL", "mul"},
	[TOKEN_NULL] = {"TOKEN_NULL", "null"},
	[TOKEN_OR] = {"TOKEN_OR", "or"},
	[TOKEN_PHI] = {"TOKEN_PHI", "phi"},
	[TOKEN_PTRTOINT] = {"TOKEN_PTRTOINT", "ptrtoint"},
	[TOKEN_RET] = {"TOKEN_RET", "ret"},
	[TOKEN_SDIV] = {"TOKEN_SDIV", "sdiv"},
	[TOKEN_SELECT] = {"TOKEN_SELECT", "select"},
	[TOKEN_SEXT] = {"TOKEN_SEXT", "sext"},
	[TOKEN_SHL] = {"TOKEN_SHL", "shl"},
	[TOKEN_SREM] = {"TOKEN_SREM", "srem"},
	[TOKEN_STACK] = {"TOKEN_STACK", "stack"},
	[TOKEN_STORE] = {"TOKEN_STORE", "store"},
	[TOKEN_SUB] = {"TOKEN_SUB", "sub"},
	[TOKEN_TO] = {"TOKEN_TO", "to"},
	[TOKEN_TRUNC] = {"TOKEN_TRUNC", "trunc"},
	[TOKEN_TYP] = {"TOKEN_TYP", "typ"},
	[TOKEN_UDIV] = {"TOKEN_UDIV", "udiv"},
	[TOKEN_UNDEF] = {"TOKEN_UNDEF", "undef"},
	[TOKEN_UNREACHABLE] = {"TOKEN_UNREACHABLE", "unreachable"},
	[TOKEN_UREM] = {"TOKEN_UREM", "urem"},
	[TOKEN_VAL] = {"TOKEN_VAL", "val"},
	[TOKEN_VOID] = {"TOKEN_VOID", "void"},
	[TOKEN_VOID] = {"TOKEN_VOID", "void"},
	[TOKEN_XOR] = {"TOKEN_XOR", "xor"},
	[TOKEN_ZEROINITIALIZER] = {"TOKEN_ZEROINITIALIZER", "zeroinitializer"},
	[TOKEN_ZEXT] = {"TOKEN_ZEXT", "zext"},
};

// A list of states that the lexer can have.
typedef enum {
	BEGIN_STATE,
	END_STATE,
	ERROR_STATE,
	STATE_A,
	STATE_ADD,
	STATE_AGGR,
	STATE_ALLOCA,
	STATE_AND,
	STATE_ASHR,
	STATE_B,
	STATE_BI,
	STATE_BIGENDIAN,
	STATE_BITCAST,
	STATE_BR,
	STATE_CALL,
	STATE_COMMA,
	STATE_D,
	STATE_DE,
	STATE_DECLARE,
	STATE_DEFINE,
	STATE_DOUBLE,
	STATE_EQUALS,
	STATE_F,
	STATE_FADD,
	STATE_FCMP,
	STATE_FDIV,
	STATE_FLOAT,
	STATE_FMUL,
	STATE_FP,
	STATE_FPEXT,
	STATE_FPTOUI,
	STATE_FRE,
	STATE_FREE,
	STATE_FREM,
	STATE_FSUB,
	STATE_G,
	STATE_GETELEMENTPTR,
	STATE_GLOBAL,
	STATE_I,
	STATE_ICMP,
	STATE_IDENTIFIER,
	STATE_INTEGERTYPE,
	STATE_INTTOPTR,
	STATE_L,
	STATE_LITTLEENDIAN,
	STATE_LOAD,
	STATE_LSHR,
	STATE_M,
	STATE_MALLOC,
	STATE_MUL,
	STATE_NULL,
	STATE_OR,
	STATE_P,
	STATE_PHI,
	STATE_PTRTOINT,
	STATE_RET,
	STATE_S,
	STATE_SDIV,
	STATE_SE,
	STATE_SELECT,
	STATE_SEXT,
	STATE_SHL,
	STATE_SREM,
	STATE_ST,
	STATE_STACK,
	STATE_STORE,
	STATE_SUB,
	STATE_T,
	STATE_TO,
	STATE_TRUNC,
	STATE_TYP,
	STATE_U,
	STATE_UDIV,
	STATE_UN,
	STATE_UNDEF,
	STATE_UNREACHABLE,
	STATE_UREM,
	STATE_V,
	STATE_VAL,
	STATE_VOID,
	STATE_XOR,
	STATE_ZE,
	STATE_ZEROINITIALIZER,
	STATE_ZEXT
} state;

state match(char* input, char* expected, state result) {
	if (strcmp(input, expected) == 0) {
		return result;
	}
}

token tokenize(char* input) {
	char* ci = input;
	state current_state = BEGIN_STATE;
	token tk;
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
						current_state = match(
							ci, "ull", STATE_NULL);
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
						current_state = match(ci, "or", STATE_XOR);
						continue;
					case 'z':
						if (*ci++ == 'e') {
							current_state = STATE_ZE;
							continue;
						}
						else {
							current_state = ERROR_STATE;
							continue;
						}
					case '%':
						current_state = STATE_IDENTIFIER;
						continue;
					case '=':
						current_state = match(ci, "", STATE_EQUALS);
						continue;
					case ',':
						current_state = match(ci, "", STATE_COMMA);
						continue;
					default: 
						current_state = ERROR_STATE;
						continue;
				}
				break;

			case ERROR_STATE: return ERROR_TOKEN;

			case STATE_A: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "d", STATE_ADD);
						continue;
					case 'g':
						current_state = match(ci, "gr", STATE_AGGR);
						continue;
					case 'l':
						current_state = match(ci, "loca", STATE_ALLOCA);
						continue;
					case 'n':
						current_state = match(ci, "d", STATE_AND);
						continue;
					case 's':
						current_state = match(ci, "hr", STATE_ASHR);
						continue;
					default:
						current_state = ERROR_STATE;
				}
				break;

			case STATE_ADD: return TOKEN_ADD;
			case STATE_AGGR: return TOKEN_AGGR;
			case STATE_ALLOCA: return TOKEN_ALLOCA;
			case STATE_AND: return TOKEN_AND;
			case STATE_ASHR: return TOKEN_ASHR;

			case STATE_B: 
				switch (*ci++) {
					case 'i': 
						current_state = STATE_BI;
						continue;
					case 'r': 
						current_state = STATE_BR;
						continue;
					default:
						current_state = ERROR_STATE;
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
						current_state = ERROR_STATE;
						continue;
				}
				break;

			case STATE_BIGENDIAN: return TOKEN_BIGENDIAN;
			case STATE_BITCAST: return TOKEN_BITCAST;
			case STATE_BR: return TOKEN_BR;

			case STATE_CALL: return TOKEN_CALL;
			case STATE_COMMA: return TOKEN_COMMA;

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
						current_state = ERROR_STATE;
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
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_DECLARE: return TOKEN_DECLARE;
			case STATE_DEFINE: return TOKEN_DEFINE;
			case STATE_DOUBLE: return TOKEN_DOUBLE;

			case STATE_EQUALS: return TOKEN_EQUALS;

			case STATE_F: 
				switch (*ci++) {
					case 'a': 
						current_state = match(
							ci, "dd", STATE_FADD);
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
								current_state = ERROR_STATE;
								continue;
						}
					case 's':
						current_state = match(ci, "ub", STATE_FSUB);
						continue;
					default: 
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_FADD: return TOKEN_FADD;
			case STATE_FCMP: return TOKEN_FCMP;
			case STATE_FDIV: return TOKEN_FDIV;
			case STATE_FLOAT: return TOKEN_FLOAT;
			case STATE_FMUL: return TOKEN_FMUL;

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
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_FPEXT: return TOKEN_FPEXT;
			case STATE_FPTOUI: return TOKEN_FPTOUI;


			case STATE_FRE: 
				switch (*ci++) {
					case 'e':
						current_state = match(ci, "", STATE_FREE);
						continue;
					case 'm':
						current_state = match(ci, "", STATE_FREM);
						continue;
					default:
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_FREE: return TOKEN_FREE;
			case STATE_FREM: return TOKEN_FREM;
			case STATE_FSUB: return TOKEN_FSUB;

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
						current_state = ERROR_STATE;
						continue;
				}
			case STATE_GETELEMENTPTR: return TOKEN_GETELEMENTPTR;
			case STATE_GLOBAL: return TOKEN_GLOBAL;

			case STATE_I: 
				switch (*ci++) {
					case 'c':
						current_state = match(
							ci, "mp", STATE_ICMP);
						continue;
					case 'n':
						if (*ci++ == 't') {
							current_state = match(
								ci, "toptr", STATE_INTTOPTR);
							continue;
						}
						else {
							current_state = ERROR_STATE;
							continue;
						}
					case '0': 
					case '1': case '2': case '3':
					case '4': case '5': case '6': 
					case '7': case '8': case '9':
						current_state = STATE_INTEGERTYPE;
						continue;
					default:
						current_state = ERROR_STATE;;
						continue;
				}

			case STATE_ICMP: return TOKEN_ICMP;
			case STATE_IDENTIFIER: return TOKEN_IDENTIFIER;

			case STATE_INTEGERTYPE:
				switch (*ci++) {
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9':
						current_state = STATE_INTEGERTYPE;
						continue;
					case NULL:
						return TOKEN_INTEGERTYPE;
					default:
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_INTTOPTR: return TOKEN_INTTOPTR;

			case STATE_L: 
				switch (*ci++) {
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

			case STATE_LITTLEENDIAN: return TOKEN_LITTLEENDIAN;
			case STATE_LOAD: return TOKEN_LOAD;
			case STATE_LSHR: return TOKEN_LSHR;

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

			case STATE_MALLOC: return TOKEN_MALLOC;
			case STATE_MUL: return TOKEN_MUL;

			case STATE_NULL: return TOKEN_NULL;

			case STATE_OR: return TOKEN_OR;

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
					current_state = ERROR_STATE;
					continue;
				}
				

			case STATE_PHI: return TOKEN_PHI;
			case STATE_PTRTOINT: return TOKEN_PTRTOINT;

			case STATE_RET: return TOKEN_RET;

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
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_SDIV: return TOKEN_SDIV;

			case STATE_SE: 
				switch (*ci++) {
				case 'l':
					current_state = match(ci, "ect", STATE_SELECT);
					continue;
				case 'x':
					current_state = match(ci, "t", STATE_SEXT);
					continue;
				default: 
					current_state = ERROR_STATE;
					continue;
				};

			case STATE_SELECT: return TOKEN_SELECT;
			case STATE_SEXT: return TOKEN_SEXT;
			case STATE_SHL: return TOKEN_SHL;
			case STATE_SREM: return TOKEN_SREM;

			case STATE_ST: 
				switch (*ci++) {
				case 'a':
					current_state = match(ci, "ck", STATE_STACK);
					continue;
				case 'o':
					current_state = match(ci, "re", STATE_STORE);
					continue;
				default: 
					current_state = ERROR_STATE;
					continue;
				}

			case STATE_STACK: return TOKEN_STACK;
			case STATE_STORE: return TOKEN_STORE;
			case STATE_SUB: return TOKEN_SUB;

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
					current_state = ERROR_STATE;
					continue;
				}

			case STATE_TO: return TOKEN_TO;
			case STATE_TRUNC: return TOKEN_TRUNC;
			case STATE_TYP: return TOKEN_TYP;

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
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_UDIV: return TOKEN_UDIV;

			case STATE_UN: 
				switch (*ci++) {
					case 'd':
						current_state = match(ci, "ef", STATE_UNDEF);
						continue;
					case 'r':
						current_state = match(ci, "eachable", STATE_UNREACHABLE);
						continue;
					default:
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_UNDEF: return TOKEN_UNDEF;
			case STATE_UNREACHABLE: return TOKEN_UNREACHABLE;
			case STATE_UREM: return TOKEN_UREM;

			case STATE_V: 
				switch (*ci++) {
					case 'a':;
						current_state = match(ci, "l", STATE_VAL);
						continue;
					case 'o':;
						current_state = match(ci, "id", STATE_VOID);
						continue;
					default:
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_VAL: return TOKEN_VAL;
			case STATE_VOID: return TOKEN_VOID;
			case STATE_XOR: return TOKEN_XOR;

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
						current_state = ERROR_STATE;
						continue;
				}

			case STATE_ZEROINITIALIZER: return TOKEN_ZEROINITIALIZER;
			case STATE_ZEXT: return TOKEN_ZEXT;
		}
	}
}

/* tokenizer:
*	Takes preprocessed input and outputs a series of tokens.
*/
token* tokenizer(char** input, token* output) {
	// For each word in input
	char** lexemes = input;
	char* lexeme = *input;
	token* dest = output;
	while (lexeme != NULL) {

		if (DEBUG_TOKENIZER) {
			printf("READ  : %-16s\t", lexeme);
		}

		token tk = tokenize(lexeme);
		
		if (DEBUG_TOKENIZER) {
			printf("WROTE : %-10s\n",
				lookup_token_as_name(tk));
		}
		lexeme = *lexemes++;
		*dest++ = tk;
	}
	// Finish the tokens off with NULL
	*dest = EOF;
}

/* is_whitespace:
* 
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

/* is_delimiter:
*	Informs the preprocessor as to if the word should 
*	continue up to and including this symbol
*/
bool is_delimiter(char chr) {
	if (is_whitespace(chr)) { return true; }
	switch (chr) {
		case ' ': case '\t': 
		case '\n': case EOF:
		case '[': case ']': case ',': case ';':
		case NULL:
			return true;
		default: 
			return false;
	}
}

/* preprocessor:
	Splits an input string into 
	substrings each containing a single word.
	Assuming a well-formed input, one token
	can be made from each word.
*/
char* preprocessor(char* input, char **output) {
	char* buffer = malloc(PREPROCESSOR_WORD_SIZE);

	char oi = 0, bi = 0; // output and buffer index
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
		// Case 2.1. This is the last character in word.
		
		if (is_delimiter(*(ci + 1))) {
			// If so finish word
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

void test_tokenize_case(char* gave, token expected) {
	token got = tokenize(gave);
	printf("Gave %-16s expected %-22s got %-22s\n", 
		gave, 
		lookup_token_as_name(expected), 
		lookup_token_as_name(got));
}

void test_tokenize() {
	for (token i = TOKEN_ADD; i < TOKEN_ZEXT; i++) {
		test_tokenize_case(lookup_token_as_lexeme(i), i);
	}
}

void test_tokenizer() {
	test_tokenizer();
}


void print_token_list(token* tokens) {
	token* tk = tokens;
	printf("%16s", "Token Name\n");
	printf("%16s", "----------\n");
	while (*tk != EOF) {
		printf("%16s\n", lookup_token_as_name(*tk++));
	}
}

/*
	The job of main here is to act as a pipeline
	where input comes in as a char[] in source form
	and output comes out as a token[] in tokenized form.
*/
void main(int argc, char* argv[])
{
	if (DEBUG_TEST_TOKENIZER) {
		test_tokenize();
	}
	char *words[PREPROCESSOR_BUFFER_SIZE];
	preprocessor(
		"%0 = add i32 %X, %X; yields i32:%0\n\
		 %1 = add i32 %0, %0; yields i32 : %1\n\
		 %result = add i32 %1, %1", words);
	token tokens[TOKENS_BUFFER_SIZE];
	tokenizer(words, tokens);
	print_token_list(tokens);
	
}
