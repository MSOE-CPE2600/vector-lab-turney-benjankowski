/*
Lab 5
Ben Jankowski
Systems Programming
*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "equate.h"
#include "vectors.h"

#define BUFFER_SIZE 50
#define VARIABLE_COUNT 15

void print_variables(const variable_t* vars, const int size) {
    for (int i = 0; i < size; i++) {
        const variable_t var = vars[i];
        printf("%s = x:%f y:%f z:%f\n",
            var.name,
            var.vector.x,
            var.vector.y,
            var.vector.z);
    }
}

void print_vector(const Vector_t vector) {
    printf("x:%f y:%f z:%f\n", vector.x, vector.y, vector.z);
}

void print_variable(const variable_t* var) {
    printf("%s = ", var->name);
    print_vector(var->vector);
}

void print_help() {
    printf("help : display help page\n");
    printf("list : see all stored variables\n");
    printf("clear: clear all stored variables\n");
    printf("quit : exit program\n");
}

int main(void) {
    char input[BUFFER_SIZE];
    int variable_count = 0;
    variable_t variables[VARIABLE_COUNT];

    action_t current_action = {0};

    while (1) {
        printf(">>> ");
        fgets(input, BUFFER_SIZE, stdin);

        // Before sending to parsing free any allocated memory
        free_action(&current_action);

        const eqErr_t parse_err = parse_action(input, &current_action);
        if (parse_err != eqErr_Ok) {
            printf("Input could not be parsed (err %d)\n", parse_err);
            continue;
        }

        if (strcmp(current_action.arg1, "help") == 0) {
            print_help();
            continue;
        }

        if (strcmp(current_action.arg1, "quit") == 0) {
            break;
        }

        if (strcmp(current_action.arg1, "list") == 0) {
            print_variables(variables, variable_count);
            continue;
        }

        if (strcmp(current_action.arg1, "clear") == 0) {
            for (int i = 0; i < variable_count; i++) {
                free_variable(&variables[i]);
                variables[i].vector = (Vector_t){0, 0, 0};
            }
            variable_count = 0;
            printf("All variables cleared\n");
            continue;
        }

        Vector_t evaluated_vector;
        const int eval_err = evaluate_action(
            &evaluated_vector,
            &current_action,
            variables,
            variable_count);

        if (eval_err != eqErr_Ok) {
            printf("Failed to evaluate expression (code %d)\n", eval_err);
            continue;
        }

        if (current_action.action == inst_Print) {
            printf("%s = ", current_action.arg1);
            print_vector(evaluated_vector);
        }

        if (current_action.action == inst_Store) {
            variable_t* lookup = lookup_variable(
                current_action.arg_store, variables, variable_count);
            char* copy_name = strdup(current_action.arg_store);

            if (!copy_name) {
                printf("Failed to evaluate expression (code %d)\n",
                    eqErr_MemoryError);
                continue;
            }

            const variable_t new_variable = {
                copy_name,
                evaluated_vector
            };

            if (lookup) {
                free_variable(lookup);
                lookup->name = new_variable.name;
                lookup->vector = new_variable.vector;
            } else {
                if (variable_count < VARIABLE_COUNT) {
                    variables[variable_count] = new_variable;
                    variable_count++;
                } else {
                    free(copy_name);
                    printf("Out of variable memory (%d >= %d max)\n",
                        variable_count,
                        VARIABLE_COUNT);
                    continue;
                }
            }

            print_variable(&new_variable);
        }
    }
}
