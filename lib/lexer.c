#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define PREPROCESSOR_BUFFER_SIZE (1 << 8)
#define PREPROCESSOR_WORD_SIZE (1 << 4)
#define TOKENS_BUFFER_SIZE (1 << 8)

#define DEBUG_PREPROCESSOR false
#define DEBUG_TOKENIZER true

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
	TOKEN_DECLARE,
	TOKEN_DEFINE,
	TOKEN_DOUBLE,
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
	TOKEN_INT,
	TOKEN_INTTOPTR,
	TOKEN_LITTLEENDIAN,
	TOKEN_LOAD,
	TOKEN_LSHR,
	TOKEN_MALLOC,
	TOKEN_MUL,
	TOKEN_NULL,
	TOKEN_OR,
	TOKEN_PHI,
	TOKEN_PTR,
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
	TOKEN_ZEROINITALIZER,
	TOKEN_ZEXT,
} token;

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
	STATE_D,
	STATE_DE,
	STATE_DECLARE,
	STATE_DEFINE,
	STATE_DOUBLE,
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
	STATE_FSUB,
	STATE_G,
	STATE_GETELEMENTPTR,
	STATE_GLOBAL,
	STATE_I,
	STATE_ICMP,
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
} state;

state match(char* input, char* expected, state result) {
	if (strcmp(input, expected) == 0) {
		return result;
	}
}

token get_token(char* input) {
	char* ci = input;
	state current_state = BEGIN_STATE;
	token tk;
	while (current_state != END_STATE) {
		if (DEBUG_TOKENIZER) { 
			printf("CURRENT STATE: %d\n", current_state);
			printf("WORD REMAINING: %s\n", ci);
		}
		switch (current_state) {

			case BEGIN_STATE:
				switch (*ci++) {
					case 'a':
						current_state = STATE_A;
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

			case STATE_B: break;
			case STATE_BI: break;

			case STATE_BIGENDIAN: break;
			case STATE_BITCAST: break;
			case STATE_BR: break;
			case STATE_CALL: break;

			case STATE_D: break;
			case STATE_DE: break;

			case STATE_DECLARE: break;
			case STATE_DEFINE: break;
			case STATE_DOUBLE: break;

			case STATE_F: break;

			case STATE_FADD: break;
			case STATE_FCMP: break;
			case STATE_FDIV: break;
			case STATE_FLOAT: break;
			case STATE_FMUL: break;
			case STATE_FP: break;
			case STATE_FPEXT: break;
			case STATE_FPTOUI: break;
			case STATE_FRE: break;
			case STATE_FSUB: break;
			case STATE_G: break;
			case STATE_GETELEMENTPTR: break;
			case STATE_GLOBAL: break;
			case STATE_I: break;
			case STATE_ICMP: break;
			case STATE_INTTOPTR: break;
			case STATE_L: break;
			case STATE_LITTLEENDIAN: break;
			case STATE_LOAD: break;
			case STATE_LSHR: break;
			case STATE_M: break;
			case STATE_MALLOC: break;
			case STATE_MUL: break;
			case STATE_NULL: break;
			case STATE_OR: break;
			case STATE_P: break;
			case STATE_PHI: break;
			case STATE_PTRTOINT: break;
			case STATE_RET: break;
			case STATE_S: break;
			case STATE_SDIV: break;
			case STATE_SE: break;
			case STATE_SELECT: break;
			case STATE_SEXT: break;
			case STATE_SHL: break;
			case STATE_SREM: break;
			case STATE_ST: break;
			case STATE_STACK: break;
			case STATE_STORE: break;
			case STATE_SUB: break;
			case STATE_T: break;
			case STATE_TO: break;
			case STATE_TRUNC: break;
			case STATE_TYP: break;
			case STATE_U: break;
			case STATE_UDIV: break;
			case STATE_UN: break;
			case STATE_UNDEF: break;
			case STATE_UNREACHABLE: break;
			case STATE_UREM: break;
			case STATE_V: break;
			case STATE_VAL: break;
			case STATE_VOID: break;
			case STATE_XOR: break;
			case STATE_ZE: break;
		}
	}
}

token tokenizer() {
}

/* is_whitespace:
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
		case '[': case ']':
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
	while (*ci != NULL && *ci != EOF) {
		if (DEBUG_PREPROCESSOR) {
			printf("CURRENT CHARACTER: '%c'\n", *ci);
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
				printf("WORD: %s\n", buffer);
			}
			// Then reset state.
			bi = 0;
			buffer = malloc(PREPROCESSOR_WORD_SIZE);

		}
		ci++;
	}
	
}

void test_tokenizer_case(char* gave, token expected) {
	token got = get_token(gave);
	printf("Gave %s, expected %d,  got %d.\n", gave, expected, got);
}

void test_tokenizer() {
	test_tokenizer_case("add", TOKEN_ADD);
	test_tokenizer_case("aggr", TOKEN_AGGR);
	test_tokenizer_case("alloca", TOKEN_ALLOCA);
	test_tokenizer_case("and", TOKEN_AND);
	test_tokenizer_case("ashr", TOKEN_ASHR);
}

/*
	The job of main here is to act as a pipeline
	where input comes in as a char[] in source form
	and output comes out as a token[] in tokenized form.
*/
void main(int argc, char* argv[])
{
	char *words[PREPROCESSOR_BUFFER_SIZE];
	preprocessor("add add[][][]", words);

	token tokens[TOKENS_BUFFER_SIZE];
	
	test_tokenizer();

}
