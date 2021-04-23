#include "driver.h"

/* Uninitialized pointers to the standard testing suite */
char *test1, *test2, *test3, *test4, *test5, *test6, *test7, *test8, *test9,
     *test10, *test11, *test12, *test13, *test14;

/* Container for Standard Testing Suite tests*/
const char** test_suite[] = {
	&test1, &test2, &test3, &test4, &test5,
	&test6, &test7, &test8, &test9, &test10,
	&test11, &test12, &test13, &test14, NULL
};

/**
* Responsible for importing the .ll files within /tests into the char pointers
* for the Standard Testing Suite tests.
* 
* @see load_txt_into_char_ptr
* @see https://www.rosettacode.org/wiki/Category:Programming_Tasks
*/
void import_standard_tests() {
	test1 = load_test_file_into_char_ptr("./tests/test1.txt");
	test2 = load_test_file_into_char_ptr("./tests/test2.txt");
	test3 = load_test_file_into_char_ptr("./tests/test3.txt");
	test4 = load_test_file_into_char_ptr("./tests/test4.txt");
	test5 = load_test_file_into_char_ptr("./tests/test5.txt");
	test6 = load_test_file_into_char_ptr("./tests/test6.txt");
	test7 = load_test_file_into_char_ptr("./tests/test7.txt");
	test8 = load_test_file_into_char_ptr("./tests/test8.txt");
	test9 = load_test_file_into_char_ptr("./tests/test9.txt");
	test10 = load_test_file_into_char_ptr("./tests/test10.txt");
	test11 = load_test_file_into_char_ptr("./tests/test11.txt");
	test12 = load_test_file_into_char_ptr("./tests/test12.txt");
	test13 = load_test_file_into_char_ptr("./tests/test13.txt");
	test14 = load_test_file_into_char_ptr("./tests/test14.txt");
}

/**
* Responsible for freeing the data associated with the standard testing suite.
* This is useful as these files could take up many Mb otherwise.
*/
void free_standard_tests() {
	free(test1);
	free(test2);
	free(test3);
	free(test4);
	free(test5);
	free(test6);
	free(test7);
	free(test8);
	free(test9);
	free(test10);
	free(test11);
	free(test12);
	free(test13);
	free(test14);
}

/* Uninitialized Pointers to Rosetta Code tests */
char *rct1, *rct2, *rct3, *rct4;		

/* Container for Rosetta Code tests*/
const char** rosetta_code_tests[] = {	
	&rct1, &rct2, &rct3, &rct4
};

/**
* Responsible for importing the .ll files within /tests/RosettaCode into the char pointers
* for the Rosetta Code tests.
* 
* @see load_txt_into_char_ptr
* @see https://www.rosettacode.org/wiki/Category:Programming_Tasks
*/
void import_rosetta_code_tests() {
	rct1 = load_test_file_into_char_ptr(
		"./tests/RosettaCode/BurrowsWheelerTransform.ll");
	rct2 = load_test_file_into_char_ptr(
		"./tests/RosettaCode/Faulhaberstriangle.ll");
	rct3 = load_test_file_into_char_ptr(
		"./tests/RosettaCode/RayCastingAlgorithm.ll");
	rct4 = load_test_file_into_char_ptr(
		"./tests/RosettaCode/RayCastingAlgorithm.ll");
}

/**
* Responsible for freeing the data associated with the Rosetta Code Tests.
* This is useful as these files could take up many Mb otherwise.
*/
void free_rosetta_code_tests() {
	free(rct1);
	free(rct2);
	free(rct3);
	free(rct4);
}

/**
* Responsible for conducting a test and seeing if it passed. 
* As opposed to using an OUTPUT vs EXPECTED format, it instead checks that the
* produced parse tree accounts for all tokens, since we can not quantitatively
* confirm the usefulness of the parse tree as that is a qualitative metric.
* 
* @param test The test that will be conducted.
* 
* @see preprocessor
* @see tokenizer
* @see parser
*/
bool test_passed(char* test) {
	/* Starting timer */
	clock_t start, end;
	if (PRINT_TEST_TIMES) { start = clock(); }

	/* Preprocessing */
	if (TESTS_REPORT_STATE) { printf("\tPreprocessing...\n"); }
	char** words = malloc(sizeof(char *) * PREPROCESSOR_BUFFER_SIZE);
	preprocessor(test, words);

	/* Tokenizing */
	if (TESTS_REPORT_STATE) { printf("\tTokenizing...\n"); }
	token** tokens = malloc(sizeof(token *) * TOKENS_BUFFER_SIZE);
	tokenizer(words, tokens);
	if (TESTS_SHOW_LEX_RESULT) { print_token_list(tokens); }

	/* Parsing */
	if (TESTS_REPORT_STATE) { printf("\tParsing...\n"); }
	ast_node* root = parse(tokens);
	if (PRINT_TEST_TIMES) { end = clock(); }
	if (TESTS_SHOW_AST_RESULT) {
		print_ast(root);
	}
	if (TESTS_SAVE_AST_RESULT) {
		FILE* fp = NULL;
		fopen_s(&fp, "log.txt", "w");
		fprint_ast(root, fp);
		fclose(fp);
	}

	/* Validating */
	int size = root->size;
	int len = 0;
	for (; tokens[len] != NULL; len++);
	bool large_as_expected = size == len;
	bool passed = large_as_expected
		&& root != NULL
		&& root->contains_error == false;
	if (!passed && TEST_ENABLE_ERROR_MSG) {
		/* We will colour this message differently to make it stand out. */
		if (DEBUG_SUPPORT_COLOUR) { printf(ERROR_COLOUR); }
		/* Now we will print the error message */
		/* First the message beginning */
		char* msgformat = "Parsing Error : %s\n";
		error_code err_code = root->error_code;
		char* msgbody = error_msgs[err_code];
		printf(msgformat, msgbody);
		/* Then print the input line */
		/* We need to find the start of the error. root.contains_error tells
		   us this but it will only be present if proper error recovery.
		   If it is not present we use the less accurate size attribute. */
		token** error_token = (root->contains_error)
			? root->error_token : tokens + size;

		int error_char_index = 0; /* Remembering where the error character is */
		for (token** tk = error_token - 10; 
			tk < error_token;
			tk++) {

			if (tk >= tokens && tk < tokens + len) { 
				printf("%s ", (*tk)->lexeme); 
				error_char_index += strlen((*tk)->lexeme) + 1;
			}
		}
		for (token** tk = error_token; tk < error_token + 10; tk++) {
			if (tk >= tokens && tk < tokens + len) { 
				printf("%s ", (*tk)->lexeme); 
			}
		}
		/* Then print a caret (^) pointing to the error. */
		printf("\n%*s^\n", error_char_index, " ");
		/* We will swap the colour back to the default now. . */
		if (DEBUG_SUPPORT_COLOUR) { printf(DEFAULT_COLOUR); }
	}

	/* Managing memory */
	if (TESTS_REPORT_STATE) { printf("\tManaging Memory...\n"); }
	/* 1. Freeing the parse tree */
	free_tree(root);
	root = NULL;
	/* 2. Freeing the tokens */
	for (token** tk = tokens; *tk != NULL; tk++) {
		free(*tk);
		*tk = NULL;
	}
	free(tokens);
	/* Freeing the lexemes */
	for (int i = 0; words[i] != NULL; i++) {
		if (TESTS_SHOW_MEMORY_MANAGEMENT_DEBUG) {
			char* prefix = "Tester (memory management) : ";
			printf(
				"%s %d. Attempted to free %s (%p).\n", 
				prefix, i, words[i], words[i]);
		}
		free(words[i]);
		words[i] = NULL;
	}
	free(words);

	/* Printing the time taken to conduct the test */
	if (PRINT_TEST_TIMES) {
		double cpu_cycles_used = (double) end - start;
		double cpu_time_used = (double) cpu_cycles_used / CLOCKS_PER_SEC;
		printf("Parsing completed in %.2fs (%.10fs per token).",
			cpu_time_used, cpu_time_used / len);
	}
	return passed; /* Return results of the test */
}

/**
* Responsible for conducting a test and seeing if it passed. Acts as a wrapper
* for test_passed. If the proper debug options are enabled (PRINT_TEST_RESULT),
* this function will also print out the results in an appropriately formatted 
* manner.
* 
* @param test The test that will be conducted.
* @param test_num The number to associate with the test. I.e. test number 1.
*
* @see test_passed
* @see tokenizer
* @see parser
*/
bool run_test(char* test, int test_num) {
	/* Checking that the test is successful */
	bool test_was_successful = test_passed(test);
	/* Checking what symbol to use based on compatibility */
	char* success_symbol = (DEBUG_SUPPORT_COLOUR) ? 
		SUCCESS_COLOUR "Y" DEFAULT_COLOUR : "Y";
	char* failure_symbol = (DEBUG_SUPPORT_COLOUR) ?
		FAILURE_COLOUR "N" DEFAULT_COLOUR : "N";
	/* Printing the test results */
	if (PRINT_TEST_RESULTS) {
		printf("\t[%s] Test %d %s.\n",
			(test_was_successful) ? 
				success_symbol : failure_symbol,
			test_num,
			(test_was_successful) ? "Passed" : "Failed");
	}
	return test_was_successful;
}

/**
* Responsible for conducting all the tests in a test suite and seeing if they 
* pass. Will print results if appropriate debug options are enabled.
* Will print times if appropriate debug options are enabled.
* 
* @param test_suite The container which contains all the tests to be conducted.
* 
* @see run_test
* @see PRINT_TEST_TIMES
*/
void run_tests(char** test_suite[]) {
	time_t start, end;
	int tests_ran = 0, tests_passed = 0;
	if (PRINT_TEST_TIMES) { start = clock(); }
	for (int i = 0; test_suite[i] != NULL; i++) {
		tests_passed += run_test(*test_suite[i], i + 1);		
		tests_ran++;
	}
	if (PRINT_TEST_TIMES) {
		end = clock();
		double cpu_clocks_used = (double)end - start;
		double cpu_time_used = (double)cpu_clocks_used / CLOCKS_PER_SEC;
		printf("All tests completed in %.2fs.\n", cpu_time_used);
	}
	if (PRINT_TEST_REPORT) {
		double percent_passed = (double) ( tests_passed / tests_ran) * 100;
		printf("%.2f%% of tests passed. (%d/%d)\n",
			percent_passed,
			tests_passed,
			tests_ran);
	}
}

/**
* Responsible for loading a test file into a character pointer and returning 
* said pointer.
* 
* @param filename The full file name for the test file relative to this file. 
*/
char* load_test_file_into_char_ptr(char* filename) {
	FILE* fp = NULL;
	fopen_s(&fp, filename, "rb");
	if (fp == NULL) { printf("could not read test file."); return NULL; }

	fseek(fp, 0L, SEEK_END);
	long long int len = ftell(fp);
	rewind(fp);

	char* buffer = malloc(sizeof(char) * len + 1);
	if (buffer == NULL) {
		printf(
			"Fatal Error : Failed to allocate memory to store input from %s",
			filename);
		return NULL;
	}
	fread(buffer, sizeof(char), len + 1, fp);
	buffer[len] = NULL;
	return buffer;
}

/**
* The job of main here is to act as a pipeline where input comes in as a char[]
* in source form and output comes out as a token[] in tokenized form.
*/
void main(int argc, char *argv[]) {
	
	if (DEBUG_TEST_TOKENIZER) {
		test_tokenize();
	}

	if (DEBUG_RUN_TESTS) {
		/* Loading the test data which would otherwise be unnecessary. */
		import_standard_tests();
		/* Performing tests */
		printf("Performing tests...\n");
		clock_t start, end; 
		run_tests(test_suite);
		/* Freeing the test data which should now be unnecessary. */
		free_standard_tests();
	}

	if (DEBUG_RUN_ROSSETTA_CODE_TESTS) {
		/* Loading Rosetta Code test data which would otherwise be unnecessary */
		import_rosetta_code_tests();
		/* Performing Rosetta Code tests */
		printf("Performing Rosetta Code tests...\n");
		run_tests(rosetta_code_tests);
		/* Freeing the Rosetta Code test data which should now be unnecessary */
		free_rosetta_code_tests();
	}
}