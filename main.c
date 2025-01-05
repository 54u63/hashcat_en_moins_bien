#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_PAGE 4096
#define MAX_LEN 15
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

typedef struct FileHandler{
  int fd;
  int mark;
  int word;
  char* page;
} FileHandler;


int load_array(FileHandler* words,char*** out){
  /*
   * in:  int fd file descriptor de la wordlist
   *      int * bookmark entier permettant de retrouver à quel mot on en était.
   *      char * word mot renvoyé 
   * return: int 
   *    0 si succès
   *    -1 si échec d'allocation
   *    1  si échec de mappage dans le buffer
   *
   *
   *  description: map un fichier directement dans la mémoire du process
   *               si le mappage réussi, itère au travers des caractères jusqu'à rencontrer un crlf
   *               les caractères lu sont alors retournés dans la variable "word"
   *               le fichier est ensuite démappé.
   *               La variable bookmark permet de reprendre directement au dernier mot lu.
   *
   * 
*/
  // première itération, on charge donc la page en mémoire
  
  if (words->mark==0 && words->page == NULL){
    words->page=(char*)mmap(NULL,MAX_PAGE,PROT_READ,MAP_PRIVATE,words->fd,0);
    if (words->page == MAP_FAILED){
      perror("[x] failed to map page");
      return 1;
    }
  }
  printf("[*] page sucessfully mapped in memory\n");
  int diff = 0;
  char* temp_buffer = (char*)malloc(MAX_LEN*sizeof(char));
  if(temp_buffer == NULL){
    perror("[x] failed to allocate temporary bufffer\n");
    return 1;
  }
  printf("[*] starting at %d index in page\n", words->mark);
  printf("[*] starting at %d word in buffer\n", words->word);
  for(;words->mark<MAX_PAGE;words->mark++){
    if (words->page[words->mark]=='\n'){
      printf("%s : word[%d]\n",temp_buffer,words->word);
      diff=0;
      words->word++;
    }
    else{
      temp_buffer[diff]=words->page[words->mark];
      diff++;
    }

  }


  if (munmap(words->page,MAX_PAGE)==-1){
    perror("[x] failed to unmap the page");
    return 1;
  }
}


int main() {

  // initialisation de la structure pour mappage du fichier
  FileHandler wordlist;
  wordlist.fd = open("./wordlist.txt", O_RDONLY);
  if (wordlist.fd == -1) {
      perror("Error opening file");
      return 1;
  }
  
  wordlist.mark = 0;

  wordlist.word = 0;

  wordlist.page = NULL;




  //vérification de la taille de la page
  int pagesize = PAGE_SIZE;
  if (pagesize == -1){
    printf("[x] failed to get size of a page\n");
  }
  printf("[*] page size is %d\n",pagesize);

  // Allocation de la mémoire pour un tableau de pointeurs de char (tableau de la wordlist)
  printf("[*] Allocating buffer for all words: %d\n", MAX_PAGE);
  char** words_buffer = (char**)malloc(MAX_PAGE * sizeof(char*));
  if (words_buffer == NULL){
    perror("[!] failed to allocate buffer of string");
  }

  printf("[*] Done loading buffer, now gonna store the word in the allocated buffers\n");
  printf("[+] DONE DOING PREROUTINE FILGHT CHECK [+]\n");
  printf("[*] now going to load pages and each words in the allocated buffer\n");

  load_array(&wordlist,&words_buffer);
  printf("[*]word mark : %d\n",wordlist.mark);
/*
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
      load_next(fd,&bookmark,(char*)malloc(MAX_LEN*sizeof(char)));
      printf("/!\\ trying load next  with bm %d/!\\ \n\n",bookmark);
    }
*/    
    printf("[*] Freeing each element\n");
    // Libération de la mémoire pour chaque élément du tableau
    for (int i = 0; i < MAX_PAGE; i++) {
      free(words_buffer[i]);
    }

    printf("[*] Freeing the global table\n");
    // Libération du tableau principal
    free(words_buffer);

    printf("[+] All free successful\n");

    // Fermeture du fichier
    close(wordlist.fd);

    return 0;
}

