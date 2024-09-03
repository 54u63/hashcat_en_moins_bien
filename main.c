#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_PAGE 500
#define MAX_LEN 100

int main(){
  //buffer des pass chargés
  char** kool_buffer = (char**)malloc(MAX_PAGE*sizeof(char*));
  assert(kool_buffer != NULL);

  //allocation des mots
  for(int i=0;i<MAX_PAGE;i++){
    kool_buffer[i]=(char*)malloc(MAX_LEN*sizeof(char));
    assert(kool_buffer != NULL);
  }

  for(int i=0;i<MAX_PAGE;i++){
    free(kool_buffer[i]);
  }
  free(kool_buffer);
  return 0;
}
