#include <string.h>
#include <stdlib.h>

#include "disasm.h"

//===================================================================


int _init_disasmstruct(disasmstruct* disasmstruct, FILE* fp,
                                      LOG_PARAMS) {

    disasm_log_report();

    file_ptr_check(fp);

    disasmstruct_ptr_check(disasmstruct);

    long code_file_size = 0;

    const unsigned char* code_array = read_from_file(fp, &code_file_size);
    char_ptr_check(code_array);
    
    disasmstruct->code_array = code_array;
    disasmstruct->code_file_size = code_file_size;

    int header_check_ret = disasm_header_check(disasmstruct);
    if (header_check_ret == -1)
        return -1;

    disasmstruct->ip = disasmstruct->code_array + sizeof(struct Header);

    return 0;
}

//===================================================================

int _disasm_header_check(disasmstruct* disasmstruct, LOG_PARAMS) {

    disasm_log_report();

    disasmstruct_ptr_check(disasmstruct);

    memcpy(&disasmstruct->header, 
            disasmstruct->code_array, 
            sizeof(struct Header));

    if (disasmstruct->header.signature != SIGN)
        set_and_process_err(HDR_INV_SIGN);

    if (disasmstruct->header.version != VERSION)
        set_and_process_err(HDR_INV_VERSION);

    if (disasmstruct->header.file_size != disasmstruct->code_file_size)
        set_and_process_err(HDR_INV_FILE_SIZE);;

    return 0;
}

//===================================================================

int _disassemble_code(disasmstruct* disasmstruct, FILE* disasm_output,
                                                           LOG_PARAMS) {

    disasm_log_report();
    disasmstruct_ptr_check(disasmstruct);
    file_ptr_check(disasm_output);

    while(disasmstruct->ip - disasmstruct->code_array
                           < disasmstruct->code_file_size) {

            int ret_val = disassemble_command(disasmstruct, 
                                             disasm_output);
            if (ret_val == -1)
                return -1;
        }

    return 0;
}

//===================================================================

#define DEF_JMP_(num, name, code, hash, instructions)               \
                                                                    \
case code: {                                                        \
                                                                    \
    int err = fprintf(disasm_output, "%s ", #name);                 \
    if (err < 0)                                                    \
        set_and_process_err(FWRITE_ERR);                            \
                                                                    \
    disasmstruct->ip++;                                             \
    disasmstruct_ip_check(disasmstruct);                            \
                                                                    \
    fprintf_int_argument;                                           \
    fprintf(disasm_output, "\n");                                   \
                                                                    \
    break;                                                          \
}

//===================================================================

#define DEF_CMD_(num, name, code, hash, instructions)               \
                                                                    \
case code: {                                                        \
                                                                    \
    int err = fprintf(disasm_output, "%s ", #name);                 \
    if (err < 0)                                                    \
        set_and_process_err(FWRITE_ERR);                            \
                                                                    \
    disasmstruct->ip++;                                             \
    disasmstruct_ptr_check(disasmstruct);                           \
                                                                    \
    if (num != 0) {                                                 \
                                                                    \
        char is_ram_using = 0;                                      \
                                                                    \
        if ((oper_code & ~RAM_MASK) == RAM_MASK) {                  \
                                                                    \
            fprintf(disasm_output, "[");                            \
            is_ram_using = 1;                                       \
        }                                                           \
                                                                    \
        for (int ct = 0; ct < num; ct++) {                          \
                                                                    \
            switch(oper_code & ~OPER_CODE_MASK ) {                  \
                                                                    \
                case IMM_MASK:                                      \
                    fprintf_elem_t_argument;                        \
                    break;                                          \
                                                                    \
                case REGISTER_MASK :                                \
                    fprintf_register_argument;                      \
                    break;                                          \
                                                                    \
                case REGISTER_MASK | IMM_MASK :                     \
                    fprintf_var_arguments;                          \
                    break;                                          \
            }                                                       \
        }                                                           \
                                                                    \
        if (is_ram_using)                                           \
            fprintf(disasm_output, "]");                            \
    }                                                               \
                                                                    \
    fprintf(disasm_output, "\n");                                   \
    break;                                                          \
}                                                                

//===================================================================

int _disassemble_command(disasmstruct* disasmstruct, FILE* disasm_output, 
                                                              LOG_PARAMS) {

    disasm_log_report();

    disasmstruct_ptr_check(disasmstruct);
    disasmstruct_ip_check(disasmstruct);

    char oper_code = *(disasmstruct->ip);

    switch(oper_code & OPER_CODE_MASK) {

        #include "../text_files/commands.txt"
        #include "../text_files/jumps.txt"

        default: 
            set_and_process_err(DISASM_INV_OPER_CODE);
    }

    #undef DEF_CMD_
    #undef DEF_JMP_

    return 0;
}

//===================================================================

int _dtor_disasmstruct(disasmstruct* disasmstruct, LOG_PARAMS) {

    disasm_log_report();

    disasmstruct_ptr_check(disasmstruct);
    disasmstruct_ip_check(disasmstruct);

    if (disasmstruct->code_array == NULL)
        set_and_process_err(INV_CODE_ARRAY_PTR);

    free((void*)disasmstruct->code_array);

    return 0;
}

//===================================================================

int _open_disasm_listing_file(LOG_PARAMS) {

    disasm_log_report();
    extern FILE* disasm_listing_file;

    FILE* fp = fopen(DISASM_LISTING_FILENAME, "wb");

    if (fp == NULL)
        set_and_process_err(FOPEN_ERROR);

    disasm_listing_file = fp;

    return 0;
}

//===================================================================

int _close_disasm_listing_file(LOG_PARAMS) {

    disasm_log_report();
    extern FILE* disasm_listing_file;

    int fclose_ret = fclose(disasm_listing_file);

    if (fclose_ret == EOF)
        set_and_process_err(FCLOSE_ERR);

    return 0;
}

//===================================================================

FILE* _open_file(const char* filename, const char* mode, int* err,  
                                                       LOG_PARAMS) {

    disasm_log_report();

    if (filename == NULL) {

        set_global_error_code(INV_FILE_NAME);
        global_error_process();

        *err = -1;
        return NULL;
    }

    if (mode == NULL) {

        set_global_error_code(FOPEN_ERROR);
        global_error_process();

        *err = -1;
        return NULL;
    }

    FILE* fp = fopen(filename, mode);
    if (fp == NULL) {

        set_global_error_code(FOPEN_ERROR);
        global_error_process();

        *err = -1;
        return NULL;
    }

    return fp;
}

//===================================================================

int _close_file(FILE* fp, LOG_PARAMS) {

    if (fp != stdin) {

        int fclose_ret = fclose(fp);
        if (fclose_ret == EOF) 
            set_and_process_err(FCLOSE_ERR);
    }

    return 0;
}

//===================================================================