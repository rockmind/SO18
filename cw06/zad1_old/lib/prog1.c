#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
  
int main() 
{ 
    int fd; 
  
    // FIFO file path 
    char * myfifo = "myfifo"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    mkfifo(myfifo, 0666); 
  
        fd = open(myfifo, O_WRONLY); 
        write(fd, "aaa", strlen("aaa")+1); 
        close(fd); 
    return 0; 
} 
