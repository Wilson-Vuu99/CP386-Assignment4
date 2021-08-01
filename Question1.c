#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>

bool sleeping;
int sleep_num;
int count;
int safe[5];
int CustCount = 5;
int MaxR[5][4]; //MaxR[i][j], first list ([i]) represents customer number, second list ([j]) represents resource number 
int Need[5][4];
int Alloc[5][4];
char FileResources[5][8];
int AvalR[4];

pthread_mutex_t lock;

int ReadFile(char *filename);
int InputReader(char* command);
void rq(char* c);
int rl (char * c);
void Status();
void Run();
void* Bankers(void* t);
//Customer* Customers = NULL;

typedef struct customer
{
        pthread_t Handle;
        int CustNum; //this is the number of the customer that the thread is representing. IE = Need[i], MaxR[i]
        int M[4];
        int N[4];
        int A[4];
        int returnVal;
        bool Flag;
        bool Complete;
        
} Customer;



int ReadFile(char *filename)
{
        
        FILE *in = fopen(filename, "r");

        struct stat st;
        fstat(fileno(in), &st);
        char *fileContent = (char *)malloc(((int)st.st_size + 1) * sizeof(char));
        fileContent[0] = '\0';
        while (!feof(in))
        {
                char line[100];
                if (fgets(line, 100, in) != NULL)
                {
                        strncat(fileContent, line, strlen(line));
                }
        }
        fclose(in);

        char *command = NULL;
        int threadCount = 0;
        char *fileCopy = (char *)malloc((strlen(fileContent) + 1) * sizeof(char));
        strcpy(fileCopy, fileContent);
        command = strtok(fileCopy, "\r\n");
        while (command != NULL)
        {
                threadCount++;
                command = strtok(NULL, "\r\n");
        }
        CustCount = threadCount;
        char *lines[threadCount];
        command = NULL;
        int i = 0;
        command = strtok(fileContent, "\r\n");
        while (command != NULL)
        {
                lines[i] = malloc(sizeof(command) * sizeof(char));
                strcpy(lines[i], command);
                i++;
                command = strtok(NULL, "\r\n");
        }

        for (int k = 0; k < threadCount; k++)
        {
                char *token = NULL;
                int j = 0;
                token = strtok(lines[k], ";");
                while (token != NULL)
                {
                        //printf("%s\n", token);

                        strcpy(FileResources[k], token);
                        Alloc[k][j] = 0;
                        j++;
                        token = strtok(NULL, " ");
                }
        }
        // Everything above was taken from A3 Q2 SkeletonCode for file reading and tokenization off of new lines
        // Everything below is tokenization off of commas

        char *token;
        for (int j = 0; j < CustCount; j++)
        {
                token = strtok(FileResources[j], ",");
                i = 0;
                while (token != NULL)
                {
                        MaxR[j][i] = atoi(token);
                        Need[j][i] = atoi(token);
                        token = strtok(NULL, ",");
                        i++;
                }
        }

        return threadCount; 
}


int main(int argc, char *argv[]){

        // ReadFile("sample4_in.txt");
        printf("Number of Clients: 5\n");
        printf("Currently Avaliable Resources: \n");
        //printf("");
        for (int i = 1; i < (argc ); i++)
        {
                
                //printf("First %s\n", argv[i]);
                int num = atoi(argv[i]);
                //printf("this is num convered %d\n",num);
                AvalR[i-1] = num;
                printf("%d ",AvalR[i-1]);


        }
        printf("\n");
        printf("Maximum Resources: \n");
         ReadFile("sample4_in.txt");

        for (int i = 0; i < 5; i++){
                for (int j = 0; j < 4; j++){
                        printf("%d ",MaxR[i][j]);
                        if(j == 3){
                                printf("\n");
                        }
                }
        }

        
        
        // int finish = 0;
        // while (finish == 0){
        //         printf("Enter Request: ");
        // }
        char cmand[20];
        char command[2];
        bool flag;

        while (flag == false){
                printf("Enter Command (Press 0 to Quit): ");
                fgets(cmand, 20, stdin);
                //printf("\n");
                flag = InputReader(cmand);

        }
        //cust = (Customer*) malloc(sizeof(Customer));
         

  
}

int InputReader(char* command){
    if (memcmp("RQ", command, 2) == 0){ //Requesting Resources
        rq(command);
    }
    else if (memcmp("RL", command, 2) == 0){ //Releasing Resources
        rl(command);
    }
    else if (memcmp("Status", command, 5) == 0){ 
        Status();
    }
    else if (memcmp("Run", command, 3) == 0){ 
        Run();
    }
    else if (memcmp("0", command, 1) == 0){ 
        exit(0);
    }

    else{
        printf("Pleae enter valid command \n");
    }
    fflush(stdin);
    return 0;
}




void rq(char* c){
        char rqStr[CustCount + 1][3]; // create array of character size of 1 (grabbing each number)
        char * token = strtok(c, " ");
        int i = 0;
        while( token != NULL ) {
                //printf( "token: %s\n", token ); //printing each token
                strcpy(rqStr[i],token);
                token = strtok(NULL, " ");
                i++;
        }
        //now convert to int arrays
        int rqR[CustCount-1];
        int customer = atoi(rqStr[1]);
        //printf("rqR[0] : %d\n", rqR[0]);
        //printf("CustCount: %d\n",CustCount);
        int x = 2;
        rqR[0] = atoi(rqStr[2]);
        //printf("rqR[] : %d\n", rqR[0]);
        rqR[1] = atoi(rqStr[3]);
        //printf("rqR[] : %d\n", rqR[1]);
        rqR[2] = atoi(rqStr[4]);
        //printf("rqR[] : %d\n", rqR[2]);
        rqR[3]= atoi(rqStr[5]);
        //printf("rqR[] : %d\n", rqR[3]);

        bool condition;
        condition = false;
        //printf("1ST: %d comapres %d\n", rqR[0], AvalR[0]);
        if (rqR[0] < AvalR[0]){
               //printf("1ST: %d comapres %d\n", rqR[0], AvalR[0]);
                if (rqR[1] < AvalR[1]){
                        //printf("2ND: %d comapres %d\n", rqR[1], AvalR[1]);
                        if (rqR[2] < AvalR[2]){
                                //printf("3RD: %d comapres %d\n", rqR[2], AvalR[2]);
                                if (rqR[3] < AvalR[3]){
                                        //printf("4th: %d comapres %d\n", rqR[3], AvalR[3]);
                                        condition = true; 
                                 }
                
                         }               
                
                }

        }
        if (condition){
                for(int i = 0; i < (CustCount-1); i ++ ){
                        AvalR[i] = AvalR[i] - rqR[i];
                        //printf("AvalR[i]: %d\n",AvalR[i]);
                }
                for(int i = 0; i < (CustCount-1); i ++ ){
                        Alloc[customer][i] = Alloc[customer][i] + rqR[i];
                        //printf("Alloc[i]: %d\n",Alloc[customer][i]);
                }
                for(int i = 0; i < (CustCount-1); i ++ ){
                        Need[customer][i] = Need[customer][i] - rqR[i];
                        //printf("Need[i]: %d\n",Need[customer][i]);
                }
                printf("State is safe, and request is satisfied\n");

        }
        else{
                printf("State is unsafe, and request is denied\n");    
        }

}


int rl(char * c){

        char rlStr[CustCount + 1][3]; // create array of character size of 1 (grabbing each number)
        char * token = strtok(c, " ");
        int i = 0;
        while( token != NULL ) {
                //printf( "token: %s\n", token ); //printing each token
                strcpy(rlStr[i],token);
                token = strtok(NULL, " ");
                i++;
        }
        //now convert to int arrays
        int rlR[CustCount-1];
        int customer = atoi(rlStr[1]);
        //printf("rlR[0] : %d\n", rlR[0]);
        //printf("CustCount: %d\n",CustCount);
        int x = 2;
        rlR[0] = atoi(rlStr[2]);
        //printf("rlR[] : %d\n", rlR[0]);
        rlR[1] = atoi(rlStr[3]);
        //printf("rlR[] : %d\n", rlR[1]);
        rlR[2] = atoi(rlStr[4]);
        //printf("rlR[] : %d\n", rlR[2]);
        rlR[3]= atoi(rlStr[5]);
        //printf("rlR[] : %d\n", rlR[3]);

        bool condition;
        condition = false;
        //printf("1ST: %d comapres %d\n", rlR[0], AvalR[0]);
        if (rlR[0] > Alloc[customer][0]){
               //printf("1ST: %d comapres %d\n", rlR[0], AvalR[0]);
                if (rlR[1] > Alloc[customer][1]){
                        //printf("2ND: %d comapres %d\n", rlR[1], AvalR[1]);
                        if (rlR[2] > Alloc[customer][2]){
                                //printf("3RD: %d comapres %d\n", rlR[2], AvalR[2]);
                                if (rlR[3] > Alloc[customer][3]){
                                        //printf("4th: %d comapres %d\n", rlR[3], AvalR[3]);
                                        condition = true; 
                                 }
                
                         }               
                
                }

        }
        if (condition){
                for(int i = 0; i < (CustCount-1); i ++ ){
                        AvalR[i] = AvalR[i] + rlR[i];
                        //printf("AvalR[i]: %d\n",AvalR[i]);
                }
                for(int i = 0; i < (CustCount-1); i ++ ){
                        Alloc[customer][i] = Alloc[customer][i] - rlR[i];
                        //printf("Alloc[i]: %d\n",Alloc[customer][i]);
                }
                for(int i = 0; i < (CustCount-1); i ++ ){
                        Need[customer][i] = Need[customer][i] + rlR[i];
                        //printf("Need[i]: %d\n",Need[customer][i]);
                }
                printf("State is safe, and request is satisfied\n");

        }
        else{
                printf("State is unsafe, and request is denied\n");    
        }       

}



void Status (){
        //Prints Available Resources Here <--------------------
        printf("Available Resources: \n");
        for (int i = 0; i < 4; i++){
                printf("%d ", AvalR[i]);
                if(i == 3){
                        printf("\n");
                }
        }
        //Prints Maximum Resources Here <--------------------
        printf("Maximum Resources: \n");
        for (int i = 0; i < 5; i++){
                for (int j = 0; j < 4; j++){
                        printf("%d ",MaxR[i][j]);
                        if(j == 3){
                                printf("\n");
                        }
                }
        }
        //Prints Allocated Resources Here <--------------------
        printf("Allocated Resources: \n");
        for (int i = 0; i < 5; i++){
                for (int j = 0; j < 4; j++){
                        printf("%d ",Alloc[i][j]);
                        if(j == 3){
                                printf("\n");
                        }
                }
        }
        //Prints Needed Resources Here <--------------------
        printf("Need Resources: \n");
        for (int i = 0; i < 5; i++){
                for (int j = 0; j < 4; j++){
                        printf("%d ",Need[i][j]);
                        if(j == 3){
                                printf("\n");
                        }
                }
        }              

}
void* Bankers(void *t){

        while (((Customer*)t) -> Complete == false){ // while the thread has not been completed (added it's number to the safe sequence).

                if (((Customer*)t) -> Flag == true){

                        ((Customer*)t) -> Flag = false; //reset it's flag before it goes to sleep, so when it wakes up it doesn't get stuck in a infinite loop

                        while (sleeping == true){ 
                                sleep_num ++; //number of asleep threads goes up 1
                                sleep(2);
                                if (sleep_num == (CustCount - count)){ //if the number of threads asleep is equal to the all of the currently incomplete threads
                                        sleeping = false; //wake them up!
                                }
                        }
                        sleep_num = 0;
                        //("Thread %d woke up! Will retry mutexlock.\n",((Customer*)t) -> CustNum );
                }       

                pthread_mutex_lock(&lock); //locks the mutex lock to not allow new threads access
                
                for (int i = 0; i < 4; i++){
                        if (((Customer*)t) -> N[i] > AvalR[i]){ // if it passes (aka the need is less than the available resources)
                                ((Customer*)t) -> Flag = true;
                                sleeping = true;
                                //printf("Thread %d was unsuccessful.\n", ((Customer*)t) -> CustNum);
                                break;
                        }
                        
                }

                if (((Customer*)t) -> Flag == false){ //If the customer is solvable, then we'll solve it
                        int cn = ((Customer*)t) -> CustNum;
                        for (int i = 0; i < 4; i++){
                                AvalR[i] = AvalR[i] + (((Customer*)t) -> A[i]);//add the customers alloc back to the available
                                (((Customer*)t) -> N[i]) = 0; //set the customers needed to zero
                                (((Customer*)t) -> A[i]) = 0;
                                
                        }

                        safe[count] = ((Customer*)t) -> CustNum; // add the customer number to the to the safe sequence
                        ((Customer*)t) -> Complete = true; // mark the thread as complete to avoid looping in this function again
                        count++; // count of completed threads goes up 1
                        sleeping = false; // no more sleeping for the threads who currently are
                        

                        //("Thread %d was successful.\n", ((Customer*)t) -> CustNum);
                        //("Value of count = %d\n", count);
                        
                
                }
                pthread_mutex_unlock(&lock); //unlocks the mutex lock to not allow new threads access
        }
}       

void Run(){
        
        Customer Thread[5];
        int AvalR_Clone[4];
        for (int i = 0; i < 4; i++){
                AvalR_Clone[i] = AvalR[i];
        }

        for (int i = 0; i < 5; i++){

                Thread[i].CustNum = i;
                Thread[i].Flag = false;

                for (int j = 0; j < 4; j ++){
                        Thread[i].M[j] = MaxR[i][j];
                        Thread[i].N[j] = Need[i][j];
                        Thread[i].A[j] = Alloc[i][j];
                }
                Thread[i].returnVal = pthread_create(&(Thread[i].Handle), NULL, Bankers, &Thread[i]);
        }
        for (int i = 0; i < 5; i++){ 
                pthread_join(Thread[i].Handle, NULL);

        }
        printf("Safe Sequence is: %d, %d, %d, %d, %d\n", safe[0], safe[1], safe[2], safe[3], safe[4]);
        for (int i = 0; i < 5; i++){
                int cn = safe[i];
                printf("--> Customer/Thread %d\n", cn);
                printf("    Allocated resources: %d %d %d %d\n",Alloc[cn][0], Alloc[cn][1], Alloc[cn][2], Alloc[cn][3]);
                printf("    Needed: %d %d %d %d\n", Need[cn][0], Need[cn][1], Need[cn][2], Need[cn][3]);
                printf("    Available: %d %d %d %d\n", AvalR_Clone[0], AvalR_Clone[1], AvalR_Clone[2], AvalR_Clone[3]);
                printf("    Thread has started\n");
                printf("    Thread has finished\n");
                printf("    Thread is releasing resources\n");
                AvalR_Clone[0] = AvalR_Clone[0] + Alloc[cn][0];
                AvalR_Clone[1] = AvalR_Clone[1] + Alloc[cn][1];
                AvalR_Clone[2] = AvalR_Clone[2] + Alloc[cn][2];
                AvalR_Clone[3] = AvalR_Clone[3] + Alloc[cn][3];
        
                printf("    New Available: %d %d %d %d\n", AvalR_Clone[0], AvalR_Clone[1], AvalR_Clone[2], AvalR_Clone[3]);
                Need[cn][i] = 0;
                Alloc[cn][i] = 0;
        }
}       
        
// IDK  ---------------------------------------------------------------------------------------------
        // int i = 0;   
	// for (int i = 0; i < (CustCount); i ++){
        //         int 
		
	// 	threads[i].Flag = pthread_create(, NULL, Bankers, &threads[i]);
        // pthread_t t1, t2, t3, t4, t5;
//------------------------------------------------------------------------------------------------------

        //if (bankers(thread[i] == true){
                //let the thread in to critical section semaphore lock queue
        //} else {
                //make it wait until it's needs can be met
        //}
        // -------------------- CRITICAL SECTION START----------------------







        // -------------------- CRITICAL SECTION END----------------------
        // int avail[3] = { 3, 3, 2 }; // Available Resources
        // int f[n], ans[n], ind = 0;
        // for (k = 0; k < n; k++) {
        //         f[k] = 0;
        // }
        // int need[n][m];
        // for (i = 0; i < n; i++) {
        //         for (j = 0; j < m; j++)
        //         need[i][j] = max[i][j] - alloc[i][j];
        // }
        // int y = 0;
        // for (k = 0; k < 5; k++) {
        //         for (i = 0; i < n; i++) {
        //         if (f[i] == 0) {
        
        //                 int flag = 0;
        //                 for (j = 0; j < m; j++) {
        //                 if (need[i][j] > avail[j]){
        //                         flag = 1;
        //                         break;
        //                 }
        //                 }
        
        //                 if (flag == 0) {
        //                 ans[ind++] = i;
        //                 for (y = 0; y < m; y++)
        //                         avail[y] += alloc[i][y];
        //                 f[i] = 1;
        //                 }
        //         }
        //         }
        // }
        
        // printf("Following is the SAFE Sequence\n");
        // for (i = 0; i < n - 1; i++)
        //         printf(" P%d ->", ans[i]);
        // printf(" P%d", ans[n - 1]);
        
        // return (0);
