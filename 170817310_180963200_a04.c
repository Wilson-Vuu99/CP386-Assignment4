#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>



int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
} 
}
