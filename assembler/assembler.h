#pragma once

#include "processor_general.h"
#include "text_processing.h"
#include "../stack/errors_and_logs.h"
#include "../stack/errors.h"

//===================================================================

struct AsmStruct {

	unsigned char code[ASM_MAX_CODE_SIZE];
	unsigned char* ip;

	struct Header header;
};

//===================================================================

#define asmstruct_check(asmstruct) {                                \
																	\
	if (asmstruct == NULL)											\
		set_and_process_err(ASMSTRUCT_INV_PTR); 					\
																	\
	if (asmstruct->code == NULL)									\
		set_and_process_err(ASMSTRUCT_EMPTY_CODE_ARR);              \
																	\
	if (asmstruct->ip - asmstruct->code > ASM_MAX_CODE_SIZE)        \
		set_and_process_err(ASMSTRUCT_INV_IP);                      \
																	\
	header_ptr_check(&asmstruct->header);							\
}

#define asmstruct_ip_check(asmstruct) {								\
																	\
	if (asmstruct->ip - asmstruct->code > ASM_MAX_CODE_SIZE)		\
		set_and_process_err(ASMSTRUCT_INV_IP)						\
}

#define file_ptr_check(fp) {                                        \
                                                                    \
	if (fp == NULL)                                                 \
		set_and_process_err(INV_FILE_PTR);                          \
}

#define header_ptr_check(header) {                                  \
                                                                    \
	if (header == NULL)                                             \
		set_and_process_err(INV_HDR_PTR);                           \
}

#define text_ptr_check(text) {                                      \
                                                                    \
	if (text == NULL)                                               \
		set_and_process_err(INV_TEXT_PTR);                          \
}

#define char_ptr_check(string) {                                    \
                                                                    \
	if (string == NULL)                                             \
		set_and_process_err(NULL_STRING);                           \
}

//===================================================================

int open_listing_file_(FUNC_FILE_LINE_PARAMS);

int close_listing_file_(FUNC_FILE_LINE_PARAMS);

int write_listing(int oper_code, const char* oper_name, int prev_pos);

int write_value_to_listing(void* base, int size);

//===================================================================

void check_unique_(FUNC_FILE_LINE_PARAMS);

//===================================================================

#define check_unigue() \
		check_unique_(FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define open_listing_file() \
		open_listing_file_(FUNC_FILE_LINE_GET_ARGS)

#define close_listing_file() \
		close_listing_file_(FUNC_FILE_LINE_GET_ARGS)

#define clear_opernamebuf() \
		clear_opernamebuf_(FUNC_FILE_LINE_GET_ARGS)

#define text_remove_comments(text) \
		text_remove_comments_(text, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define convert_operations_to_binary(text, asmstruct)  \
		convert_operations_to_binary_(text, asmstruct, \
							  FUNC_FILE_LINE_GET_ARGS) \

#define process_string(string, asmstruct) \
		process_string_(string, asmstruct, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define open_code_file(filename, mode) \
		open_code_file_(filename , mode, FUNC_FILE_LINE_GET_ARGS)

#define close_code_file(fp) \
		close_code_file_(fp, FUNC_FILE_LINE_GET_ARGS)

#define write_code(asmstruct, code_file) \
	   _write_code(asmstruct, code_file, FUNC_FILE_LINE_GET_ARGS)

#define header_make(asmstruct) \
		header_make_(asmstruct, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define get_hash(base, len) \
        get_hash_(base, len, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

int clear_opernamebuf_(FUNC_FILE_LINE_PARAMS);

int64_t get_hash_(char* base, unsigned int len, FUNC_FILE_LINE_PARAMS);

//===================================================================

int text_remove_comments_(struct Text* text, FUNC_FILE_LINE_PARAMS);

int convert_operations_to_binary_(struct Text* text, 
								  AsmStruct* asmstruct,  
								  FUNC_FILE_LINE_PARAMS);

int process_string_(const char* string, AsmStruct* asmstruct, 
									   FUNC_FILE_LINE_PARAMS);

//===================================================================

FILE* open_code_file_(const char* filename, const char* mode,
									  FUNC_FILE_LINE_PARAMS);

int close_code_file_(FILE* fp, FUNC_FILE_LINE_PARAMS);

int _write_code(AsmStruct* asmstruct, FILE* code_file, FUNC_FILE_LINE_PARAMS);

//===================================================================

int header_make_(AsmStruct* asmstruct, FUNC_FILE_LINE_PARAMS);

//===================================================================

