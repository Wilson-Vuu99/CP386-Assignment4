#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>


int CustCount = 5;
int MaxR[5][4];
int Need[5][4];
int Alloc[5][4];
char FileResources[5][8];

typedef struct Customer
{
    /* data */
    int Max[4];
    int Alloc[4];
    int Need[4];
};

int ReadFile (char* filename){


        FILE *in = fopen(filename, "r");

        struct stat st;
        fstat(fileno(in), &st);
        char* fileContent = (char*)malloc(((int)st.st_size+1)* sizeof(char));
        fileContent[0]='\0';    
        while(!feof(in))
        {
                char line[100];
                if(fgets(line,100,in)!=NULL)
                {
                        strncat(fileContent,line,strlen(line));
                }
        }
        fclose(in);

        char* command = NULL;
        int threadCount = 0;
        char* fileCopy = (char*)malloc((strlen(fileContent)+1)*sizeof(char));
        strcpy(fileCopy,fileContent);
        command = strtok(fileCopy,"\r\n");
        while(command!=NULL)
        {
                threadCount++;
                command = strtok(NULL,"\r\n");
        }

        char* lines[threadCount];
        command = NULL;
        int i=0;
        command = strtok(fileContent,"\r\n");
        while(command!=NULL)
        {
               lines[i] = malloc(sizeof(command)*sizeof(char));
                strcpy(lines[i],command);
                i++;
                command = strtok(NULL,"\r\n");
        }

        for(int k=0; k<threadCount; k++)
        {
                char* token = NULL;
                int j = 0;
                token =  strtok(lines[k],";");
                while(token!=NULL)
                {
                 printf("%s\n",token);

                        strcpy(FileResources[k], token);
                        Alloc[k][j] =  0;
                        j++;
                        token = strtok(NULL," ");

                }
        }
// Everything above was taken from A3 Q2 SkeletonCode for file reading and tokenization off of new lines
// Everything below is tokenization off of commas

        char *token;
        for(int j = 0; j < CustCount; j++){
                token = strtok(FileResources[j], ",");
                i = 0;
                while( token != NULL ) {
                        MaxR[j][i] = atoi(token);
                        Need[j][i] = atoi(token);
                        token = strtok(NULL, ",");
                        i++;
                }
        }
        return threadCount;
}

int main(int argc, char *argv[]) {
    
    ReadFile("sample4_in.txt");
    printf("Number of Clients: %d\n",CustCount);
    printf("Currently Avaliable Resources: ");
    for (int i = 1; i < (argc -1); i ++){
        printf("%s\n", argv[i]);
    }
    
}

