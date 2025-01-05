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
#define MAX_LEN 55
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

typedef struct FileHandler{
  int fd; //file descriptor du fichier à ouvrir
  int mark; //repère de caractère dans le fichier (à quel caractère on en est)
  int word; //repère de mot dans le fichier (a quel mot on en est dans le fichier)
  char* page; //page mappée par mmap
} FileHandler;


int load_array(FileHandler* words,char*** out){
  /*
   * input: FileHandler* words : structure représentant le fichier traité
   *        char***      out   : tableau à deux dimension passé par référence qui permettra de stocker les mots
   * output: int:
   *        0 si succès
   *        1 si échec
   * 
   * description: lis les éléments contenus dans la page chargée puis divise les mots un a un. Un mot est considéré
   *              comme une suite de caractère terminé par un '\n'. l'intéret est de réaliser un équivalent à la
   *              fonction strtok() sans avoir besoin des permissions en écriture sur le fichier
   *
   *
   *
*/
  
  if (words->mark==0 && words->page == NULL){ //ouverture du fichier et chargement #TODO modifier pour charger plusieurs page
    words->page=(char*)mmap(NULL,MAX_PAGE,PROT_READ,MAP_PRIVATE,words->fd,0);
    if (words->page == MAP_FAILED){
      perror("[x] failed to map page");
      return 1;
    }
  }
  printf("[*] page sucessfully mapped in memory\n");
  int diff = 0; //compteur de caractères lus par le programme (sert pour strncpy)
  char* temp_buffer = (char*)malloc(MAX_LEN*sizeof(char)); //buffer temporaire 
  if(temp_buffer == NULL){ //vérification malloc
    perror("[x] failed to allocate temporary bufffer\n");
    return 1;
  }
  printf("[*] starting at %d index in page\n", words->mark);
  printf("[*] starting at %d word in buffer\n", words->word);
  for(;words->mark<MAX_PAGE;words->mark++){//itération dans les charactères du fichier
    if (diff >= MAX_LEN) {//si le mot qu'on est entrain de charger est plus grand que la taille de notre buffer
      perror("[x] temp_buffer overflow");//on saute
      return 1;
    }
    if (words->page[words->mark]=='\n'){//si la lettre est \n -> fin du mot
      //printf("%s : word[%d] of len %d\n",temp_buffer,words->word,diff); //on affiche un message pour valider que le mot à bien été lu
      (*out)[words->word]=(char*)malloc((diff+1)*sizeof(char)); //on alloue la mémoire dans notre tableau final pour stocker notre mot
      if ((*out)[words->word]==NULL){//vérification de l'allocation la mémoire est free par la suite
        perror("[x]failed to allocate wordlist buffer\n");
        return 1;
      }
      //printf("[*] sucessfully allocated memory into the words buffer\n");
      strncpy((*out)[words->word],temp_buffer,diff);
      //(*out)[words->word]='\0';
      printf("[*] word loaded in the array -> %s\n",(*out)[words->word]);
      diff=0; //réinitalisation de la taille du mot
      words->word++;// on chargera le mot qui va être lu dans la case suivante
    }
    else{//sinon pas de fin du mot donc continuation
      temp_buffer[diff]=words->page[words->mark];//stockage du caractère dans le buffer temporaire
      diff++; //incrémentation de la taille du mot lue
    }

  }


  if (munmap(words->page,MAX_PAGE)==-1){
    perror("[x] failed to unmap the page");
    return 1;
  }
}
/*
  for(int i=0;i<3;i++){
    (*out)[i]=(char*)malloc(6*sizeof(char));
    if ((*out)[i]==NULL){
      perror("[x] failed to allocate the memory to buffer");
      return 1;
    }
    strncpy((*out)[i],"pouet",5);
    (*out)[i][5]='\0';
    printf("[%d] %s\n",i,(*out)[i]);
  }
    for (int i = 0; i < 3; i++) {
    // Allouer de la mémoire pour chaque chaîne dans *out
    (*out)[i] = (char*)malloc(5 * sizeof(char)); // Notez la correction ici : (*out)[i] au lieu de *out[i]
    if ((*out)[i] == NULL) {
      printf("Erreur d'allocation mémoire pour *out[%d]\n", i);
      return -1; // Retourner une valeur d'erreur
    }
    strncpy((*out)[i], "pouet", 5);  // Copier "pouet" dans la chaîne
    (*out)[i][4] = '\0'; // Ajouter un terminant NULL explicite à la fin de la chaîne
    printf("[%d] %s\n", i, (*out)[i]);
  }
  return 0; // Retourne 0 si tout s'est bien passé
*/


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
    return 1;
  }

  printf("[*] Done loading buffer, now gonna store the word in the allocated buffers\n");
  printf("[+] DONE DOING PREROUTINE FILGHT CHECK [+]\n");
  printf("[*] now going to load pages and each words in the allocated buffer\n");

  load_array(&wordlist,&words_buffer);
  printf("[*]word mark : %d\n",wordlist.mark);
  for(int i=0;i<9;i++){
    printf("[%d] -> %s\n",i,words_buffer[i]);
  }
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

