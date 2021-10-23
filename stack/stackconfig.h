#pragma once

#include <stdio.h>

/// Element type using in stack
/// 
/// Typedef for type that will be using in stack
typedef int elem_t;

/// Name of the element
///
/// String definition for the name of element type using in stack
#define TYPE_NAME "int"

#define ELEM_SPEC "%d"

//===================================================================

/// DEBUG 
///
/// Definition debug to turn on debug 
/// In debug mode turns on stack_validator_() and error_processing functions
/// that prints messages in file that is pointed by error_file 
//#define DEBUG

//===================================================================

///LOGS definition
///
/// Definition logs to turn on log reporting
/// Log reports will be printed in lof_file 
//#define LOGS

//===================================================================

#ifdef DEBUG

    ///CANARIES
    ///
    /// Definition that turns on canary protection  
    #define CANARIES

    ///HASH
    ///
    /// Definition that turns on hash protection
    #define HASH

#endif

//===================================================================

///    Maximum capacity
///
/// Maaximum capacity for stack(number of elements)
#define MAXCAPACITY 100

/// Size of name buff
///
/// Size of constant char array for names of file and function and line
/// where the stack was conctructed by calling stack_ctor_();
#define NAMEBUFSIZE 20

/// Starting size
///
/// Starting size of the stack when the first element is pushed 
#define STACK_STARTING_SIZE 4

//===================================================================

/// Default pointer
///
/// Magical number for the stack->data pointer before the first stack->push call
#define DEFAULT_PTR 282

/// Poison value for stack->data
///
/// Poison value for the stack->data that to be set after stack_dtor_(); call
#define POISON_PTR 228

/// Poison value for the stack fields after stack_dtor_() call
#define POISON_VALUE (1000 - 7)

///Poison string that to be set in struct origin in debug mode
#define POISONSTR "POISONED"

///Default canary value to be checked in debug mode with canary protection turned on
#define CANARY_VAL 0xDEAD1488C0DE1000 - 7

/// Enumeration type used for stack_resize_ parameter
///
/// Parameter that stands to resize mode : expanding stack->data or reducing it
enum { REDUCE = 1000, EXPAND = 2000 };

//===================================================================

/// Log file FILE* pointer
/// 
/// Default file pointer to log file
static FILE* log_file = stderr;

/// Error file FILE* pointer
/// 
/// Default error file pointer
static FILE* error_file = stderr;


