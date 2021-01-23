#include "parser.h"

char* test1 =
"%c = fmul i32 %a, %b";

char* test2 = "%3 = load i32 * %a.addr, align 4";

char* test3 =
"define i32 @sum(i32 %a, i32 %b) {\n\
entry:\n\
	 %c = fmul i32 %a, %b\n\
	%3 = load i32 * %a.addr, align 4\n\
	ret i32 %add\n\
}";

char* test4 =
"define i32 @sum(i32 %a, i32 %b) #0 {\n\
entry:\n\
	%a.addr = alloca i32, align 4\n\
	%b.addr = alloca i32, align 4\n\
	store i32 %a, i32* %a.addr, align 4\n\
	store i32 %b, i32* %b.addr, align 4\n\
	%0 = load i32 * %a.addr, align 4\n\
	%1 = load i32 * %b.addr, align 4\n\
	%add = add nsw i32 %0, %1\n\
	ret i32 %add\n\
}";

char* test5 =
"define i32 @main() #0 {\n\
 %1 = alloca i32, align 4\n\
  %2 = alloca i32, align 4\n\
  %3 = alloca i32, align 4\n\
  %4 = alloca i32, align 4\n\
  %5 = alloca i32, align 4\n\
  %6 = alloca i32, align 4\n\
  store i32 0, i32* %1, align 4\n\
  store i32 30, i32* %2, align 4\n\
  store i32 0, i32* %3, align 4\n\
  store i32 0, i32* %4, align 4\n\
  store i32 0, i32* %6, align 4\n\
  br label %7\n\
\n\
; <label>:7:  ; preds = %25, %0\n\
  %8 = load i32, i32* %6, align 4\n\
  %9 = load i32, i32* %2, align 4\n\
  %10 = icmp slt i32 %8, %9\n\
  br i1 %10, label %11, label %28\n\
\n\
; <label>:11: ; preds = %7\n\
  %12 = load i32, i32* %6, align 4\n\
  %13 = icmp sle i32 %12, 1\n\
  br i1 %13, label %14, label %16\n\
\n\
; <label>:14: ; preds = %11\n\
  %15 = load i32, i32* %6, align 4\n\
  store i32 %15, i32* %5, align 4\n\
  br label %22\n\
\n\
; <label>:16:  preds = %11\n\
  %17 = load i32, i32* %3, align 4\n\
  %18 = load i32, i32* %4, align 4\n\
  %19 = add nsw i32 %17, %18\n\
  store i32 %19, i32* %5, align 4\n\
  %20 = load i32, i32* %4, align 4\n\
  store i32 %20, i32* %3, align 4\n\
  %21 = load i32, i32* %5, align 4\n\
  store i32 %21, i32* %4, align 4\n\
  br label %22\n\
\n\
; <label>:22: ; preds = %16, %14\n\
  %23 = load i32, i32* %5, align 4\n\
  br label %25\n\
\n\
; <label>:25: ; preds = %22\n\
  %26 = load i32, i32* %6, align 4\n\
  %27 = add nsw i32 %26, 1\n\
  store i32 %27, i32* %6, align 4\n\
  br label %7\n\
\n\
; <label>:28:; preds = %7\n\
  ret i32 0\n\
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
		preprocessor(test5, words);
		token* tokens[TOKENS_BUFFER_SIZE];
		tokenizer(words, tokens);
		//print_token_list(tokens);
		ast_node* root = match_module(tokens);
		print_ast(root);
	}
}