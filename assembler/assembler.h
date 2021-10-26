#pragma once

#include "processor_general.h"
#include "text_processing.h"
#include "../stack/errors_and_logs.h"
#include "../stack/errors.h"

//===================================================================

struct Str_Jump {

	int string_number;
	int code_ip;
};

//===================================================================

struct Label {

	char label_name[LABEL_NAME_SIZE];
	int code_pos;
	int label_name_length;
};

//===================================================================

struct AsmStruct {

	unsigned char* code;
	unsigned char* ip;

	struct Header header;

	struct Label* labels;
	int labels_number;
	int labels_capacity;

	//Unknown destination jumps
	struct Label* ud_jumps;
	int ud_jumps_number;
	int ud_jumps_capacity;

	struct Str_Jump* str_jumps;
	int str_jumps_number;
	int str_jumps_capacity;

	int* string_start_ips;
	int strings_number;
};

//==========================================================================

#define read_imm_label														\
	do 																		\
	{																		\
		int byte_offset = 0;												\
		int destination_ip = 0;												\
																			\
		sscanf_ret = sscanf(string + byte_ct,								\
					 " %d%n", &destination_ip, &byte_offset);				\
																			\
		if (sscanf_ret != 1)												\
			set_and_process_err(INV_INSTRUCTION_STR);						\
																			\
		if (destination_ip < 0 || destination_ip > ASM_MAX_CODE_SIZE)		\
			set_and_process_err(INV_DESTINATION_OF_JMP);					\
																			\
		asmstruct->header.elem_t_args_number++;								\
																			\
		*(elem_t*)asmstruct->ip = destination_ip;							\
		asmstruct->ip += sizeof(elem_t);									\
																			\
		write_value_to_listing(&destination_ip, sizeof(elem_t));			\
		asmstruct_check(asmstruct);										    \
																			\
		byte_ct += byte_offset;												\
																			\
	} while(0);

//==========================================================================

#define read_label_name														\
	do																		\
	{																		\
		char label_name_buf[LABEL_NAME_SIZE] = { 0 };						\
		int byte_start = 0;													\
		int byte_end   = 0;													\
																			\
		sscanf_ret = sscanf(ptr + 1, " %n%s%n",	&byte_start,                \
											    label_name_buf, &byte_end); \
		printf("\n\n founded %s %ld\n\n", label_name_buf, strlen(label_name_buf));						\
		if (sscanf_ret == 0)												\
			set_and_process_err(INV_INSTRUCTION_STR);						\
																			\
		if (byte_end - byte_start  > LABEL_NAME_SIZE)						\
			set_and_process_err(LABEL_NAME_TOO_THICK);						\
																			\
		int code_pos = 	search_label_name(label_name_buf, asmstruct);		\
																			\
		if (code_pos == -1)													\
			return -1;														\
																			\
		if (code_pos == -2) {												\
																			\
			int ret = add_jump_without_known_label(label_name_buf, 			\
								  asmstruct->ip - asmstruct->code,			\
								                       asmstruct);		    \
			if (ret == -1) 													\
				return -1;													\
		}																	\
																			\
		asmstruct->header.elem_t_args_number++;								\
																			\
		if (code_pos != -2) 												\
			*(elem_t*)asmstruct->ip = code_pos;								\
																			\
		asmstruct->ip += sizeof(elem_t);									\
		asmstruct_check(asmstruct);											\
																			\
		byte_ct += 	ptr + 1 - (string + byte_ct - 1) + byte_end;			\
																			\
	} while(0);

//==========================================================================

#define read_register_argument		                                        \
	do																		\
	{                                                                       \
		oper_code = oper_code | REGISTER_MASK;                              \
																			\
		int byte_offset = 0;                                                \
		char register_number = 0;											\
																			\
		sscanf_ret = sscanf(string + byte_ct, " %cx %n",             		\
						&register_number, &byte_offset);          			\
																			\
		if (sscanf_ret != 1)                                           	 	\
			set_and_process_err(INV_INSTRUCTION_STR);                  	 	\
																			\
		asmstruct->header.register_args_number++;	                        \
																			\
		byte_ct += byte_offset;                                         	\
		register_number -= ASCII_OFFSET;									\
																			\
		if (register_number < 1 || register_number > 4)						\
			set_and_process_err(INV_INSTRUCTION_STR);						\
			                                                                \
		*asmstruct->ip = register_number;									\
		asmstruct->ip++;													\
																			\
		write_value_to_listing(&register_number, sizeof(char));         	\
																			\
		asmstruct_check(asmstruct);											\
																			\
	} while (0);

//===========================================================================

#define read_elem_t_argument		                                        \
	do 																		\
	{                                                                       \
		oper_code = oper_code | IMM_MASK;                                   \
																			\
		int byte_offset = 0;                                                \
																			\
		sscanf_ret = sscanf(string + byte_ct, " " ELEM_SPEC " %n",       	\
										   &value, &byte_offset);      		\
		if (sscanf_ret != 1)                                            	\
			set_and_process_err(INV_INSTRUCTION_STR);          	            \
																			\
		asmstruct->header.elem_t_args_number++;                             \
																			\
		byte_ct += byte_offset;												\
																			\
		*(elem_t*)asmstruct->ip = value;									\
		asmstruct->ip += sizeof(elem_t);									\
																			\
		write_value_to_listing(&value, sizeof(elem_t));                 	\
		asmstruct_check(asmstruct);											\
																			\
	} while(0);

//===========================================================================

#define read_argument														\
	do																		\
	{																		\
		elem_t value = 0;													\
		int sscanf_ret = sscanf(string + byte_ct, ELEM_SPEC, &value);		\
																			\
		if (sscanf_ret == 1) {												\
			read_elem_t_argument											\
			break;															\
		}																	\
		char register_number = 0;											\
		sscanf_ret = sscanf(string + byte_ct, "%c", &register_number);		\
																			\
		if (sscanf_ret == 1) {												\
			read_register_argument   										\
			break;															\
		}																	\
	} while(0);																

//===========================================================================

#define check_is_ram_using_command											\
	do																		\
	{																		\
		if (*(string + byte_ct) == '[' ) {									\
																			\
				oper_code = oper_code | RAM_MASK;							\
				asmstruct->header.ram_using_commands_number++;				\
				byte_ct++;													\
			}																\
	} while(0);																
	
//===========================================================================

#define check_second_square_bracket											\
	do																		\
	{																		\
			if ((oper_code & ~OPER_CODE_MASK) >= RAM_MASK) {				\
																			\
				if (*(string + byte_ct++) != ']')							\
					set_and_process_err(INV_INSTRUCTION_STR);				\
			}																\
	} while(0);


//==========================================================================

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

//===================================================================

#define asmstruct_ip_check(asmstruct) {								\
																	\
	if (asmstruct->ip - asmstruct->code > ASM_MAX_CODE_SIZE)		\
		set_and_process_err(ASMSTRUCT_INV_IP)						\
}

//===================================================================

#define file_ptr_check(fp) {                                        \
                                                                    \
	if (fp == NULL)                                                 \
		set_and_process_err(INV_FILE_PTR);                          \
}

//===================================================================

#define header_ptr_check(header) {                                  \
                                                                    \
	if (header == NULL)                                             \
		set_and_process_err(INV_HDR_PTR);                           \
}

//===================================================================

#define text_ptr_check(text) {                                      \
                                                                    \
	if (text == NULL)                                               \
		set_and_process_err(INV_TEXT_PTR);                          \
}

//===================================================================

#define char_ptr_check(string) {                                    \
                                                                    \
	if (string == NULL)                                             \
		set_and_process_err(NULL_STRING);                           \
}

//===================================================================

#define check_if_string_is_empty(string) {							\
																	\
	char ch = 0; 													\
	char ret = sscanf((string), " %c", &ch); 						\
																	\
	if (ret > 0)													\
		set_and_process_err(INV_INSTRUCTION_STR);					\
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

#define asmstruct_allocate_memory(number, asmstruct) \
	   _asmstruct_allocate_memory(number, asmstruct, \
	   						FUNC_FILE_LINE_GET_ARGS)

#define convert_operations_to_binary(text, asmstruct)  \
		convert_operations_to_binary_(text, asmstruct, \
							  FUNC_FILE_LINE_GET_ARGS) 

#define read_label(string, asmstruct) \
		read_label_(string, asmstruct, FUNC_FILE_LINE_GET_ARGS)

#define add_label(asmstruct, label_name, length) \
	   _add_label(asmstruct, label_name, length, \
	   					FUNC_FILE_LINE_GET_ARGS)	

#define search_label_name(string, asmstruct) \
	   _search_label_name(string, asmstruct, \
	   				FUNC_FILE_LINE_GET_ARGS)

#define add_jump_without_known_label(string, ip, asmstruct) \
	   _add_jump_without_known_label(string, ip, asmstruct, \
	                               FUNC_FILE_LINE_GET_ARGS)

#define add_string_jump(code_ip, dest_string, asmstruct) \
	   _add_strinf_jump(code_ip, dest_string, asmstruct, \
	   							FUNC_FILE_LINE_GET_ARGS)

#define process_string(string, asmstruct, str_number)  \
		process_string_(string, asmstruct, str_number, \
		                     FUNC_FILE_LINE_GET_ARGS)

#define save_string_start_ip(asmstruct, code_ip, number) \
	   _save_string_start_ip(asmstruct, code_ip, number, \
	   							FUNC_FILE_LINE_GET_ARGS)

#define jumps_fill_gaps(asmstruct) \
	   _jumps_fill_gaps(asmstruct, FUNC_FILE_LINE_GET_ARGS)

#define asmstruct_free_memory(asmstruct) \
	   _asmstruct_free_memory(asmstruct, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define open_code_file(filename, mode) \
	    open_code_file_(filename, mode, FUNC_FILE_LINE_GET_ARGS)

#define close_code_file(fp) \
		close_code_file_(fp, FUNC_FILE_LINE_GET_ARGS)

#define write_code(asmstruct, code_file) \
	   _write_code(asmstruct, code_file, FUNC_FILE_LINE_GET_ARGS)

#define header_make(asmstruct) \
		header_make_(asmstruct, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define get_hash(base, len) \
        get_hash_(base, len, FUNC_FILE_LINE_GET_ARGS)

//========================================================================

int clear_opernamebuf_(FUNC_FILE_LINE_PARAMS);

int64_t get_hash_(char* base, unsigned int len, FUNC_FILE_LINE_PARAMS);

//========================================================================

int text_remove_comments_(struct Text* text, FUNC_FILE_LINE_PARAMS);

int convert_operations_to_binary_(struct Text* text, AsmStruct* asmstruct,  
												   FUNC_FILE_LINE_PARAMS);

int _save_string_start_ip(AsmStruct* asmstruct, int code_ip, int number,
												 FUNC_FILE_LINE_PARAMS);

int process_string_(const char* string, AsmStruct* asmstruct, 
					  int str_number, FUNC_FILE_LINE_PARAMS);

int read_label_(const char* string, AsmStruct* asmstruct,
								  FUNC_FILE_LINE_PARAMS);

int _add_label(AsmStruct* asmstruct, const char* label_name, int length, 
												 FUNC_FILE_LINE_PARAMS);

int _add_string_jump(int code_ip, int dest_string, AsmStruct* asmstruct, 
												 FUNC_FILE_LINE_PARAMS);

int _jumps_fill_gaps(AsmStruct* asmstruct, FUNC_FILE_LINE_PARAMS);

int _asmstruct_free_memory(AsmStruct* asmstruct, FUNC_FILE_LINE_PARAMS);

//========================================================================

FILE* open_code_file_(const char* filename, const char* mode,
									  FUNC_FILE_LINE_PARAMS);

int close_code_file_(FILE* fp, FUNC_FILE_LINE_PARAMS);

int _write_code(AsmStruct* asmstruct, FILE* code_file, FUNC_FILE_LINE_PARAMS);

//========================================================================

int header_make_(AsmStruct* asmstruct, FUNC_FILE_LINE_PARAMS);

//========================================================================

int label_struct_compare(const void* first, const void* second);

int _search_label_name(const char* string, AsmStruct* asmstruct, 
										 FUNC_FILE_LINE_PARAMS);

int _add_jump_without_known_label(const char* string, int ip, 
                AsmStruct* asmstruct, FUNC_FILE_LINE_PARAMS);

void print_labels(AsmStruct* asmstruct);