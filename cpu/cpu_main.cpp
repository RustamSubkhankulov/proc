#include "processor.h"

int main(int argc, char* argv[]) {

    FILE* logs_file = open_log_file(argv[4]);

    if (logs_file == NULL)
        return -1;

    proc_smpl_log_report();
    int err_val = 0;

    procstruct Procstruct = { 0 };

    FILE* fp = open_code_file(argv[1], "rb");
    if (fp == NULL)
        return -1;

    err_val = init_procstruct(&Procstruct, fp); $

    err_val = close_code_file(fp); $

    err_val = open_proc_input(argv[2]); $
    err_val = open_proc_output(argv[3]); $

    err_val = proc_perform(&Procstruct); $

    err_val = dtor_procstruct(&Procstruct); $

    err_val = close_proc_input(); $
    err_val = close_proc_output(); $

    err_val = close_log_file(logs_file); $ 
}