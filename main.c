/*
 Мой интерпретатор эзотерического языка Brainfuck
 
 Правила классического Brainfuck'а
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#define ASIZE 30000

//#define _DEBUG 

FILE *out;

void _debug(const char *str, ... )
{
#ifdef _DEBUG
    va_list vl;
    va_start(vl, str);

    vfprintf(out, str, vl);

    va_end(vl);
#endif
}

struct Stack {
    int size;
    int *head;
    int current;
};

void push_stack(struct Stack *st, int position)
{
    if(st->current >= st->size) {
        _debug("Now we're doing realloc\n");
        st->head = realloc(st->head, sizeof(int) * st->size * 1.5);
        st->size = st->size * 1.5;
    }

    if(st->head[st->current] != position) {
        _debug("First position: %d\n", position);
        ++st->current;
        st->head[st->current] = position;
    }
}

int pop_stack(struct Stack *st)
{
    assert(st->current >= 0);
    _debug("Now we're doing pop\n");
    int r = st->head[st->current];
    --st->current;
    return r;
}

int head_of_stack(struct Stack *st)
{
    return st->head[st->current];
}

int main(int argc, char **argv)
{
    int printFileName = 0;
    out  =  stderr; // fopen("log", "w");

    char ch = (char)72;
    _debug("%c", ch);

    if(argc == 1) {
        printf("Нет входных файлов!\n");
        return 1;
    }

    if(argc > 2) {
        printFileName = 1;
    }

    char **p = argv + 1;
    unsigned char memory[ASIZE] = {0}; /* лента */
    unsigned char *_m;
    FILE *file;
    int c;
    int pos;

    /* для поддержки циклов */
    struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
    stack->head = (int*)malloc(sizeof(int)*10);
    stack->size = 10;
    stack->current = -1;

    while(argc > 1) { 
        file = fopen(*p, "r");

        if(printFileName) {
            printf("%s:\n", *p);
        }

        /* DEBUG */
        _debug("%s:\n", p);
        /* */

        _m = memory;
        pos = 0;
        c = getc(file);
/*        int loop = 0; */

        while(c != EOF) {
/*            _debug("%c", c);  */

            switch(c) {
            case '>':
                ++_m;
                break;
            case '<':
                --_m;
                break;
            case '+':
                ++(*_m);
                break;
            case '-':
                --(*_m);
                break;
            case ',':
                scanf("%c", &(*_m));
                break;
            case '.':
                printf("%c", (char)*_m);
                break;
            case '[':
                if(*_m) {
                    push_stack(stack, pos);
                } else {    // пропускаем цикл
                    while(c != ']') {
                        c = getc(file);
                        ++pos;
                    }
                    pop_stack(stack);
                }
                break;
            case ']':
                pos = head_of_stack(stack);
                fseek(file, pos, SEEK_SET);
                --pos;
                break;
            }

            c = getc(file);
            ++pos;
/*            _debug("Position %d; char %c\n", pos, (char)c); */
        }

        _debug("One file end..");
        memset(memory, 0, sizeof(unsigned char) * ASIZE);
        _debug("After memset\n");
        fclose(file);
        _debug("Close file\n");
        --argc;
        ++p;
    }

    free(stack->head);
    free(stack);

    return 0;
}

