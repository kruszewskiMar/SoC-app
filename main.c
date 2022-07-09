#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER 1024

#define ADRES_CZUJNIK1_READ "/proc/sykom/kmc7el1"
#define ADRES_CZUJNIK1_WRITE "/proc/sykom/kmc7el2"
#define ADRES_CZUJNIK2_READ "/proc/sykom/kmc7el3"
#define ADRES_CZUJNIK2_WRITE "/proc/sykom/kmc7el4"
#define ADRES_INT_READ "/proc/sykom/kmc7el5"
#define ADRES_INT_WRITE "/proc/sykom/kmc7el6"



int main(void){
  
  char buffer[MAX_BUFFER];
  
  
  int cz1_adres_write=open(ADRES_CZUJNIK1_WRITE, O_RDWR); //otwarcie pliku „in” sterownika
  if(cz1_adres_write<0){
    printf("Open %s – error: %d\n", ADRES_CZUJNIK1_WRITE, errno);
    exit(1);
  }
  
  int cz2_adres_write=open(ADRES_CZUJNIK2_WRITE, O_RDWR); //otwarcie pliku „in” sterownika
  if(cz2_adres_write<0){
    printf("Open %s – error: %d\n", ADRES_CZUJNIK2_WRITE, errno);
    close(cz1_adres_write);
    exit(2);
  }
     

  printf("TESTOWANIE ZAPISU CZUJNIKOW \n");

  printf("\n");
  
  printf("Trwa zapisywanie\n");
  
  printf("\n");
  
  snprintf(buffer, MAX_BUFFER, "00004000"); //tresc do przekazania sterownikowi
  int cz1_adres_write_zapis=write(cz1_adres_write, buffer, strlen(buffer)); //wlasciwe przekazanie danych
  if(cz1_adres_write_zapis!=strlen(buffer)){
    printf("Write %s – error: %d\n", cz1_adres_write, errno);
    close(cz1_adres_write);
    close(cz2_adres_write);
    exit(4);
  }
  snprintf(buffer, MAX_BUFFER, "0000001c"); //tresc do przekazania sterownikowi
  int cz2_adres_write_zapis=write(cz2_adres_write, buffer, strlen(buffer)); //wlasciwe przekazanie danych
  if(cz2_adres_write_zapis!=strlen(buffer)){ 
    printf("Write %s – error: %d\n", cz2_adres_write, errno);
    close(cz2_adres_write);
    close(cz1_adres_write);
    exit(5);
  }
  
  
   close(cz2_adres_write);
   close(cz1_adres_write);
   
  
  
 
  printf("_________________ TEST ODCZYTU ORAZ LICZNIKA_________________  \n");
  
  printf("\n");
  
  int i = 0;
  int j = 0;
  
  while(i<20){
  
    i++;
    
    char buffer[MAX_BUFFER];
    
    int cz1_adres_read=open(ADRES_CZUJNIK1_READ, O_RDONLY); //otwarcie pliku „out” sterownika
    if(cz1_adres_read<0){
      printf("Open %s – error: %d\n", ADRES_CZUJNIK1_READ, errno);
      close(cz1_adres_read); //nikt inny nie zamknie wczesniej otwartego pliku
      exit(6);
      }
      
    int cz2_adres_read=open(ADRES_CZUJNIK2_READ, O_RDONLY); //otwarcie pliku „out” sterownika
    if(cz2_adres_read<0){
      printf("Open %s – error: %d\n", ADRES_CZUJNIK2_READ, errno);
      close(cz1_adres_read);
      close(cz2_adres_read);
      exit(7);
    }
    
    int int_adres_read=open(ADRES_INT_READ, O_RDONLY); //otwarcie pliku „out” sterownika
    if(int_adres_read<0){
      printf("Open %s – error: %d\n", ADRES_INT_READ, errno);
      close(cz1_adres_read);
      close(int_adres_read);
      close(cz2_adres_read);
      exit(8);
    } 
  
    printf("__________________________________ \n");
    printf("\n");
    
    
    int cz1_adres_read_odczyt=read(cz1_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
    if(cz1_adres_read_odczyt>0){
      buffer[cz1_adres_read_odczyt]='\0';
      printf("Odczyt z czujnika 1: %s\n", buffer);
    }
    
    int cz2_adres_read_odczyt=read(cz2_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
    if(cz2_adres_read_odczyt>0){
      buffer[cz2_adres_read_odczyt]='\0';
      printf("Odczyt z czujnika 2: %s\n", buffer);
    }
    
    int int_adres_read_odczyt=read(int_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
    if(int_adres_read_odczyt>0){
      buffer[int_adres_read_odczyt]='\0';
      if(!strncmp(buffer, "40 \n", 1) && j == 0){
        printf("________________ZEROWANIE LICZNIKA I ZAPIS JEDYNKI_________________ \n");
        printf("\n");
        printf("sygnal INT : %s\n", buffer);
        j ++;
      }
      else{
        printf("sygnal INT : %s\n", buffer);
      }
      
    }
    
    close(cz1_adres_read);
  
    close(cz2_adres_read);
  
    close(int_adres_read);
    sleep(1);
  
  }
  
  
  int int_adres_write=open(ADRES_INT_WRITE, O_RDWR); //otwarcie pliku „in” sterownika
  if(int_adres_write<0){
    printf("Open %s – error: %d\n", ADRES_INT_WRITE, errno);
    close(cz1_adres_write);
    close(cz2_adres_write);
    exit(8);
  }

  snprintf(buffer, MAX_BUFFER, "40"); //tresc do przekazania sterownikowi
  int int_adres_write_zapis=write(int_adres_write, buffer, strlen(buffer)); //wlasciwe przekazanie danych
  if(int_adres_write_zapis!=strlen(buffer)){ 
    printf("Write %s – error: %d\n", int_adres_write, errno);
    close(int_adres_write);
    exit(9);
  }


  int int_adres_read=open(ADRES_INT_READ, O_RDONLY); //otwarcie pliku „out” sterownika
    if(int_adres_read<0){
      printf("Open %s – error: %d\n", ADRES_INT_READ, errno);
      close(int_adres_write);
      exit(10);
    }

    int int_adres_read_odczyt=read(int_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
    if(int_adres_read_odczyt>0){
      buffer[int_adres_read_odczyt]='\0';
      printf("__________Wyslanie jedynki  z przesunieciem do INT______________ \n");
      printf("INT: %s", buffer);
    }

    close(int_adres_write);
    close(int_adres_read);

    i = 0;

    while(i<6){
      i++;
      char buffer[MAX_BUFFER];
      int int_adres_read=open(ADRES_INT_READ, O_RDONLY); //otwarcie pliku „out” sterownika
      if(int_adres_read<0){
        printf("Open %s – error: %d\n", ADRES_INT_READ, errno);
        close(int_adres_read);
        exit(11);
      }
      int int_adres_read_odczyt=read(int_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
      if(int_adres_read_odczyt>0){
        buffer[int_adres_read_odczyt]='\0';
        printf("INT: %s", buffer);
        }
      close(int_adres_read);
      sleep(1);
  }
  
  return 0;
  
}