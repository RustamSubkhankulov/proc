#pragma once

#include "processor_general.h"
#include "text_processing.h"
#include "../stack/errors_and_logs.h"
#include "../stack/errors.h"

//===================================================================

struct Str_Jump
{

	int string_number;
	int code_ip;
};

//===================================================================

struct Label
{

	int64_t label_hash;
	int code_pos;
};

//===================================================================

struct AsmStruct
{

	unsigned char *code;
	unsigned char *ip;

	struct Header header;

	struct Label *labels;
	int labels_number;
	int labels_capacity;

	//Unknown destination jumps
	struct Label *ud_jumps;
	int ud_jumps_number;
	int ud_jumps_capacity;

	//Jumps by the string number
	struct Str_Jump *str_jumps;
	int str_jumps_number;
	int str_jumps_capacity;

	//Instruction pointer values for the start of each string
	int *string_start_ips;
	int strings_number;
};

//==========================================================================

#define scan_label													\
	do																\
	{																\
		int byte_start = 0;											\
		byte_end = 0;												\
																	\
		char sscanf_ret = sscanf(string, " %n%s%n", &byte_start, 	\
												label_name_buf, 	\
												&byte_end);			\
		label_name_length = byte_end - byte_start;					\
																	\
		if (sscanf_ret < 1)											\
		set_and_process_err(INV_INSTRUCTION_STR);					\
																	\
		char ch = 0;												\
		int byte_offset = 0;										\
																	\
		sscanf_ret = sscanf(string + byte_end, " %c%n", 			\
									&ch, &byte_offset);				\
		if (sscanf_ret == 1)										\
		byte_end += byte_offset;									\
																	\
		if ((label_name_buf[label_name_length - 1] != ':' 			\
			&& sscanf_ret != 1)										\
		 	|| (sscanf_ret == 1 && ch != ':'))						\
																	\
			return 1;												\
																	\
		if (label_name_length  > LABEL_NAME_SIZE)					\
			set_and_process_err(LABEL_NAME_TOO_THICK);				\
																	\
		if (label_name_buf[label_name_length - 1] == ':') {			\
																	\
			label_name_buf[label_name_length - 1] = '\0';			\
			label_name_length -= 1;									\
		}															\
																	\
	} while(0);

//==========================================================================

#define find_ariphmetic_sign                                        \
	do                                                              \
	{                                                               \
		int byte_offset = 0;                                        \
		char sign = 0;                                              \
                                                                    \
		int scan_sign =                                             \
			sscanf(string + byte_ct, " %c%n", &sign, &byte_offset); \
                                                                    \
		if (scan_sign == -1)                                        \
			set_and_process_err(INV_INSTRUCTION_STR);               \
																	\
		if (sign == '+')											\
			sign_multiplier = 1;									\
																	\
		else if (sign == '-')										\
			sign_multiplier = -1;									\
																	\
		else 														\
			set_and_process_err(INV_INSTRUCTION_STR);				\
																	\
		byte_ct += byte_offset;										\
	} while (0);

//==========================================================================

#define read_imm_label                                                 \
	do                                                                 \
	{                                                                  \
		int byte_offset = 0;                                           \
		int dest_str = 0;                                              \
                                                                       \
		sscanf_ret = sscanf(string + byte_ct,                          \
							" %d%n", &dest_str, &byte_offset);         \
                                                                       \
		if (sscanf_ret != 1)                                           \
			set_and_process_err(INV_INSTRUCTION_STR);                  \
                                                                       \
		if (dest_str < 1 || dest_str > asmstruct->strings_number)      \
			set_and_process_err(INV_DESTINATION_OF_JMP);               \
                                                                       \
		if (str_number >= dest_str)                                    \
		{                                                              \
                                                                       \
			int dest_ip = get_string_start_ip(dest_str, asmstruct);    \
                                                                       \
			if (dest_ip == -1)                                         \
				return -1;                                             \
                                                                       \
			*(int *)asmstruct->ip = dest_ip;                           \
		}                                                              \
                                                                       \
		else                                                           \
		{                                                              \
                                                                       \
			int ret = add_string_jump(asmstruct->ip - asmstruct->code, \
									  dest_str, asmstruct);            \
			if (ret == -1)                                             \
				return -1;                                             \
		}                                                              \
                                                                       \
		asmstruct->ip += sizeof(int);                                  \
		asmstruct_check(asmstruct);                                    \
                                                                       \
		byte_ct += byte_offset;                                        \
                                                                       \
	} while (0);

//==========================================================================

#define read_label_name                                                             \
	do                                                                              \
	{                                                                               \
		char label_name_buf[LABEL_NAME_SIZE] = {0};                                 \
		int byte_start = 0;                                                         \
		int byte_end = 0;                                                           \
                                                                                    \
		sscanf_ret = sscanf(ptr + 1, " %n%s%n", &byte_start,                        \
							label_name_buf, &byte_end);                             \
																					\
		if (sscanf_ret == 0)                                                        \
			set_and_process_err(INV_INSTRUCTION_STR);                               \
                                                                                    \
		if (byte_end - byte_start > LABEL_NAME_SIZE)                                \
			set_and_process_err(LABEL_NAME_TOO_THICK);                              \
                                                                                    \
		int64_t label_hash =                                                        \
			get_hash(label_name_buf, byte_end - byte_start);                        \
                                                                                    \
		int code_pos = search_label_name(label_hash, asmstruct);                    \
                                                                                    \
		if (code_pos == -1)                                                         \
			return -1;                                                              \
                                                                                    \
		if (code_pos == -2) {                                                       \
                                                                                    \
			int ret = add_ud_label(label_name_buf, asmstruct->ip 					\
			                                     - asmstruct->code, 				\
								                   asmstruct); 						\
			if (ret == -1)                                                          \
				return -1;                                                          \
		}                                                                           \
                                                                                    \
		else                                                                        \
			*(int *)asmstruct->ip = code_pos;                                       \
                                                                                    \
		asmstruct->ip += sizeof(int);                                               \
		asmstruct_check(asmstruct);                                                 \
                                                                                    \
		byte_ct += ptr + 1 - (string + byte_ct - 1) + byte_end;                     \
                                                                                    \
	} while (0);

//==========================================================================

#define read_register_argument                                      \
	do                                                              \
	{                                                               \
		oper_code = oper_code | REGISTER_MASK;                      \
                                                                    \
		int byte_offset = 0;                                        \
		char register_number = 0;                                   \
                                                                    \
		sscanf_ret = sscanf(string + byte_ct, " %cx %n",            \
							&register_number, &byte_offset);        \
                                                                    \
		if (sscanf_ret != 1)                                        \
			set_and_process_err(INV_INSTRUCTION_STR);               \
                                                                    \
		byte_ct += byte_offset;                                     \
		register_number -= ASCII_OFFSET;                            \
                                                                    \
		if (register_number < 1 || register_number > REGISTER_SIZE) \
			set_and_process_err(INV_INSTRUCTION_STR);               \
                                                                    \
		*asmstruct->ip = register_number;                           \
		asmstruct->ip++;                                            \
                                                                    \
		asmstruct_check(asmstruct);                                 \
                                                                    \
	} while (0);

//===========================================================================

#define read_int_argument         	                               \
	do                                                             \
	{                                                              \
		oper_code = oper_code | IMM_MASK;                          \
                                                                   \
		int byte_offset = 0;                                       \
                                                                   \
		sscanf_ret = sscanf(string + byte_ct, " %d %n",            \
							&value, &byte_offset);                 \
		if (sscanf_ret != 1)                                       \
			set_and_process_err(INV_INSTRUCTION_STR);              \
                                                                   \
		byte_ct += byte_offset;                                    \
        value *= sign_multiplier;								   \
		                                                           \
		*(int *)asmstruct->ip = value;                             \
		asmstruct->ip += sizeof(int);                              \
                                                                   \
		asmstruct_check(asmstruct);                                \
                                                                   \
	} while (0);

//===========================================================================

#define read_elem_t_argument                                       \
	do                                                             \
	{                                                              \
		oper_code = oper_code | IMM_MASK;                          \
                                                                   \
		int byte_offset = 0;                                       \
                                                                   \
		sscanf_ret = sscanf(string + byte_ct, " " ELEM_SPEC " %n", \
							&value, &byte_offset);                 \
		if (sscanf_ret != 1)                                       \
			set_and_process_err(INV_INSTRUCTION_STR);              \
                                                                   \
		byte_ct += byte_offset;                                    \
        value *= sign_multiplier;								   \
		                                                           \
		*(elem_t *)asmstruct->ip = value;                          \
		asmstruct->ip += sizeof(elem_t);                           \
                                                                   \
		asmstruct_check(asmstruct);                                \
                                                                   \
	} while (0);

//===========================================================================

#define read_var_arguments                                                \
	do                                                                    \
	{                                                                     \
		char register_number = 0;                                         \
		int scan_reg = sscanf(string + byte_ct, " %c", &register_number); \
                                                                          \
		if (scan_reg == 1)                                                \
			read_register_argument                                        \
                                                                          \
		else if (scan_reg == -1)                                  		  \
			set_and_process_err(INV_INSTRUCTION_STR);                     \
																		  \
		int sign_multiplier = 0;										  \
		find_ariphmetic_sign;											  \
		                                                                  \
		int value = 0;                                                    \
		int scan_int = sscanf(string + byte_ct, "%d", &value);            \
																		  \
		if (scan_int == 1)                                                \
			read_int_argument                                             \
                                                                          \
		else if (scan_int == -1)                                          \
			set_and_process_err(INV_INSTRUCTION_STR);                     \
                                                                          \
		if (scan_int == 0 && scan_reg == 0)                               \
			set_and_process_err(INV_INSTRUCTION_STR);                     \
																		  \
	} while (0);

	//===========================================================================

#define read_argument                                                  \
	do                                                                 \
	{                                                                  \
		if ((oper_code & ~OPER_CODE_MASK) == RAM_MASK) {               \
																	   \
			read_var_arguments;                                        \
			break;                                                     \
		}                                                              \
																	   \
		int sign_multiplier = 1;									   \
		elem_t value = 0;                                              \
																	   \
		int sscanf_ret = sscanf(string + byte_ct, ELEM_SPEC, &value);  \
                                                                       \
		if (sscanf_ret == 1) {                                         \
																	   \
			read_elem_t_argument 									   \
			break;                                                     \
		}                                                              \
																	   \
		char register_number = 0;                                      \
		sscanf_ret = sscanf(string + byte_ct, "%c", &register_number); \
                                                                       \
		if (sscanf_ret == 1) {                                         \
																	   \
			read_register_argument 									   \
			break;                                                     \
		}                                                              \
                                                                       \
		if (sscanf_ret == -1 || sscanf_ret == 0)                       \
			set_and_process_err(INV_ARGUMENT_NUMBER);                  \
                                                                       \
	} while (0);

//===========================================================================

#define check_is_ram_using_command												 \
	do                                    										 \
	{																			 \
		int byte_offset = 0;													 \
		char symbol = 0;														 \
																				 \
		int scan_bracket = 														 \
		         sscanf(string + byte_ct, " %c%n", &symbol, &byte_offset);       \
		                                         						         \
		if (scan_bracket == -1 || scan_bracket == 0)					   		 \
			set_and_process_err(INV_INSTRUCTION_STR);						     \
																				 \
		if (symbol == '[') {													 \
																				 \
			oper_code = oper_code | RAM_MASK;									 \
			byte_ct += byte_offset;												 \
		}																         \
	} while (0);

//===========================================================================

#define check_second_square_bracket                                              \
	do                                                                           \
	{                                                                            \
		if ((oper_code & ~OPER_CODE_MASK) >= RAM_MASK)                           \
		{                                                                        \
                                                                                 \
			if (*(string + byte_ct++) != ']')                                    \
				set_and_process_err(INV_INSTRUCTION_STR);                        \
		}                                                                        \
	} while (0);

//==========================================================================

#define asmstruct_check(asmstruct)                               \
	{                                                            \
                                                                 \
		if (asmstruct == NULL)                                   \
			set_and_process_err(ASMSTRUCT_INV_PTR);              \
                                                                 \
		if (asmstruct->code == NULL)                             \
			set_and_process_err(ASMSTRUCT_EMPTY_CODE_ARR);       \
                                                                 \
		if (asmstruct->ip - asmstruct->code > ASM_MAX_CODE_SIZE) \
			set_and_process_err(ASMSTRUCT_INV_IP);               \
                                                                 \
		header_ptr_check(&asmstruct->header);                    \
	}

//===================================================================

#define asmstruct_ip_check(asmstruct)                            \
	{                                                            \
                                                                 \
		if (asmstruct->ip - asmstruct->code > ASM_MAX_CODE_SIZE) \
			set_and_process_err(ASMSTRUCT_INV_IP)                \
	}

//===================================================================

#define asmstruct_ptr_check(asmstruct)              \
	{                                               \
                                                    \
		if (asmstruct == NULL)                      \
			set_and_process_err(INV_ASMSTRUCT_PTR); \
	}

//===================================================================

#define file_ptr_check(fp)                     \
	{                                          \
                                               \
		if (fp == NULL)                        \
			set_and_process_err(INV_FILE_PTR); \
	}

//===================================================================

#define header_ptr_check(header)              \
	{                                         \
                                              \
		if (header == NULL)                   \
			set_and_process_err(INV_HDR_PTR); \
	}

//===================================================================

#define text_ptr_check(text)                   \
	{                                          \
                                               \
		if (text == NULL)                      \
			set_and_process_err(INV_TEXT_PTR); \
	}

//===================================================================

#define char_ptr_check(string)                \
	{                                         \
                                              \
		if (string == NULL)                   \
			set_and_process_err(NULL_STRING); \
	}

//===================================================================

#define check_if_string_is_empty(string)              \
	{                                                 \
                                                      \
		char ch = 0;                                  \
		char ret = sscanf((string), " %c", &ch);      \
                                                      \
		if (ret > 0)                                  \
			set_and_process_err(INV_INSTRUCTION_STR); \
	}

//===================================================================

#define check_unigue() \
	check_unique_(LOG_ARGS)

//===================================================================

#define open_listing_file() \
	open_listing_file_(LOG_ARGS)

#define close_listing_file() \
	close_listing_file_(LOG_ARGS)

#define clear_opernamebuf() \
	clear_opernamebuf_(LOG_ARGS)

#define text_remove_comments(text) \
	text_remove_comments_(text, LOG_ARGS)

//===================================================================

#define asmstruct_allocate_memory(number, asmstruct) \
	_asmstruct_allocate_memory(number, asmstruct,    \
							   LOG_ARGS)

#define convert_operations_to_binary(text, asmstruct) \
	convert_operations_to_binary_(text, asmstruct,    \
								  LOG_ARGS)

#define read_label(string, asmstruct) \
	read_label_(string, asmstruct, LOG_ARGS)

#define add_label(asmstruct, label_hash) \
	_add_label(asmstruct, label_hash,    \
			   LOG_ARGS)

#define search_label_name(hash, asmstruct) \
	_search_label_name(hash, asmstruct,    \
					   LOG_ARGS)

#define add_ud_label(string, ip, asmstruct) \
	_add_ud_label(string, ip, asmstruct,    \
				  LOG_ARGS)

#define add_string_jump(code_ip, dest_string, asmstruct) \
	_add_string_jump(code_ip, dest_string, asmstruct,    \
					 LOG_ARGS)

#define process_string(string, asmstruct, str_number) \
	process_string_(string, asmstruct, str_number,    \
					LOG_ARGS)

#define save_string_start_ip(asmstruct, number) \
	_save_string_start_ip(asmstruct, number,    \
						  LOG_ARGS)

#define jumps_fill_gaps(asmstruct) \
	_jumps_fill_gaps(asmstruct, LOG_ARGS)

#define get_string_start_ip(str_number, asmstruct) \
	_get_string_start_ip(str_number, asmstruct,    \
						 LOG_ARGS)

#define asmstruct_free_memory(asmstruct) \
	_asmstruct_free_memory(asmstruct, LOG_ARGS)

//===================================================================

#define open_code_file(filename, mode) \
	open_code_file_(filename, mode, LOG_ARGS)

#define close_code_file(fp) \
	close_code_file_(fp, LOG_ARGS)

#define write_code(asmstruct, code_file) \
	_write_code(asmstruct, code_file, LOG_ARGS)

#define header_make(asmstruct) \
	header_make_(asmstruct, LOG_ARGS)

//===================================================================

#define get_hash(base, len) \
	get_hash_(base, len, LOG_ARGS)

//========================================================================

int clear_opernamebuf_(LOG_PARAMS);

int64_t get_hash_(char *base, unsigned int len, LOG_PARAMS);

//========================================================================

int text_remove_comments_(struct Text *text, LOG_PARAMS);

int convert_operations_to_binary_(struct Text *text, AsmStruct *asmstruct,
								  LOG_PARAMS);

int _save_string_start_ip(AsmStruct *asmstruct, int number,
						  LOG_PARAMS);

int process_string_(const char *string, AsmStruct *asmstruct,
					int str_number, LOG_PARAMS);

int read_label_(const char *string, AsmStruct *asmstruct,
				LOG_PARAMS);

int _asmstruct_allocate_memory(long number, AsmStruct *asmstruct,
							   LOG_PARAMS);

int _asmstruct_free_memory(AsmStruct *asmstruct, LOG_PARAMS);

//========================================================================

FILE *open_code_file_(const char *filename, const char *mode,
					  LOG_PARAMS);

int close_code_file_(FILE *fp, LOG_PARAMS);

int _write_code(AsmStruct *asmstruct, FILE *code_file, LOG_PARAMS);

//========================================================================

int header_make_(AsmStruct *asmstruct, LOG_PARAMS);

//========================================================================

int _search_label_name(int64_t label_hash, AsmStruct *asmstruct,
					   LOG_PARAMS);

int _add_ud_label(const char *string, int ip, AsmStruct *asmstruct,
				  LOG_PARAMS);

int _add_label(AsmStruct *asmstruct, int64_t label_hash,
			   LOG_PARAMS);

int _add_string_jump(int code_ip, int dest_string, AsmStruct *asmstruct,
					 LOG_PARAMS);

//========================================================================

int _jumps_fill_gaps(AsmStruct *asmstruct, LOG_PARAMS);

int _get_string_start_ip(int str_number, AsmStruct *asmstruct,
						 LOG_PARAMS);

//=======================================================================

void print_labels(AsmStruct *asmstruct);

int open_listing_file_(LOG_PARAMS);

int close_listing_file_(LOG_PARAMS);

int write_listing(const char* string, int length, 
				  AsmStruct* asmstruct, int str_number);

int fix_up_listing(int jump_pos, int jump_dest);

void check_unique_(LOG_PARAMS);

//=======================================================================