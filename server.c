#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include<errno.h>
#include<dirent.h>
#include <sys/stat.h>

#define HELLO_WORLD_SERVER_PORT    6666
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

// 定义包的大小为512KB
#define PACK_SIZE 1024*512

typedef struct cmdline
{
    char  cmd[256];
    unsigned long  filesize;
}cmdline;

//--------------------------------------splitString
int splitString(char * str, char *p[] )
{
    int in=0;
    char *buf=str;

    while((p[in]=strtok(buf,"\n"))!=NULL)
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
//-----------------------------------------------------createDir

int createDir(const char *sPathName)
{
    char   DirName[256];
    strcpy(DirName,   sPathName);
    int   i,len   =   strlen(DirName);

    if(DirName[len-1]!='/')
       strcat(DirName,   "/");

    len   =   strlen(DirName);

    for(i=1;   i<len;   i++)
    {
        if(DirName[i]=='/')
        {
            DirName[i]   =   0;
            if(access(DirName, NULL)!=0   )
            {
                  if(mkdir(DirName, 777)==-1)
                  {
                      perror("mkdir   error");
                      return   -1;
                  }
            }
            DirName[i]   =   '/';
        }
    }
    return   0;
}

int deleteDir(char *path)
{

    DIR *dirp;
    dirp = opendir(path);

    if (dirp == NULL)
    {
        (void)fprintf(stderr, "opendir(): %s: %s\n", path, strerror(errno));
        return -1;
    }

    if(rmdir(path)==0)
    {
        printf("deleted the directory %s.\n",path);\
        return 0;
    }
    else
    {
        printf("cant't delete the directory %s.\n",path);
        printf("errno : %d\n",errno);
        printf("ERR  : %s\n",strerror(errno));
    }
    return -1;
}
//------------------------------------------------------------------
int isDir(const char *path)
{
    struct stat statbuf;
    if(lstat(path, &statbuf) ==0)
    {
        return S_ISDIR(statbuf.st_mode) != 0;
    }
    return 0;
}


int isFile(const char *path)
{
    struct stat statbuf;
    if(lstat(path, &statbuf) ==0)
        return S_ISREG(statbuf.st_mode) != 0;
    return 0;
}

int isSpecialDir(const char *path)
{
    return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;
}

void getFilePath(const char *path, const char *file_name,  char *file_path)
{
    strcpy(file_path, path);
    if(file_path[strlen(path) - 1] != '/')
        strcat(file_path, "/");
    strcat(file_path, file_name);
}

int deleteFile(const char *path)
{
    DIR *dir;
    struct dirent *dir_info;
    char file_path[PATH_MAX];
    if(isFile(path))
    {
        remove(path);
        return 0;
    }
    if(isDir(path))
    {
        if((dir = opendir(path)) == NULL)
            return -1;
        while((dir_info = readdir(dir)) != NULL)
        {
            getFilePath(path, dir_info->d_name, file_path);
            if(isSpecialDir(dir_info->d_name))
                continue;
            deleteFile(file_path);
            rmdir(file_path);
        }
    }
    return 0;
}
//--------------------------------------listDir
int listDir(char*name, char * lst)
{
    static char dot[] =".", dotdot[] ="..";

    DIR *dirp;
    struct dirent *dp;

    dirp = opendir(name);

    if (dirp == NULL)
    {
        (void)fprintf(stderr, "opendir(): %s: %s\n", name, strerror(errno));
        return -1;
    }

    while ((dp = readdir(dirp)) != NULL)
    {
        if (dp->d_type == DT_DIR)
        {
            if ( strcmp(dp->d_name, dot) && strcmp(dp->d_name, dotdot) )
            {
                strcat(lst,"\n");
                strcat(lst,dp->d_name);
            }
        }
    }
    (void)closedir(dirp);

    return 0;
}
//--------------------------------------analysisCMD
int listDirFile(char*name, char * lst)
{
    DIR *dirp;
    struct dirent *dp;

    dirp = opendir(name);

    if (dirp == NULL)
    {
        (void)fprintf(stderr, "opendir(): %s: %s\n", name, strerror(errno));
        return -1;
    }

    while ((dp = readdir(dirp)) != NULL)
    {
        if(dp->d_type == DT_REG)
        {
            strcat(lst,"\n");
            strcat(lst,dp->d_name);
        }
    }

    (void)closedir(dirp);

    return 0;
}
//--------------------------------------findFile
int findFile(char * name, char * path, char *fondFinePath)
{
    DIR *dirp;
    struct dirent *dp;
    char * d = "/";
    static char dot[] =".", dotdot[] ="..";

    char tmp[256] = "";

    strcpy(tmp, path);
    printf("   %s\n ", tmp);
    printf("   %s\n ", path);
    dirp = opendir(tmp);


    if (dirp == NULL)
    {
        (void)fprintf(stderr, "opendir(): %s: %s\n", path, strerror(errno));
        return -1;
    }

    while ((dp = readdir(dirp)) != NULL)
    {
        if(dp->d_type == DT_REG)
        {
            if(strcmp(name, &(dp->d_name)) == 0)
            {
                printf("OK!\n");
                strcpy(fondFinePath,tmp);
                strcat(fondFinePath, d);
                strcat(fondFinePath, &(dp->d_name));
                return 0;
            }
        }
        if (dp->d_type == DT_DIR)
        {
            if ( strcmp(dp->d_name, dot) && strcmp(dp->d_name, dotdot) )
            {
                strcat(tmp, d);
                strcat(tmp, &(dp->d_name));
                if(findFile(name, tmp, fondFinePath)==0)
                {
                    strcpy(tmp, path);
                    return 0;
                }
                strcpy(tmp, path);
            }
        }
    }

    (void)closedir(dirp);
    return -1;
}

//--------------------------------------analysisCMD
int analysisCMD(char *cmd[], char * bakstrng, char *lst)
{
    if (strcmp(cmd[0],"mkdir")==0)
    {
        printf("cmd[0] : %s\n", cmd[0]);
        printf("cmd[1] : %s\n", cmd[1]);
        if(createDir(cmd[1]) == -1)
        {
            strcpy(bakstrng,"mkdir fail!");
            return -1;
        }
        strcpy(bakstrng,"mkdir success!");
        return 0;
    }
    else if (strcmp(cmd[0],"rmdir")==0)
    {
        if(deleteDir(cmd[1]) == -1)
        {
            strcpy(bakstrng,"rmdir fail!");
            return -1;
        }
        strcpy(bakstrng,"rmdir success!");
        return 0;
    }
    else if (strcmp(cmd[0],"deletefile")==0)
    {
        if(deleteFile(cmd[1]) == -1)
        {
            strcpy(bakstrng,"deletefile fail!");
            return -1;
        }
        strcpy(bakstrng,"deletefile success!");
        return 0;
    }
    else if (strcmp(cmd[0],"listdir")==0)
    {
        if(listDir(cmd[1], lst) == -1)
        {
            strcpy(bakstrng,"listdir fail!");
            return -1;
        }
        strcpy(bakstrng,"listdir success!");
        return 0;
    }
    else if (strcmp(cmd[0],"listdirfile")==0)
    {
        if(listDirFile(cmd[1], lst) == -1)
        {
            strcpy(bakstrng,"listdirfile fail!");
            return -1;
        }
        strcpy(bakstrng,"listdirfile success!");
        return 0;
    }
    else if (strcmp(cmd[0],"findfile")==0)
    {
        if(findFile(cmd[2], cmd[1], lst) == -1)
        {
            strcpy(bakstrng,"findfile fail!");
            return -1;
        }
        strcpy(bakstrng,"findfile success!");
        return 0;
    }
}
//--------------------------------------------------getlen
int getlen(char * value[])
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
//--------------------------------------main
int main(int argc, char **argv)
{
    printf("Server Start!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    char *CMDList[256];
    char bakstr[256];

    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);

    int server_socket = socket(PF_INET,SOCK_STREAM,0);
    if( server_socket < 0)
    {
        printf("Create Socket Failed!");
        exit(1);
    }
{
   int opt =1;
   setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}

    if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        printf("Server Bind Port : %d Failed!", HELLO_WORLD_SERVER_PORT);
        exit(1);
    }

    if ( listen(server_socket, LENGTH_OF_LISTEN_QUEUE) )
    {
        printf("Server Listen Failed!");
        exit(1);
    }
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);

        int new_server_socket = accept(server_socket,(struct sockaddr*)&client_addr,&length);
        if ( new_server_socket < 0)
        {
            printf("Server Accept Failed!\n");
            break;
        }

        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);

        char cmd[FILE_NAME_MAX_SIZE+1];
        bzero(cmd, FILE_NAME_MAX_SIZE+1);

        bzero(buffer, BUFFER_SIZE);
        int file_block_length = 0;

        unsigned long file_len  = 0;
        int fd;

        while(1)
        {
            cmdline cmdbuff;
            memset(&cmdbuff,0,sizeof(cmdbuff));
            char *str=(char *)malloc(1024);

            if((file_block_length = recv(new_server_socket,&cmdbuff,sizeof(cmdbuff),0))>0)
            {
                printf("%s\n",cmdbuff.cmd);
                splitString(cmdbuff.cmd, &CMDList);
                analysisCMD(&CMDList, bakstr, str);
                if (strcmp(CMDList[0],"sendto")==0 && (getlen(&CMDList)-1)==3)
               {
                    printf("Received a message: cmd=%s filesize=%d\n", cmdbuff.cmd,cmdbuff.filesize);

                    if((fd=open(CMDList[2],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR))==-1)
                    {
                        fprintf(stderr,"Open %s Error：%s\n",CMDList[2],strerror(errno));
                        close(new_server_socket);
                        continue;
                    }

                    char buff[PACK_SIZE] = {'\0'};
                    unsigned long  filesize = cmdbuff.filesize;
                    unsigned long  read_size = 0;

                    while(read_size < filesize)
                    {
                        int rlen = recv(new_server_socket, buff, PACK_SIZE,0);
                        if(rlen)
                        {
                            system("clear");
                            printf("\n\nRead package size = %d\n", rlen);

                            int wn=write(fd,buff,rlen);
                            read_size += rlen;

                            printf("write file size = %d\n", wn);
                            printf("Read  total  size = %d\n", read_size);
                        }
                        else
                        {
                            printf("Read over!...%d\n", rlen);
                            free(str);
                            break;
                        }
                    }
                    printf("File len = %ld ... Already read size = %ld\n", file_len, read_size);
                    close(fd);
                    free(str);
                }
                else
                {
                    strcat(bakstr,"\n");
                    strcat(bakstr, str);
                    if(send(new_server_socket,bakstr,strlen(bakstr),0)<0)
                    {
                        break;
                    }
                }
            }
            memset(&cmdbuff,0,sizeof(cmdbuff));
            bzero(str,strlen(str));
            free(str);
        }
        close(new_server_socket);
    }
    close(server_socket);
    return 0;
}
