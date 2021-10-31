#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "processor.h"
#include "../assembler/general.h"
#include "../assembler/config.h"

//===================================================================

static FILE* proc_output = stdout;

static FILE* proc_input = stdin;

//===================================================================

int _open_proc_input(const char* filename, LOG_PARAMS) {

    proc_log_report();

    if (filename == NULL) 
        set_and_process_err(INV_FILE_NAME);

    extern FILE* proc_input;

    FILE* fp = fopen(filename, "rb");

    if (fp == NULL) 
        set_and_process_err(FOPEN_ERROR);

    proc_input = fp;

    return 0;
}

//==================================================================

int _close_proc_input(LOG_PARAMS) {

    proc_log_report();

    extern FILE* proc_input;

    if (proc_input != stdin) {

        int fclose_ret = fclose(proc_input);

        if (fclose_ret == EOF)
            set_and_process_err(FCLOSE_ERR);
    }

    return 0;
}

//===================================================================

int _open_proc_output(const char* filename, LOG_PARAMS) {

    proc_log_report();

    if (filename == NULL)
        set_and_process_err(INV_FILE_NAME);

    extern FILE* proc_output;

    FILE* fp = fopen(filename, "wb");

    if (fp == NULL)
        set_and_process_err(INV_FILE_PTR);

    proc_output = fp;

    return 0;
}

//===================================================================

int _close_proc_output(LOG_PARAMS) {

    proc_log_report();

    extern FILE* proc_output;

    if (proc_output != stdout) {

        int fclose_ret = fclose(proc_output);

        if (fclose_ret == EOF)
            set_and_process_err(FCLOSE_ERR);
    }

    return 0;
}

//===================================================================

int _header_check(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    memcpy(&procstruct->header, 
            procstruct->code_array, sizeof(struct Header));

    if (procstruct->header.signature != SIGN)
        set_and_process_err(HDR_INV_SIGN);

    if (procstruct->header.version != VERSION)
        set_and_process_err(HDR_INV_VERSION);

    if (procstruct->header.file_size != procstruct->code_file_size)
        set_and_process_err(HDR_INV_FILE_SIZE);

    return 0;
}

//===================================================================

int _procstruct_allocate_memory(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();
    
    elem_t* temp_ptr = (elem_t*)calloc(RAM_SIZE, sizeof(elem_t));

    if (temp_ptr == NULL)
        set_and_process_err(CANNOT_ALLOCATE_MEM);

    procstruct->ram = temp_ptr;

    return 0;
}

//===================================================================

int _procstruct_free_memory(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    if (procstruct->ram == NULL)
        set_and_process_err(PROC_RAM_IS_NULL);

    free(procstruct->ram);

    return 0;
}

//===================================================================

int _init_procstruct(procstruct* procstruct, FILE* fp, LOG_PARAMS) {

    proc_log_report();

    file_ptr_check(fp);
    procstruct_ptr_check(procstruct);

    long code_file_size = 0;

    const unsigned char* code_array = read_from_file(fp, &code_file_size);

    char_ptr_check(code_array);

    procstruct->code_array = code_array;
    procstruct->code_file_size = code_file_size;

    int header_check_ret = header_check(procstruct);
    if (header_check_ret == -1)
        return -1;

    procstruct->ip = procstruct->code_array + sizeof(struct Header);

    int ret = stack_ctor(&procstruct->Stack);
    if (ret == -1)
        return -1;

    ret = procstruct_allocate_memory(procstruct);

    if (ret == -1)
        return -1;

    procstruct->video = procstruct->ram + VIDEO_MEMORY_ADDRESS;

    return 0;
}

//===================================================================


int _dtor_procstruct(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);
    procstruct_ip_check(procstruct);

    if (procstruct->code_array == NULL)
        set_and_process_err(INV_CODE_ARRAY_PTR);

    free((void*)procstruct->code_array);

    int ret = procstruct_free_memory(procstruct);

    if (ret == -1)
        return -1;

    return 0;
}
//===================================================================

const unsigned char* _read_from_file(FILE* fp, long* size, LOG_PARAMS) {

    proc_log_report();

    if (fp == NULL) {

        set_global_error_code(INV_FILE_PTR);
        global_error_process();

        return NULL;
    }

    long code_file_size = file_size_(fp);
    if ( code_file_size < 0)
        return NULL;

    if (size) *size = code_file_size;

    const unsigned char* code_array = 
    (const unsigned char*)copy_data_to_buf(code_file_size, fp, LOG_ARGS);

    if (code_array == NULL)
        return NULL;

    return code_array;
}

//===================================================================

int _proc_perform(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    while (1) {

        int ret_val = proc_execute_command(procstruct);

        if (ret_val == -1) {
            
            processor_dump(procstruct);
            return -1;
        }

        if (ret_val == 1)
            break;
    }

    int final_check_value = proc_final_check(procstruct);
    if (final_check_value == -1)
        return -1;

    return 0;
}

//===================================================================

#define DEF_CMD_(num, name, code, hash, instructions)               \
                                                                    \
    case code: {                                                    \
                                                                    \
        instructions                                                \
                                                                    \
        if (code == 0)                                              \
            return 1;                                               \
                                                                    \
        break;                                                      \
    }

//===================================================================

#define DEF_JMP_(num, name, code, hash, instructions)               \
                                                                    \
    case code: {                                                    \
                                                                    \
        instructions                                                \
                                                                    \
        break;                                                      \
    }                                                                

//===================================================================

int _proc_execute_command(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);
    procstruct_ip_check(procstruct);

    unsigned char oper_code = *(procstruct->ip);

    switch (oper_code & OPER_CODE_MASK) {

        #include "../text_files/commands.txt"
        #include "../text_files/jumps.txt"

        #undef DEF_CMD_
        #undef DEF_JMP_

        default: 
            set_and_process_err(PROC_INV_OPER_CODE);
    }

    return 0;
}

//===================================================================


int _proc_final_check(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    if (procstruct->ip - procstruct->code_array
        > procstruct->code_file_size
     || procstruct->ip - procstruct->code_array
        < 0)

        set_and_process_err(INV_INSTR_PTR);

    return 0;
}

//===================================================================

int _processor_dump(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);


    fprintf(logs_file, "\n" "Commands executed %d" "\n", 
                                procstruct->command_ct);

    fprintf(logs_file, "\n" "Size of code array: %lu", 
                          procstruct->code_file_size);

    fprintf(logs_file, "\n" "Code array address <%p>" "\n", 
                                   procstruct->code_array);


    if (procstruct->code_file_size != procstruct->header.file_size)

        fprintf(logs_file, "\n" "ERROR: incorrect code file size:");

        fprintf(logs_file, "\n" "File size written in header"
                                    " during compiling: %ld",
                               procstruct->header.file_size);

        fprintf(logs_file, "\n" "Size of file copied during processing: %ld",
                                                 procstruct->code_file_size);

    if (procstruct->code_array == NULL) {

        fprintf(logs_file, "\n" "ERROR: invalid pointer to the code array");
        return -1;
    }

    fprintf(logs_file, "\n" "Instruction pointer: %ld" "\n", 
                   procstruct->ip - procstruct->code_array);

    if (procstruct->ip - procstruct->code_array 
                       > procstruct->code_file_size) {

        fprintf(logs_file, "\n" "ERROR: instruction pointer"
                               " points out of code array");
        return -1;
    }

    fprintf(logs_file, "\n" "Code array: " "\n\n");

    int ip_value = procstruct->ip - procstruct->code_array;
    int val = 0;

    for (int ct = 0; ct < procstruct->code_file_size;) {

        fprintf(logs_file, "%02x ", procstruct->code_array[ct++]);

        if ((ct / 16) * 16 + 16 
                    > (ip_value / 16) * 16 + 16 && val == 0) {

            fprintf(logs_file, "\n" "%*c\n", 
                    (ip_value % 16) * 3 + 1, '^');
            val++;
        }

        else if ((ct % 16) == 0)
            fprintf(logs_file, "\n\n");
    }

    fprintf(logs_file, "\n\n");

    #ifdef DEBUG

        if (stack_validator(&procstruct->Stack) == -1) {

            fprintf(logs_file, "\n" "Error in stack work" "\n");
            stack_dump(&procstruct->Stack);
        }
        else {

            fprintf(logs_file, "\n" "No errors in stack working" "\n");
            stack_out(&procstruct->Stack);
        }

    #else 

        fprintf(logs_file, "\n" "No errors in stack working" "\n");
        stack_out(&procstruct->Stack);    

    #endif

    fprintf(logs_file, "\n\n" "Registers: " "\n\n");

    for (int ct = 0; ct < REGISTER_SIZE; ct++) {

        fprintf(logs_file, "[%c] DEC %g HEX %x \n", ct + ASCII_OFFSET + 1,
                                                   procstruct->regist[ct],
                                             (int)procstruct->regist[ct]);
    
        if ( (ct + 1) % 8 == 0)
        fprintf(logs_file, "\n\n");
    }

    fprintf(logs_file, "\n\n");

    return 0;
}
