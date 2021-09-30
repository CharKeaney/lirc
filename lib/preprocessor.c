#pragma once

#include "preprocessor.h"
#include "tokens.h"

/**
* Responsible for determining if the preprocessor should ignore these
* input symbols. It differs from is_delimiter as whitespace is never
* used in the output, while a delimiter may be the start of a
* new word and while constitute the next word.
*
* @param chr The character which we validate as whitespace.
*/
bool is_whitespace(char chr) {
	switch (chr) {
	case ' ': case '\t':
	case '\n': case '\r':
	case '\v': case '\f':
	case EOF: case NULL:
		return true;
	default:
		return false;
	}
}

/**
* Responsible for informing the preprocessor as to if the word should
* finish and restart anew including this given symbol.
*/
bool is_delimiter(char chr) {
	switch (chr) {
		case '[': case ']':
		case '(': case ')':
		case '<': case '>':
		case '{': case '}':
		case ',': case ';':
		case '*': case '!':
		case '=':
			return true;
		default:
			return false;
	}
}

/* The various states used by the preprocessor */
typedef enum preprocessor_state {
	EXPECTING_WORD,		/* We expect a word to start (not started yet) */
	IN_WORD,			/* A new word has already started				*/
	IN_COMMENT,			/* We are in the middle of a comment			*/
	ENCOUNTERED_WORD,	/* We have already encountered the full word	*/
} preprocessor_state;

/**
* Responsible for splitting an input string into
* substrings each containing a single word.
* Assuming a well-formed input, one token
* can be made from each word.
*
* @param input The input to preprocess.
* @param output The location to store the preprocessed output.
*
* @see is_whitespace
* @see is_delimiter
*/
void preprocessor(
	char* input, 
	char* filename, 
	preprocessed_word** output) {

	char buffer[PREPROCESSOR_WORD_SIZE];

	long oi = 0, bi = 0; // output and buffer index
	char* ci = input; // Character index
	// For each character in input
	bool in_identifier = false, in_whitespace = false;

	preprocessor_state state = EXPECTING_WORD;

	int line_number = 0;
	int character_number = 0;

	while ((*ci != NULL && *ci != EOF && ci != NULL)
		|| state != EXPECTING_WORD) {

		if (DEBUG_PREPROCESSOR) {
			char* debug_msg = "Preprocessor : "
				"Output Index = %d, "
				"Buffer Index = %-3d, "
				"Character Index = %-3d, "
				"State = %d, "
				"Character = %c\n";
			printf(debug_msg, oi, bi, ci, state, *ci);
		}

		switch (state) {

			case EXPECTING_WORD:

				/* Comments have highest precendence. */
				if (*ci == ';') {
					ci++; character_number++;
					state = IN_COMMENT;
					continue;
				}

				/* Identifiers have lower precedence than comments */
				if (*ci == '"') {
					in_identifier = true;
					buffer[bi++] = *ci++; character_number++;
					state = IN_WORD;
					continue;
				}

				/* Whitespace has higher precedence than delimiters	*/
				if (is_whitespace(*ci)) {
					if (*ci == '\n') {
						line_number++;
						character_number = 0;
						ci++;
						continue;
					}
					ci++; character_number++;
					continue;
				}

				/* Delimiters have lower precedence than whitespace. */
				if (is_delimiter(*ci)) {
					buffer[bi++] = *ci++; character_number++;
					state = ENCOUNTERED_WORD;
					continue;
				}

				/* Add the character to a new word we are starting. */
				buffer[bi++] = *ci++; character_number++;
				state = IN_WORD;
				continue;

			case IN_WORD:

				/* Check if we were in an identifier. */
				if (in_identifier) {

					if (*ci == '"') { /* Check if we are no longer. */
						in_identifier = false;
					}
					buffer[bi++] = *ci++; character_number++; 
					/* and record the character */
					continue;
				}
				
				/* Check if we are entering an identifier. */
				if (*ci == '"') {
					in_identifier = true;
					buffer[bi++] = *ci++; character_number++;
					continue;
				}

				/* Check if we have reached a comment. */
				if (*ci == ';') {
					/* save word if so */
					state = ENCOUNTERED_WORD;
					continue;
				}

				/* Check if we have reached whitespace */
				if (is_whitespace(*ci)) {
					/* save word if so. */
					state = ENCOUNTERED_WORD;
					continue;
				}
				
				/* Check if we have reached a delimiter */
				if (is_delimiter(*ci)) {
					/* save word if so. */
					state = ENCOUNTERED_WORD;
					continue;
				}

				/* Add the character to a new word we are starting. */
				buffer[bi++] = *ci++; character_number++;
				continue;

			case IN_COMMENT:
				/* If we are in comment, just skip to next newline. */
				if (*ci == '\n') {
					/* When we reach new line, we expect a word after.*/
					line_number++;
					character_number = 0;
					ci++;
					state = EXPECTING_WORD;
					continue;
				}
				ci++; character_number++;
				continue;

			case ENCOUNTERED_WORD:
				/* Copy the word as encountered so far into the output */
				buffer[bi] = NULL;
				int final_word_len = strlen(buffer) + 1;
				char* final_word = malloc(final_word_len);
				if (final_word == NULL) {
					printf(
						"Fatal Error : "
						"Failed to allocate memory to store word %s.\n",
						buffer);
					return NULL;
				}
				strcpy_s(final_word, final_word_len, buffer);
				final_word[final_word_len - 1] = NULL;
				/* Record position of this word. */
				position_in_file pos;
				/* Record the character number as the current
				   character number minus the length of the word
				   to get the start of the word.
				   subtract 1 to exclude the ending null*/
				pos.character_number = 
					character_number - (final_word_len - 1);
				pos.line_number = line_number;
				pos.filename = filename;
				/* Save word and position in output. */
				preprocessed_word* word = malloc(sizeof(preprocessed_word));
				word->pos = pos;
				word->word = final_word;
				output[oi++] = word;
				bi = 0;
				state = EXPECTING_WORD;
				continue;
		}
	}
	/* Now we mark a final preprocessed word with EOF. */
	char* eof = (char*) malloc(2);
	*eof = strcpy_s(eof, 2, "\0");

	position_in_file pos;
	pos.character_number = character_number;
	pos.line_number = line_number;
	pos.filename = filename;

	preprocessed_word* final_pw = malloc(sizeof(preprocessed_word));
	final_pw->word = eof;
	final_pw->pos = pos;

	output[oi] = final_pw;
	oi++;
	/* Now mark the end of the output with null. */
	output[oi] = NULL;
}