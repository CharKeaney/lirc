/* lexer.h - Peforms lexical analysis on given input after preprocessing */

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

#define lookup_token_as_name(a) token_name_lookup[a][0]
#define lookup_token_as_lexeme(a) token_name_lookup[a][1]

#define PREPROCESSOR_BUFFER_SIZE (1 << 16)
#define PREPROCESSOR_WORD_SIZE (1 << 8)
#define TOKENS_BUFFER_SIZE (1 << 16)

#define DEBUG_PREPROCESSOR false
#define DEBUG_TOKENIZE false
#define DEBUG_TOKENIZER false
#define DEBUG_TEST_TOKENIZER false

/* states represents the states that the lexer can have.
*  where possible, each state is named according to the
*  symbols encountered up to that point.
* e.g.	STATE_BI means 'b' and 'i' was read.
*	however this has some exceptions,
* e.g.	STATE_INTEGERTYPE does not mean read "integertype" but
*		instead means read an integertype iX for some number x.
*/
typedef enum {
	BEGIN_STATE,
	END_STATE,
	STATE_UNRECOGNISED,
	STATE_A,
	STATE_ADD,
	STATE_AGGR,
	STATE_AL,
	STATE_ALIGN,
	STATE_ALLOCA,
	STATE_AND,
	STATE_ASHR,
	STATE_B,
	STATE_BI,
	STATE_BIGENDIAN,
	STATE_BITCAST,
	STATE_BLOCK_LABEL,
	STATE_BR,
	STATE_CALL,
	STATE_CLOSE_PAREN,
	STATE_CLOSE_CURLY_BRACKET,
	STATE_CLOSE_BRACKET,
	STATE_COMMA,
	STATE_D,
	STATE_DE,
	STATE_DECLARE,
	STATE_DEFINE,
	STATE_DOTDOTDOT,
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
	STATE_IN,
	STATE_INBOUNDS,
	STATE_INTEGERTYPE,
	STATE_INTTOPTR,
	STATE_L,
	STATE_LABEL,
	STATE_LITTLEENDIAN,
	STATE_LOAD,
	STATE_LSHR,
	STATE_M,
	STATE_MALLOC,
	STATE_MUL,
	STATE_N,
	STATE_NSW,
	STATE_NULL,
	STATE_OPEN_CURLY_BRACKET,
	STATE_OPEN_BRACKET,
	STATE_OPEN_PAREN,
	STATE_OR,
	STATE_P,
	STATE_PHI,
	STATE_PTR,
	STATE_PTRTOINT,
	STATE_RET,
	STATE_S,
	STATE_SDIV,
	STATE_SE,
	STATE_SELECT,
	STATE_SEXT,
	STATE_SHL,
	STATE_SL,
	STATE_SLE,
	STATE_SLT,
	STATE_SREM,
	STATE_ST,
	STATE_STACK,
	STATE_STORE,
	STATE_SUB,
	STATE_SZ,
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
	STATE_X,
	STATE_XOR,
	STATE_ZE,
	STATE_ZEROINITIALIZER,
	STATE_ZEXT
} state;

char* token_name_lookup[][2];

struct Token* make_token(token_name name, char* lexeme, int value);
state match(char* input, char* expected, state result);
struct Token* tokenize(char* input);
void  tokenizer(char** input, token** output);
bool is_whitespace(char chr);
bool is_delimiter(char chr); 
void preprocessor(char* input, char** output);
void test_tokenize_case(char* gave, token_name expected);
void test_tokenize(); 
void print_token_list(token** tokens); 
