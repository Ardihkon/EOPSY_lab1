#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define NUM_DISHES 5 //number of dishes before leaving the table

#define N 5 //number of philosophers
#define LEFT ( i + N - 1 ) % N   //id of left fork
#define RIGHT ( i + 1 ) % N   //id of right fork
#define THINKING 0   //state thinking
#define HUNGRY 1  //state hungry
#define EATING 2  //state eating
#define up pthread_mutex_unlock  //unlock command for mutex
#define down pthread_mutex_lock  //lock command for mutex
#define mutex pthread_mutex_t   //name convention for mutex


int state[N]; //will be initiated to THINKING's 
mutex s[N]; //mutexes for each philosopher
mutex m; //mutex for state table of philosophers
pthread_t threads[N]; //thread table for individual threads for our philosophers

//definition of the function necessary now, declaration below
void test(int i);

//function to set state to HUNGRY and wait for available forks on both sides to eat
void grab_forks(int i){

   //lock critical state table
   down(&m);
   state[i] = HUNGRY;

   //test if possible to eat
   test(i);

   //release critical state table
   up(&m);

   //if philosopher can eat (his lock is disabled) the lock is enabled again for eating
   down(&s[i]);
}

//put down forks, set state to THINKING and let other philosophers on both sides start eating if they are waiting
void put_away_forks(int i){

   //lock critical state table
   down(&m);
   state[i] = THINKING;

   //test philosophers on the sides, if they can start eating
   test(LEFT);
   test(RIGHT);

   //release critical state table
   up(&m); 
}


//test function - if philosopher is HUNGRY and can start eating (forks on both sides are free), he does have his lock released
void test(int i){  
   if( state[i] == HUNGRY   && state[LEFT] != EATING   && state[RIGHT] != EATING )  {
      state[i] = EATING;   
      up(&s[i]); 
    }
 }


//--------------------------------PHILOSOPHER CYCLE
//Since state was started with THINKING, the thinking is the first to begin
//philosophers eat up to NUM_DISHES meals - defined in #define section
void *philosopher_cycle(void* passed_id){

   //initialize id and dishes left to eat 
   int phil_id = *(int*) passed_id;
   int dishesLeft = NUM_DISHES;
   sleep(1);

   printf("Philosopher %d created! \n", phil_id);

   //cycle - thinking for 1second  and eating for 2 seconds (for visible synchronisation of threads)
   while(dishesLeft>0){
      printf("Philosopher %d is thinking! \n", phil_id);
      sleep(1);

      //grab_forks function will end when philosopher was free and lock was enabled for eating
      grab_forks(phil_id);
      printf("Philosopher %d started eating!! \n", phil_id);
      sleep(2);

      //set state t thinking, allow other philosophers on the sides to eat
      put_away_forks(phil_id);

      //decrease counter of dishes
      dishesLeft--;
   }

   //exit after finishing 5 meals 
   printf("Philosopher %d finished his role!! \n", phil_id);
   pthread_exit(NULL);
}


//--------------------------------MAIN FUNCTION
int main(){

   //counter value
   int i = 0;

   //pointer (for correct passing of id to our philosophers)
   int* value;

   //initialize individual mutex (responsible for state table)
   if(pthread_mutex_init(&m, NULL)!=0 ){
      perror("ERROR - not able to initialize mutex!!!");
      exit(-1);
   }

   //lock table of states (will be modified while creating the philosophers)
   down(&m);

   printf("Created philosophers of id: ");

   //create philosophers loop
   for(i=0;i<N;i++){
      printf("  %d", i);

      //set the state of philosopher to THINKING
      state[i] = THINKING;

      //create another value to pass correctly id to philosopher
      value = malloc(sizeof(value));

      //initialize mutex and value for id. down command on mutex of philosopher prevents them from all starting to eat at the same time (test command will release mutex when possible)
      pthread_mutex_init(&s[i], NULL);
      down(&s[i]);
      *value = i;

      //call to create thread with philosopher id  
      if(pthread_create(&threads[i], NULL, philosopher_cycle, value)!=0 ){
         perror("ERROR - not able to create philosopher!!!");
         exit(-1);
      }
   }

   //after done with creating philosophers, release lock on state table
   up(&m);
   printf(".\n");

   //gather exit messages of individual threads
   for(i=0;i<N;i++){
      if(pthread_join(threads[i], NULL)!=0 ){
         perror("ERROR - error removing threads!!!");
         exit(-1);
      }
   }

   printf("All threads finished operation!\n");
   return 0;
}
