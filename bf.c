/**
 * @file bf.c
 * @author Chen QingYu (chen_qingyu@qq.com)
 * @brief A simple Brainfuck interpreter implemented in C. Reference to http://brainfuck.org/
 * @version 1.1.1
 * @date 2021.04.25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BF_VERSION "1.1.1"

// Copy of the program code.
char* code = NULL;
int code_size = 0;

// To save matching '[' for each ']' and vice versa.
int* targets = NULL;

// Show help message.
void show_help();

// Read the contents of the file.
void read_file(const char* file);

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

    // Flag for comment feature: ';'.
    int enable_comment = 0;

    // Flag for debug feature: '#'.
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

    free(code);
    free(targets);

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

void check_pointer(void* p)
{
    if (p == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

void read_file(const char* file)
{
    // Open file.
    FILE* program;
    if (fopen_s(&program, file, "rb"))
    {
        fprintf(stderr, "Error: Can't open file: \"%s\".\n", file);
        exit(EXIT_FAILURE);
    }

    // Allocate memory for code and targets.
    fseek(program, 0, SEEK_END);
    code_size = ftell(program);
    code = (char*)calloc(code_size, sizeof(char));
    check_pointer(code);
    targets = (int*)calloc(code_size, sizeof(int));
    check_pointer(targets);

    // Read file.
    rewind(program);
    fread(code, 1, code_size, program);
    fclose(program);
}

void preprocess()
{
    // To store locations of still-unmatched '['s.
    int* stack = (int*)calloc(code_size, sizeof(int));
    check_pointer(stack);
    int stack_ptr = 0;

    for (int code_ptr = 0; code_ptr < code_size; code_ptr++)
    {
        if (code[code_ptr] == '[') // put each '[' on the stack
        {
            stack[stack_ptr++] = code_ptr;
        }
        if (code[code_ptr] == ']') // if meet a ']'
        {
            if (stack_ptr == 0) // and there is no '[' left on the stack, it's an error
            {
                fprintf(stderr, "Error: Unmatched ']' at position %d.\n", code_ptr);
                exit(EXIT_FAILURE);
            }
            else
            {
                targets[code_ptr] = stack[--stack_ptr]; // pop the matching '[', save it as the match for the current ']'
                targets[stack[stack_ptr]] = code_ptr;   // and save the current ']' as the match for it
            }
        }
    }
    if (stack_ptr > 0) // any unmatched '[' still left on the stack raise an error too
    {
        fprintf(stderr, "Error: Unmatched '[' at position %d.\n", stack[--stack_ptr]);
        exit(EXIT_FAILURE);
    }

    free(stack);
}

int next_power_of_two(int n)
{
    if (n <= 1)
    {
        return 2;
    }
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n++;
    return n;
}

void interpret(int enable_comment, int enable_debug)
{
    // The memory used by the program runtime.
    int memory_size = next_power_of_two(code_size + 8); // at least 16 bytes
    char* memory = (char*)calloc(memory_size, sizeof(char));
    check_pointer(memory);
    int memory_ptr = 0;

    int ch = 0;
    for (int code_ptr = 0; code_ptr < code_size; code_ptr++)
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
                // Dynamic memory growth at runtime.
                if (++memory_ptr > memory_size / 2)
                {
                    int old_size = memory_size;
                    memory_size = next_power_of_two(memory_size);
                    memory = (char*)realloc(memory, memory_size * sizeof(char));
                    check_pointer(memory);
                    memset(memory + old_size, 0, memory_size - old_size);
                }
                break;

            // Move data pointer to previous address.
            case '<':
                if (--memory_ptr < 0)
                {
                    fprintf(stderr, "Error: Memory pointer crosses memory lower bound.\n");
                    exit(EXIT_FAILURE);
                }
                break;

            // Accept one character from user.
            case ',':
                if ((ch = getchar()) != EOF)
                {
                    memory[memory_ptr] = ch;
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
                    for (int i = memory_ptr - 8; i < memory_ptr + 8; i++)
                    {
                        if (i < 0)
                        {
                            printf("-- ");
                        }
                        else
                        {
                            printf("%02hhX ", memory[i]);
                        }
                    }
                    printf("\n%*s\n", 8 * 3 + 2, "^^");
                    printf("%*X\n", 8 * 3 + 2, memory_ptr);
                }
                break;

            // Ignore other characters.
            default:
                break;
        }
    }

    free(memory);
}
