#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include<errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>


#define SERVER_PORT    6666
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

// 定义包的大小为512KB
#define PACK_SIZE 1024*512

//创建文件加命令：           mkdir /home/wjh                            命令+(文件夹路径)
//删除文件夹命令：           rmdir /home/wjh                            命令+(文件夹路径)
//文件上传到指定的文件夹：   sendto /homt/test.txt /home/wjh/test.txt   命令+上传文件+上传到的路径
//删除文件：                 deletefile /home/wjh                       命令+删除文件
//列举文件夹下的文件夹：     listdir /home                              命令+路径
//列举文件夹下的文件：       listdirfile /home                          命令+路径
//在某个文件夹下，查找文件： findfile /home wjh                         命令+路径+文件名


typedef struct cmdline
{
    char  cmd[256];
    unsigned long  filesize;
}cmdline;


//-----------------------------------splitString
int splitString(char * str, char *p[] )
{
    int in=0;
    char *buf=str;

    while((p[in]=strtok(buf,"*"))!=NULL)
    {
        buf=p[in];
        while((p[in]=strtok(buf," "))!=NULL)
        {
                 in++;
                 buf=NULL;
        }
        p[in++]="***"; //表现分割
        buf=NULL;
    }
}
//------------------------------------------------------readFile

unsigned long getFileSize(const char *path)
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        return filesize;
    }else{
        filesize = statbuff.st_size;
    }
    return filesize;
}

int readFile(char * fileName, char *buffer, unsigned long filesize)
{
    int pF;
    int bytes_read;

    if((pF=open(fileName, O_RDONLY))==-1)
    {
        fprintf(stderr,"Open %s Error：%s\n",fileName,strerror(errno));
        return -1;
    }

    while(bytes_read=read(pF,buffer,filesize))
    {
        if((bytes_read==-1)&&(errno!=EINTR))
            break;
        else if(bytes_read==filesize)
        {
            close(pF);
            return 0;
        }
    }

    close(pF);
    return -1;
}

char* getFileName(char* fn)
{
    int last = 0;
    char* pfn = fn+strlen(fn)-1;
    int i=0;
    for(i=0; i<strlen(fn); ++i)
    {
        if(*pfn-- == '/')
        {
            last = strlen(fn)-i;
            break;
        }
    }

    char* name = (char*)malloc(sizeof(char)*256);
    char* pname = name;
    int j=0;
    for(j=last; j<strlen(fn); ++j, ++pname)
    {
        *pname = fn[j];
    }

    return name;
}

int getLen(char * value[])
{
    int j;
    int i=0;
    for (j=0; j<6; j++)
    {
        if(strcmp(value[j],"***")==0)
        {
            i++;
            break;
        }
        else
        {
            i++;
        }
    }
    return i;
}
//------------------------------------------------------mian
int main(int argc, char **argv)
{
    char *CMDList[256];

    if (argc != 2)
    {
        printf("Usage: ./%s ServerIPAddress \n",argv[0]);
        exit(1);
    }

    printf("Client Start!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    struct sockaddr_in client_addr;
    bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);

    int client_socket = socket(AF_INET,SOCK_STREAM,0);
    if( client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }

    if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_aton(argv[1],&server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    server_addr.sin_port = htons(SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);

    if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n",argv[1]);
        exit(1);
    }

    char file_name[FILE_NAME_MAX_SIZE+1];
    char cmd[FILE_NAME_MAX_SIZE+1];
    bzero(file_name, FILE_NAME_MAX_SIZE+1);

    char buffer[BUFFER_SIZE];
    bzero(buffer,BUFFER_SIZE);

    int length = 0;

    while( 1)
    {
        cmdline buff;
loop:
        printf("Please Input send Server cmd:\n");
        fgets(cmd,1024,stdin);

        printf("%s \n", cmd);
        strcpy(buff.cmd, cmd);

        splitString(cmd, CMDList);

        if (strcmp(CMDList[0],"sendto")==0 && (getLen(&CMDList)-1)==3)
        {
            unsigned long len = getFileSize(CMDList[1]);
            printf("file size = %lu\n", len);
            buff.filesize=len;

            length = send(client_socket, &buff, sizeof(buff),0);

            if(length < 0)
            {
                printf("send Data to Server Failed!\n");
                continue;
            }

            printf("file path = %s\n", CMDList[1]);
            FILE* pf = fopen(CMDList[1], "rb");
            if(pf == NULL)
            {
                printf("open file failed!\n");
            }

            char pack[PACK_SIZE] = {'\0'};
            while((len = fread(pack, sizeof(char), PACK_SIZE, pf)) > 0)
                {
                system("clear");
                printf("%d \n",strlen(pack));
                printf("send data size = %d \t", len);
                length = send(client_socket, pack, len,0);
                bzero(pack,PACK_SIZE);
                }
            fclose(pf);
            printf("----------------------------------------------\n");
            goto loop;
        }
        else
        {
            buff.filesize=0;
            length = send(client_socket, &buff, sizeof(buff),0);
            if(length < 0)
            {
                printf("send Data to Server Failed!\n");
                continue;
            }
            length = recv(client_socket,buffer,BUFFER_SIZE,0);
            if(length < 0)
            {
                printf("Recieve Data From Server %s Failed!\n", argv[1]);
                break;
            }
            printf("___________________________________________\n");
            printf("get message: %s:\n",buffer);
            bzero(buffer,BUFFER_SIZE);
        }
        memset(&buff,0,sizeof(buff));
    }
    printf("Recieve File:\t %s From Server[%s] Finished\n",file_name, argv[1]);

    close(client_socket);
    return 0;
}
