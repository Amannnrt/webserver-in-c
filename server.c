#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>


int main(){
  int sockfd;
  int portno = 9000;

  sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd <0){
    perror("socket");
    exit(-1);
  }
  struct sockaddr_in serv_addr;
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
    perror("bind");
    exit(-1);
  }

  if(listen(sockfd,5)<0){
    perror("listen");
    exit(-1);
  }

  char buffer[4096];
  while(1){
    int clientfd = accept(sockfd,NULL,NULL);
    if(clientfd <0){
      perror("accept");
      continue;
    }
    
    printf("client connected\n");
    int n = read(clientfd,buffer,sizeof(buffer)-1);
    if (n>0){
      buffer[n] = '\0';
      printf("%s\n",buffer);
    }

    FILE *fp = fopen("index.html","r");
    if(fp == NULL){
      perror("fopen");
      close(clientfd);
      continue;
    }
    char html[8192];
    int bytes = fread(html,1,sizeof(html)-1,fp);
    html[bytes] = '\0';
    fclose(fp);

    char header[256];
    snprintf(header,sizeof(header),"HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
              "Content-Length: %d\r\n"
              "\r\n",bytes);


    write(clientfd,header,strlen(header));
    write(clientfd,html,bytes);
    
    close(clientfd);
    sleep(1);
  }

  return 0;
}
