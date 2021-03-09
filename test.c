#include <unistd.h> /* read */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

char buf[4096];
int buf_i;	/* para analisar o conteúdo do buf */ 
int buf_o; /* para saber onde o buf se encontra no ficheiro */
int buf_r; /* para saber o que foi efetivamente lido do ficheiro */
int buf_begin; /*para saber que posições já se leram do buf */
int buf_end; /* para saber que posições se está a ler do buf */
int file_no; /* controlo de manipulação, se é o mesmo fichiero */
int b_size; /* tamanho do b passado pela main */
int b_i; /* índice do b passado pela main */
int b_o; /* offset do fd quando chamada a função + bytes de uma linha */
int b_flag_o; /* controlar se é preciso fazer soma ao offset anterior (caso em que se faz novo carregamento de bloco) */

int readln_read ()
{
	lseek (file_no, buf_o, SEEK_SET);
	buf_r = read (file_no, buf, 4096);
	buf_o += buf_r;
	buf_i = 0;
	buf_end = 0;
	buf_begin = 0;

	return (buf_r > 0 ? 1 : 0);
}

int readln_cfg (int fd, char *b, size_t size)
{
	file_no = fd;
	b_size = size;
	b_i = 0;
	b_o = 0;
	b_flag_o = 0;

	buf_o = 0;
	
	return ( readln_read () );	
}		

ssize_t readln (int fd, char *b, size_t size)
{	
	int stop = 0;
	ssize_t r = 0;

	if (fd == file_no)
	{
		while ((b_i < b_size) && (buf_i < buf_r) && !stop)
		{
			if (buf[buf_i] == '\n') stop = 1 ;			

			b[b_i] = buf[buf_i];
			b_i++;
			buf_i++;
			buf_end++;

			if (!stop && (buf_i == buf_r)) //o buf acabou e não encontrou \n
			{
				b_o += buf_end - buf_begin;
				b_flag_o = 1;
				readln_read(); //stop = 1-readln_read();
			}
		}

		if (b_flag_o) {
			b_o += buf_end - buf_begin;
			b_flag_o = 0;
		}
		else 
			b_o = buf_end - buf_begin;
		
		buf_begin = buf_end;	
	
		if (stop)
		{
			lseek (file_no, b_o, SEEK_SET);
			r = b_o;
			b_i = 0;
		}
	}

	return r;
}
		

int main (int argc, char *argv[]){
	int f;
	ssize_t r;
	char b[64];

	f = open (argv[1], O_RDONLY);
	
	readln_cfg (f, b, 64);
	while ( (r = readln (f, b, 64)) > 0) {
		write (1, b, r);
	}
	
}