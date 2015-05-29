#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <semaphore.h>
#include <assert.h>

struct client_t{
      int client_id[20];
      //pthread_t send[20];
      //pthread_t recv[20];
};

typedef struct a_arg
{
     int ff;
     char filename[100];
}Arg;

void *send_msg(void *arg);//接收数据线程函数
void *recv_msg(void *arg);//发送数据线程函数
//void *recv_file(void *arg);


struct client_t s_client;

int sos = 1;
int once = 0;
int twice = 1;
int one = 1;
int mut = 1;
char filename[100];
int many = 0;
int trans = 0;
int two = 1;

typedef struct worker
{
     void *(*process)(void *arg);
     void *arg;
     struct worker *next;
}CThread_worker;

typedef struct
{
     pthread_mutex_t queue_lock;
     pthread_cond_t queue_ready;
     CThread_worker *queue_head;
     int shutdown;
     pthread_t *threadid;
     int max_thread_num;
     int cur_queue_size;
}CThread_pool;

int pool_add_worker(void *(*process)(void *arg),void *arg);
void *thread_routine(void *arg);

static CThread_pool *pool = NULL;
void pool_init(int max_thread_num)
{
     pthread_attr_t attr;
     pthread_attr_init(&attr);
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

     pool = (CThread_pool*)malloc(sizeof(CThread_pool));
     pthread_mutex_init(&(pool->queue_lock),NULL);
     pthread_cond_init(&(pool->queue_ready),NULL);
     pool->queue_head = NULL;
     pool->max_thread_num = max_thread_num;
     pool->cur_queue_size = 0;
     pool->shutdown = 0;
     pool->threadid = (pthread_t*)malloc(max_thread_num *sizeof(pthread_t));
     int i = 0;
     for(i = 0;i < max_thread_num;i++)
     {
         pthread_create(&(pool->threadid[i]),&attr,thread_routine,NULL);
     }
}

int pool_add_worker(void *(*process)(void *arg),void *arg)
{
    CThread_worker *newworker = (CThread_worker *)malloc(sizeof(CThread_worker));
    newworker->process = process;
    newworker->arg = arg;
    newworker->next = NULL;
    pthread_mutex_lock(&(pool->queue_lock));
    CThread_worker *member = pool->queue_head;
    if(member != NULL)
    {
         while(member->next != NULL)
              member = member->next;
         member->next = newworker;
    }
    else
    {
         pool->queue_head = newworker;
    }
    
    assert(pool->queue_head !=NULL);

    pool->cur_queue_size++;
    pthread_mutex_unlock(&(pool->queue_lock));
    pthread_cond_signal(&(pool->queue_ready));

    return 0;
}

int pool_destroy()
{
    if(pool->shutdown)
         return -1;
    pool->shutdown = 1;
    pthread_cond_broadcast(&(pool->queue_ready));
    int i;
    for(i = 0;i<pool->max_thread_num;i++)
         pthread_join(pool->threadid[i],NULL);
    free(pool->threadid);
    CThread_worker *head = NULL;
    while(pool->queue_head != NULL)
    {
         head = pool->queue_head;
         pool->queue_head = pool->queue_head->next;
         free(head);
    }
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));
    free(pool);
    pool = NULL;
    return 0;
}

void *thread_routine(void *arg)
{
     printf("starting thread %d\n",pthread_self());
     while(1){
         pthread_mutex_lock(&(pool->queue_lock));
         while(pool->cur_queue_size == 0 && !pool->shutdown)
         {
              printf("thread %d is waiting\n",pthread_self());
              pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));
         }
         if(pool->shutdown){
              pthread_mutex_unlock(&(pool->queue_lock));
              printf("thread %d will exit\n",pthread_self());
              pthread_exit(NULL);
         }
     printf("thread %d is starting to work\n",pthread_self());
     assert(pool->cur_queue_size != 0);
     assert(pool->queue_head != NULL);
     pool->cur_queue_size--;
     CThread_worker *worker = pool->queue_head;
     pool->queue_head = worker->next;
     pthread_mutex_unlock(&(pool->queue_lock));
     (*(worker->process))(worker->arg);
     free(worker);
     worker = NULL;
     }
     pthread_exit(NULL);
}



void Init(){
    int i;
    for(i = 0;i < 20;i++){
        s_client.client_id[i] = -1;
    }
}

int get_id(){
        int i;
	for(i=0;i<20;i++){
		if(s_client.client_id[i]==-1)
			return i;
	}
	return -1;
}

int main(int argc,char *argv[]){
     int flag;
     int s;
     int port_reuse = 1;
     socklen_t len;
     struct sockaddr_in s_addr;
     struct sockaddr_in c_addr;
     Arg arg_1;
     int err;
     int judge;


     pool_init(5);
     if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("can not get a socket!\n");
		exit(1);
	}	
     s_addr.sin_family = AF_INET;
     s_addr.sin_port = htons(6669);	
     s_addr.sin_addr.s_addr = INADDR_ANY;
     memset(s_addr.sin_zero, '\0', sizeof(s_addr.sin_zero));

     if((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &port_reuse, sizeof(int))) == -1){
	       perror("addr port reuse error!\n");
	       exit(1);
	}
     /*bind*/
     if(bind(s, (struct sockaddr *)&s_addr, sizeof(struct sockaddr)) == -1){
		perror("bind error\n");
		exit(1);
      }     
	   
     /*listen*/
     if(listen(s, 20) == -1){
		perror("listen error\n");
		exit(1);
     }		
      printf("listening...\n");
      Init();
     
    // while(1){
          int a;
          int b;
          int e;
          len = sizeof(struct sockaddr);
          printf("1.connect the client.\n");
          printf("2.exit.\n");
          scanf("%d",&a);
          //a= 1;
      while(1){
          if(a==1){
            while(once);
            printf("Do you wanna to go on accepting y/n\n");
            printf("1.yes\n");
            printf("2.no\n");
            scanf("%d",&judge);
            if(judge == 1){
                  e = accept(s, (struct sockaddr *)&c_addr, &len);
                  if(e < 0){
                       perror("accept fault");
                       exit(1);
                  }
                  //while(!sos);
                  flag = get_id();
                  s_client.client_id[flag] = e;
              }
             // printf("%d\n",flag);
              printf("New connection from client[%d]\n",flag);
              printf("run successfully.\n");
              printf("1.send file.\n");
              printf("2.receive file.\n");
              scanf("%d",&b);
              mut = 1;
              twice = 1;
              if(b == 1){
                 printf("please input the client number you want to send to.\n");
                 int d;
                 scanf("%d",&d);
                 arg_1.ff = d;
                 printf("please input the filename you want to send\n");
                 scanf("%s",arg_1.filename);
                 //for(trans = -1;trans <= many;trans++)
                 pool_add_worker(send_msg,(void *)&arg_1);
                 two = 1;
              }
              else if(b == 2){
                 //arg_1.ff = flag;
                 //err = pthread_create(&s_client.recv[flag], &attr, recv_msg, (void *)&flag);
                 //pool_add_worker(recv_file,(void *)&arg_1);
                 //printf("%s\n",arg_1.filename);
                 //while(twice){
                     if(mut){
                        mut = 0; 
                        pool_add_worker(recv_msg,(void *)&flag);      
                        sos = 0;
                        once = 1;
                        //printf("twice:%d\n",twice);
                    // }
                     //printf("mutex:%d\n",mut);
                 }
                 //one = 1;
	         //if(0 != err){
	         //printf("can't create recv_msg thread!\n");
                 //exit(1);
		//}

              }
          }else
              break;
          printf("sos:%d\n",sos);
          //while(!sos);
          //flag = get_id();
     }
     return 0;
}

/*void *recv_file(void *arg)
{
        //char filename[100];
        Arg_1 arg_2;
        FILE *fp;
        //bzero(filename,sizeof(filename));
        int numbytes;
        //int id;
        arg_2 = *(Arg_1 *)arg;
        numbytes = read(s_client.client_id[arg_2.ff],arg_2.filename,sizeof(arg_2.filename));
        if(numbytes == -1){
            perror("file");
            exit(1);
        }
        if(numbytes == 0)
        {
                printf("client[%d] socket is closed!\n", arg_2.ff);
	        flag=arg_2.ff;
	        close(s_client.client_id[arg_2.ff]);
        }
        else
        {
                printf("this is file\n");
                printf("%s\n",arg_2.filename);
        }
}*/

/*recv thread*/
void *recv_msg(void *arg)
{       printf("bye\n");
        int id;
        //Arg_1 arg_3;
        int c;
        FILE *fd; 
        //char filename[100];
        //bzero(filename,sizeof(filename));
        
        //struct timeval timeout;	
        //fd_set recvfds;
        int numbytes;
        int file;
        char csize[10];

        char buf[4096];
        int numbuf;
        int n;
        int cnum;
        int count;
        id = *(int *)arg;
        printf("id:%d\n",id);
        //arg_3 = *(Arg_1 *)arg;
        /*
        timeout.tv_sec = 2;
	timeout.tv_usec =0;*/
        int same = 1;
        //int f = 1;
        int yy = 0;
        while(same){
             printf("^-^\n");
             cnum = read(s_client.client_id[id],csize,10);
             count = atoi(csize);
             printf("count:%d\n",count);
             for(yy = 0;yy <= count / 4096;yy++){
                      numbuf = strlen(buf);
                      printf("numbuf:%d\n",numbuf);
                      //f = 0;
                      if(one == 1){
                          file = read(s_client.client_id[id], filename, sizeof(filename));
                          if(file < 0){
                                perror("beiju");
                                exit(1);
                          }
                          one = 0;
                       }

                       if(((numbytes = read(s_client.client_id[id], buf, sizeof(buf))) == -1) && cnum != 0){
	                               perror("receive error\n");
	                               close(s_client.client_id[id]);
	                               exit(1);
	               }
                                 
                        printf("%d\n",file);
	                if(0 == cnum || numbytes == 0){     //若客户端关闭,一关掉相应的客户端的套接字,二,关掉相应的发送线程,三,关掉本线程
	                               printf("client[%d] socket is closed!\n", id);
					//flag=id;
	                               close(s_client.client_id[id]);
//					FD_CLR(g_client.sockid[client_id], &recvfds);
					s_client.client_id[id]=-1;
	                               //pthread_cancel(g_client.send_tid[client_id]);
	                               //pthread_exit(NULL);
					same=0;
					//break;
	                }
	                        
		        else{
                                       
                                       mut = 1;
                                       printf("%s\n",filename);
	       	                       printf("receive client[%d] data:%s", id, buf);  
	       	                       printf("receive client[%d] len:%d\n\n", id, strlen(buf)-1); 
                                       printf("numbytes:%d\n",numbytes);

                                       fd = fopen(filename,"a");
                                       fseek(fd,0,SEEK_END);
                                       if(fd == NULL)
                                           printf("no files\n");
                                       c = fwrite(buf,sizeof(char),numbytes,fd);
                                       printf("%d\n",c);
                                       printf("receive successfully\n");
                                       fclose(fd);
				}	          
			//} 
               sos = 1;
           }  
               one = 1;
               sos = 1;
               once = 0;
       }
}


/*send thread*/
void *send_msg(void *arg)
{	
    FILE *fp;
    Arg arg_2;
    int len;
    int fsize;
    struct sockaddr_in addr;
    char mybuffer[4096];
    char csize[10];
    arg_2 = *(Arg *)arg;

    fp = fopen(arg_2.filename,"r");
    fseek(fp,0,SEEK_END);
    fsize = ftell(fp);
    printf("fsize:%d",fsize);
    sprintf(csize,"%d",fsize);
    send(s_client.client_id[arg_2.ff],csize,10,0);
    if(two == 1){
        if(send(s_client.client_id[arg_2.ff],arg_2.filename,100,0) < 0){
          perror("filename");
          exit(1);
       } 
       two = 0;
    }
    fseek(fp,0,SEEK_SET);
    while((len = fread(mybuffer,sizeof(char),4096,fp))>0){
            //fseek(fp,trans * 100,SEEK_SET);
            //mybuffer = (char *)malloc(fsize * sizeof(char));
            //bzero(mybuffer,sizeof(mybuffer));
            //len = fread(mybuffer,sizeof(char),100,fp);
            //many = fsize / 99;

            printf("%s\n",mybuffer);
            printf("%d\n",s_client.client_id[arg_2.ff]);

   
    //read(STDIN_FILENO,mybuffer,sizeof(mybuffer));
           if(send(s_client.client_id[arg_2.ff],mybuffer,4096,0) < 0){
                    perror("send");
                    exit(1);
                    printf("sending successfully.\n");
           }
           bzero(mybuffer,sizeof(mybuffer));
    }
      once = 0;
      printf("len:%d\n",len);
    printf("%s\n",arg_2.filename);
}
