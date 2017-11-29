#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "daemonlibutil.h"
#include "globals_daemons_consts.h"
#include "client_consts.h"
#include "client.h"

int main(void){
    greet_user();
    int fifoFD  = initialize_fifo();
    char* next_request;
    next_request = wait_user_input(fifoFD);
}

void greet_user(){
    printf("%s Welcome to DaemonUserInfo Client (Version : %s)\n", CLIENT_HEADER, CLIENT_VERSION);
    printf("%s Available commands are 'proc {PID}'' 'useru {UID}' 'usern {UserName}' and 'exit'\n", CLIENT_HEADER);
    printf("%s please visit https://github.com/antoineguillory/DaemonUserProcInfo for other informations\n", CLIENT_HEADER);
}

int initialize_fifo(){
    int fifo_fd = mkfifo(FIFO_RQST_NAME, 0666);
    switch(fifo_fd){
        case -1:
            fprintf(stderr, "%s Fifo creation failed. Initialisation aborted.\n", CLIENT_HEADER);
            perror("Unknown FIFO");
            exit(EXIT_FAILURE);
        default:
            return fifo_fd;
    }
}

char* wait_user_input(int fifo_fd) {
    unsigned int usr_or_proc_id;
    char* user_name = malloc(256); //256 is the max size of a username...
    //We have to find a solution to reduce this buffer cause' it is vulnerable 
    //to buffer overflow...
    for (;;){
        char cmd[CMD_SIZE];
        printf("Command ?> ");
        scanf("%s",cmd);
        if ((strcmp(to_lower(cmd),"exit"))==0){
            close_client(fifo_fd);
        }   
        else if ((strcmp(to_lower(cmd),"proc"))==0){
            usr_or_proc_id=0;
            concat(cmd, ",");
            printf("PID ?>");
            scanf("%u",&usr_or_proc_id);
            if (usr_or_proc_id>PID_MAX){
                printf("Invalid PID value.\n");
                continue;
            }
            char str[6];
            sprintf(str, "%d", usr_or_proc_id);
            concat(cmd,str);
            concat(cmd,",");
            //TODO...
        } else if((strcmp(to_lower(cmd),"useru"))==0){
            concat(cmd, ",");
            char str[6];
            printf("UID?(type 'uid' or 'name')>");
            scanf("%u",&usr_or_proc_id);
            sprintf(str, "%d", usr_or_proc_id);
            concat(cmd,str);
            concat(cmd,",");
            //TODO...

        } else if((strcmp(to_lower(cmd),"usern"))==0){
            concat(cmd, ",");
            printf("Username ?>");
            scanf("%s",user_name);
            if( (strpbrk(user_name,"/\\")) == NULL){
                printf("Don't try to buff overflow my prgm pls\n");
                continue;
            }
            concat(cmd,user_name);
            concat(cmd,",");
            //TODO...
        }
    }
}

void close_client(int fifo_fd){
    int unlinkstate = unlink(fifo_fd);
    if(unlinkstate==-1){
        fprintf(stderr, "%s Fifo destruction failed. please look at /tmp/ to delete the unlinkable fifo\n", CLIENT_HEADER);
        exit(EXIT_FAILURE);
    }
    printf("Goodbye !\n");
    exit(EXIT_SUCCESS);
}