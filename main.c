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

typedef struct FileHandler {
    int fd;			//file descriptor du fichier à ouvrir
    int mark;			//repère de caractère dans le fichier (à quel caractère on en est)
    int word;			//repère de mot dans le fichier (a quel mot on en est dans le fichier)
    char *page;			//page mappée par mmap
} FileHandler;

int page_integ(FileHandler *words, char **word)
{
    if (words->mark == 0 && words->page == NULL) {	//ouverture du fichier et chargement #TODO modifier pour charger plusieurs page
	words->page =
	    mmap(NULL, MAX_PAGE, PROT_READ, MAP_PRIVATE, words->fd, 0);
	if (words->page == MAP_FAILED) {
	    perror("[x] failed to map page");
	    return 1;
	    printf("[*] page sucessfully mapped in memory\n");
	    return 0;
	}

    }
    if (words->mark != 0 && words->mark < MAX_PAGE && words->page != NULL) {
	/*
	 * chargement de la mage suivante
	 * */
	printf("here");
	return 0;
    } else {
	return 0;
    }
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
    printf("[*] starting at %d index in page\n", words->mark);
    printf("[*] starting at %d word in buffer\n", words->word);
    for (; words->mark < MAX_PAGE; words->mark++) {	//itération dans les charactères du fichier
	page_integ(words, &temp_buffer);
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
	} else {		//sinon pas de fin du mot donc continuation
	    temp_buffer[diff] = words->page[words->mark];	//stockage du caractère dans le buffer temporaire
	    diff++;		//incrémentation de la taille du mot lue
	}

    }


    if (munmap(words->page, MAX_PAGE) == -1) {
	perror("[x] failed to unmap the page");
	return 1;
    }
    printf("[*]temp buffer ended with %s\n", temp_buffer);
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

    wordlist.mark = 0;
    wordlist.word = 0;
    wordlist.page = NULL;
    //vérification de la taille de la page
    int pagesize = PAGE_SIZE;
    if (pagesize == -1) {
	printf("[x] failed to get size of a page\n");
    }
    printf("[*] page size is %d\n", pagesize);
    // Allocation de la mémoire pour un tableau de pointeurs de char (tableau de la wordlist)
    printf("[*] Allocating buffer for all words: %d\n", MAX_PAGE);
    char **words_buffer = (char **) malloc(MAX_PAGE * sizeof(char *));
    if (words_buffer == NULL) {
	perror("[!] failed to allocate buffer of string");
	return 1;
    }

    printf
	("[*] Done loading buffer, now gonna store the word in the allocated buffers\n");
    printf("[+] DONE DOING PREROUTINE FILGHT CHECK [+]\n");
    printf
	("[*] now going to load pages and each words in the allocated buffer\n");
    load_array(&wordlist, &words_buffer);
    for (int i = 0; i < wordlist.word; i++) {
	printf("[%d] -> %s\n", i, words_buffer[i]);
    }
    printf("[*]word mark : %d\n", wordlist.mark);
    load_array(&wordlist, &words_buffer);
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
