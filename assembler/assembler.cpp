
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>

#include "assembler.h"
#include "../stack/stackconfig.h"

//===================================================================

static char operation_name_buf[OPERATION_NAME_BUF_SIZE] = { 0 };

//===================================================================

int text_remove_comments_(struct Text* text, FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	if (text == NULL)
		set_and_process_err(INV_TEXT_PTR);

	for (int str_count = 0; str_count < text->strings_number; str_count++) {

		char* ptr = strchr(text->strings[str_count].data, ';');

		if (ptr != NULL)
		   *ptr = '\0';
	}
	
	return 0;
}

//=============================================================================


#define read_elem_t_arguments(num)                                          \
	{                                                                       \
		asmstruct->header.elem_t_args_number += num;                        \
                                                                            \
		oper_code = oper_code | IMM_MASK;                                   \
		write_listing(oper_code, operation_name_buf, 						\
					  asmstruct->ip - asmstruct->code);             		\
																			\
		*asmstruct->ip = oper_code;											\
		asmstruct->ip++;													\
																			\
		asmstruct_ip_check(asmstruct);										\
																			\
		int byte_offset = 0;                                                \
																			\
		for (int ct = 0; ct < num; ct++) {                                  \
																			\
			sscanf_val = sscanf(string + byte_ct, " " ELEM_SPEC "%n",       \
											   &value, &byte_offset);       \
			if (sscanf_val != 1)                                            \
				set_and_process_err(INV_INSTRUCTION_STR);                   \
																			\
			byte_ct += byte_offset;											\
																			\
			*(elem_t*)asmstruct->ip = value;								\
			asmstruct->ip += sizeof(elem_t);								\
																			\
			write_value_to_listing(&value, sizeof(elem_t));                 \
			asmstruct_check(asmstruct);										\
		}                                                                   \
	}

//===================================================================

#define read_register_arguments(num)                                        \
    {                                                                       \
		asmstruct->header.register_args_number += num;                      \
                                                                            \
		oper_code = oper_code | REGISTER_MASK;                              \
		write_listing(oper_code, operation_name_buf, 						\
					  asmstruct->ip - asmstruct->code);             		\
																			\
		*asmstruct->ip = oper_code;											\
		asmstruct->ip++;													\
																			\
		asmstruct_ip_check(asmstruct);										\
																			\
		int byte_offset = 0;                                                \
																			\
		for (int ct = 0; ct < num; ct++) {                                  \
																			\
			sscanf_val = sscanf(string + byte_ct, " %cx%n",             	\
								  &register_number, &byte_offset);          \
																			\
			if (sscanf_val != 1)                                            \
				set_and_process_err(INV_INSTRUCTION_STR);                   \
																			\
			byte_ct += byte_offset;                                         \
			register_number -= ASCII_OFFSET;								\
			                                                                \
			*asmstruct->ip = register_number;								\
			asmstruct->ip++;												\
			write_value_to_listing(&register_number, sizeof(char));         \
																			\
			asmstruct_check(asmstruct);										\
		}                                                                   \
	}


//=============================================================================
#undef DEF_CMD_

#define DEF_CMD_(num, name, op_code, hash)									\
																			\
	if (oper_hash == hash) {				    							\
		                                                                    \
        oper_code = CMD_##name;												\
		asmstruct->header.commands_number++;								\
																			\
		if (num != 0) {														\
																			\
			char ch = 0;													\
			if (*(string + byte_ct) == '[' ) {								\
																			\
				oper_code = oper_code | RAM_MASK;							\
				asmstruct->header.ram_using_commands_number++;				\
				byte_ct++;													\
			}																\
																			\
			int sscanf_val = sscanf(string + byte_ct,                       \
									ELEM_SPEC, &value);                     \
																			\
			if (sscanf_val == 1)                                            \
				read_elem_t_arguments(num)									\
			else                                                            \
				read_register_arguments(num)								\
																			\
			if ((oper_code & ~OPER_CODE_MASK) >= RAM_MASK) {				\
																			\
				if (*(string + byte_ct) != ']')								\
					set_and_process_err(INV_INSTRUCTION_STR);				\
			}																\
		}	                                                                \
		else {                                                              \
			                                                                \
			write_listing(oper_code, operation_name_buf, 					\
						 asmstruct->ip - asmstruct->code);					\
																			\
			*asmstruct->ip = oper_code;										\
			asmstruct->ip++;												\
		}                                                                   \
	}																		\
	else

//===================================================================

int process_string_(const char* string, AsmStruct* asmstruct, 
				                        FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);
	char_ptr_check(string);

	extern char operation_name_buf[OPERATION_NAME_BUF_SIZE];
	int byte_ct = 0;

	int sscanf_ret = 
		sscanf(string, "%s %n", operation_name_buf, &byte_ct);

	operation_name_buf[OPERATION_NAME_BUF_SIZE - 1] = '\0';
	
	if (sscanf_ret == 0)
		set_and_process_err(INV_INSTRUCTION_STR);

	int oper_code = 0;
	elem_t value = 0;
	char register_number = 0;       

	int64_t oper_hash = get_hash(operation_name_buf, 
							 OPERATION_NAME_BUF_SIZE);
							
	#include "../text_files/commands.txt"
	//else
		set_and_process_err(INV_INSTRUCTION_STR);

	#undef DEF_CMD_

	clear_opernamebuf();

	return 0;
}

//==================================================================

int clear_opernamebuf_(FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	extern char operation_name_buf[OPERATION_NAME_BUF_SIZE];

	for (int ct = 0; ct < OPERATION_NAME_BUF_SIZE; ct++)
		operation_name_buf[ct] = 0;

	return 0;
}

//===================================================================

int convert_operations_to_binary_(struct Text* text, AsmStruct* asmstruct,
													FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	open_listing_file();

	asmstruct_check(asmstruct);
	text_ptr_check(text);

	if (text->strings_number <= 0)
		set_and_process_err(TEXT_NO_STRINGS);
	
	asmstruct->ip = asmstruct->code + HDRSIZE;

	for (int str_count = 0; str_count < text->strings_number; str_count++) {

		int value = process_string(text->strings[str_count].data, asmstruct);
		if (value == -1) 
			return -1;
	}

	close_listing_file();

	return 0;
}

//===================================================================

FILE* open_code_file_(const char* filename, const char* mode, FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	if (filename == NULL) {

		set_global_error_code(INV_FILE_NAME);
		global_error_process();

		return NULL;
	}

	if (mode == NULL) {

		set_global_error_code(NULL_STRING);
		global_error_process();

		return NULL;
	}

	FILE* fp = fopen(filename, mode);

	if (fp == NULL) {

		set_global_error_code(FOPEN_ERROR);
		global_error_process();

		return NULL;
	}

	return fp;
}

//===================================================================

int close_code_file_(FILE* fp, FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	file_ptr_check(fp);

	int fclose_return_value = fclose(fp);

	if (fclose_return_value == EOF)
		set_and_process_err(FCLOSE_ERROR);

	return 0;
}

//===================================================================

int header_make_(AsmStruct* asmstruct, FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);

	asmstruct->header.signature = SIGN;
	asmstruct->header.version = VERSION;

	int file_size = asmstruct->ip - asmstruct->code;
	if (file_size != sizeof(struct Header)
		+ asmstruct->header.register_args_number * sizeof(char)
		+ asmstruct->header.elem_t_args_number * sizeof(elem_t)
		+ asmstruct->header.commands_number * sizeof(char))

		set_and_process_err(CODE_FILE_SIZE_ERR);

	asmstruct->header.file_size = file_size;

	*(struct Header*)asmstruct->code = asmstruct->header;

	return 0;
}

//====================================================

int _write_code(AsmStruct* asmstruct, FILE* code_file, 
								FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	file_ptr_check(code_file);
	asmstruct_check(asmstruct);

	int fwrite_ret = fwrite(asmstruct->code, 
							sizeof(char), 
							asmstruct->header.file_size, 
							code_file);

	if (fwrite_ret != asmstruct->header.file_size) 
		set_and_process_err(FWRITE_ERR);

	return 0;
}

//====================================================

int open_listing_file_(FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	extern FILE* asm_listing_file;

	FILE* fp = fopen(ASM_LISTING_FILENAME, "wb");

	if (fp == NULL)
		set_and_process_err(FOPEN_ERROR);

	asm_listing_file = fp;

	return 0;
}

//====================================================

int close_listing_file_(FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	extern FILE* asm_listing_file;

	int fclose_ret = fclose(asm_listing_file);

	if (fclose_ret == EOF)
		set_and_process_err(FCLOSE_ERROR);

	return 0;
}

//====================================================

int write_value_to_listing(void* base, int size) {

	extern FILE* asm_listing_file;

	char* base_char = (char*)base;

	if (size == sizeof(elem_t)) {

		fprintf(asm_listing_file, "Argument value: " ELEM_SPEC " ", *(elem_t*)base_char);
	}

	fprintf(asm_listing_file, "Written bytes: ");

	unsigned char* ptr = (unsigned char*)base_char;

	for (int ct = 0; ct < size ; ct++) {

		fprintf(asm_listing_file, "%02x ", *(ptr + ct));
	}

	return 0;
}	

//====================================================

int write_listing(int oper_code, const char* oper_name, int prev_pos) {

	extern FILE* asm_listing_file;

	file_ptr_check(asm_listing_file);
	char_ptr_check(oper_name);

	fprintf(asm_listing_file, "\n\n" "Current position: %03x ", prev_pos);

	fprintf(asm_listing_file, "Command name: %-*s ",OPERATION_NAME_BUF_SIZE, oper_name);
	fprintf(asm_listing_file, "Written bytes: %02x ", (unsigned char)oper_code);

	return 0;
}

//===================================================================

#define DEF_CMD_(num, name, code, hash) \
	case CMD_##name: ;

void check_unique_(FUNC_FILE_LINE_PARAMS) {

	asm_log_report();

	switch (0) {

		#include "../text_files/commands.txt"
	}
}

#undef DEF_CMD_

//===================================================================

int64_t get_hash_(char* base, unsigned int len, FUNC_FILE_LINE_PARAMS) {

    log_report();

    assert(base);

    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ len;

    const unsigned char* data = (const unsigned char*)base;
    unsigned int k = 0;

    while (len >= 4) {

        k = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;

    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;
        case 2:
            h ^= data[1] << 8;
        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}