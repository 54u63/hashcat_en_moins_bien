#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>

#define MAX_PAGE 500
#define MAX_LEN 100

char** fill_array(char** arr,int fd, int* bookmark){
  int bookmark_v = *bookmark;
  printf("bookmark -> %d\n",bookmark_v);

  char* page = mmap(NULL,(size_t)MAX_LEN*MAX_PAGE,PROT_READ,MAP_PRIVATE,fd,(off_t)bookmark_v);
  printf("%s\n",page);

  bookmark_v += MAX_LEN*MAX_PAGE;
  *bookmark = bookmark_v;
  
  return arr;
}


int main(){
  //int "marque page" dans le fichier 
  int bookmark = 0;

  //syscall open: ouvertur du dict
  int fd = open("../rockyou.txt",O_RDONLY);

  //buffer des pass chargés
  char** kool_buffer = (char**)malloc(MAX_PAGE*sizeof(char*));
  assert(kool_buffer != NULL);

  //allocation des mots
  for(int i=0;i<MAX_PAGE;i++){
    kool_buffer[i]=(char*)malloc(MAX_LEN*sizeof(char));
    assert(kool_buffer != NULL);
  }
  
  char** ouient=fill_array(kool_buffer,fd,&bookmark);

  printf("\nnew bookmark -> %d\n", bookmark);














  //free de chaque éléments
  for(int i=0;i<MAX_PAGE;i++){
    free(kool_buffer[i]);
  }

  //free du tableau global
  free(kool_buffer);
  return 0;
}
