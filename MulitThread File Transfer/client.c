#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<pthread.h>
#include<netdb.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>


#define PORT 6669
#define IP "127.0.0.1"

int s;
//int many = 1;
//int trans;
int one = 1;

int sos = 0;
int once = 0;
int twice = 1;
int mut = 1;
int two = 1;
char fn[100];

void *sendfile(void *arg){
     FILE *fp;
    int len;
    int fsize;
    int bull;
    struct sockaddr_in addr;
    char mybuffer[4096];
    char *filename;
    char csize[10];
    filename = (char *)arg;

    fp = fopen(filename,"r");
    fseek(fp,0,SEEK_END);
    fsize = ftell(fp);
    printf("fsize:%d\n",fsize);
    sprintf(csize,"%d",fsize);
    printf("csize:%s\n",csize);
    fseek(fp,0,SEEK_SET);
    send(s,csize,10,0);
    //mybuffer = (char *)malloc(fsize * sizeof(char));
    while((len = fread(mybuffer,sizeof(char),4096,fp))>0){
              printf("len:%d\n",len);
              //many = fsize / 99;
              printf("%s\n",mybuffer);
              if(two == 1){
                     if(send(s,filename,100,0) < 0){
                            perror("filename");
                            exit(1);
                     } 
                     two = 0;
              }
    //read(STDIN_FILENO,mybuffer,sizeof(mybuffer));
              if(send(s,mybuffer,4096,0) < 0){
                       perror("send");
                       exit(1);
                       printf("sending successfully.\n");
              }
              bull = strlen(mybuffer);
              printf("bull:%d\n",bull);
              bzero(mybuffer,sizeof(mybuffer));
    }
    printf("%s\n",filename);
}

/*void *sendname(void *arg)
{
     char *filename;
     filename = (char *)arg;
     if(send(s,filename,100,0) < 0){
        perror("oh,filename");
        exit(1);
     }
}*/

void *readfile(void *arg){
    int i;
        //Arg_1 arg_3;
        int c;
        FILE *fd;
        //char filename[100];
        //bzero(filename,sizeof(filename));
        
        //struct timeval timeout;	
        //fd_set recvfds;
        int numbytes;
        int file;

        char buf[4096];
        int n;
        //arg_3 = *(Arg_1 *)arg;
        /*
        timeout.tv_sec = 2;
	timeout.tv_usec =0;*/
        int f = 1;
        char csize[10];
        int count;
        read(s,csize,10);
        count = atoi(csize);
        int yy;
        for(yy = 0;yy <= count/4096;yy++){
         printf("ob\n");
         if(one == 1){
              file = recv(s, fn, sizeof(fn),0);
              if(file < 0){
              perror("ohmygod");
              exit(1);
              }
              one = 0;
         }
        //while(f){        
              /* FD_ZERO(&recvfds);
               FD_SET(s_client.client_id[id], &recvfds);
	       switch(select(s_client.client_id[id] + 1, &recvfds, NULL, NULL, &timeout)){
	       case -1:	             
	               printf("Select error!\n");
		       flag=0;
	               break;
	       case 0:		
                       printf("fu\n");	
                       f = 0;		
	               break;//超时
	       default:*/
	             // if(FD_ISSET(s_client.client_id[id], &recvfds) > 0) {
                               /* if((n = read(s_client.client_id[id],filename,sizeof(filename))) == -1){
                                       perror("filename error\n");
                            }*/
                           // int n;
                            //for(n = 0;n<20;n++)

                                if((numbytes = read(s, buf, sizeof(buf))) == -1){
	                               perror("receive error\n");
	                               exit(1);
	                         }
                                 
                                 printf("%d\n",file);
	                        if(0 == numbytes){     //若客户端关闭,一关掉相应的客户端的套接字,二,关掉相应的发送线程,三,关掉本线程
	                               printf("server socket is closed!\n");
					exit(1);
	                        }
	                        
				else{
                                       
                                       mut = 1;
                                       printf("%s\n",fn);
	       	                       printf("data:%s", buf);  
	       	                       printf("len:%d\n\n", strlen(buf)-1); 

                                       fd = fopen(fn,"a");
                                       fseek(fd,0,SEEK_END);
                                       if(fd == NULL)
                                           printf("no files\n");
                                       c = fwrite(buf,sizeof(char),4096,fd);
                                       printf("%c\n",buf[0]);
                                       printf("%d\n",c);
                                       printf("receive successfully\n");
                                       fclose(fd);
				}
              }	          
			//} 
               sos = 1;
}

int main(){
    char filename[100];
    struct sockaddr_in addr;
    int a;
    int error;
    pthread_t psend,prev,sname;
    if((s = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("wrong");
        exit(1);
    }
    else{
        printf("%d\n",s);
    }
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(connect(s,&addr,sizeof(addr)) < 0){
        perror("connect error");
        exit(1);
    }
    else{
        printf("%s\n",IP);
        printf("%d\n",PORT);
    }

    while(1){
    printf("1.send file.\n");
    printf("2.receive file.\n");
    printf("3.exit\n");
    printf("please input the number of the selection.\n");
    scanf("%d",&a);
          //  printf("a\n");
    if(a == 1){
        printf("please input the filename.\n");
        scanf("%s",filename);
        //pthread_create(&sname,NULL,sendname,(void *)filename);
        //pthread_join(sname,NULL);
        //for(trans = 0;trans <= many;trans++){
             error = pthread_create(&psend,NULL,sendfile,(void *)filename);
             if(error != 0){
                  perror("pthread error");
                  exit(1);
             }
             pthread_join(psend,NULL);
        //}
            two = 1;
    }
    if(a == 2){
       // while(twice){
              if(mut){
                   mut = 0;
                   error = pthread_create(&prev,NULL,readfile,NULL);
                   sos = 0;
                   once = 1;
                   if(error != 0){
                   perror("pthread error");
                   exit(1);
                   }
                   pthread_join(prev,NULL);
               }
        // }
         one = 1;
    }
    if(a == 3)
        break;
    }
    return 0;
}
