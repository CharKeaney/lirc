#pragma once

#include "tokens.h"

#include <stdbool.h>
#include <stdio.h>

typedef struct Preprocessed_word {
	char* word ;
	position_in_file pos;
} preprocessed_word;


#define PREPROCESSOR_WORD_SIZE (1 << 10)

#define DEBUG_PREPROCESSOR false