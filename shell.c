/*Engineers: Luis Enrique Carranza Escobar
**Avendaño Cabanillas Gustavo Eduardo
**School: Facultad de Ingeniería UNAM
**Sistemas Operativos
*/
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#define MAX 50

int comando () {
   char str[MAX]; //creamos los valores con los que se va a dividir nuestro comando
   int pid;
   int ban = 1;
   char strDos[MAX];
   char *token;
   char aux;
   char s[2] = " ";

   while(ban != 100){
      printf("$> ");
      gets(str);
      fflush(stdin); //limpiamos el buffer
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
            }
         }else{
            execvp(comandos[0],comandos);
         }
      }
      wait(&ban);
   }
   return(0);
}

int main(){
   printf("Hola mundo");
   comando();
   return 0;
}
