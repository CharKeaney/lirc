#pragma once

#include <time.h>

#include "preprocessor.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

/* Buffer size options - Only adjust if encountering memory errors. */
#define PREPROCESSOR_BUFFER_SIZE (1 << 24)
#define TOKENS_BUFFER_SIZE (1 << 24)

/* Testing options. */
/* which tests should be ran? */
#define DEBUG_RUN_STANDARD_TESTS false
#define DEBUG_RUN_ROSSETTA_CODE_TESTS false
#define DEBUG_RUN_C_TEST_SUITE_TESTS false
/* and under which configurations? */
#define PRINT_TEST_TIMES true
#define PRINT_TEST_RESULTS true
#define PRINT_TEST_REPORT true
#define TEST_ENABLE_ERROR_MSG true

#define DEBUG_TEST_TOKENIZER false

/* Test output options. */
#define TESTS_REPORT_STATE false
#define TESTS_SHOW_MEMORY_MANAGEMENT_DEBUG false
#define TESTS_SHOW_LEX_RESULT false
#define TESTS_SHOW_AST_RESULT false
#define TESTS_SAVE_AST_RESULT false

/* Compatibility options. */
#define DEBUG_SUPPORT_COLOUR true
#define SUCCESS_COLOUR "\033[32m"
#define FAILURE_COLOUR "\033[33m"
#define ERROR_COLOUR "\033[31m"
#define DEFAULT_COLOUR "\033[0m"

typedef struct compilation_environment {
	char* input;
	preprocessed_word** words;
	token** tokens;
	ast_node* root;
} compilation_environment;

/* Forward declarations. */
bool test_passed(char*);
bool run_test(char*, int);
void run_tests(char** []);
char* load_test_file_into_char_ptr(char*);
