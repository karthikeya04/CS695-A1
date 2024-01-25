#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t p = fork();
    if(p<0){
      perror("fork fail");
      exit(1);
    }
    if(p)
    {
        printf("[Parent] process_id(pid) = %d \n",getpid());
    }
    else
    {
        printf("[Child] process_id(pid) = %d \n",getpid());   
    }
    while(1) {;}
    return 0;
}
