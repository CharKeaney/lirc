/* lexer.h - Peforms lexical analysis on given input after preprocessing */

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

#define lookup_token_as_name(a) token_name_lookup[a][0]
#define lookup_token_as_lexeme(a) token_name_lookup[a][1]

#define PREPROCESSOR_WORD_SIZE (1 << 10)

#define DEBUG_PREPROCESSOR false
#define DEBUG_TOKENIZE false
#define DEBUG_TOKENIZER false

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
	STATE_AS,
	STATE_ASM,
	STATE_ASHR,
	STATE_B,
	STATE_BI,
	STATE_BIGENDIAN,
	STATE_BITCAST,
	STATE_BLOCK_LABEL,
	STATE_BR,
	STATE_C,
	STATE_CALL,
	STATE_CLEANUP,
	STATE_CLOSE_ANGLE_BRACKET,
	STATE_CLOSE_PAREN,
	STATE_CLOSE_CURLY_BRACKET,
	STATE_CLOSE_BRACKET,
	STATE_CMPXCHG,
	STATE_COMMA,
	STATE_COMDAT,
	STATE_D,
	STATE_DE,
	STATE_DECLARE,
	STATE_DEFINE,
	STATE_DIGITS,
	STATE_DIGITS_DOT,
	STATE_DIGITS_DOT_DIGITS,
	STATE_DIGITS_DOT_DIGITS_E,
	STATE_DIGITS_DOT_DIGITS_E_SIGN,
	STATE_DIGITS_DOT_DIGITS_E_SIGN_DIGITS,
	STATE_DLLIMPORT,
	STATE_DOTDOTDOT,
	STATE_DOUBLE,
	STATE_DSO_LOCAL,
	STATE_ERROR_ENCOUNTERED,
	STATE_E,
	STATE_EQ,
	STATE_EQUALS,
	STATE_EX,
	STATE_EXACT,
	STATE_EXTRACTVALUE,
	STATE_F,
	STATE_FA,
	STATE_FADD,
	STATE_FALSE,
	STATE_FCMP,
	STATE_FDIV,
	STATE_FENCE,
	STATE_FLOAT,
	STATE_FLOAT_LITERAL,
	STATE_FMUL,
	STATE_FNEG,
	STATE_FP,
	STATE_FPEXT,
	STATE_FPTO,
	STATE_FPTOUI,
	STATE_FPTOSI,
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
	STATE_IMMARG,
	STATE_IN,
	STATE_INT,
	STATE_INTERNAL,
	STATE_INBOUNDS,
	STATE_INTEGERTYPE,
	STATE_INTTOPTR,
	STATE_INVOKE,
	STATE_L,
	STATE_LA,
	STATE_LANDINGPAD,
	STATE_LABEL,
	STATE_LI,
	STATE_LITTLEENDIAN,
	STATE_LINKONCE_ODR,
	STATE_LOAD,
	STATE_LSHR,
	STATE_M,
	STATE_MALLOC,
	STATE_MINUS,
	STATE_MINUS_DIGITS,
	STATE_MUL,
	STATE_N,
	STATE_NE,
	STATE_NO,
	STATE_NOALIAS,
	STATE_NOCAPTURE,
	STATE_NSW,
	STATE_NU,
	STATE_NUW,
	STATE_NULL,
	STATE_OPEN_ANGLE_BRACKET,
	STATE_OPEN_CURLY_BRACKET,
	STATE_OPEN_BRACKET,
	STATE_OPEN_PAREN,
	STATE_O,
	STATE_OEQ,
	STATE_OR,
	STATE_OG,
	STATE_OGE,
	STATE_OGT,
	STATE_OL,
	STATE_OLE,
	STATE_OLT,	
	STATE_OX,
	STATE_OX_DIGITS,
	STATE_P,
	STATE_PERSONALITY,
	STATE_PHI,
	STATE_PTR,
	STATE_PTRTOINT,
	STATE_RE,
	STATE_RET,
	STATE_READONLY,
	STATE_S,
	STATE_STRING,
	STATE_SDIV,
	STATE_SE,
	STATE_SELECT,
	STATE_SEQ_CST,
	STATE_SEXT,
	STATE_SG,
	STATE_SGT,
	STATE_SGE,
	STATE_SHL,
	STATE_SITOFP,
	STATE_SL,
	STATE_SLE,
	STATE_SLT,
	STATE_SREM,
	STATE_ST,
	STATE_STACK,
	STATE_STORE,
	STATE_SUB,
	STATE_SWITCH,
	STATE_SYNCSCOPE,
	STATE_SZ,
	STATE_T,
	STATE_TAIL,
	STATE_TO,
	STATE_TR,
	STATE_TRUNC,
	STATE_TRUE,
	STATE_TYP,
	STATE_U,
	STATE_UITOFP,
	STATE_UG,
	STATE_UGE,
	STATE_UGT,
	STATE_UL,
	STATE_ULE,
	STATE_ULT,
	STATE_UDIV,
	STATE_UN,
	STATE_UNE,
	STATE_UNDEF,
	STATE_UNREACHABLE,
	STATE_UNWIND,
	STATE_UREM,
	STATE_V,
	STATE_VAL,
	STATE_VO,
	STATE_VOID,
	STATE_VOLATILE,
	STATE_X,
	STATE_XOR,
	STATE_WRITEONLY,
	STATE_ZE,
	STATE_ZEROINITIALIZER,
	STATE_ZEXT
} state;

char* token_name_lookup[][2];

struct Token* make_token(token_name, char*, int);
state match(char*, char*, state);
struct Token* tokenize(char*);
void  tokenizer(char**, token**);
bool is_whitespace(char);
bool is_delimiter(char); 
void preprocessor(char*, char**);
void test_tokenize_case(char*, token_name);
void test_tokenize(); 
void print_token_list(token**); 
