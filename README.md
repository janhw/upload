upload
======
Implement a program (server) that has minimum functions of a NFS
server. Also implement a program (client) that can communicate with
the server. In detail, the functions that need to be implemented are
as follows:

1. The server listens on a TCP port to accept client connections
2. clients can send the following commands to the server
        a. create a directoy
        b. delete a directory
        c. create a file under a specified directory, and put the
content of the file to the server
        d. delete a file under a specified directory
        e. list files and sub-directories under a specified directory
        f. retrieve a file from the server

Bonus program:
implement a program that is similar to unix shell. The program can
accept user inputs, verify these inputs, translate them to file system
commands, send to the server and finally display the server's
response.

upload

客户端的命令格式：

//创建文件加命令：           mkdir /home/wjh                            命令+(文件夹路径)
//删除文件夹命令：           rmdir /home/wjh                            命令+(文件夹路径)
//文件上传到指定的文件夹：   sendto /homt/test.txt /home/wjh/test.txt   命令+上传文件+上传到的路径
//删除文件：                 deletefile /home/wjh                       命令+删除文件
//列举文件夹下的文件夹：     listdir /home                              命令+路径
//列举文件夹下的文件：       listdirfile /home                          命令+路径
//在某个文件夹下，查找文件： findfile /home wjh                         命令+路径+文件名

