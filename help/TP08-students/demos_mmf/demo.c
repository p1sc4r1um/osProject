#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/mman.h>	
#include	<fcntl.h>
#include	<stdio.h>	
#include	<stdlib.h>	
#include	<string.h>	
#include	<unistd.h>
#include	<limits.h>	
#include 	<pthread.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#ifndef	MAP_FILE	/* 44BSD defines this & requires it to mmap files */
#define	MAP_FILE	0	/* to compile under systems other than 44BSD */
#endif

#define SIZE 20

#define N 3

struct stat statbuf;

struct pixel
{
	char R;
	char G;
	char B;
};

char * src;
int size;

struct pixel *get_pixel(char *buf, int *pos)
{
	struct pixel pix;
	struct pixel *ppix = &pix;
	ppix->R = buf[*pos];
	ppix->G = buf[(*pos)+1];
	ppix->B = buf[(*pos)+2];
	(*pos) += 3;
	return ppix;
}

void write_pixel(struct pixel *ppix, char *buf, int *pos)
{
	buf[*pos] = ppix->G;
	buf[(*pos)+1] = ppix->B;
	buf[(*pos)+2] = ppix->R;
	(*pos) += 3;
}

void * do_rotation(void *rotation)
{
	int	fdout;
	char	*dst;
	char fich[SIZE];

	int counter,index, pos, x, y;
	int xmax = 0;
	int ymax = 0;
	int colormax = 0;
	
	/* Construção do nome do ficheiro de saída */
	sprintf(fich, "rot%d.ppm", *((int*)rotation));

	/* Zona #2 */

	if ( (fdout = open(fich, O_RDWR | O_CREAT | O_TRUNC,FILE_MODE)) < 0)
	{
		fprintf(stderr,"can't creat %s for writing\n", fich);
		exit(1);
	}

	/* set size of output file */
	if (lseek(fdout, size - 1, SEEK_SET) == -1)
	{
		fprintf(stderr,"lseek error\n");
		exit(1);
	}
	if (write(fdout, "", 1) != 1)
	{
		fprintf(stderr,"write error\n");
		exit(1);
	}
	

	if ( (dst = mmap(0, size, PROT_READ | PROT_WRITE,MAP_SHARED,fdout, 0)) == (caddr_t) -1)
	{
		fprintf(stderr,"mmap error for output\n");
		exit(1);
	}
	/* Fim da Zona #2 */ 
	
	sscanf(src,"P6\n%d %d\n%d\n",&xmax,&ymax,&colormax);
	
	struct pixel imagem [ymax][xmax];
	
	for (counter=0, index=0; counter<3;index++)
	{
		if (src[index]=='\n')
			++counter;
	} 	
	pos=index-1;
	for (y=0;y<ymax;y++)
		for (x=0;x<xmax;x++)
			imagem[y][x] = *(get_pixel(src,&pos));
	pos=index;
	
	switch (*((int*)rotation))
	{
		case 0 :
			//Rotação 90º clockwise
			sprintf(dst,"P6\n%d %d\n%d\n",ymax,xmax,colormax);
			for (x=0; x<xmax; x++)
				for (y=ymax-1; y> -1;y--)
					write_pixel(&(imagem[y][x]),dst,&pos);
			break;
		case 1:
			//Rotação 180º
			sprintf(dst,"P6\n%d %d\n%d\n",xmax,ymax,colormax);
			for (y=ymax-1;y>-1;y--)
				for (x = xmax-1; x>-1; x--)
					write_pixel(&(imagem[y][x]),dst,&pos);
			break;
		case 2:
			//Rotação 270º clockwise
			sprintf(dst,"P6\n%d %d\n%d\n",ymax,xmax,colormax);
			for (x=xmax-1; x>-1; x--)
				for (y=0; y<ymax;y++)
					write_pixel(&(imagem[y][x]),dst,&pos);
			break;
		
		
	}

/* Zona #3 */
	munmap(dst,size);
	close(fdout);
/* Fim da Zona #3 */

 	pthread_exit(NULL);

}

int get_stat(int fdin)
{
	struct stat pstatbuf;	
	if (fstat(fdin, &pstatbuf) < 0)	/* need size of input file */
	{
		fprintf(stderr,"fstat error\n");
		exit(1);
	}
	return pstatbuf.st_size;
}


int main(int argc, char *argv[])
{
	int id[N];
	int fdin, i;

	if (argc != 2)
	{
		fprintf(stderr,"usage: demo <fromfile>\n");
		exit(1);
	}

	if ( (fdin = open(argv[1], O_RDONLY)) < 0)
	{
		fprintf(stderr,"can't open %s for reading\n", argv[2]);
		exit(1);
	}

	size = get_stat(fdin);

/* Zona # 1*/
	if ( (src = mmap(0, size, PROT_READ, MAP_FILE | MAP_SHARED, fdin, 0)) == (caddr_t) -1)
	{
		fprintf(stderr,"mmap error for input\n");
		exit(1);
	}
/* Fim da Zona #1 */

/* Zona #5 */
	pthread_t tid[N];
	for (i=0; i<N; i++){
		id[i]=i;
		pthread_create (&tid[i], NULL, do_rotation, &id[i]);
	}
	for (i=0; i<N; i++)
		pthread_join (tid[i], NULL);
/* Fim da Zona #5 */

/* Zona #4 */
	munmap(src,size);
	close(fdin);
/* Fim da Zona #4 */

	exit(0);
}
