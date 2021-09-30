#pragma once

#include "tokens.h"

#include <stdbool.h>

/* Represents an error code used to store the reason why an error occured.*/
typedef enum ErrorCode {
	GENERIC_ERROR,

	EXPECTED_END_OF_MODULE,
	EXPECTED_TERMINATOR,

	INVALID_BLOCK,
	INVALID_BOP,
	INVALID_COMMAND,
	INVALID_CONDITION,
	INVALID_CONSTANT,
	INVALID_CONVERSION_OP,
	INVALID_COP,
	INVALID_FBOP,
	INVALID_GLOBALS,
	INVALID_ID,
	INVALID_LAYOUT,
	INVALID_METADATA,
	INVALID_MODULE,
	INVALID_PARAMS,
	INVALID_PNODE,
	INVALID_PROD,
	INVALID_TERMINATOR,
	INVALID_TROP,
	INVALID_TYP,
	INVALID_VAL
} error_code;

/* Stores the error message associated with a given error code. */
const static char* error_msgs[] = {
	[GENERIC_ERROR] =
		"failed to parse the input after the indicated location.",
	[EXPECTED_END_OF_MODULE] =
		"expected end of module (program) but more input remaining.",
	[EXPECTED_TERMINATOR] =
		"expected end of block terminator but instead seen as follows.",
	[INVALID_BLOCK] = "invalid block.",
	[INVALID_BOP] = "invalid bop.",
	[INVALID_COMMAND] = "invalid command.",
	[INVALID_CONDITION] = "invalid condition.",
	[INVALID_CONSTANT] = "invalid constant.",
	[INVALID_CONVERSION_OP] = "Cannot convert between these two types. ",
	[INVALID_COP] = "invalid cop.",
	[INVALID_FBOP] = "invalid fbop",
	[INVALID_GLOBALS] = "invalid globals",
	[INVALID_ID] = "invalid id.",
	[INVALID_LAYOUT] = "invalid layout",
	[INVALID_METADATA] = "invalid metadata",
	[INVALID_MODULE] = "invalid module",
	[INVALID_PARAMS] = "invalid params",
	[INVALID_PNODE] = "invalid pnode",
	[INVALID_PROD] = "invalid prod.",
	[INVALID_TERMINATOR] = "invalid terminator.",
	[INVALID_TROP] = "invalid trop.",
	[INVALID_TYP] = "invalid typ.",
	[INVALID_VAL] = "invalid val."
};

typedef struct error_info {
	bool contains_error;	/* Form of tree poisoning to show errors		*/
	error_code error_code;  /* The code associated with the error.			*/
	token error_token;	/* Identifies where error is in terms of lexema */
} error_info;
