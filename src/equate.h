#pragma once

#include "vectors.h"

#define ARGUMENT_SIZE 20
#define ARGUMENT_COUNT 3

typedef enum {
    eqErr_Ok,
    eqErr_BadInput,
    eqErr_InvalidArguments,
    eqErr_UnexpectedCondition,
    eqErr_MemoryError,
    eqErr_VariableNotFound,
    eqErr_ArgumentSizeOutOfBounds,
    eqErr_ArgumentCountOutOfBounds
} eqErr_t;

typedef enum {
    inst_None,
    inst_Store,
    inst_Print,
    inst_Add,
    inst_Subtract,
    inst_Multiply,
    inst_Dot,
    inst_Cross
} inst_t;

typedef struct {
    char* name;
    Vector_t vector;
} variable_t;

typedef struct {
    inst_t action; // What to do with instruction
    inst_t instruction; // Instruction to perform
    char* arg_store;
    char* arg1;
    char* arg2;
    char* arg3;
} action_t;

vsize_t parse_number(const char* str);
int is_number(const char* str);

void free_action(action_t* action);
void free_variable(variable_t* variable);
/*
 * Attempts to look up a vector by
 * variable name given an array of variables
 *
 * Returns NULL if the variable is not found
 */
Vector_t* lookup_vector(
    const char* name,
    variable_t* variables,
    int variable_count);
/*
 * Attempts to look up a variable by
 * name given an array of variables
 *
 * Returns NULL if the variable is not found
 */
variable_t* lookup_variable(
    const char* name,
    variable_t* variables,
    int variable_count);

eqErr_t parse_action(const char* str, action_t* result);
eqErr_t evaluate_action(
    Vector_t* output,
    const action_t* action,
    variable_t* variables,
    int variable_count);
