#pragma once

#include <stdio.h>


//===================================================================

#define REGISTER_MASK 0x40

#define IMM_MASK 0x20

#define RAM_MASK 0x80

#define OPER_CODE_MASK ~(7 << 5)

#define ASCII_OFFSET 96

#define REGISTER_SIZE 8

#define RAM_SIZE 1024 

#define PRECISION 1e-3

//===================================================================

#define SIGN 'SRR!'

#define VERSION 1

//===================================================================

struct Header {

	int signature;
	char version;

	long file_size;
};

#define HDRSIZE sizeof(struct Header)

//===================================================================

#define DEF_CMD_(num, name, code, hash, instructions) \
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

#define START_LABELS_NUMBER 10

#define UD_JUMPS_NUMBER 10

#define STRING_JUMPS_NUMBER 10

#define OPERATION_NAME_BUF_SIZE 10

static FILE* asm_listing_file = NULL;

static const char* ASM_LISTING_FILENAME = "text_files/listing.txt";

//LABELS

#define LABEL_NAME_SIZE 15

//===================================================================

//DISASSEMBLER OPTIONS

#define DISASM_DEBUG

#define DISASM_LOGS

#define DISASM_LISTING


static FILE* disasm_listing_file = NULL;

static const char* DISASM_LISTING_FILENAME = "text_files/disasm_listing.txt";

//===================================================================

//PROCESSOR

#define PROC_LOGS

#define PROC_DEBUG

//===================================================================