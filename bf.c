/**
 * @file bf.c
 * @author Chen QingYu (chen_qingyu@qq.com)
 * @brief A simple Brainfuck interpreter implemented in C. Reference to http://brainfuck.org/
 * @version 1.1.0
 * @date 2021.04.25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BF_VERSION "1.1.0"

#define CODE_SIZE 65536
#define MEMORY_SIZE (65536 * 4)

// Copy of the program code.
char code[CODE_SIZE] = {0};
int code_ptr = 0;
int code_length = 0;

// The memory used by the brainfuck program.
short int memory[MEMORY_SIZE] = {0};
int memory_ptr = 0;

// To save matching '[' for each ']' and vice versa.
int targets[CODE_SIZE] = {0};

// Show help message.
void show_help();

// Read the contents of the file.
void read_file(char* file);

// Preprocess and check whether the brackets match.
void preprocess();

// Execute the program.
void interpret(int enable_comment, int enable_debug);

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        show_help();
        exit(EXIT_FAILURE);
    }

    // Flag for comment feature: ';'
    int enable_comment = 0;

    // Flag for debug feature: '#'
    int enable_debug = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--comment") == 0 || strcmp(argv[i], "-c") == 0)
        {
            enable_comment = 1;
        }
        else if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0)
        {
            enable_debug = 1;
        }
        else if (strcmp(argv[i], "-cd") == 0 || strcmp(argv[i], "-dc") == 0)
        {
            enable_comment = 1;
            enable_debug = 1;
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0)
        {
            printf("bf.exe %s by Chen QingYu\n", BF_VERSION);
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            show_help();
            exit(EXIT_SUCCESS);
        }
        else
        {
            read_file(argv[i]);
            break;
        }
    }

    preprocess();

    interpret(enable_comment, enable_debug);

    return EXIT_SUCCESS;
}

void show_help()
{
    puts("A simple Brainfuck interpreter written by Chen QingYu.");
    puts("Usage: bf [options] <filename>");
    puts("  options:");
    puts("    -c, --comment  Enable comment feature: ';'.");
    puts("    -d, --debug    Enable debug feature: '#'.");
    puts("    -v, --version  Show program version and exit.");
    puts("    -h, --help     Show this help message and exit.");
}

void read_file(char* file)
{
    FILE* program;
    if (fopen_s(&program, file, "r"))
    {
        fprintf(stderr, "Error: Can't open file: \"%s\".\n", file);
        exit(EXIT_FAILURE);
    }

    code_length = fread(code, 1, CODE_SIZE, program);
    fclose(program);
}

void preprocess()
{
    // To store locations of still-unmatched '['s.
    int stack[CODE_SIZE];
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

void interpret(int enable_comment, int enable_debug)
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
                    memory[memory_ptr] = c;
                }
                break;

            // Output character at current data cell.
            case '.':
                putchar(memory[memory_ptr]);
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

            // Single-line comment.
            case ';':
                if (enable_comment)
                {
                    while (code[++code_ptr] != '\n')
                    {
                    }
                }
                break;

            // Print the program's internal state for debugging.
            case '#':
                if (enable_debug)
                {
                    printf("\n");
                    for (int i = 0; i < 16; i++)
                    {
                        printf("%02X ", (signed char)memory[i]);
                    }
                    printf("\n%*s\n", memory_ptr * 3 + 2, "^^");
                }
                break;

            // Ignore other characters.
            default:
                break;
        }
    }
}
