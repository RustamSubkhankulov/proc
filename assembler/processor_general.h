#pragma once

#include <stdio.h>

//===================================================================

#define REGISTER_MASK 0x40

#define IMM_MASK 0x20

#define RAM_MASK 0x80

#define OPER_CODE_MASK ~(7 << 5)

#define ASCII_OFFSET 96

#define REGISTER_SIZE 4

#define RAM_SIZE 50

//===================================================================

#define SIGN 'SRR!'

#define VERSION 1

//===================================================================

struct Header {

	int signature;
	char version;

	long file_size;

	int commands_number;
	int elem_t_args_number;
	int register_args_number;
	int ram_using_commands_number;
};

#define HDRSIZE sizeof(struct Header)

//===================================================================

#define DEF_CMD_(num, name, code, hash) \
	CMD_##name = (code),

enum operations {

	#include "../text_files/commands.txt"
};

#undef DEF_CMD_

//===================================================================

//GLOBAL

#define DEBUG

//===================================================================

//ASSEMBLER OPTIONS

#define ASM_DEBUG

#define ASM_LOGS

#define LISTING

#define ASM_MAX_CODE_SIZE 200

#define OPERATION_NAME_BUF_SIZE 10

static FILE* asm_listing_file = NULL;

static const char* ASM_LISTING_FILENAME = "text_files/listing.txt";

//===================================================================

//DISASSEMBLER OPTIONS

#define DISASM_DEBUG

#define DISASM_LOGS

#define DISASM_LISTING


static FILE* disasm_listing_file = NULL;

static const char* DISASM_LISTING_FILENAME = "text_files/disasm_listin.txt";

//===================================================================

//PROCESSOR

#define PROC_LOGS

#define PROC_DEBUG


static FILE* proc_output = stdout;

static FILE* proc_input = stdin;