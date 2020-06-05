#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------#defined variables

//key parameters , N1 (female), N2 (male), N3 (both), M (places in waiting room) and CLIENT_NUM (how many clients should come):
#define N1 1
#define N2 1
#define N3 1
#define M 2


#define CLIENT_NUM 60

//other parameters, to be helpful with code execution
#define SEM_KEY 0x1231 //arbitrary key for semaphors
#define ALL_BARBERS N1 + N2 + N3

//single seat struct. It will be used both in waitroom as well as main barber seats
typedef struct {
bool taken;
int id;
bool male;
}seat;

//our Barbershop structure. M - waiting seats, main seats as much as barbers.
//clients_done is a counter to know when to end the simulation
typedef struct{
seat waitRoom[M];
seat barberSeats[ALL_BARBERS];
int clientsDone;
}BARBER_SHOP;

//function for the barber to find out what their profession is, or parent to get info
int getProfession(int id){
if(id<N1) return 0;
if(id<ALL_BARBERS-N3) return 1;
return 2;
}





//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//------------------------------main function - create necessary variables

int main(void) {
int barberId = 0, profession, clID = 0, semaphore_id;
bool barberSleeping = false;

srand(time(NULL));

if(N1==0 && N2==0 && N3==0){
perror("ERROR - You have to give any barbers to do the job!");
exit(-1);
}
if(CLIENT_NUM==0){
perror("ERROR - You have to give any clients to see results!");
exit(-1);
}

//var used for forking
pid_t forkStatus;

//working variables
int i = 0, j = 0;
bool ifNextClientMale = false;


struct sembuf p = {0, 1, SEM_UNDO}; //semaphore wait
struct sembuf v = {0, -1, SEM_UNDO}; //semaphore signal


//------------------------create shared memory array of seats and waitroom

//data for memory
key_t memKey; 
int shmid; //shared memory id
BARBER_SHOP *shop;

//create shared memory key
memKey = ftok("task5.c", 5204);
if(memKey<0){
	perror("MemKey error!");
	exit(1);
}

//get shared memory id 
//printf("memkey: %d\n", memKey);
shmid = shmget(memKey, sizeof(BARBER_SHOP), 0660|IPC_CREAT);
if(shmid<0){
	perror("shmget");
	exit(1);
}

//allocate enough space for the shop to be shared
shop = (BARBER_SHOP*) shmat(shmid, 0, 0);
if(shop==(BARBER_SHOP*)(-1)){
	perror("shmat");
	exit(1);
}


//initialize all data in BARBER_SHOP shop
for(i = 0; i<M; i++){
shop->waitRoom[i].taken = false;
shop->waitRoom[i].id = i;
shop->waitRoom[i].male = false;
}
for(i = 0; i<ALL_BARBERS; i++){
shop->barberSeats[i].taken = false;
shop->barberSeats[i].id = i;
shop->barberSeats[i].male = false;
//printf("Barber seat %d initialized.\n", shop->barberSeats[i].id);
}
shop->clientsDone = 0;
//printf("clientsDone = %d initialized.\n", shop->clientsDone);



//-----------------------------------------------------semaphore creation
i = 0;
semaphore_id = semget(SEM_KEY, ALL_BARBERS+1, 0644 | IPC_CREAT);
if(semaphore_id==-1){
	perror("Semaphore init failed!");
	exit(1);
}

//initialize semaphores
for(i=0;i<=ALL_BARBERS;i++){
semctl(semaphore_id, i, SETVAL, 1);
}

//-------------------------------------------------prompt with set values

printf("\n__________\n\nStarted Execution with parameters:\nN1 (female): %d, \nN2 (male): %d, \nN3 (both): %d, \nM (places): %d.\nBarber sum: %d\nClients to come: %d\n__________\n\n",N1, N2, N3, M, ALL_BARBERS, CLIENT_NUM);

sleep(1);



//----------------------------------------------main loop, create barbers
do{
forkStatus = fork();


if(forkStatus==0){
//------------------CHILD BEHAVIOUR

//get profession [0 - women, 1 - men, 2 - both]
profession = getProfession(barberId);
printf("Barber[%d]: Profession = %d\n", barberId, profession);

//set semaphore values for barber
p.sem_num = barberId;
v.sem_num = barberId;
barberSleeping= true;
j = 0;

struct sembuf roomAndSeat[2] = {
{barberId, -1, 0},
{ALL_BARBERS, -1, 0}
};

while(shop->clientsDone<CLIENT_NUM){

if(barberSleeping){

//sleep(1);
//TODO check your seat, if there is anyone, wake up!
semop(semaphore_id, &p, 1);

if(shop->barberSeats[barberId].taken != false){
clID = shop->barberSeats[barberId].id;
//printf("Barber[%d]: Client %d at seat!\n", barberId, clID);
barberSleeping = false;
}

semop(semaphore_id, &v, 1);
	
}else{

printf("Barber[%d]: Taking care of client %d!\n", barberId, clID);
sleep(1 + rand()%5);

//increment overall clients served
shop->clientsDone++;
printf("Barber[%d]: Client %d was served and went away!\n", barberId, clID);
j++;
//leave loop if all clients were served
if(shop->clientsDone>=CLIENT_NUM) break;

//raise semaphores for seat and waitroom
roomAndSeat[0].sem_op = -1;
roomAndSeat[1].sem_op = -1;
semop(semaphore_id, roomAndSeat, 2);



//look for client in waitingroom
for(i=0;i<M;i++){
if(shop->waitRoom[i].taken==false) continue;

if(profession==2 || shop->waitRoom[i].male==profession){
shop->barberSeats[barberId].id=shop->waitRoom[i].id;
clID = shop->waitRoom[i].id;
shop->barberSeats[barberId].male=shop->waitRoom[i].male;

shop->waitRoom[i].id = 0;
shop->waitRoom[i].taken = false;
shop->waitRoom[i].male = false;
break;
}
}


//if there is no one found for seat, then go to sleep
if(i==M){
shop->barberSeats[barberId].taken=false;
printf("Barber[%d]: No one at the barber seat, going to sleep!\n", 	barberId);
barberSleeping = true;
}

//lower semaphore
roomAndSeat[0].sem_op = 1;
roomAndSeat[1].sem_op = 1;
semop(semaphore_id, roomAndSeat, 2);




}
} //end of child lifetime loop


printf("Barber[%d]: Finished work, people serviced: %d!\n", barberId, j);
exit(0);

}else if (forkStatus>0){
//------------------PARENT BEHAVIOUR (in loop for creating children)

printf("shop[%d]: Barber %d successfuly created!\n",getpid(), barberId);
//sleep(2);


}else{
//IN CASE OF ERROR BEHAVIOUR
perror("Error creating child processes\n");
exit(1);
}

barberId++;
} while ((forkStatus != 0 && forkStatus != -1) && (barberId < ALL_BARBERS));




//-------------------------parent OUT OF LOOP, SUCCCESS CREATING CHILDREN
printf("shop[%d]: All the children successfuly created!\n", getpid());

sleep(2);

//now the program will start feeding clients in 1-4 second intervals (CLIENT_NUM times)
printf("shop[%d]: Feeding clients to shop!\n", getpid());


for(i=0;i<CLIENT_NUM;i++){
sleep(1 + rand()%4);

ifNextClientMale = (rand()%2);
if(N3==0){
if(ifNextClientMale==0 && N1==0) ifNextClientMale=1;
if(ifNextClientMale==1 && N2==0) ifNextClientMale=0;
}

printf("shop[%d]: New ", getpid());
if(ifNextClientMale) {printf("male ");} else {printf("female ");}
printf("client has come with id: %d!\n", i);

//first check barber seats
for(j=0;j<ALL_BARBERS;j++){

//set semaphore for barber seat value
p.sem_num = j;
v.sem_num = j;


semop(semaphore_id, &p, 1);

//if barber seat is empty...
if(shop->barberSeats[j].taken==false){
	
	//...and profession matches put client in seat
	if((getProfession(j) == ifNextClientMale) || getProfession(j)==2){
	printf("shop[%d]: Free place for %d found at seat %d!\n", getpid(), i, j);
	shop->barberSeats[j].taken=true;
	shop->barberSeats[j].id = i;
	shop->barberSeats[j].male = ifNextClientMale;
	semop(semaphore_id, &v, 1);
	//printf("semaphore lowered for : %d!\n", j);
	break;
	}else{
	semop(semaphore_id, &v, 1);
	//printf("semaphore lowered for : %d!\n", j);
	}
	
}
}

//if there was no free seat for client, look for waiting room
//semaphore of waiting room is the last, with index ALL_BARBERS (N1+N2+N3)
if(j==ALL_BARBERS){

	p.sem_num = ALL_BARBERS;
	v.sem_num = ALL_BARBERS;
	printf("shop[%d]: Client checks waitroom for place!\n", getpid());
	semop(semaphore_id, &p, 1);
	
	//check waiting room seats
	for(j=0;j<M;j++){
	
	//if free place found
	if(shop->waitRoom[j].taken==false){
		printf("shop[%d]: Free place for %d found at waiting room  seat %d!\n", getpid(),i, j);
		shop->waitRoom[j].taken=true;
		shop->waitRoom[j].id = i;
		shop->waitRoom[j].male = ifNextClientMale;
		break;
	}
	}
//free waiting room semaphore
semop(semaphore_id, &v, 1);

//nothing found - no free places
if(j==M){
printf("shop[%d]: No place found for client %d! Client went away!\n", getpid(), i);
	shop->clientsDone++;
	}
}
}

//after loop give prompt
printf("shop[%d]: No more clients to come!\n", getpid());


//------------------GATHER CHILDREN EXIT CODES
while(wait(NULL) != -1){
}


printf("shop[%d]: All the barbers finished work!\n", getpid());

//remove semaphores
if(semctl(semaphore_id, 0, IPC_RMID, 1)<0){
perror("Error seallocating semaphores!");
exit(-1);
}

//remove shared memory
shmdt (shop);
if(shmctl (shmid, IPC_RMID, 0)<0){
perror("Error seallocating memory!");
exit(-1);
}

  return 0;
}




