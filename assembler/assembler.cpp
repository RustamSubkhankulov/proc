
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "general.h"
#include "assembler.h"
#include "../stack/stackconfig.h"

//===================================================================

static char operation_name_buf[OPERATION_NAME_BUF_SIZE] = { 0 };

//===================================================================

int text_remove_comments_(struct Text* text, LOG_PARAMS) {

	asm_log_report();

	if (text == NULL)
		set_and_process_err(INV_TEXT_PTR);

	for (int str_count = 0; str_count < text->strings_number; str_count++) {

		char* ptr = strchr(text->strings[str_count].data, ';');

		if (ptr != NULL) {
		  
		   *ptr = '\0';

		   text->strings[str_count].length = 
		   						    strlen(text->strings[str_count].data);
		}	
	}
	
	return 0;
}

//===========================================================================

#undef DEF_CMD_

#define DEF_CMD_(num, name, op_code, hash)									\
																			\
	if (oper_hash == hash) {				    							\
		                                                                    \
		printf("\n\n %s \n\n", #name);										\
																			\
		int code_ip_position = asmstruct->ip - asmstruct->code;  			\
		asmstruct->ip++;													\
		asmstruct_ip_check(asmstruct);										\
																			\
		oper_code = op_code;												\
																			\
		if (num != 0) {														\
																			\
			check_is_ram_using_command;										\
																			\
			for (int ct = 0; ct < num; ct++) {								\
				read_argument;												\
			}																\
																			\
			check_second_square_bracket;									\
		}	                                                                \
																			\
		*(asmstruct->code + code_ip_position) = oper_code;					\
	}																		\
	else

//===========================================================================

#define DEF_JMP_(num, name, op_code, hash)		        					\
																			\
	if (oper_hash == hash) {												\
																			\
		*asmstruct->ip = op_code;											\
																			\
		asmstruct->ip++;													\
		asmstruct_check(asmstruct);											\
																			\
		char ch = 0;														\
		int byte_offset = 0;												\
																			\
		const char* ptr = strchr(string + byte_end - 1, ':');				\
																			\
		if (ptr != NULL)													\
			read_label_name													\
		else																\
			read_imm_label													\
	}																		\
	else 

//===========================================================================

int process_string_(const char* string, AsmStruct* asmstruct, int str_number, 
													   LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);
	char_ptr_check(string);
	
	save_string_start_ip(asmstruct, str_number);

	extern char operation_name_buf[OPERATION_NAME_BUF_SIZE];

	int byte_ct    = 0;
	int byte_start = 0;
	int byte_end   = 0;

	printf("\n\n string:%s\n\n", string);

	int sscanf_ret = 
		sscanf(string, " %n%s%n %n", &byte_start, operation_name_buf, &byte_end, &byte_ct);

	int length = byte_end - byte_start;

	operation_name_buf[OPERATION_NAME_BUF_SIZE - 1] = '\0';

	if (operation_name_buf[byte_end - byte_start - 1] == ':') {

		operation_name_buf[byte_end - byte_start - 1] = '\0';
		length -= 1;
	}
	
	if (sscanf_ret == 0)
		set_and_process_err(NO_COMMAND_IN_STRING);

	int64_t oper_hash = get_hash(operation_name_buf, length);
					
	printf("\n\n hash is %ld  name is %s %d \n \n ", oper_hash, operation_name_buf, length);

							// char dest[10] = { 0 };
							// strncpy(dest, "RET", OPERATION_NAME_BUF_SIZE);
							// int64_t h_hash = get_hash(dest, OPERATION_NAME_BUF_SIZE);
							//  printf("\n\n add hash %lu", h_hash)

	unsigned char oper_code = 0;
							
	#include "../text_files/commands.txt"
	#include "../text_files/jumps.txt"
	//else
		set_and_process_err(INV_INSTRUCTION_STR);

	#undef DEF_CMD_
	#undef DEF_JMP_

	clear_opernamebuf();

	check_if_string_is_empty(string + byte_ct);
	write_listing(string, length, asmstruct, str_number);

	return 0;
}

//==================================================================


int _jumps_fill_gaps(AsmStruct* asmstruct, LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);

	for (int i = 0; i < asmstruct->ud_jumps_number; i++) {

		int jump_pos = asmstruct->ud_jumps[i].code_pos;

		int dest_pos = search_label_name(asmstruct->ud_jumps[i].label_hash,
										                        asmstruct);

		if (dest_pos == -1)
			return -1;

		if (dest_pos == -2)
			set_and_process_err(NO_LABEL_FOR_JUMP);

		*(int*)(asmstruct->code + jump_pos) = dest_pos;

		fix_up_listing(jump_pos, dest_pos);
	}

	for (int i = 0; i < asmstruct->str_jumps_number; i++) {

		if (asmstruct->str_jumps[i].string_number < 1
		||	asmstruct->str_jumps[i].string_number > asmstruct->strings_number)

		set_and_process_err(INV_STRING_NUMBER);

		int dest_pos = get_string_start_ip(asmstruct->str_jumps[i].string_number, 
										 							 asmstruct);
		if (dest_pos == -1)
			return -1;

		int jump_pos = asmstruct->str_jumps[i].code_ip;

		*(int*)(asmstruct->code + jump_pos) = dest_pos;

		fix_up_listing(jump_pos, dest_pos);
	}
	
	return 0;
}

//==================================================================

int _get_string_start_ip(int str_number, AsmStruct* asmstruct, 
										LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);

	if (str_number == 1)
		return sizeof(struct Header);
	
	if (str_number < 1 || str_number > asmstruct->strings_number)
		set_and_process_err(INV_STRING_NUMBER);

	int code_pos = asmstruct->string_start_ips[str_number - 1];

	if (code_pos == 0)
		code_pos = get_string_start_ip(str_number + 1, asmstruct);

	return code_pos;
}

//==================================================================

int _add_ud_label(const char* string, int ip, AsmStruct* asmstruct, 
                                             LOG_PARAMS) {

	asm_log_report();

	char_ptr_check(string);

	if (asmstruct->ud_jumps_capacity == 0) {

		asmstruct->ud_jumps = (struct Label*)calloc(UD_JUMPS_NUMBER,
													sizeof(struct Label));
		asmstruct->ud_jumps_capacity = UD_JUMPS_NUMBER;											
	}

	else if (asmstruct->ud_jumps_number >= asmstruct->ud_jumps_capacity) {

		asmstruct->ud_jumps = (struct Label*)my_recalloc(asmstruct->ud_jumps,
														 asmstruct->ud_jumps_capacity * 2,
														 asmstruct->ud_jumps_capacity,
														 sizeof(struct Label));
		asmstruct->ud_jumps_capacity *= 2;
	}

	if (asmstruct->ud_jumps == NULL)
		return -1;

	int number = asmstruct->ud_jumps_number;

	int length = strlen(string);
	int64_t ud_label_hash = get_hash((char*)string, length);

	asmstruct->ud_jumps[number].label_hash = ud_label_hash;
	asmstruct->ud_jumps[number].code_pos = ip;

	printf("\n\nзаписан айдишник %d \n\n", ip);

	asmstruct->ud_jumps_number++;

	printf("\n\n ud jumps number in the end %d \n\n", asmstruct->ud_jumps_number);

	return 0;
}

//==================================================================

int _search_label_name(int64_t label_hash, AsmStruct* asmstruct, 
										  LOG_PARAMS) {

	asm_log_report();
    asmstruct_check(asmstruct);

	if (asmstruct->labels_number == 0)
		return -2;

	printf("\n\n im alive before search\n\n");

	//print_labels(asmstruct);

	int cntr = 0;
	while (cntr < asmstruct->labels_number) {

		if (label_hash == asmstruct->labels[cntr].label_hash)
		break;

		cntr++;
	}

	if (cntr == asmstruct->labels_number)
		return -2;

	else 
		return asmstruct->labels[cntr].code_pos;
	
}

//==================================================================

int clear_opernamebuf_(LOG_PARAMS) {

	asm_log_report();

	extern char operation_name_buf[OPERATION_NAME_BUF_SIZE];

	for (int ct = 0; ct < OPERATION_NAME_BUF_SIZE; ct++)
		operation_name_buf[ct] = 0;

	return 0;
}

//===================================================================

#define DEF_JMP_(num, name, code, hash)		         				\
																	\
	case hash: {													\
																	\
		return 1;													\
	}

//===================================================================

int read_label_(const char* string, AsmStruct* asmstruct, LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);
	char_ptr_check(string);

	char label_name_buf[LABEL_NAME_SIZE] = { 0 };
	
	int label_name_length = 0;
	int byte_end = 0;

	scan_label;

	int64_t label_hash = get_hash(label_name_buf, label_name_length);
	switch (label_hash) {

		#include "../text_files/jumps.txt"
		defaul: break;
	}
	#undef DEF_JMP_

	int do_label_repeat = search_label_name(label_hash, asmstruct);
	
	if (do_label_repeat == -1) return -1;
	
	if (do_label_repeat != -2) 
		set_and_process_err(REPEATING_LABELS)
	
	else {
		
		int ret = add_label(asmstruct, label_hash);
		if (ret == -1)
			set_and_process_err(CANNOT_ADD_NEW_LABEL);
	}
	
	check_if_string_is_empty(string + byte_end);

	return 0;
}

//===================================================================

int _save_string_start_ip(AsmStruct* asmstruct, int number, 
								     LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);

	int ip = asmstruct->ip - asmstruct->code;

	if (ip > ASM_MAX_CODE_SIZE)
		set_and_process_err(INV_INSTR_PTR);

	if (number > asmstruct->strings_number)
		set_and_process_err(INV_STRING_NUMBER);

	asmstruct->string_start_ips[number] = ip;

	return 0;
}

//===================================================================

int _add_string_jump(int code_ip, int dest_string, AsmStruct* asmstruct, 
												 LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);

	if (asmstruct->str_jumps_capacity == 0) {

		asmstruct->str_jumps = (struct Str_Jump*)calloc(STRING_JUMPS_NUMBER, 
												       sizeof(struct Str_Jump));
		asmstruct->str_jumps_capacity - STRING_JUMPS_NUMBER;
	}

	else if (asmstruct->str_jumps_number >= asmstruct->str_jumps_capacity) {

		asmstruct->str_jumps = (struct Str_Jump*)my_recalloc(asmstruct->str_jumps, 
													  		 asmstruct->str_jumps_capacity * 2, 
													   		 asmstruct->str_jumps_capacity, 
													   		 sizeof(struct Str_Jump));

		asmstruct->str_jumps_capacity *= 2;
	}

	if (asmstruct->str_jumps == NULL)
	return -1;

	int number = asmstruct->str_jumps_number;

	asmstruct->str_jumps[number].code_ip = code_ip;
	asmstruct->str_jumps[number].string_number = dest_string;

	asmstruct->str_jumps_number++;

	return 0;
}

//===================================================================

int _add_label(AsmStruct* asmstruct, int64_t label_hash, LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);
	
	if (asmstruct->labels_capacity == 0) {

		asmstruct->labels = (struct Label*)calloc(START_LABELS_NUMBER, 
												  sizeof(struct Label));
		asmstruct->labels_capacity = START_LABELS_NUMBER;
	}

	else if (asmstruct->labels_number  >= asmstruct->labels_capacity) {

		asmstruct->labels = (struct Label*)my_recalloc(asmstruct->labels, 
													   asmstruct->labels_capacity * 2, 
													   asmstruct->labels_capacity, 
													   sizeof(struct Label));

		asmstruct->labels_capacity *= 2;
	}

	if (asmstruct->labels == NULL)
		return -1;

	int label_num = asmstruct->labels_number;

	asmstruct->labels[label_num].code_pos= asmstruct->ip - asmstruct->code;
	asmstruct->labels[label_num].label_hash = label_hash;
	
	asmstruct->labels_number++;

	return 0;
}

//===================================================================

void print_labels(AsmStruct* asmstruct) {

	printf("\n\n im alive in print labels\n\n");

	for (int i = 0; i < asmstruct->labels_number; i++) {

		printf("\n\n %ld %x \n\n", asmstruct->labels[i].label_hash,                                    ///без поииска. добавить в fill gaps
								  asmstruct->labels[i].code_pos);                                     ///обработка str jumpov в дефайне
	}
}

//===================================================================

int convert_operations_to_binary_(struct Text* text, AsmStruct* asmstruct,
													LOG_PARAMS) {

	asm_log_report();

	open_listing_file();

	asmstruct_ptr_check(asmstruct);
	text_ptr_check(text);

	//hack_pentagon();

	if (text->strings_number <= 0)
		set_and_process_err(TEXT_NO_STRINGS)

	else {

		int ret = asmstruct_allocate_memory(text->strings_number, asmstruct);
		if (ret == -1)
			return -1;	

		asmstruct->strings_number = text->strings_number;
	}

	asmstruct->ip = asmstruct->code + HDRSIZE;

	for (int str_count = 0; str_count < text->strings_number; str_count++) {

		if (text->strings[str_count].data == NULL)
			continue;

		int value = read_label(text->strings[str_count].data, asmstruct);

		if (value == -1)
			return -1;
		if (value == 0)
			continue;

		value = process_string(text->strings[str_count].data, 
											       asmstruct, 
												  str_count);
		if (value == -1) 
			return -1;
	}

	print_labels(asmstruct);
	printf("\n\n");

	jumps_fill_gaps(asmstruct);

	asmstruct_free_memory(asmstruct);
	close_listing_file();

	return 0;
}

//===================================================================

int _asmstruct_free_memory(AsmStruct* asmstruct, LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);

	if (asmstruct->code != NULL)
		free(asmstruct->code);

	if (asmstruct->labels != NULL)
		free(asmstruct->labels);

	if (asmstruct->ud_jumps != NULL)
		free(asmstruct->ud_jumps);

	if (asmstruct->str_jumps != NULL)
		free(asmstruct->str_jumps);

	if (asmstruct->string_start_ips != NULL)
		free(asmstruct->string_start_ips);

	return 0;
}

//===================================================================

int _asmstruct_allocate_memory(long number, AsmStruct* asmstruct, 
										   LOG_PARAMS) {

	asm_log_report();

	asmstruct_ptr_check(asmstruct);

	void* ptr = calloc(ASM_MAX_CODE_SIZE, sizeof(char));

	if (ptr == NULL)
		set_and_process_err(CANNOT_ALLOCATE_MEM)
	else
		asmstruct->code = (unsigned char*)ptr;

	ptr = calloc(number, sizeof(int));

	if (ptr == NULL)
		set_and_process_err(CANNOT_ALLOCATE_MEM)
	else
		asmstruct->string_start_ips = (int*)ptr;
	
	return 0;
}

//===================================================================

FILE* open_code_file_(const char* filename, const char* mode, 
								       LOG_PARAMS) {

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

int close_code_file_(FILE* fp, LOG_PARAMS) {

	asm_log_report();

	file_ptr_check(fp);

	int fclose_return_value = fclose(fp);

	if (fclose_return_value == EOF)
		set_and_process_err(FCLOSE_ERROR);

	return 0;
}

//===================================================================

int header_make_(AsmStruct* asmstruct, LOG_PARAMS) {

	asm_log_report();

	asmstruct_check(asmstruct);

	asmstruct->header.signature = SIGN;
	asmstruct->header.version = VERSION;

	int file_size = asmstruct->ip - asmstruct->code;

	asmstruct->header.file_size = file_size;

	*(struct Header*)asmstruct->code = asmstruct->header;

	return 0;
}

//====================================================

int _write_code(AsmStruct* asmstruct, FILE* code_file, 
								LOG_PARAMS) {

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

int open_listing_file_(LOG_PARAMS) {

	asm_log_report();

	extern FILE* asm_listing_file;

	FILE* fp = fopen(ASM_LISTING_FILENAME, "wb");

	if (fp == NULL)
		set_and_process_err(FOPEN_ERROR);

	asm_listing_file = fp;

	return 0;
}

//====================================================

int close_listing_file_(LOG_PARAMS) {

	asm_log_report();

	extern FILE* asm_listing_file;

	int fclose_ret = fclose(asm_listing_file);

	if (fclose_ret == EOF)
		set_and_process_err(FCLOSE_ERROR);

	return 0;
}	

//===================================================================

#define DEF_CMD_(num, name, code, hash) \
	case CMD_##name: ;

void check_unique_(LOG_PARAMS) {

	asm_log_report();

	switch (0) {

		#include "../text_files/commands.txt"
	}
}

#undef DEF_CMD_

//===================================================================

int64_t get_hash_(char* base, unsigned int len, LOG_PARAMS) {

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

//===================================================================

int write_listing(const char* string, int length, 
				  AsmStruct* asmstruct, int str_number) {

	extern FILE* asm_listing_file;
	
	const unsigned char* printer = asmstruct->code + 
								   asmstruct->string_start_ips[str_number];

	int number = asmstruct->ip - printer;

	fprintf(asm_listing_file, "Current position: %x. ", 
							   asmstruct->string_start_ips[str_number]);

	fprintf(asm_listing_file,"Input string : %*s. Written bytes: ", 
												   length, string);

	for (int ct = 0; ct < number; ct++) {

		fprintf(asm_listing_file, "%02x ", *(printer + ct));
	}

	fprintf(asm_listing_file, "\n\n");

	return 0;
}

//=====================================================================

int fix_up_listing(int jump_pos, int jump_dest) {

	extern FILE* listing_file;

	fprintf(asm_listing_file, "Filled jump destination. Ip: %x. "
							  "Destination Ip: %x \n\n",
							   jump_pos, jump_dest);
	return 0;
}
