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

char* test7 =
"define i32 @main() #0 {\n\
  %1 = alloca i8*, align 8\n\
  %2 = alloca i8*, align 8\n\
  %3 = alloca i64, align 8\n\
  %4 = alloca i8*, align 8\n\
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i32 0, i32 0), i8** %1, align 8\n\
  store i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i32 0, i32 0), i8** %2, align 8\n\
  %5 = load i8*, i8** %1, align 8\n\
  %6 = call i64 @strlen(i8* %5)\n\
  %7 = add i64 %6, 1\n\
  store i64 %7, i64* %3, align 8\n\
  %8 = load i64, i64* %3, align 8\n\
  %9 = call i8* @malloc(i64 %8)\n\
  store i8* %9, i8** %4, align 8\n\
  %10 = load i8*, i8** %4, align 8\n\
  %11 = load i8*, i8** %1, align 8\n\
  %12 = load i64, i64* %3, align 8\n\
  %13 = load i8*, i8** %4, align 8\n\
  %14 = call i64 @llvm.objectsize.i64.p0i8(i8* %13, i1 false)\n\
  %15 = call i8* @__strncat_chk(i8* %10, i8* %11, i64 %12, i64 %14) #4\n\
  %16 = load i8*, i8** %2, align 8\n\
  %17 = call i64 @strlen(i8* %16)\n\
  %18 = load i64, i64* %3, align 8\n\
  %19 = add i64 %18, %17\n\
  store i64 %19, i64* %3, align 8\n\
  %20 = load i8*, i8** %4, align 8\n\
  %21 = load i64, i64* %3, align 8\n\
  %22 = call i8* @realloc(i8* %20, i64 %21)\n\
  store i8* %22, i8** %4, align 8\n\
  %23 = load i8*, i8** %4, align 8\n\
  %24 = load i8*, i8** %2, align 8\n\
  %25 = load i64, i64* %3, align 8\n\
  %26 = load i8*, i8** %4, align 8\n\
  %27 = call i64 @llvm.objectsize.i64.p0i8(i8* %26, i1 false)\n\
  %28 = call i8* @__strncat_chk(i8* %23, i8* %24, i64 %25, i64 %27) #4\n\
  %29 = load i8*, i8** %4, align 8\n\
  %30 = call i32 @puts(i8* %29)\n\
  %31 = load i8*, i8** %4, align 8\n\
  ret i32 0\n\
}";
/*
  call void @free(i8* %31)\n\
  ret i32 0\n\
}";*/

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
		preprocessor(test7, words);
		token* tokens[TOKENS_BUFFER_SIZE];
		tokenizer(words, tokens);
		print_token_list(tokens);
		ast_node* root = match_module(tokens);
		print_ast(root);
	}
}