#include <stdio.h> /* argc, argv, perror */
#include <stdlib.h> /* exit, malloc */
#include <sys/types.h> /* open, read, write, close */
#include <fcntl.h> /* O-* */
#include <unistd.h> /* ? */

#define cache_mem 2048
#define ram_mem 64

char cache [2048];
int cache_i = 0;

void coldCache ()
{
	cache_i = 0;

	for (int i = 0; i < cache_mem ; i++)
		cache[i] = '\0';
}

void writeRam (char *ram)
{
	int i;
	for (i=0  ; i < cache_i ; i++)
		cache[i] = ram[i];
	ram[i] = '\0'; //for ( ; i<ram_mem ; i++) ram[i] = '\0';
}	

ssize_t readln (int f, char *ram, size_t ram_mem)
{
	ssize_t r;

	if (cache_i == 0)
	{
		r = read (f, cache, cache_mem);
		for ( ; cache_i < ram_mem ; cache_i++) 
			if ( cache[cache_i] == '\n') break;
		       	// (cache[cache_i] == '\n' ? break; : ) pragma omp	
		//condição para preencher o buffer com '\0'
		if (cache_i < ram_mem) writeRam (ram);
	}

	return r;
}

int main (int argc, int *argv[]) {
	int f;
	ssize_t r;
	char ram[ram_mem];

	/* Condições de Arranque */
	if (argc > 2) { perror ("readln source"); exit (1); }

	if (argc == 1) f = 0;

	if (argc == 2) {
		f = open (argv[1], O_RDONLY);
		if (f < 0) { perror ("source"); exit (2); }
	}

	coldCache();
	r = readln (f, ram, ram_mem);
	write (1, ram, r);

	return 0;
}	
