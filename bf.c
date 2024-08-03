/**
 * @file bf.c
 * @author Qingyu Chen (chen_qingyu@qq.com)
 * @brief A simple Brainfuck interpreter implemented in C. Reference to http://brainfuck.org/
 * @version 1.0.0
 * @date 2021.04.25
 *
 * @copyright Copyright (c) 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BF_VERSION "1.0.0"

#define MAX_CODE_SIZE 65536
#define MEMORY_SIZE (65536 * 4)

// Copy of the program code.
char code[MAX_CODE_SIZE] = {0};
int code_ptr = 0;
int code_length = 0;

// The memory used by the brainfuck program.
short int memory[MEMORY_SIZE] = {0};
int memory_ptr = 0;

// To save matching '[' for each ']' and vice versa.
int targets[MAX_CODE_SIZE] = {0};

// Read the contents of the file.
void read_file(char* file);

// Preprocess and check whether the brackets match.
void preprocess();

// Execute the program.
void interpret();

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: bf <filename>\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "--version") == 0)
    {
        printf("bf.exe %s\n", BF_VERSION);
        exit(EXIT_SUCCESS);
    }

    read_file(argv[1]);

    preprocess();

    interpret();

    return EXIT_SUCCESS;
}

void read_file(char* file)
{
    FILE* program;
    if (fopen_s(&program, file, "r"))
    {
        fprintf(stderr, "Error: Can't open file: \"%s\".\n", file);
        exit(EXIT_FAILURE);
    }

    code_length = fread(code, 1, MAX_CODE_SIZE, program);
    fclose(program);
}

void preprocess()
{
    // To store locations of still-unmatched '['s.
    int stack[MAX_CODE_SIZE];
    int stack_ptr = 0;

    for (code_ptr = 0; code_ptr < code_length; code_ptr++)
    {
        if (code[code_ptr] == '[') // put each '[' on the stack.
        {
            stack[stack_ptr++] = code_ptr;
        }
        if (code[code_ptr] == ']') // if meet a ']',
        {
            if (stack_ptr == 0) // and there is no '[' left on the stack, it's an error.
            {
                fprintf(stderr, "Error: Unmatched ']' at position %d.", code_ptr);
                exit(EXIT_FAILURE);
            }
            else
            {
                targets[code_ptr] = stack[--stack_ptr]; // pop the matching '[', save it as the match for the current ']'
                targets[stack[stack_ptr]] = code_ptr;   // and save the current ']' as the match for it.
            }
        }
    }
    if (stack_ptr > 0) // any unmatched '[' still left on the stack raise an error too.
    {
        fprintf(stderr, "Error: Unmatched '[' at position %d.", stack[--stack_ptr]);
        exit(EXIT_FAILURE);
    }
}

void interpret()
{
    int c;
    for (code_ptr = 0; code_ptr < code_length; code_ptr++)
    {
        switch (code[code_ptr])
        {
            // Increase value at current data cell by one.
            case '+':
                memory[memory_ptr]++;
                break;

            // Decrease value at current data cell by one.
            case '-':
                memory[memory_ptr]--;
                break;

            // Move data pointer to next address.
            case '>':
                memory_ptr++;
                break;

            // Move data pointer to previous address.
            case '<':
                memory_ptr--;
                break;

            // Accept one character from user.
            case ',':
                if ((c = getchar()) != EOF)
                {
                    memory[memory_ptr] = (c == '\n' ? 10 : c);
                }
                break;

            // Output character at current data cell.
            case '.':
                putchar(memory[memory_ptr] == 10 ? '\n' : memory[memory_ptr]);
                fflush(stdout);
                break;

            // Begin loop.
            case '[':
                if (!memory[memory_ptr])
                {
                    code_ptr = targets[code_ptr];
                }
                break;

            // End loop.
            case ']':
                if (memory[memory_ptr])
                {
                    code_ptr = targets[code_ptr];
                }
                break;

            // Print the program's internal state for debugging.
            case '#':
                printf("\n");
                for (int i = 0; i < 16; i++)
                {
                    printf("%4d", (signed char)memory[i]);
                }
                printf("\n%*s\n", memory_ptr * 4 + 4, "^");
                break;

            // Ignore other characters.
            default:
                break;
        }
    }
}
