_______________________________________________________________________________

			██╗     ██╗██████╗  ██████╗
			██║     ██║██╔══██╗██╔════╝
			██║     ██║██████╔╝██║     
			██║     ██║██╔══██╗██║     
			███████╗██║██║  ██║╚██████╗
			╚══════╝╚═╝╚═╝  ╚═╝ ╚═════╝
_______________________________________________________________________________

◆ Project Goal
	
	◇ To create a system capable of converting source code from one language
           to another quickly and easily. This system is different from other 
	   systems in that it should be language independent, allowing for free 
	   translation between any two accepted languages. I.e. C to Java, Java to 
	   JavaScript, JavaScript to C etc. 
	   This can be realised by utilizing the LLVM Intermediate Representation 
	   as a language independent middle end. By doing this we can make use of
	   already existing infrastructure, negating the immediate need for front
	   ends.
	   
	◇ Essentially, the ambitious aim is utilize LLVM IR to create a system 
	   that is to source-to-source compilation what LLVM has become to 
	   source-to-assembly compilation.
	   
	◇ Eventually language specific front ends will need to be wrote in order 
	   to achieve the greater goal of creating code that is humanly readable,
	   preferably abiding to accepted Coding Standards for the respective 
	   language where possible.

_______________________________________________________________________________

◆ Installation

	◇ The project has no complicated build instructions. Simply compiling 
	   driver.c should be sufficient to get the system running. The project 
	   does not use anyincludes outside of what should be present in the 
	   standard library.

	◇ Eventually as the project gets larger, (most likely when more than C
	   is supported) it will become neccessary to create a proper makefile,
	   in which case new installation isntructions will be added.
_______________________________________________________________________________

◆ Q&A.

	◇ What is in the project so far?
	   A preprocessor, lexer, and parser. It takes in a file or character 
	   string and processes it into an Abstract Syntax Tree as defined in 
	   parser.h.
_______________________________________________________________________________

◆ Common bugs.

	◇ My computer says it ran out of memory? 
	   This is an error related to Windows Visual Studio - it does not like 
	   programs using more than a certain amount of memory. The program itself
	   does not use a lot of memory, but due to OS  and other environment 
	   inefficiencies it is usually allocated .5gb regardless of how much it 
	   actually uses. To fix this issue, adjust Visual Studios' alloted memory
	   to the program in the Linker settings. Swapping to another build 
	   environment would also be likely to fix this.

	◇ The lexer returned ERROR_TOKEN for a token?
	   Not all tokens are currently implemented. If this is an obscure token 
	   then it is likely not yet included in the system. Adding tokens is not 
	   difficultthough, and only requires a basic knowledge of C and the 
	   structure of the lexer (lexer.c and lexer.h) and the representation of 
	   tokens in tokens.h
	   
	   ◆ To add a token:
			◇ add the new tokens name to TOKENS.h, 
			◇ add the new tokens name to the lookup table in lexer.h
			◇ add an example of the tokens lexeme to the lookup table
			◇ define one or more states for the token and its mutual 
			   substring in lexer. 
			   (i.e. fptoui and fptosi would need a fpto state since 
			   you can not determistically say which is which without 
			   lookahead past 'o'). 
			◇ add case statements to lexer.c for each state and 
			◇ add a transition function for each state to lexer.c

	◇ The parse tree has (null) in it?
	   This means that the token present in the parse tree does exist, but it 
	   does not have a humanly legible name. To fix this, add the tokens name 
	   to the lookup table in lexer.h alongside an example lexeme for testing.

	◇ The start of the input isn't being parsed? 
	  The parser is not yet suited for parsing declarations. This will 
	  soon be added and is one of the first places for expansion. Skip 
	  to the first function definition to resolve this for the time being.
	  For what it is worth, a decent amount of that information 
	  (i.e. the OS of the target) is not relevant for code generation 
	  anyways.

	◇ The very end of the input isn't being parsed? 
	   The parser is not yet suited for function attributes. 
	   This will soon be added and is one of the first places for 
	   expansion. Truncate the endin order to resolve this for the time 
	   being.

	◇ The error message is not properly showing? 
	   This means for some reason the error has not been assigned a proper
	   error code by the parser. If you have an idea of what the error is 
	   (Hint :  you can figure this out by checking the parser's matching 
	   history using the debug options) then check that that particular 
	   error has an appropriate error code and message assigned to it.

_______________________________________________________________________________

◆ Project roadmap

	◇ Create a codegen.h/codegen.c prototype that supports basic functions.

	◇ Add support to the parser for global declarations and other header 
	   information

	◇ Add support to the parser for function attribute descriptions.

	◇ More properly flesh out the code generator for C

	◇ Develop an intense test suite for the code generator

	◆ Make code generation more flexible and language independent 
	  (no need to add languages other than C yet though)

		◇ Work out a scheme for how language dependent information 
		  can be represented, stored, and later used by the code 
		  generator. Perhaps a DSL could be useful here as opposed 
		  to a purely data oriented system. I.e. each module could 
		  be a 'program' that the code generator runs.

		◇ Make a schema for C and have the newly language 
		  independent code generator use that.

	◇ Conform to a formal coding standard (MISRA C looks promising)

	◇ Add some other languages (likely Java/JavaScript)

	◆ Maintenance (Final stage)

		◇ More properly insure that code produced is safe for safety
		   critical tasks. This includes adding warnings for undefined 
		   behaviour and indeed marking behaviour as undefined in the 
		   first place for certain constructs/data types.

		◇ Add support for more front-ends and back ends.
	

_______________________________________________________________________________

◆ Explanation of files

	◇ README.txt - The file you are reading, dedicated to documenting 
			the programs purpose, basic structure, and future 
			direction.
	
	◇ tests.dir - Directory containing test cases used by the driver
	
	◇ tokens.h - Header containing token information used by the lexer
	
	◇ lexer.h - Header containing declarations related to lexer
	
	◇ lexer.c - Responsible for processing given input string into tokens. 
		     Since it has no main it must be driven by the driver.
	
	◇ parser.h - Header containing declarations related to parser
	
	◇ parser.c - Responsible for processing given token string into an 
				  Abstract Syntax Tree as defined by parser.h
				  Since it has no main it must be driven by 
				  the driver.
	
	◇ driver.c - Responsible for stringing together the parser and lexer
				  in order to process the test cases. 
				  Capable of string constants processing files 
				  but does not yet parse directly from command 
				  line arguments. 
	
	◇ codegen.h - Header containing final code generation information for 
				   codegen.c. Currently Unused.

	◇ codegen.c - Responsible for language independent code generation 
		       through the use of language specific modules.

_______________________________________________________________________________

◇ Author Charlie Keaney at charkeaney@gmail.com
◇ Supervised by Dr John Bustard at Queens University Belfast
