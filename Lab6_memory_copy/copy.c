#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void copy_read_write(int fd_from, int fd_to);
void copy_mmap(int fd_from, int fd_to);

int main (int argc, char **argv){
int calledAction = 0; // 0 - read_write, 1 - memcpy, 2 - help

//flags for program - if help true, help is shown
bool helpCalled = false;
bool useReadWrite = true;

//to check out option args
char c;

int fd_from, fd_to;

//gcheck options, set flags accordingly
while( (c = getopt(argc, argv, "hm")) != -1){

switch(c){
case 'm':{
useReadWrite = false;
break;
}
case 'h':{
helpCalled = true;
break;
}
default:{
perror("ERROR - improper argument!");
}
}//end switch
}//end while


if(helpCalled){
printf("\n\n--------------HELP--------------\n\n");
//printf("");
printf("Syntax:\ncopy [-m] <filename> <new_filename>     Use mmap and memcpy\ncopy <filename> <new_filename>     Use read and write\ncopy [-h]     Help (invokes this panel)\n\n--------------------------------\n\n");
}else{

//printf("\n\n--------------here should the program be--------------\n\n");

//if copy function was called, it has the arguments on optind and optind+1
//in int values we will have descriptors of files
fd_from = open(argv[optind], O_RDONLY);
if(fd_from==-1){
perror("ERROR - could not open file to copy from!!");
exit(-1);
}

//flag rdwr has to be here because of permission denied in mmap otherwise
fd_to = open(argv[optind+1], O_CREAT | O_RDWR | O_TRUNC, 0666);
if(fd_to==-1){
perror("ERROR - could not open file to copy to!!\n");
exit(-1);
}

//check what method to use
if(useReadWrite){
printf("\n\n-------------- Read-Write chosen--------------\n\n");
copy_read_write(fd_from,  fd_to);
printf("\n\n-------------- DONE--------------\n\n");
}else{
printf("\n\n-------------- memcpy chosen--------------\n\n");
copy_mmap(fd_from, fd_to);
printf("\n\n-------------- DONE--------------\n\n");
}

}


return 0;
}//-------------------------end main


//copying function using read and write with buffer 10000bytes
void copy_read_write(int fd_from, int fd_to){

size_t filesize;

char buffer[10000]; 

do{
filesize = read(fd_from, &buffer, 10000);
if(write(fd_to, &buffer, filesize) < 0){
perror("ERROR - writing file was impossible!!!");
exit(-1);
}
}while(filesize > 0);

if(filesize<0){
perror("ERROR - reading file was impossible!!!");
} 

}

//copying function using mmap and memcpy
void copy_mmap(int fd_from, int fd_to){
void *from_ptr, *to_ptr;
struct stat fileSize;

//get file size from fd_from using fstat
if(fstat(fd_from, &fileSize)<0){
perror("ERROR - couldn't get file statistics!!!");
exit(-1);
}

//set resulting file size to the same as input
ftruncate(fd_to, fileSize.st_size);

//map pointers for the copy
if( (from_ptr = mmap(NULL, fileSize.st_size, PROT_READ, MAP_SHARED, fd_from, 0)) == MAP_FAILED){
perror("ERROR - couldn't map file to copy from!!!");
exit(-1);
}else if( (to_ptr = mmap(NULL, fileSize.st_size, PROT_WRITE, MAP_SHARED, fd_to, 0)) == MAP_FAILED){
perror("ERROR - couldn't map file to copy to!!!");
exit(-1);
}

//use mmcpy 
if(memcpy(to_ptr, from_ptr, fileSize.st_size)==NULL){
perror("ERROR - couldn't copy the contents!!!");
}


}
