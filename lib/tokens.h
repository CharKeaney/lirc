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
	TOKEN_ASM,
	TOKEN_BIGENDIAN,
	TOKEN_BITCAST,
	TOKEN_BR,
	TOKEN_CALL,
	TOKEN_CLOSE_ANGLE_BRACKET,
	TOKEN_CLOSE_CURLY_BRACKET,
	TOKEN_CLOSE_BRACKET,
	TOKEN_CLOSE_PAREN,
	TOKEN_CLEANUP,
	TOKEN_CMPXCHG,
	TOKEN_COMMA,
	TOKEN_COMDAT,
	TOKEN_DECLARE,
	TOKEN_DEFINE,
	TOKEN_DOTDOTDOT,
	TOKEN_DOUBLE,
	TOKEN_DLLIMPORT,
	TOKEN_DSO_LOCAL,
	TOKEN_EQUALS,
	TOKEN_EQ,
	TOKEN_EXTRACTVALUE,
	TOKEN_EXACT,
	TOKEN_FADD,
	TOKEN_FALSE,
	TOKEN_FCMP,
	TOKEN_FDIV,
	TOKEN_FENCE,
	TOKEN_FLOAT,
	TOKEN_FLOAT_LITERAL,
	TOKEN_FMUL,
	TOKEN_FPEXT,
	TOKEN_FPTOUI,
	TOKEN_FPTOSI,
	TOKEN_FREE,
	TOKEN_FREM,
	TOKEN_FSUB,
	TOKEN_FNEG,
	TOKEN_GETELEMENTPTR,
	TOKEN_GLOBAL,
	TOKEN_ICMP,
	TOKEN_INBOUNDS,
	TOKEN_IDENTIFIER,
	TOKEN_IMMARG,
	TOKEN_INTEGERTYPE,
	TOKEN_INTERNAL,
	TOKEN_INTTOPTR,
	TOKEN_INVOKE,
	TOKEN_BLOCKLABEL,
	TOKEN_LANDINGPAD,
	TOKEN_LITTLEENDIAN,
	TOKEN_LINKONCE_ODR,
	TOKEN_LABEL,
	TOKEN_LOAD,
	TOKEN_LSHR,
	TOKEN_MALLOC,
	TOKEN_MUL,
	TOKEN_NE,
	TOKEN_NOALIAS,
	TOKEN_NOCAPTURE,
	TOKEN_NSW,
	TOKEN_NUW,
	TOKEN_NULL,
	TOKEN_OEQ,
	TOKEN_OPEN_ANGLE_BRACKET,
	TOKEN_OPEN_BRACKET,
	TOKEN_OPEN_CURLY_BRACKET,
	TOKEN_OPEN_PAREN,
	TOKEN_OGE,
	TOKEN_OGT,
	TOKEN_OLE,
	TOKEN_OLT,
	TOKEN_OR,
	TOKEN_PERSONALITY,
	TOKEN_PHI,
	TOKEN_PTR,
	TOKEN_PTRTOINT,
	TOKEN_READONLY,
	TOKEN_RET,
	TOKEN_SDIV,
	TOKEN_SELECT,
	TOKEN_SEQ_CST,
	TOKEN_SEXT,
	TOKEN_SYNCSCOPE,
	TOKEN_SGE,
	TOKEN_SGT,
	TOKEN_SHL,	
	TOKEN_SITOFP,
	TOKEN_SLE,
	TOKEN_SLT,
	TOKEN_SREM,
	TOKEN_STACK,
	TOKEN_STRING,
	TOKEN_STORE,
	TOKEN_SUB,
	TOKEN_SWITCH,
	TOKEN_SZ,
	TOKEN_TAIL,
	TOKEN_TO,
	TOKEN_TRUNC,
	TOKEN_TRUE,
	TOKEN_TYP,
	TOKEN_UDIV,
	TOKEN_UGT,
	TOKEN_UGE,
	TOKEN_UITOFP,
	TOKEN_ULE,
	TOKEN_ULT,
	TOKEN_UNDEF,
	TOKEN_UNE,
	TOKEN_UNREACHABLE,
	TOKEN_UNWIND,
	TOKEN_UREM,
	TOKEN_VAL,
	TOKEN_VOID,
	TOKEN_VOLATILE,
	TOKEN_WRITEONLY,
	TOKEN_X,
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
	token_name name;	/* The name or 'class' of token.		*/
	char* lexeme;		/* The lexeme or 'contents' of token	*/
	int value;  /* The value or meaning behind a token	*/
} token;

char* token_name_lookup[][2];