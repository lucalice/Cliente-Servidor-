#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>

/* the port users will be connecting to */
#define MYPORT 3490

/* how many pending connections queue will hold */
#define BACKLOG 10
#define MAX 50

int comando (char str[MAX]) {
  int pid;
  int ban = 1;
  char strDos[MAX];
  char *token;
  char aux;
  char s[2] = " ";
  int sockfd;

  
  if (memcmp(str,"exit",4) == 0){ //comparando los primeros 4 caracteres
    exit(100);
  }

  pid = fork();
  //Este es el hijo
  if (pid == 0){
      int j = 0;
      char *comandos[MAX];
      int iBan = 0;
      char complemento[MAX];
      int z = 0;
      //BUSCAREMOS EL |
      for (int i = 0; i < strlen(str); ++i){
        if (str[i] =='|'){
            s[0] = '|';
            iBan = 1;
        }
        if ((iBan == 1) && (str[i] != '|')){
            //Recuperando el comando que hay despues de |
            complemento[z] = str[i]; 
            z++;
        }
      }
      //DIVIDIENDO LA CADENA DE ENTRADA
      token = strtok(str,s); //Obtenemos el primer valor
      while(token != NULL ){
        comandos[j] = strdup(token);
        token = strtok(NULL, s);
        j++;
      }
      comandos[j] = token; //comandos en la ultima posicion = NULL
      if (iBan == 1){
        int pidh;
        int iConexion[2]; //canal de comunicación
        pipe(iConexion); //creamos el pipe
        pidh = fork();
        if (pidh){//hijo
            char *comandosHijo[MAX];
            char *tokenDos;
            s[0] = ' '; 
            int j = 0;
            tokenDos = strtok(complemento,s); //Obtenemos el primer valor
            while(tokenDos != NULL ){
              comandosHijo[j] = strdup(tokenDos);
              tokenDos = strtok(NULL, s);
              j++;
            }
            comandosHijo[j] = tokenDos; //comandos en la ultima posicion = NULL
            close(0);
            close(iConexion[1]);
            dup(iConexion[0]);
            execvp(comandosHijo[0],comandosHijo);
            return 0;
        }else{ //nieto
            char *comandosNieto[MAX];
            char *tokenTres;
            s[0] = ' '; 
            int j = 0;
            tokenTres = strtok(str,s); //Obtenemos el primer valor
            while(tokenTres != NULL ){
              comandosNieto[j] = strdup(tokenTres);
              tokenTres = strtok(NULL, s);
              j++;
            }
            comandosNieto[j] = tokenTres; //comandos en la ultima posicion = NULL
            close(1);
            close(iConexion[0]);
            dup(iConexion[1]);
            execvp(comandosNieto[0],comandosNieto);
            return 0;
        }
      }else{
        execvp(comandos[0],comandos);
        return 0;
      }
  }
  wait(&ban);
  return(0);
}

int main(int argc, char *argv[ ]){
  /* listen on sock_fd, new connection on new_fd */
  int sockfd, new_fd, numbytes;

  /* my address information */
  struct sockaddr_in my_addr;

  /* connectors address information */
  struct sockaddr_in their_addr;
  int sin_size;
  // max number of bytes we can get at once
  #define MAXDATASIZE 300
  char buf[MAXDATASIZE];

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Server-socket() error lol!");
    exit(1);
  }
  else
    printf("Server-socket() sockfd is OK...\n");

  /* host byte order */
  my_addr.sin_family = AF_INET;

  /* short, network byte order */
  my_addr.sin_port = htons(MYPORT);

  /* automatically fill with my IP */
  my_addr.sin_addr.s_addr = INADDR_ANY;
  printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);

  /* zero the rest of the struct */memset(&(my_addr.sin_zero), '\0', 8);
  if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
    perror("Server-bind() error");
    exit(1);
  }
  else
    printf("Server-bind() is OK...\n");

  if(listen(sockfd, BACKLOG) == -1){
    perror("Server-listen() error");
    exit(1);
  }

  printf("Server-listen() is OK...Listening...\n");

  sin_size = sizeof(struct sockaddr_in);
  while(1)
  {
    if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
      perror("Server-accept() error");
      exit(1);
    }

    printf("Server-accept() is OK...\n");
    printf("Server-new socket, new_fd is OK...\n");
    printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));
    while(strcmp(buf, "exit") != 0){
      if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1){
        perror("recv()");
        exit(1);
      }
      else{
        buf[numbytes] = '\0';
        printf("luiscarranza@MacBook-Pro-de-Luis %s \n", buf);
        comando(buf);
      }
    }
    close(new_fd);
    printf("Server-new socket, new_fd closed successfully...\n");
  }
  close(sockfd);
  return 0;
}