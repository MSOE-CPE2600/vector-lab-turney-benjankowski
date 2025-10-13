/*
Lab 5
Ben Jankowski
Systems Programming

Handled parsing and evaluating expressions
*****************************************/

#include "equate.h"
#include "vectors.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define NEXT_ARGUMENT if (argument_pos != 0) { \
                      args[current_argument++][argument_pos] = '\0'; \
                      argument_pos = 0; \
                      in_number = 0; \
                      }

#define LOAD_AB Vector_t* a = lookup_vector(action->arg1, variables, variable_count); \
                Vector_t* b = lookup_vector(action->arg2, variables, variable_count); \
                if (!a || !b) return eqErr_VariableNotFound;


vsize_t parse_number(const char* str) {
    char* end = NULL;
    return strtod(str, &end);
}

int is_number(const char* str) {
    int cursor = 0;

    int seen_number = 0;
    int seen_dot = 0;

    if (!str) return 0;

    while (str[cursor] != '\0') {
        if (str[cursor] == '-' && cursor != 0) return 0;
        if (str[cursor] == '.') { seen_dot += 1; cursor++; continue; }
        if (str[cursor] < '0' || str[cursor] > '9') return 0;
        seen_number = 1;
        cursor++;
    }

    return seen_number && seen_dot <= 1;
}

void free_action(action_t* action) {
    if (!action) return;

    free(action->arg_store);
    free(action->arg1);
    free(action->arg2);
    free(action->arg3);

    action->arg_store = NULL;
    action->arg1 = NULL;
    action->arg2 = NULL;
    action->arg3 = NULL;
}

void free_variable(variable_t* variable) {
    if (!variable) return;

    free(variable->name);
    variable->name = NULL;
}

Vector_t* lookup_vector(
    const char* name,
    variable_t* variables,
    int variable_count) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(name, variables[i].name) == 0) {
            return &variables[i].vector;
        }
    }
    return NULL;
}

variable_t* lookup_variable(
    const char* name,
    variable_t* variables,
    int variable_count) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(name, variables[i].name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

eqErr_t parse_action(const char* str, action_t* result) {
    int cursor = 0; // Stores the position of the parser in the string
    int argument_pos = 0; // Stores the current argument position
    int current_argument = 0; // Stores the current argument being parsed

    int in_number = 0; // flag to check if the last character parsed was a number

    inst_t action = inst_Print;
    inst_t instruction = inst_None;

    char args_store[ARGUMENT_SIZE];
    char args[ARGUMENT_COUNT][ARGUMENT_SIZE];

    while (str[cursor] != '\0') {
        const char current_char = str[cursor++];

        switch (current_char) {
            case '=':
                action = inst_Store;

                if (argument_pos != 0) {
                    // Add null terminator
                    args[0][argument_pos] = '\0';
                }
                strcpy(args_store, args[0]);

                argument_pos = 0;
                current_argument = 0;
                continue;
            case '+':
                instruction = inst_Add;
                NEXT_ARGUMENT
                continue;
            case '-':
                instruction = inst_Subtract;
                NEXT_ARGUMENT
                continue;
            case '*':
                instruction = inst_Multiply;
                NEXT_ARGUMENT
                continue;
            case '.':
                /*
                 * Since a number can have a . if a number
                 * is detected. Don't parse as a dot product
                 */
                if (in_number) { break; }
                instruction = inst_Dot;
                NEXT_ARGUMENT
                continue;
            case 'x':
                instruction = inst_Cross;
                NEXT_ARGUMENT
                continue;
            case ',':
            case ' ':
                NEXT_ARGUMENT
                continue;
            case '\n':
                args[current_argument][argument_pos++] = '\0';
                continue;
            default:
                break;
        }

        if (current_argument >= ARGUMENT_COUNT) {
            return eqErr_ArgumentCountOutOfBounds;
        }

        if (argument_pos >= ARGUMENT_SIZE) {
            return eqErr_ArgumentSizeOutOfBounds;
        }

        if (current_char >= '0' && current_char <= '9') in_number = 1;
        args[current_argument][argument_pos++] = current_char;
    }

    if (action != inst_Store) {
        // If we are not storing, null out the array to avoid
        // undefined behavior with unknown memory.
        args_store[0] = '\0';
    }

    // Blank out unused strings to not leave the struct with broken strings
    for (int i = current_argument + 1; i < ARGUMENT_COUNT; i++) {
        args[i][0] = '\0';
    }

    result->action      = action;
    result->instruction = instruction;
    /*
     * Because the strings are being created in this function
     * strdup is needed to use malloc to allocate memory for it
     * and provide that pointer to the Action object.
     * Otherwise, the Action struct would contain dangling pointers.
     */
    result->arg_store   = strdup(args_store);
    result->arg1        = strdup(args[0]);
    result->arg2        = strdup(args[1]);
    result->arg3        = strdup(args[2]);

    // strdup will set the pointer to NULL if the malloc fails
    if (!result->arg_store || !result->arg1 || !result->arg2 || !result->arg3) {
        return eqErr_MemoryError;
    }

    return eqErr_Ok;
}

eqErr_t evaluate_action(
    Vector_t* output,
    const action_t* action,
    variable_t* variables,
    const int variable_count
    ) {
    if (action->instruction == inst_None) {
        if (
            !is_number(action->arg1) ||
            !is_number(action->arg2) ||
            !is_number(action->arg3)) {
            variable_t* variable = lookup_variable(
                action->arg1, variables, variable_count);

            if (strlen(action->arg2) != 0 || strlen(action->arg3) != 0) {
                return eqErr_InvalidArguments;
            }

            if (!variable) {
                return eqErr_VariableNotFound;
            }

            output->x = variable->vector.x;
            output->y = variable->vector.y;
            output->z = variable->vector.z;
            return eqErr_Ok;
        }

        output->x = parse_number(action->arg1);
        output->y = parse_number(action->arg2);
        output->z = parse_number(action->arg3);
        return eqErr_Ok;
    }

    if (action->instruction == inst_Add) {
        LOAD_AB
        const Vector_t r = vector_add(a, b);
        output->x = r.x;
        output->y = r.y;
        output->z = r.z;
        return eqErr_Ok;
    }

    if (action->instruction == inst_Subtract) {
        LOAD_AB
        const Vector_t r = vector_sub(a, b);
        output->x = r.x;
        output->y = r.y;
        output->z = r.z;
        return eqErr_Ok;
    }

    if (action->instruction == inst_Multiply) {
        if (is_number(action->arg1)) {
            const vsize_t a = parse_number(action->arg1);
            const Vector_t* b = lookup_vector(action->arg2, variables, variable_count);

            if (!b) return eqErr_VariableNotFound;

            const Vector_t r = vector_smul(b, a);
            output->x = r.x;
            output->y = r.y;
            output->z = r.z;
            return eqErr_Ok;
        }

        if (is_number(action->arg2)) {
            const Vector_t* a = lookup_vector(action->arg1, variables, variable_count);
            const vsize_t b = parse_number(action->arg2);

            if (!a) return eqErr_VariableNotFound;

            const Vector_t r = vector_smul(a, b);
            output->x = r.x;
            output->y = r.y;
            output->z = r.z;
            return eqErr_Ok;
        }

        return eqErr_InvalidArguments;
    }

    if (action->instruction == inst_Dot) {
        LOAD_AB
        output->x = vector_dot(a, b);
        output->y = 0.0;
        output->z = 0.0;
        return eqErr_Ok;
    }

    if (action->instruction == inst_Cross) {
        LOAD_AB
        const Vector_t r = vector_cross(a, b);
        output->x = r.x;
        output->y = r.y;
        output->z = r.z;
        return eqErr_Ok;
    }

    return eqErr_UnexpectedCondition;
}
