#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
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
    push(b - a);
}

void times()
{
    assert(count >= 2 && "ERROR: Must have atleast 2 elements in the stack");
    int a = pop();
    int b = pop();
    push(a * b);
}

void sim(char *file_path)
{
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
}

int num_of_digits (int n) {
    if (n < 0) n = (n == INT_MIN) ? INT_MAX : -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    return 10;
}

void com(char *file_path)
{
    FILE *f = fopen(file_path, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: %s: %s\n", strerror(errno), file_path);
    }

    FILE *out = fopen("out.asm", "w");
    if (out == NULL) {
        fprintf(stderr, "Error: %s: %s\n", strerror(errno), "out.asm");
    }

    fputs("section .text\n", out);
    fputs("global _start\n", out);
    fputs("dump:\n", out);
    fputs("    mov     r9, -3689348814741910323\n", out);
    fputs("    sub     rsp, 40\n", out);
    fputs("    mov     BYTE [rsp+31], 10\n", out);
    fputs("    lea     rcx, [rsp+30]\n", out);
    fputs(".L2:\n", out);
    fputs("    mov     rax, rdi\n", out);
    fputs("    lea     r8, [rsp+32]\n", out);
    fputs("    mul     r9\n", out);
    fputs("    mov     rax, rdi\n", out);
    fputs("    sub     r8, rcx\n", out);
    fputs("    shr     rdx, 3\n", out);
    fputs("    lea     rsi, [rdx+rdx*4]\n", out);
    fputs("    add     rsi, rsi\n", out);
    fputs("    sub     rax, rsi\n", out);
    fputs("    add     eax, 48\n", out);
    fputs("    mov     BYTE [rcx], al\n", out);
    fputs("    mov     rax, rdi\n", out);
    fputs("    mov     rdi, rdx\n", out);
    fputs("    mov     rdx, rcx\n", out);
    fputs("    sub     rcx, 1\n", out);
    fputs("    cmp     rax, 9\n", out);
    fputs("    ja      .L2\n", out);
    fputs("    lea     rax, [rsp+32]\n", out);
    fputs("    mov     edi, 1\n", out);
    fputs("    sub     rdx, rax\n", out);
    fputs("    xor     eax, eax\n", out);
    fputs("    lea     rsi, [rsp+32+rdx]\n", out);
    fputs("    mov     rdx, r8\n", out);
    fputs("    mov     rax, 1\n", out);
    fputs("    syscall\n", out);
    fputs("    add     rsp, 40\n", out);
    fputs("    ret\n", out);
    fputs("\n", out);
    fputs("_start:\n", out);
    
    char *line = NULL;
    int line_no = 1;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, f)) != -1) {
        Token *tokens = tokenize(line, line_no);
        for (int i = 0; tokens[i].kind != End; i++) {
            TokenKind tk = tokens[i].kind;
            
            if (tk == Bang) {
                fputs("    ;; <-- Dump --> ;;\n", out);
                fputs("    pop rdi\n", out);
                fputs("    call dump\n", out);
            } else if (tk == Over) { 
                fputs("    ;; <-- Over --> ;;\n", out);
                fputs("    pop rax\n", out);
                fputs("    pop rbx\n", out);
                fputs("    push rax\n", out);
                fputs("    push rbx\n", out);
            } else if (tk == Dup) {
                fputs("    ;; <-- Dup --> ;;\n", out);
                fputs("    pop rax\n", out);
                fputs("    push rax\n", out);
                fputs("    push rax\n", out);
            } else if (tk == Number) {
                fputs("    push ", out);
                char str[num_of_digits(tokens[i].value)];
                sprintf(str, "%d", tokens[i].value);
                fputs(str, out);
                fputc('\n', out);
            } else if (tk == Plus) {
                fputs("    ;; <-- Plus --> ;;\n", out);
                fputs("    pop rax\n", out);
                fputs("    pop rbx\n", out);
                fputs("    add rax, rbx\n", out);
                fputs("    push rax\n", out);
            } else if (tk == Minus) {
                fputs("    ;; <-- Minus --> ;;\n", out);
                fputs("    pop rax\n", out);
                fputs("    pop rbx\n", out);
                fputs("    sub rbx, rax\n", out);
                fputs("    push rbx\n", out);
            } else if (tk == Times) {
                fputs("    ;; <-- Times --> ;;\n", out);
                fputs("    pop rax\n", out);
                fputs("    pop rbx\n", out);
                fputs("    mul rbx\n", out);
                fputs("    push rax\n", out);
            } else {
                fprintf(stderr, "%s:%d:%d: error: unknown operation: `%s`\n", file_path, tokens[i].line, tokens[i].col, tokens[i].err);
                exit(1);
            }
        }
        line_no++;
    }

    fputs("    mov rax, 60\n", out);
    fputs("    mov rdi, 0\n", out);
    fputs("    syscall\n", out);
    
    if (fclose(f) != 0 || fclose(out)) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(1);
    }

    system("nasm -f elf64 -o out.o out.asm");
    system("ld out.o -o out");
}

int main(int argc, char **argv)
{
    (void) argc;
    assert(argv != NULL);

    char *program = *argv++;
    char *mode = *argv++;

    if (mode == NULL) {
        fprintf(stderr, "Error: Expected a program type\n");
        fprintf(stderr, "Usage: %s sim some_file.basel\n", program);
        exit(1);
    }
    
    if (strcmp(mode, "sim") != 0 && strcmp(mode, "com") != 0) {
        fprintf(stderr, "Error: Invalid program mode: %s\n", mode);
        fprintf(stderr, "Usage: %s (sim or com) some_file.basel\n", program);
        exit(1);
    }

    char *file_path = *argv++;
    if (file_path == NULL) {
        fprintf(stderr, "Error: No input file path provided\n");
        fprintf(stderr, "Usage: %s sim some_file.basel\n", program);
        exit(1);
    }

    if (strstr(file_path, ".basel") == NULL) {
        fprintf(stderr, "Error: %s is not a basel file\n", file_path);
        exit(1);
    }

    if (strcmp(mode, "sim") == 0) {
        sim(file_path);
    } else {
        com(file_path);
    }

    return 0;
}
