#include "disasm.h"

int main(int argc, char* argv[]) {

    disasm_smpl_log_report();
    int err_val = 0;

    disasmstruct Disasmstruct = { 0 };

    FILE* fp = open_code_file(argv[1], "rb");
    if (fp == NULL)
        return -1;

    err_val = init_disasmstruct(&Disasmstruct, fp); $
    err_val = close_code_file(fp); $

    FILE* disasm_out = open_file(argv[2], "wb", &err_val); $
    err_val = disassemble_code(&Disasmstruct, disasm_out); $
    err_val = close_file(disasm_out); $

    err_val = dtor_disasmstruct(&Disasmstruct);
}