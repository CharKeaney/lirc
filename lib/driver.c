#include "parser.h"

char* test1 =
"%c = add i32 %a, %b";

char* test2 =
"define i32 @sum(i32 %a, i32 %b) #0 {\n\
entry:\n\
	%a.addr = alloca i32, align 4\n\
		%b.addr = alloca i32, align 4\n\
		store i32 %a, i32* %a.addr, align 4\n\
		store i32 %b, i32* %b.addr, align 4\n\
		%0 = load i32* %a.addr, align 4\n\
		%1 = load i32* %b.addr, align 4\n\
		%add = add nsw i32 %0, %1\n\
		ret i32 %add\n\
}";

void main(int argc, char* argv[]) {
	/*
		The job of main here is to act as a pipeline
		where input comes in as a char[] in source form
		and output comes out as a token[] in tokenized form.
	*/
	{
		if (DEBUG_TEST_TOKENIZER) {
			test_tokenize();
		}
		char* words[PREPROCESSOR_BUFFER_SIZE];
		preprocessor(test1, words);
		token* tokens[TOKENS_BUFFER_SIZE];
		tokenizer(words, tokens);
		print_token_list(tokens);
		ast_node* root = match_command(tokens);
		print_ast(root);
	}
}