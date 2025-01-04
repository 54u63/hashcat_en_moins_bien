#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_PAGE 5
#define MAX_LEN 15


int load_next(int fd, int* bookmark){
  int mark=*bookmark;
  char* words=(char*)mmap(NULL,MAX_LEN,PROT_READ,MAP_PRIVATE,fd,mark);
  if(words == MAP_FAILED){
    printf("[x] failed to map on the expected words\n");
    return -1;
  }
  printf("[*] map successful\n");
  int max_read=mark+MAX_LEN;
  while (words[mark]!='\n' && mark<max_read){
    printf("%c",words[mark]);
    mark+=1;
  }
  if(munmap(words,MAX_LEN)==-1){
    printf("[x] failed to unmap memory\n");
    return -1;
  }
  printf("[*]unmap successful\n");
  *bookmark=mark;
  return 0;

}


int fill_array(char*** arr, int fd, int* bookmark) {
    // Allocation de mémoire pour la première case
    (*arr)[0] = (char*)malloc(MAX_LEN * sizeof(char));
    if ((*arr)[0] != NULL) {
        printf("[+] Successfully allocated first cell of array\n");
        strcpy((*arr)[0], "test");  // Copie de la chaîne dans la première case
    } else {
        printf("[!] Memory allocation failed for first cell\n");
        return -1;
    }
    return 0;
}

int main() {
    // "Marque-page" dans le fichier
    int bookmark = 0;

    // Ouverture du fichier avec la syscall open
    int fd = open("./wordlist.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // Allocation de la mémoire pour un tableau de pointeurs de char
    printf("[*] Allocating buffer for all words: %d\n", MAX_PAGE);
    char** kool_buffer = (char**)malloc(MAX_PAGE * sizeof(char*));
    assert(kool_buffer != NULL);  // Assurez-vous que l'allocation a réussi

    printf("[+] Done loading buffer, now gonna store the word in the allocated buffers\n");

    // Initialisation de chaque case à NULL (optionnel, pour éviter des accès indéfinis)
    for (int i = 0; i < MAX_PAGE; i++) {
        kool_buffer[i] = NULL;
    }

    // Affichage avant modification (la première case est NULL)
    printf("[*] First element of buffer: %s\n", kool_buffer[0] ? kool_buffer[0] : "NULL");

    // Appel de la fonction pour remplir le tableau
    if (fill_array(&kool_buffer, fd, &bookmark) != 0) {
        printf("[!] Error while filling the array\n");
        free(kool_buffer);
        close(fd);
        return 1;
    }

    // Affichage après modification (la première case contient "test\n")
    printf("[*] First element of buffer: %s\n", kool_buffer[0]);
    
    printf("/!\\ trying load next  with bm %d/!\\ \n\n",bookmark);
    for(int i=0;i<3;i++){
      load_next(fd,&bookmark);
      printf("/!\\ trying load next  with bm %d/!\\ \n\n",bookmark);
    }
    
    printf("[*] Freeing each element\n");
    // Libération de la mémoire pour chaque élément du tableau
    for (int i = 0; i < MAX_PAGE; i++) {
        if (kool_buffer[i] != NULL) {
            free(kool_buffer[i]);
        }
    }

    printf("[*] Freeing the global table\n");
    // Libération du tableau principal
    free(kool_buffer);

    printf("[+] All free successful\n");

    // Fermeture du fichier
    close(fd);

    return 0;
}

