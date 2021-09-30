#pragma once

#include "driver.h"

const char* standard_test_suite[] = {
	"./tests/StandardTests/test1.txt",
	"./tests/StandardTests/test2.txt",
	"./tests/StandardTests/test3.txt",
	"./tests/StandardTests/test4.txt",
	"./tests/StandardTests/test5.txt",
	"./tests/StandardTests/test6.txt",
	"./tests/StandardTests/test7.txt",
	"./tests/StandardTests/test8.txt",
	"./tests/StandardTests/test9.txt",
	"./tests/StandardTests/test10.txt",
	"./tests/StandardTests/test11.txt",
	"./tests/StandardTests/test12.txt",
	"./tests/StandardTests/test13.txt",
	"./tests/StandardTests/test14.txt",
	NULL
};

const char* rosetta_code_tests[] = {
	"./tests/RosettaCode/BioinformaticsBaseCount.ll",
	"./tests/RosettaCode/BioinformaticsSequenceMutation.ll",
	"./tests/RosettaCode/BurrowsWheelerTransform.ll",
	"./tests/RosettaCode/DrawACuboid.ll",
	"./tests/RosettaCode/DrawASphere.ll",
	"./tests/RosettaCode/Faulhaberstriangle.ll",
	"./tests/RosettaCode/GaussLegendreQuadrature.ll",
	"./tests/RosettaCode/MandelbrotSet.ll",
	"./tests/RosettaCode/ManOrBoyTest.ll",
	"./tests/RosettaCode/MatrixExponentiationOperator.ll",
	"./tests/RosettaCode/MatrixMultiplication.ll",
	"./tests/RosettaCode/MatrixTransposition.ll",
	"./tests/RosettaCode/PerlinNoise.ll",
	"./tests/RosettaCode/RayCastingAlgorithm.ll",
	"./tests/RosettaCode/TonelliShanksAlgorithm.ll",
	"./tests/RosettaCode/YCombinator.ll",
	"./tests/RosettaCode/ZeckendorfNumberRepresentation.ll",
	"./tests/RosettaCode/ZhangSuenThinningAlgorithm.ll",
	NULL
};

const char* c_test_suite_tests[] = {
	"./tests/c-testsuite/00001.ll",
	"./tests/c-testsuite/00002.ll",
	"./tests/c-testsuite/00003.ll",
	"./tests/c-testsuite/00004.ll",
	"./tests/c-testsuite/00005.ll",
	"./tests/c-testsuite/00006.ll",
	"./tests/c-testsuite/00007.ll",
	"./tests/c-testsuite/00008.ll",
	"./tests/c-testsuite/00009.ll",
	"./tests/c-testsuite/00010.ll",
	"./tests/c-testsuite/00011.ll",
	"./tests/c-testsuite/00012.ll",
	"./tests/c-testsuite/00013.ll",
	"./tests/c-testsuite/00014.ll",
	"./tests/c-testsuite/00015.ll",
	"./tests/c-testsuite/00016.ll",
	"./tests/c-testsuite/00017.ll",
	"./tests/c-testsuite/00018.ll",
	"./tests/c-testsuite/00019.ll",
	"./tests/c-testsuite/00020.ll",
	"./tests/c-testsuite/00021.ll",
	"./tests/c-testsuite/00022.ll",
	"./tests/c-testsuite/00023.ll",
	"./tests/c-testsuite/00024.ll",
	"./tests/c-testsuite/00025.ll",
	"./tests/c-testsuite/00026.ll",
	"./tests/c-testsuite/00027.ll",
	"./tests/c-testsuite/00028.ll",
	"./tests/c-testsuite/00029.ll",
	"./tests/c-testsuite/00030.ll",
	"./tests/c-testsuite/00031.ll",
	"./tests/c-testsuite/00032.ll",
	"./tests/c-testsuite/00033.ll",
	"./tests/c-testsuite/00034.ll",
	"./tests/c-testsuite/00035.ll",
	"./tests/c-testsuite/00036.ll",
	"./tests/c-testsuite/00037.ll",
	"./tests/c-testsuite/00038.ll",
	"./tests/c-testsuite/00039.ll",
	"./tests/c-testsuite/00040.ll",
	"./tests/c-testsuite/00041.ll",
	"./tests/c-testsuite/00042.ll",
	"./tests/c-testsuite/00043.ll",
	"./tests/c-testsuite/00044.ll",
	"./tests/c-testsuite/00045.ll",
	"./tests/c-testsuite/00046.ll",
	"./tests/c-testsuite/00047.ll",
	"./tests/c-testsuite/00048.ll",
	"./tests/c-testsuite/00049.ll",
	"./tests/c-testsuite/00050.ll",
	"./tests/c-testsuite/00051.ll",
	"./tests/c-testsuite/00052.ll",
	"./tests/c-testsuite/00053.ll",
	"./tests/c-testsuite/00054.ll",
	"./tests/c-testsuite/00055.ll",
	"./tests/c-testsuite/00056.ll",
	"./tests/c-testsuite/00057.ll",
	"./tests/c-testsuite/00058.ll",
	"./tests/c-testsuite/00059.ll",
	"./tests/c-testsuite/00060.ll",
	"./tests/c-testsuite/00061.ll",
	"./tests/c-testsuite/00062.ll",
	"./tests/c-testsuite/00063.ll",
	"./tests/c-testsuite/00064.ll",
	"./tests/c-testsuite/00065.ll",
	"./tests/c-testsuite/00066.ll",
	"./tests/c-testsuite/00067.ll",
	"./tests/c-testsuite/00068.ll",
	"./tests/c-testsuite/00069.ll",
	"./tests/c-testsuite/00070.ll",
	"./tests/c-testsuite/00071.ll",
	"./tests/c-testsuite/00072.ll",
	"./tests/c-testsuite/00073.ll",
	"./tests/c-testsuite/00074.ll",
	"./tests/c-testsuite/00075.ll",
	"./tests/c-testsuite/00076.ll",
	"./tests/c-testsuite/00077.ll",
	"./tests/c-testsuite/00078.ll",
	"./tests/c-testsuite/00079.ll",
	"./tests/c-testsuite/00080.ll",
	"./tests/c-testsuite/00081.ll",
	"./tests/c-testsuite/00082.ll",
	"./tests/c-testsuite/00083.ll",
	"./tests/c-testsuite/00084.ll",
	"./tests/c-testsuite/00085.ll",
	"./tests/c-testsuite/00086.ll",
	"./tests/c-testsuite/00087.ll",
	"./tests/c-testsuite/00088.ll",
	"./tests/c-testsuite/00089.ll",
	"./tests/c-testsuite/00090.ll",
	"./tests/c-testsuite/00091.ll",
	"./tests/c-testsuite/00092.ll",
	"./tests/c-testsuite/00093.ll",
	"./tests/c-testsuite/00094.ll",
	"./tests/c-testsuite/00095.ll",
	"./tests/c-testsuite/00096.ll",
	"./tests/c-testsuite/00097.ll",
	"./tests/c-testsuite/00098.ll",
	"./tests/c-testsuite/00099.ll",
	"./tests/c-testsuite/00100.ll",
	"./tests/c-testsuite/00101.ll",
	"./tests/c-testsuite/00102.ll",
	"./tests/c-testsuite/00103.ll",
	"./tests/c-testsuite/00104.ll",
	"./tests/c-testsuite/00105.ll",
	"./tests/c-testsuite/00106.ll",
	"./tests/c-testsuite/00107.ll",
	"./tests/c-testsuite/00108.ll",
	"./tests/c-testsuite/00109.ll",
	"./tests/c-testsuite/00110.ll",
	"./tests/c-testsuite/00111.ll",
	"./tests/c-testsuite/00112.ll",
	"./tests/c-testsuite/00113.ll",
	"./tests/c-testsuite/00114.ll",
	"./tests/c-testsuite/00115.ll",
	"./tests/c-testsuite/00116.ll",
	"./tests/c-testsuite/00117.ll",
	"./tests/c-testsuite/00118.ll",
	"./tests/c-testsuite/00119.ll",
	"./tests/c-testsuite/00120.ll",
	"./tests/c-testsuite/00121.ll",
	"./tests/c-testsuite/00122.ll",
	"./tests/c-testsuite/00123.ll",
	"./tests/c-testsuite/00124.ll",
	"./tests/c-testsuite/00125.ll",
	"./tests/c-testsuite/00126.ll",
	"./tests/c-testsuite/00127.ll",
	"./tests/c-testsuite/00128.ll",
	"./tests/c-testsuite/00129.ll",
	"./tests/c-testsuite/00130.ll",
	"./tests/c-testsuite/00131.ll",
	"./tests/c-testsuite/00132.ll",
	"./tests/c-testsuite/00133.ll",
	"./tests/c-testsuite/00134.ll",
	"./tests/c-testsuite/00135.ll",
	"./tests/c-testsuite/00136.ll",
	"./tests/c-testsuite/00137.ll",
	"./tests/c-testsuite/00138.ll",
	"./tests/c-testsuite/00139.ll",
	"./tests/c-testsuite/00140.ll",
	"./tests/c-testsuite/00141.ll",
	"./tests/c-testsuite/00142.ll",
	"./tests/c-testsuite/00143.ll",
	"./tests/c-testsuite/00144.ll",
	"./tests/c-testsuite/00145.ll",
	"./tests/c-testsuite/00146.ll",
	"./tests/c-testsuite/00147.ll",
	"./tests/c-testsuite/00148.ll",
	"./tests/c-testsuite/00149.ll",
	"./tests/c-testsuite/00150.ll",
	"./tests/c-testsuite/00151.ll",
	"./tests/c-testsuite/00152.ll",
	"./tests/c-testsuite/00153.ll",
	"./tests/c-testsuite/00154.ll",
	"./tests/c-testsuite/00155.ll",
	"./tests/c-testsuite/00156.ll",
	"./tests/c-testsuite/00157.ll",
	"./tests/c-testsuite/00158.ll",
	"./tests/c-testsuite/00159.ll",
	"./tests/c-testsuite/00160.ll",
	"./tests/c-testsuite/00161.ll",
	"./tests/c-testsuite/00162.ll",
	"./tests/c-testsuite/00163.ll",
	"./tests/c-testsuite/00164.ll",
	"./tests/c-testsuite/00165.ll",
	"./tests/c-testsuite/00166.ll",
	"./tests/c-testsuite/00167.ll",
	"./tests/c-testsuite/00168.ll",
	"./tests/c-testsuite/00169.ll",
	"./tests/c-testsuite/00170.ll",
	"./tests/c-testsuite/00171.ll",
	"./tests/c-testsuite/00172.ll",
	"./tests/c-testsuite/00173.ll",
	"./tests/c-testsuite/00174.ll",
	"./tests/c-testsuite/00175.ll",
	"./tests/c-testsuite/00176.ll",
	"./tests/c-testsuite/00177.ll",
	"./tests/c-testsuite/00178.ll",
	"./tests/c-testsuite/00179.ll",
	"./tests/c-testsuite/00180.ll",
	"./tests/c-testsuite/00181.ll",
	"./tests/c-testsuite/00182.ll",
	"./tests/c-testsuite/00183.ll",
	"./tests/c-testsuite/00184.ll",
	"./tests/c-testsuite/00185.ll",
	"./tests/c-testsuite/00186.ll",
	"./tests/c-testsuite/00187.ll",
	"./tests/c-testsuite/00188.ll",
	"./tests/c-testsuite/00189.ll",
	"./tests/c-testsuite/00190.ll",
	"./tests/c-testsuite/00191.ll",
	"./tests/c-testsuite/00192.ll",
	"./tests/c-testsuite/00193.ll",
	"./tests/c-testsuite/00194.ll",
	"./tests/c-testsuite/00195.ll",
	"./tests/c-testsuite/00196.ll",
	"./tests/c-testsuite/00197.ll",
	"./tests/c-testsuite/00198.ll",
	"./tests/c-testsuite/00199.ll",
	"./tests/c-testsuite/00200.ll",
	"./tests/c-testsuite/00201.ll",
	"./tests/c-testsuite/00202.ll",
	"./tests/c-testsuite/00203.ll",
	"./tests/c-testsuite/00204.ll",
	"./tests/c-testsuite/00205.ll",
	"./tests/c-testsuite/00206.ll",
	"./tests/c-testsuite/00207.ll",
	"./tests/c-testsuite/00208.ll",
	"./tests/c-testsuite/00209.ll",
	"./tests/c-testsuite/00210.ll",
	"./tests/c-testsuite/00211.ll",
	"./tests/c-testsuite/00212.ll",
	"./tests/c-testsuite/00213.ll",
	"./tests/c-testsuite/00214.ll",
	"./tests/c-testsuite/00215.ll",
	"./tests/c-testsuite/00216.ll",
	"./tests/c-testsuite/00217.ll",
	"./tests/c-testsuite/00218.ll",
	"./tests/c-testsuite/00219.ll",
	"./tests/c-testsuite/00220.ll",
	NULL
};

/**
* Reponsible for testing an individual case of tokenize and printing a short
* report on the result.
*
* @param gave The input we give the tokenize function.
* @param expected The output we expect in terms of the tokens name.
*
* @see tokenize
*/
void test_tokenize_case(char* gave, token_name expected) {

	preprocessed_word word;
	word.word = gave;
	word.pos.character_number = 0;
	word.pos.filename = "test";
	word.pos.line_number = 0;

	token* got = tokenize(word.word);
	printf("Gave %-16s expected %-22s got %-22s\n",
		gave,
		lookup_token_as_name(expected),
		lookup_token_as_name((*got).name));
}

/**
* Reponsible for testing many cases of tokenize and prints
* a report on each result.
*/
void test_tokenize() {
	for (token_name i = TOKEN_ADD; i < TOKEN_ZEXT; i++) {
		test_tokenize_case(lookup_token_as_lexeme(i), i);
	}
}

void print_error_message(error_info err_info, char* input, char* msgbody) {
	/* We will colour this message differently to make it stand out. */
	if (DEBUG_SUPPORT_COLOUR) { printf(ERROR_COLOUR); }
	/* Now we will print the error message */
	/* First the message beginning */
	/* We need to find the start of the error. root.contains_error tells
	   us this but it will only be present if proper error recovery.
	   If it is not present we use the less accurate size attribute. */
	token error_token = err_info.error_token;
	position_in_file err_pos = error_token.pos;

	const char* msgformat = "%s:%d:%d:%s\n";
	printf(msgformat,
		err_pos.filename,
		err_pos.line_number,
		err_pos.character_number,
		msgbody);
	/* Then print the input line */
	char* cursor = input;
	int current_line_num = 0;
	while (current_line_num != err_pos.line_number) {
		if ((*cursor) == '\n') { current_line_num++; }
		cursor++;
	}
	while (*cursor != '\n') {
		printf("%c", *cursor++);
	}
	printf("\n");

	/* Then print a 'pointing message' below the eroneous line;
	a caret (^) pointing to the error followed by tilde trail. */
	/* First we create the tilde trail,
	   subtract len - 1 for '^' at the start. */
	int tilde_trail_len = strlen(error_token.lexeme);
	char* tilde_trail = malloc(sizeof(char) * tilde_trail_len);
	for (int i = 0; i < tilde_trail_len - 1; i++) {
		tilde_trail[i] = '~';
	};
	tilde_trail[tilde_trail_len - 1] = NULL;

	for (int i = 0; i < err_pos.character_number; i++) {
		putchar(' ');
	}

	printf("^%s\n",
		/* subtract 1 to character number to disregard '\n' */
		tilde_trail);
	free(tilde_trail);
	/* We will swap the colour back to the default now. */
	if (DEBUG_SUPPORT_COLOUR) { printf(DEFAULT_COLOUR); }
}

void print_parser_error_message(error_info err_info, char* input) {
	error_code err_code = err_info.error_code;
	char* msgbody = error_msgs[err_code];
	print_error_message(err_info, input, msgbody);
}

void print_codegen_error_message(error_info err_info, char* input) {
	error_code err_code = err_info.error_code;
	char* msgbody = error_msgs[err_code];
	print_error_message(err_info, input, msgbody);
}

void free_environment(compilation_environment *environment) {
	/* 1. Freeing the parse tree */
	free_tree(environment->root);
	environment->root = NULL;

	/* 2. Freeing the tokens */
	for (token** tk = environment->tokens; 
		*tk != NULL; 
		tk++) {
		free(*tk);
		*tk = NULL;
	}
	free(environment->tokens);
	environment->tokens = NULL;

	/* Freeing the lexemes */
	for (preprocessed_word** pw = environment->words; 
		*pw != NULL; 
		pw++) {
		free((*pw)->word);
		free(*pw);
	}
	free(environment->words);
	free(environment->input);
	free(environment);
}

compilation_environment* analyse(char* filename) {
	/* Preprocessing */
	if (TESTS_REPORT_STATE) { printf("\tPreprocessing...\n"); }
	preprocessed_word** words = malloc(sizeof(preprocessed_word*) * PREPROCESSOR_BUFFER_SIZE);
	char* file = load_test_file_into_char_ptr(filename);
	preprocessor(file, filename, words);

	/* Tokenizing */
	if (TESTS_REPORT_STATE) { printf("\tTokenizing...\n"); }
	token** tokens = malloc(sizeof(token*) * TOKENS_BUFFER_SIZE);
	tokenizer(words, tokens);
	if (TESTS_SHOW_LEX_RESULT) { print_token_list(tokens); }

	/* Parsing */
	if (TESTS_REPORT_STATE) { printf("\tParsing...\n"); }
	ast_node* root = parse(tokens);
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
		&& root->err_info.contains_error == false;

	/* Return the environment so it can be used later. */
	compilation_environment* env = malloc(sizeof(compilation_environment));
	env->input = file;
	env->words = words;
	env->tokens = tokens;
	env->root = root;
	return env;
}

char* synthesize(compilation_environment* env) {

	generated_file* file = gen_module(env->root);
	if (file == NULL) { printf("failed to generate file from parse tree"); return NULL; }
	char* target = malloc(sizeof(char) * (1 << 18));
	for (int i = 0; i < (1 << 18); i++) { target[i] = 0; }
	if (file->err->contains_error) {
		print_codegen_error_message(*(file->err), env->input);
	}
	output_target_from_file(file, target);
	return target;
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

	/* Managing memory */	
	if (TESTS_REPORT_STATE) { printf("\tManaging Memory...\n"); }
	compilation_environment *env = analyse(test);
	
	if (PRINT_TEST_TIMES) { end = clock(); }

	/* Validating */
	ast_node* root = env->root;
	int size = root->size;
	int len = 0;
	for (; env->tokens[len] != NULL; len++);
	bool large_as_expected = size == len;
	bool passed = large_as_expected
		&& root != NULL
		&& root->err_info.contains_error == false;
	if (!passed && TEST_ENABLE_ERROR_MSG) {
		print_parser_error_message(root->err_info, env->input);
	}
	free_environment(env);

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
void run_tests(char* test_suite[]) {
	time_t start, end;
	int tests_ran = 0, tests_passed = 0;
	if (PRINT_TEST_TIMES) { start = clock(); }
	for (int i = 0; test_suite[i] != NULL; i++) {
		tests_passed += run_test(test_suite[i], i + 1);
		tests_ran++;
	}
	if (PRINT_TEST_TIMES) {
		end = clock();
		double cpu_clocks_used = (double)end - start;
		double cpu_time_used = (double)cpu_clocks_used / CLOCKS_PER_SEC;
		printf("All tests completed in %.2fs.\n", cpu_time_used);
	}
	if (PRINT_TEST_REPORT) {
		double percent_passed = ((double) tests_passed / (double) tests_ran) * 100;
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

void synthesize_test(char* filename) {
	compilation_environment* env = analyse(filename);
	printf("driver.c:%s: outputing synthesized code.\n", filename);
	if (env->root->err_info.contains_error) {
		printf("driver.c: failed to generate from source file due to fault encountered during parsing.\n");
		print_parser_error_message(env->root->err_info, env->input);
		return;
	}
	char* syn = synthesize(env);
	printf("%s", syn);
}

/**
* The job of main here is to act as a pipeline where input comes 
* in as a char[] in source form and output comes out as a token[] 
* in tokenized form.
*/
void main(int argc, char *argv[]) {
	
	if (DEBUG_TEST_TOKENIZER) {
		test_tokenize();
	}

	if (DEBUG_RUN_STANDARD_TESTS) {
		/* Performing tests */
		printf("Performing tests...\n");
		clock_t start, end;
		run_tests(standard_test_suite);
	}

	if (DEBUG_RUN_ROSSETTA_CODE_TESTS) {
		/* Performing Rosetta Code tests */
		printf("Performing Rosetta Code tests...\n");
		run_tests(rosetta_code_tests);
	}

	if (DEBUG_RUN_C_TEST_SUITE_TESTS) {
		/* Performing Rosetta Code tests */
		printf("Performing C Test Suite tests...\n");
		run_tests(c_test_suite_tests);
	}
	synthesize_test("./tests/c-testsuite/00033.ll");
}

