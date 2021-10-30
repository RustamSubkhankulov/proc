#pragma once

#include "../cpu/processor.h"
#include "../assembler/processor_general.h"

struct Disasmstruct {

    long code_file_size;
    const unsigned char* code_array;
    const unsigned char* ip;

    int commands_ct;
    int elem_t_args_ct;
    int register_args_ct;

    struct Header header;
};

typedef struct Disasmstruct disasmstruct;

//===================================================================

#define disasmstruct_ptr_check(disasmstruct) {                      \
                                                                    \
    if (disasmstruct == NULL)                                       \
                                                                    \
        set_and_process_err(INV_DISASMSTRUCT_PTR);                  \
}

#define disasmstruct_ip_check(disasmstruct) {                       \
                                                                    \
    if (disasmstruct->ip - disasmstruct->code_array                 \
       > disasmstruct->code_file_size)                              \
                                                                    \
        set_and_process_err(INV_INSTR_PTR)                          \
}

//===================================================================

#define init_disasmstruct(disasmstruct, fp) \
       _init_disasmstruct(disasmstruct, fp, LOG_ARGS)

#define disassemble_code(disasmstruct, fp) \
       _disassemble_code(disasmstruct, fp, LOG_ARGS) 

#define disasm_final_check(disasmstruct) \
       _disasm_final_check(disasmstruct, LOG_ARGS) 

#define dtor_disasmstruct(disasmstruct) \
       _dtor_disasmstruct(disasmstruct, LOG_ARGS) 

#define disasm_header_check(disasmstruct) \
       _disasm_header_check(disasmstruct, LOG_ARGS)
       
#define disassemble_command(disasmstruct, fp) \
       _disassemble_command(disasmstruct, fp, LOG_ARGS)   

//===================================================================

#define open_file(filename, mode, err) \
       _open_file(filename, mode, err, LOG_ARGS) 

#define close_file(fp) \
       _close_file(fp ,LOG_ARGS)

#define open_disasm_listing_file() \
       _opem_disasm_listing_file(LOG_ARGS)

#define close_disasm_listing_file() \
       _close_disasm_listing_file(LOG_ARGS) 
//===================================================================

int _init_disasmstruct(disasmstruct* disasmstruct, FILE* fp,
                                      LOG_PARAMS);

int _disasm_header_check(disasmstruct* disasmstruct, LOG_PARAMS);

int _disassemble_command(disasmstruct* disasmstruct, FILE* disasm_output,
                                                   LOG_PARAMS);

int _disassemble_code(disasmstruct* disasmstruct, FILE* disasm_output,
                                                LOG_PARAMS);

int _disasm_final_check(disasmstruct* disasmstruct, LOG_PARAMS);

int _dtor_disasmstruct(disasmstruct* disasmstruct, LOG_PARAMS);

//===================================================================

FILE* _open_file(const char* filename, const char* mode,  int* err,  
                                            LOG_PARAMS);


int _close_file(FILE* fp, LOG_PARAMS);

int _open_disasm_listing_file(LOG_PARAMS);

int _close_disasm_listing_file(LOG_PARAMS);