#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
 


//---------------------------------------------------------
//---------------------------------------------------------TSIG WITH SIGNALS
//---------------------------------------------------------
#ifdef WITH_SIGNALS
// to call gcc -D WITH_SIGNALS tsig.c


bool interrupt = false;


//------------------PARENT INTERRUPT
void handleInterrupt(){
    printf("\nparent[%d]: Detected interrupt!\n", getpid());
    interrupt = true;

}

//------------------CHILD HANDLING OF SIGTERM
void terminate(){
    printf("child[%d]: Received SIGTERM, terminating...\n", getpid());
    exit(0);
}

//------------------SEND SIGTERM TO ALL EXISTING CHILDREN
void sigtermCreated(int lastchild){
    for(;lastchild>getpid();lastchild--){
    printf("parent[%d]: Sending SIGTERM signal to child:  %d\n",getpid(), lastchild);
    kill (lastchild, SIGTERM);
}

}

//------------------MAIN FUNCTION
int main(void) {

int NUM_CHILD;
int i = 1;
pid_t forkStatus;
int lastchild = 0;
int waitval = 0;
int childProcesses = 0;

//------------------DISABLE SIGNALS FOR MAIN PROCESS
for(;i<NSIG;i++){
    signal(i, SIG_IGN);
}
signal(SIGCHLD, SIG_DFL);


//------------------ASK FOR NUM_CHILD
printf("Give a number of child processes you want to start: ");
scanf ("%d", &NUM_CHILD);


//------------------LOOP FOR PROCESSES
i=0;
do {
forkStatus = fork();

    if(forkStatus==0){
        //------------------CHILD BEHAVIOUR
        signal(SIGTERM, terminate);
        signal(SIGINT, SIG_IGN);

        printf("child[%d]: Parent process identifier = %d\n", getpid(), getppid());
        sleep(10);
        printf("child[%d]: Execution completion!\n", getpid()) ;
        exit(0);

    }else if (forkStatus>0){
        //------------------PARENT BEHAVIOUR
        signal(SIGINT, handleInterrupt);
        
        //------------------ID OF LAST CHILD
        lastchild = forkStatus;
        
        printf("parent[%d]: child[%d] successfuly created!\n", getpid(), lastchild);


        //------------------IF INTERRUPT FOUND, SIGTERM EXISTING CHILDREN
        if(interrupt){
        sleep(1);
        sigtermCreated(lastchild);
        interrupt = false;
        }

    }else{
        //------------------IN CASE OF ERROR BEHAVIOUR
        perror("Error creating child processes\n");
        sigtermCreated(lastchild);
        exit(1);
    }


sleep(1);
i++;
} while ((forkStatus != 0 && forkStatus != -1) && (i < NUM_CHILD));


//------------------OUT OF LOOP, SUCCESS CREATING CHILDREN
printf("parent[%d]: All the children successfuly created!\n", getpid());

//------------------GATHER EXIT CODES
while((waitval = wait(NULL)) != -1){
    childProcesses++;
}

printf("parent[%d]: No more child processes\n", getpid());
printf("parent[%d]: Received %d exit codes from child processes.\n", getpid(), childProcesses);


//------------------RESTORE SIGNAL HANDLERS
i=1;
for(;i<NSIG;i++){
    signal(i, SIG_DFL);
}

  return 0;
}


//---------------------------------------------------------
//---------------------------------------------------------TSIG WITHOUT SIGNALS
//---------------------------------------------------------
#else



//------------------SEND SIGTERM TO ALL EXISTING CHILDREN
void sigtermCreated(int lastchild){
    for(;lastchild>getpid();lastchild--){
    printf("parent[%d]: Sending SIGTERM signal to child:  %d\n",getpid(), lastchild);
    kill (lastchild, SIGTERM);
}

}

//------------------MAIN FUNCTION
int main(void) {

int NUM_CHILD;
int i = 0;
pid_t forkStatus;
int lastchild = 0;
int waitval = 0;
int childProcesses = 0;


printf("Give a number of child processes you want to start: ");

scanf ("%d", &NUM_CHILD);



do {
forkStatus = fork();


    if(forkStatus==0){
        //------------------CHILD BEHAVIOUR

        printf("child[%d]: Parent process identifier = %d\n", getpid(), getppid());
        sleep(10);
        printf("child[%d]: Execution completion!\n", getpid()) ;
        exit(0);

    }else if (forkStatus>0){
        //------------------PARENT BEHAVIOUR

        lastchild = forkStatus;

        printf("parent[%d]: child[%d] successfuly created!\n", getpid(), lastchild);


    }else{
        //------------------IN CASE OF ERROR BEHAVIOUR
        perror("Error creating child processes\n");
        sigtermCreated(lastchild);
        exit(1);
    }


sleep(1);
i++;
} while ((forkStatus != 0 && forkStatus != -1) && (i < NUM_CHILD));


//------------------OUT OF LOOP, SUCCCESS CREATING CHILDREN
printf("parent[%d]: All the children successfuly created!\n", getpid());


//------------------GATHER CHILDREN EXIT CODES
while((waitval = wait(NULL)) != -1){
    childProcesses++;
}


printf("parent[%d]: No more child processes\n", getpid());
printf("parent[%d]: Received %d exit codes from child processes.\n", getpid(), childProcesses);

return 0;


}

#endif