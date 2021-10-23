#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "../stack/stackconfig.h"
#include "text_processing.h"
#include "../stack/errors_and_logs.h"

//#define PRINTNUMBEROFLINE

//=============================================================================
char* text_init(const char* filename, struct Text* text) {

	extern FILE* log_file;

	//filename = "hamlet.txt";

	assert(filename != NULL);
	assert(text != NULL);

	errno = 0;

	char* buf = file_to_buf_copy(filename, text);
	if (buf == NULL)
		return NULL;

	strings_init(buf, text);

	#ifdef LOGS
		fprintf(log_file, "Number of strings read is %ld\n Size of the file: %ld.\n",
																text->strings_number,
																text->size);
	#endif

	return buf;
}

//============================================================================
char* file_to_buf_copy(const char* filename, struct Text* text) {

	assert(filename != NULL);
	assert(text != NULL);

	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) 
		return NULL;
	
	text->size = file_size(fp);
	if (text->size < -1)
		return NULL;

	char* buf = copy_data_to_buf(text->size, fp);

	fclose(fp);

	#ifdef LOGS
		fprintf(log_file, "File %s copied to buffer. Size of copied data is %ld.\n\n",
																		    filename,
																		 text->size);
	#endif

	return buf;
}

//=============================================================================
long file_size(FILE* fp) {

	smpl_log_report();

	assert(fp != NULL);

	int fseek_return = fseek(fp, 0L, SEEK_END);
	if (fseek_return != 0) 
		return -1;

	long size = ftell(fp);
	if (size == -1L) 
		return -1;

	rewind(fp);

	return size;
}

//============================================================================
char* copy_data_to_buf(long size, FILE* fp) {

	smpl_log_report();

	assert(fp != NULL);

	char* buf = (char*)calloc(size + 1, sizeof(char));
	if   (buf == NULL) {

		set_global_error_code(CANNOT_ALLOCATE_MEM);
		global_error_process();

		return NULL;
	}

	long end_of_file = fread(buf, sizeof(char), size, fp);
	if  (end_of_file != size) {

		set_global_error_code(FREAD_ERR);
		global_error_process();

		return NULL;
	}

	buf[end_of_file] = '\0';

	return buf;
}

//============================================================================
long count_strings(char* buf) {

	assert(buf != NULL);

	char* n_pointer = buf;
	long strings_number = 0;

	while ((n_pointer = strchr(n_pointer, '\n')) != NULL) {

		strings_number++;
		n_pointer++;
	}

	#ifdef LOGS
		fprintf(log_file, "Strings counted. Number of strings %ld.\n", strings_number);
	#endif

	return strings_number;
}

//============================================================================
void strings_init(char* buf, struct Text* text) {

	assert(buf != NULL);
	assert(text != NULL);

	text->strings_number = count_strings(buf);

	text->strings = (struct String*)calloc(text->strings_number,
		sizeof(struct String));
	assert(text->strings != NULL);

	struct String* starting_pointer = text->strings;

	char* string_start = strtok(buf, "\n");
	assert(string_start != NULL);

	text->strings_number = 0;
	long length = 0;

	while (string_start != NULL) {

		if (does_contain_letters(string_start)) { 

			string_start = string_skip_blank(string_start);
			length = strlen(string_start);
			text->strings_number++;

			string_init(text->strings, length, string_start, text->strings_number);

			text->strings++;
		}
		string_start = strtok(NULL, "\n");
	}

	text->strings = starting_pointer;

	#ifdef LOGS
		fprintf(log_file, "Strings (expept empty strings) initializated."
								 " Number of initializated strings %ld\n",
													text->strings_number);
	#endif


}

//===========================================================================
int does_contain_letters(const char* string) {

	assert(string != NULL);

	const char* string_ptr = string;
	while (*string_ptr != '\0') {

		if (isalpha(*string_ptr)) return 1;
		string_ptr++;
	}

	return 0;
}

//============================================================================
char* string_skip_blank(char* string) {

	assert(string != NULL);

	while (*string == ' ') 
		    string++;

	return string;
}

//============================================================================
void string_init(struct String* strings, long length, char* data, long number) {

	assert(strings != NULL);
	assert(data != NULL);

	strings->length = length;
	strings->data   = data;
	strings->number = number;

}

//==============================================================================================
void clear_strings(struct Text* text) {

	assert(text != NULL);

	if (text->strings_number != 0)
		free(text->strings);

	text->strings_number = 0;

	#ifdef LOGS
		fprintf(log_file, "Memory allocated for strings is free.\n");
	#endif
}

//===========================================================================================

struct Text* print_strings(struct Text* text, const char* filename) {

	assert(text != NULL);
	assert(filename != NULL);

	FILE* fp = fopen(filename, "wb");
	if (fp == NULL) 
		return NULL;

	for (long string_count = 0; string_count < text->strings_number; string_count++) {


		#ifdef PRINTNUMBEROFLINE
			fprintf(fp, "%5ld %5ld", text->strings[string_count].number, \
									text->strings[string_count].length);
		#endif

		fprintf(fp, " %s\n", text->strings[string_count].data);

	}

	int fclose_return =  fclose(fp);
	if (fclose_return != 0)
		return NULL;
	
	return text;
}

//============================================================================================

void clear_buf(char* buf) {
	
	assert(buf != NULL);

	free(buf);
}


//==============================================================================================

void text_clear_mem(struct Text* text, char* buf) {

	assert(text != NULL);
	assert(buf != NULL);

	clear_buf(buf);
	clear_strings(text);
}

//==============================================================================================