#pragma once

/* A token_name is a name of a token.
*  The tokens name is not neccessarily the same as its lexeme.
*  ERROR_TOKEN signals eroneous input.
* */
typedef enum {
	ERROR_TOKEN,
	TOKEN_ADD,
	TOKEN_AGGR,
	TOKEN_ALIGN,
	TOKEN_ALLOCA,
	TOKEN_AND,
	TOKEN_ASHR,
	TOKEN_BIGENDIAN,
	TOKEN_BITCAST,
	TOKEN_BR,
	TOKEN_CALL,
	TOKEN_CLOSE_CURLY_BRACKET,
	TOKEN_CLOSE_BRACKET,
	TOKEN_CLOSE_PAREN,
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
	TOKEN_NSW,
	TOKEN_NULL,
	TOKEN_OPEN_BRACKET,
	TOKEN_OPEN_CURLY_BRACKET,
	TOKEN_OPEN_PAREN,
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
	TOKEN_SZ,
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
} token_name;

/*
* Token is used to store data on a token.
* This is neccessary as a token may in some cases have information
* that cannot be communicated simply through its name.
*/
typedef struct Token {
	token_name name;	/* The name or 'class' of token.		 */
	char* lexeme;		/* The lexeme or 'contents' of token	 */
	int value;			/* The value or meaning behind a token	 */
} token;

char* token_name_lookup[][2];