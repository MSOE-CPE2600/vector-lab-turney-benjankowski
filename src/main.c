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
#define VARIABLE_COUNT_START 2
#define VARIABLE_INCREMENT 10

#define VARIABLE_LIST_RESIZE if (variable_count >= variable_capacity) { \
                                 variable_capacity += VARIABLE_INCREMENT; \
                                 variable_t** v = realloc( \
                                         variables, \
                                         variable_capacity * sizeof(variable_t) \
                                     ); \
                                 if (!v) { \
                                     printf("Unable to resize array\n"); \
                                     continue; \
                                 } \
                                 variables = v; \
                             }

#define READ_FILE_NAME char file_name[20];\
                       strcpy(file_name, current_action.arg2); \
                       if (strlen(current_action.arg3) != 0) { \
                           strcat(file_name, "."); \
                           strcat(file_name, current_action.arg3); \
                       }

void print_variables(variable_t** vars, const int size) {
    if (!vars) { return; }

    for (int i = 0; i < size; i++) {
        const variable_t* var = vars[i];
        printf("%s = x:%f y:%f z:%f\n",
            var->name,
            var->vector.x,
            var->vector.y,
            var->vector.z);
    }
}

void free_variables(variable_t** vars, const int size) {
    for (int i = 0; i < size; i++) {
        free_variable(vars[i]);
    }
    free(vars);
}

void print_vector(const Vector_t vector) {
    printf("x:%f y:%f z:%f\n", vector.x, vector.y, vector.z);
}

void print_variable(const variable_t* var) {
    printf("%s = ", var->name);
    print_vector(var->vector);
}

void print_help() {
    printf("help ...... : display help page\n");
    printf("list ...... : see all stored variables\n");
    printf("clear ..... : clear all stored variables\n");
    printf("load <file> : load a csv file of variables\n");
    printf("save <file> : save current vectors into csv file\n");
    printf("quit ...... : exit program\n");
}

int main(void) {
    char input[BUFFER_SIZE];
    int variable_count = 0;
    int variable_capacity = VARIABLE_COUNT_START;

    variable_t** variables = malloc(variable_capacity * sizeof(variable_t));

    if (!variables) {
        printf("Failed to allocate memory for variables\n");
        return 1;
    }

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
            /* Needed to satisfy Valgrind
             * Because the free is done at the start of each loop
             * the program always will exit without freeing some memory
             * this makes Valgrind complain as it sees the memory
             * is not free on program exit even though it would be
             * in normal program execution.
             */
            free_action(&current_action);
            free_variables(variables, variable_count);
            break;
        }

        if (strcmp(current_action.arg1, "list") == 0) {
            print_variables(variables, variable_count);
            continue;
        }

        if (strcmp(current_action.arg1, "clear") == 0) {
            for (int i = 0; i < variable_count; i++) {
                free_variable(variables[i]);
            }
            variable_count = 0;
            printf("All variables cleared\n");
            continue;
        }

        if (strcmp(current_action.arg1, "load") == 0) {
            READ_FILE_NAME

            FILE* file_ptr = fopen(file_name, "rb");
            if (!file_ptr) {
                printf("File could not be opened: %s\n", file_name);
                continue;
            }

            char line_buffer[50];
            while (fgets(line_buffer, sizeof(line_buffer), file_ptr) != NULL) {
                VARIABLE_LIST_RESIZE

                char variable_name[20];
                double x, y, z;

                const int result = sscanf(line_buffer, "%[^,],%lf,%lf,%lf",
                    variable_name, &x, &y, &z);

                if (result != 4) {
                    continue;
                }

                const int var_id = lookup_variable(
                    variable_name, variables, variable_count);

                variable_t* new_variable = malloc(sizeof(variable_t));
                new_variable->name = strdup(variable_name);
                new_variable->vector.x = x;
                new_variable->vector.y = y;
                new_variable->vector.z = z;

                if (var_id == -1) {
                    variables[variable_count++] = new_variable;
                } else {
                    free_variable(variables[var_id]);
                    variables[var_id] = new_variable;
                }
            }
            
            fclose(file_ptr);
            continue;
        }

        if (strcmp(current_action.arg1, "save") == 0) {
            READ_FILE_NAME
            
            FILE* file_ptr = fopen(file_name, "w");
            if (!file_ptr) {
                printf("File could not be opened: %s\n", file_name);
                continue;
            }

            for (int i = 0; i < variable_count; i++) {
                const variable_t* v = variables[i];

                fprintf(file_ptr, "%s,%lf,%lf,%lf\n", 
                    v->name, 
                    v->vector.x, 
                    v->vector.y, 
                    v->vector.z);
            }

            fclose(file_ptr);
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
            const int lookup = lookup_variable(
                current_action.arg_store, variables, variable_count);
            char* copy_name = strdup(current_action.arg_store);

            if (!copy_name) {
                printf("Failed to evaluate expression (code %d)\n",
                    eqErr_MemoryError);
                continue;
            }

            variable_t* new_variable = malloc(sizeof(variable_t));
            new_variable->name = copy_name;
            new_variable->vector = evaluated_vector;

            if (lookup != -1) {
                free_variable(variables[lookup]);
                variables[lookup] = new_variable;
            } else {
                VARIABLE_LIST_RESIZE
                variables[variable_count++] = new_variable;
            }

            print_variable(new_variable);
        }
    }
}
