#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "lex.c"

#define STACK_CAP 255

int stack[STACK_CAP];
int count = 0;

void push(int v)
{
    assert(count < 255 && "ERROR: Stack is exhausted");
    stack[count] = v;
    count++;
}

int pop()
{
    assert(count > 0 && "ERROR: Cannot pop value from empty stack");
    int a = stack[count - 1];
    count--;
    return a;
}

void over()
{
    assert(count > 1 && "ERROR: Stack must have more than 1 element to use over");
    int top = pop();
    int over = pop();
    push(top);
    push(over);
}

void dup()
{
    int tmp = pop();
    push(tmp);
    push(tmp);
}

void dump()
{
    assert(count > 0 && "ERROR: Cannot dump value from empty stack");
    int s = pop();
    printf("%d\n", s);
}

void add()
{
    assert(count >= 2 && "ERROR: Must have atleast 2 elements in the stack");
    int a = pop();
    int b = pop();
    push(a + b);
}

void minus()
{
    assert(count >= 2 && "ERROR: Must have atleast 2 elements in the stack");
    int a = pop();
    int b = pop();
    push(a - b);
}

void times()
{
    assert(count >= 2 && "ERROR: Must have atleast 2 elements in the stack");
    int a = pop();
    int b = pop();
    push(a * b);
}

void divide()
{
    assert(count >= 2 && "ERROR: Must have atleast 2 elements in the stack");
    int a = pop();
    int b = pop();
    assert(a != 0 && b != 0 && "ERROR: Cannot divide by zero");
    push(b / a);
}

int main(int argc, char **argv)
{
    (void) argc;
    assert(argv != NULL);

    char *program = *argv++;
    char *file_path = *argv++;
    if (file_path == NULL) {
        fprintf(stderr, "Error: No input file path provided\n");
        fprintf(stderr, "Usage: %s some_file.basel\n", program);
        exit(1);
    }

    if (strstr(file_path, ".basel") == NULL) {
        fprintf(stderr, "Error: %s is not a basel file\n", file_path);
        exit(1);
    }

    FILE *f = fopen(file_path, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: %s: %s\n", strerror(errno), file_path);
    }

    char *line = NULL;
    int line_no = 1;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, f)) != -1) {
        Token *tokens = tokenize(line, line_no);
        for (int i = 0; tokens[i].kind != End; i++) {
            TokenKind tk = tokens[i].kind;
            if      (tk == Bang) { dump(); }
            else if (tk == Over) { over(); }
            else if (tk == Dup)  { dup(); }
            else if (tk == Number) { push(tokens[i].value); }
            else if (tk == Plus) { add(); }
            else if (tk == Minus) { minus(); }
            else if (tk == Divide) { divide(); }
            else if (tk == Times) { times(); }
            else {
                fprintf(stderr, "%s:%d:%d: error: unknown operation: `%s`\n", file_path, tokens[i].line, tokens[i].col, tokens[i].err);
                exit(1);
            }
        }
        line_no++;
    }

    if (fclose(f) != 0) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(1);
    }
    
    return 0;
}
