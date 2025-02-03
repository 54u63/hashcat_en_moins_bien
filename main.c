#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_LEN 55
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

typedef struct FileHandler {
    int fd;			//file descriptor du fichier à ouvrir
    int mark;			//repère de caractère dans le fichier (à quel caractère on en est)
    int word;			//repère de mot dans le fichier (a quel mot on en est dans le fichier)
    int offset;   //nombre de pages lues
    char *page;			//page mappée par mmap
} FileHandler;

int load_page(FileHandler *words)
{ /**/
  printf("[+]CALL TO load_page[+]\n");
  if (words->page != NULL){
    if (munmap(words->page, PAGE_SIZE) == -1) {
          perror("[x] failed to unmap the page");
          return 1;
      }
    printf("{*} unmap successful\n");
  }
  words->page = mmap(NULL, PAGE_SIZE, PROT_READ, MAP_PRIVATE, words->fd, words->offset*PAGE_SIZE);
  
  if (words->page == MAP_FAILED) {
    perror("[x] failed to map page");
    return 1;
  }
  
  printf("[*] page sucessfully mapped in memory\n");
  return 0;
}


int load_array(FileHandler *words, char ***out)
{
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


    int diff = 0;		//compteur de caractères lus par le programme (sert pour strncpy)
    char *temp_buffer = malloc(MAX_LEN * sizeof(char));	//buffer temporaire 
    if (temp_buffer == NULL) {	//vérification malloc
      perror("[x] failed to allocate temporary bufffer\n");
      return 1;
    }
    printf("[*] page loaded now gonna split the \\n \n");
    for (; words->mark < PAGE_SIZE; words->mark++) {	//itération dans les charactères du fichier
      if (diff >= MAX_LEN) {	//si le mot qu'on est entrain de charger est plus grand que la taille de notre buffer
        perror("[x] temp_buffer overflow");	//on saute
        return 1;
      }
      if (words->page[words->mark] == '\n') {	//si la lettre est \n -> fin du mot
        (*out)[words->word] = (char *) malloc(diff + 1);	//on alloue la mémoire dans notre tableau final pour stocker notre mot
        if ((*out)[words->word] == NULL) {	//vérification de l'allocation la mémoire est free par la suite
          perror("[x]failed to allocate wordlist buffer\n");
          return 1;
        }
        strncpy((*out)[words->word], temp_buffer, diff);
        diff = 0;		//réinitalisation de la taille du mot
        words->word++;	// on chargera le mot qui va être lu dans la case suivante
      }  
      else if (words->page[words->mark] != '\n' && words->mark == PAGE_SIZE-1){ //si on est arrivé au bout du fichier mais que le dernier mot n'est pas lu en entier
        printf("  page ended on char ->%c att index %d\n",words->page[words->mark],diff);
        temp_buffer[diff] = words->page[words->mark];	//stockage du caractère dans le buffer temporaire
        diff++;
        goto _EndPage;
      }
      else {		//sinon pas de fin du mot donc continuation
          temp_buffer[diff] = words->page[words->mark];	//stockage du caractère dans le buffer temporaire
          diff++;		//incrémentation de la taille du mot lue
        }
     

    }
_EndPage:
  words->offset++;
  words->mark=0;
  load_page(words);
  for(;words->page[words->mark]!='\n';diff++){
    if (diff>MAX_LEN){
      break;
    }
    temp_buffer[diff]=words->page[words->mark];
    words->mark++;
  }

  (*out)[words->word]=(char *) malloc(diff+1);
  if((*out)[words->word]==NULL){
    perror("[x] failed to allocate last cell of array");
    return 1;
  }
  strncpy((*out)[words->word],temp_buffer,diff);
  diff=0;
  return 0;
}

int main(int argc, char **argv)
{

    // initialisation de la structure pour mappage du fichier
    FileHandler wordlist;
    wordlist.fd = open(argv[1], O_RDONLY);
    if (wordlist.fd == -1) {
      perror("Error opening file");
      return 1;
    }
    /*
    printf("[preparing fstat]\n");
    struct stat file_info;
    if(fstat(wordlist.fd,&file_info)==-1){
      perror("[x] failed to get wordlist size");
      return 0;
    }
    off_t size = file_info.st_size;
    printf("[*] file %d of size : %d\n",size,wordlist.fd);
    */
    wordlist.mark = 0;
    wordlist.page = NULL;
    //vérification de la taille de la page
    int pagesize = PAGE_SIZE;
    if (pagesize == -1) {
      printf("[x] failed to get size of a page\n");
    }
    printf("[*] page size is %d\n", pagesize);
    // Allocation de la mémoire pour un tableau de pointeurs de char (tableau de la wordlist)
    printf("[*] Allocating buffer for all words: %d\n", PAGE_SIZE);
    char **words_buffer = (char **) malloc(PAGE_SIZE * sizeof(char *));
    if (words_buffer == NULL) {
      perror("[!] failed to allocate buffer of string");
      return 1;
    }
    load_page(&wordlist);

    printf("[+] DONE DOING PREROUTINE FILGHT CHECK [+]\n");
    for(int i=0;i<10;i++){
      load_array(&wordlist, &words_buffer);
      printf("[0]->%s\n...\n[%d]->%s\n",words_buffer[0],wordlist.word,words_buffer[wordlist.word]);
      wordlist.word = 0;
      printf("loading next page\n");
    }
   

  printf("[*] Freeing each element\n");
    // Libération de la mémoire pour chaque élément du tableau
    for (int i = 0; i < PAGE_SIZE; i++) {
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
