#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void print_text(char* name) {
    printf("text1 %s\n", name);
    printf("text2 %s\n", name);
    printf("text3 %s\n", name);
    printf("text4 %s\n", name);
    printf("text5 %s\n", name);
    printf("text6 %s\n", name);
    printf("text7 %s\n", name);
    printf("text8 %s\n", name);
    printf("text9 %s\n", name);
    printf("text10 %s\n", name);
}

void* thread_func(void* param) {
    print_text("child");
    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    int code = 0;
    if ((code = pthread_create(&thread, NULL, thread_func, NULL)) != 0) {
        fprintf(stderr, "pthread_create error: %d", code);
        exit(EXIT_FAILURE);
    }
    print_text("parent");

    pthread_exit(NULL);
}