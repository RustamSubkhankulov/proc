#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "stack.h"
#include "errors_and_logs.h"

int main(int argc, char* argv[]) {

    int err = 0;

    FILE* logs_file = open_log_file(argv[1]);

    stack Stack = { 0 };

    stack_ctor(&Stack);

    stack_push(&Stack, 10);

    stack_pop(&Stack, &err);

    stack_dtor(&Stack);

    close_log_file(logs_file);
}