#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LEN_BUFFER
#define LEN_BUFFER 10
#endif

typedef struct elem_of_vector_off_t
{
    off_t off;
    off_t len;
} elem_of_vector_off_t;

typedef struct vector_off_t
{
    elem_of_vector_off_t *elems;
    int cur;
    int cap;
} vector_off_t;

vector_off_t vector;

int initVector()
{
    vector.cur = 0;
    vector.cap = 20;
    vector.elems = malloc(sizeof(elem_of_vector_off_t) * 20);
    return vector.elems == NULL ? 1 : 0;
}

int addElem(off_t off, off_t len)
{
    if (vector.cap == vector.cur)
    {
        vector.cap *= 2;
        vector.elems = realloc(vector.elems, sizeof(elem_of_vector_off_t) * vector.cap);
        if (vector.elems == NULL)
        {
            return 1;
        }
    }
    vector.elems[vector.cur].len = len;
    vector.elems[vector.cur].off = off;
    vector.cur++;

    return 0;
}

int searchString(int num_of_line, int file)
{
    if (num_of_line > vector.cur)
    {
        puts("Num of line is too big");
        return 0;
    }
    char *string = calloc(vector.elems[num_of_line - 1].len + 1, sizeof(char));
    if (string == NULL)
    {
        return 1;
    }

    if (lseek(file, vector.elems[num_of_line - 1].off, SEEK_SET) == -1)
    {
        return 1;
    }

    if (read(file, string, vector.elems[num_of_line - 1].len) == -1)
    {
        return 1;
    }
    puts(string);
    free(string);

    return 0;
}

void freeVector()
{
    free(vector.elems);
    vector.cap = 0;
    vector.cur = 0;
}


void exitProgram(int Code, char* message){
    if (message != NULL){
        perror(message);
    }
    freeVector();
    exit(Code);
}

int main(int argc, char *argv[]) {
    vector.elems = NULL;
    if (argc < 2)
    {
        exitProgram(EXIT_FAILURE, "missing filename");
    }

    int file = open(argv[1], O_RDONLY);
    if (file == -1)
    {
        exitProgram(EXIT_FAILURE, "open failed");
    }

    if (initVector(&vector))
    {
        exitProgram(EXIT_FAILURE, "initVector failed");
    }


    char buffer[LEN_BUFFER];
    ssize_t sym_read;
    off_t cur_len = 0, cur_off = 0;
    while (1)
    {
        sym_read = read(file, buffer, LEN_BUFFER);
        if (sym_read == -1)
        {
            exitProgram(EXIT_FAILURE, "read failed");
        }
        if (sym_read == 0)
        {
            if (addElem(cur_off, cur_len))
            {
                exitProgram(EXIT_FAILURE, "addElem failed");
            }
            break;
        }

        for (ssize_t i = 0; i < sym_read; i++)
        {
            cur_len++;
            if (buffer[i] == '\n')
            {
                if (addElem(cur_off, cur_len))
                {
                    exitProgram(EXIT_FAILURE, "addElem failed");
                }
                cur_off += cur_len;
                cur_len = 0;
            }
        }
    }

    int num_of_line, res;
    while (1)
    {
        puts("Enter number of string (for end programm enter 0)");
        
        res = scanf("%d", &num_of_line);
        if (res == EOF){
            if (feof(stdin)) {
                fprintf(stderr, "EOF\n");
                exitProgram(EXIT_FAILURE, NULL);
            }
            else{
                exitProgram(EXIT_FAILURE, "scanf failed");
            }
        }
        else if (res == 0) {
            while(getc(stdin) != '\n'){
                if (feof(stdin)) {
                    fprintf(stderr, "EOF\n");
                    exitProgram(EXIT_FAILURE, NULL);
                }
                else{
                    exitProgram(EXIT_FAILURE, "getc failed");
                }
            }
            continue;
        }
        else if (num_of_line == 0){
            break;
        }
        if (searchString(num_of_line, file))
        {
            exitProgram(EXIT_FAILURE, "searchString failed");
        }
    }

    if (close(file)){
        perror("close failed");
    }
    exitProgram(EXIT_SUCCESS, NULL);
}