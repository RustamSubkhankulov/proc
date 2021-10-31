#pragma once

#include <stdio.h>

#include "config.h"

//===================================================================

#define REGISTER_MASK 0x40

#define IMM_MASK 0x20

#define RAM_MASK 0x80

#define OPER_CODE_MASK ~(7 << 5)

#define ASCII_OFFSET 96

//===================================================================

#define SIGN 559108690

#define VERSION 5

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

static FILE* asm_listing_file = NULL;

//===================================================================

static FILE* disasm_listing_file = NULL;
