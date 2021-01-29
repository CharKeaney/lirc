
#include "parser.h"

#define RUN_TESTS false

char* test1 =
"define i32 @sum(i32 %a, i32 %b) {\n\
entry:\n\
	 %c = fmul i32 %a, %b\n\
	%3 = load i32 * %a.addr, align 4\n\
	ret i32 %add\n\
}";

char* test2 =
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

char* test3 =
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

char* test4 =
"define internal void @__crystal_main(i32 %argc, i8** %argv) {\n\
alloca:\n\
  %num = alloca i32\n\
  br label %entry\n\
\n\
entry:                                            ; preds = %alloca\n\
  store i32 %argc, i32* @ARGC_UNSAFE\n\
  store i8** %argv, i8*** @ARGV_UNSAFE\n\
  store i32 0, i32* %num\n\
  br label %while\n\
\n\
while:                                            ; preds = %body, %entry\n\
  %0 = load i32, i32* %num\n\
  %1 = icmp slt i32 %0, 5\n\
  br i1 %1, label %body, label %exit\n\
\n\
body:                                             ; preds = %while\n\
  %2 = load i32, i32* %num\n\
  %3 = add i32 %2, 1\n\
  store i32 %3, i32* %num\n\
  br label %while\n\
\n\
exit:                                             ; preds = %while\n\
  ret void\n\
}";

char* test5 =
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
  call void @free(i8* %31)\n\
  ret i32 0\n\
}";

test6 = 
"define internal %String* @__crystal_main(i32 %argc, i8** %argv) {\n\
alloca:\n\
  %test = alloca %TestClass*\n\
  %output = alloca %String*\n\
  br label %entry\n\
\n\
entry:                                            ; preds = %alloca\n\
  store i32 %argc, i32* @ARGC_UNSAFE\n\
  store i8** %argv, i8*** @ARGV_UNSAFE\n\
  %0 = call %TestClass* @\"*TestClass::new<String>:TestClass\"(%String* bitcast ({ i32, i32, i32, [5 x i8] }* @\"\'Demo\'\" to %String*))\n\
  store %TestClass* %0, %TestClass** %test\n\
  %1 = load %TestClass*, %TestClass** %test\n\
  store %String* bitcast ({ i32, i32, i32, [20 x i8] }* @\"\'This is a TestClass...\'\" to %String*), %String** %output\n\
  ret %String* bitcast ({ i32, i32, i32, [20 x i8] }* @\"\'This is a TestClass...\'\" to %String*)\n\
}";

/*
declare i32 @printf(i8*, ...)

; Function Attrs: uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
entry:
  %0 = call %String* @__crystal_main(i32 %argc, i8** %argv)
  ret i32 0
}

; Function Attrs: uwtable
define internal %TestClass* @"*TestClass::new<String>:TestClass"(%String* %name) #0 {
alloca:
  %_ = alloca %TestClass*
  br label %entry

entry:                                            ; preds = %alloca
  %malloccall = tail call i8* @malloc(i32 ptrtoint (%TestClass* getelementptr (%TestClass, %TestClass* null, i32 1) to i32))
  %0 = bitcast i8* %malloccall to %TestClass*
  %1 = bitcast %TestClass* %0 to i8*
  call void @llvm.memset.p0i8.i32(i8* %1, i8 0, i32 ptrtoint (%TestClass* getelementptr (%TestClass, %TestClass* null, i32 1) to i32), i32 4, i1 false)
  %2 = getelementptr inbounds %TestClass, %TestClass* %0, i32 0, i32 0
  store i32 6, i32* %2
  store %TestClass* %0, %TestClass** %_
  %3 = load %TestClass*, %TestClass** %_
  %4 = call %String* @"*TestClass#initialize<String>:String"(%TestClass* %3, %String* %name)
  %5 = load %TestClass*, %TestClass** %_\n\
  ret %TestClass* %5\n\
}";*/



bool test_passed(char* test) {
	char* words[PREPROCESSOR_BUFFER_SIZE];
	preprocessor(test, words);
	token* tokens[TOKENS_BUFFER_SIZE];
	tokenizer(words, tokens);
	
	int len = 0;
	for (; tokens[len] != NULL; len++);

	ast_node* root = match_module(tokens);
	return root->size == len;
}

void run_test(char* test, int test_num) {
	printf("\t[%c] Test %d %s.\n",
		(test_passed(test)) ? 'Y' : 'N',
		test_num,
		(test_passed(test)) ? "Passed" : "Failed");
}

void run_tests(char** tests[]) {
	for (int i = 0; tests[i] != NULL; i++) {
		run_test(*tests[i], i + 1);
	}
}

char **tests[] = {
	&test1, &test2, &test3, &test4, &test5, //&test6
};


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

		if (RUN_TESTS) {
			printf("Performing tests...\n");
			run_tests(tests);
		}

		run_test(test6, 6);

	}
}