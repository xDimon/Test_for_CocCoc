/*
 * main.cpp
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: Dmitriy Khaustov aka Dиmon (khaustov.dm@gmail.com)
 */

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

#include "LinesBlock.hpp"
#include "SortingOfLines.hpp"

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Usage: %s <inFile> <outFile> <memoryLimit>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Open finput file
	int inFd = open(argv[1], O_RDWR | O_NOATIME);
	if (inFd < 0)
	{
//		int fd = creat(argv[1], 0666);
//		for (int line = 0; line < 1<<20; line++)
//		{
//			int length = rand() % ((1<<8) - 1);
//			char buff[1<<15];
//			for (int p = 0; p < length; p++)
//			{
//				buff[p] = ((rand() % 7) == 0) ? 0x20 :
//					(rand() % 2) ? ((rand() % 26)+'a') : ((rand() % 26)+'A');
//			}
//			buff[length] = '\n';
//			write(fd, buff, length+1);
//			if (0 == (line % (1<<20)))
//			{
//				printf("Complete %u\n", line);
//			}
//		}
//		close(fd);

		printf("Can't open input file (%s): %s\n", argv[1], strerror(errno));
		return EXIT_FAILURE;
	}

	// Get size of input file
 	struct stat st;
	if (fstat(inFd, &st) < 0)
	{
		printf("Can't get stat of input file (%s): %s\n", argv[1], strerror(errno));
		return EXIT_FAILURE;
	}
	off_t size = st.st_size;
	printf("Size of input file: %lu bytes\n", size);

	// Create output file (occupy name)
	int fd = creat(argv[2], 0600);
	if (fd < 0)
	{
		printf("Can't create output file (%s): %s\n", argv[2], strerror(errno));
		return EXIT_FAILURE;
	}
	close(fd);

	// Empty input file. Follow empty output file
	if (!size)
	{
		puts("done");
		return EXIT_SUCCESS;
	}

	// Check and repair last line in file (all lines must ended as '\n')
	char lastChar;
	lseek(inFd, size-1, SEEK_SET);
	read(inFd, &lastChar, 1);
	if (lastChar != '\n')
	{
		lastChar = '\n';
		write(inFd, &lastChar, 1);
		size++;
		printf("Repair input file: append '\\n' to end of file\n");
	}

	// Mapping file in memory
	unsigned char *in = (unsigned char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, inFd, 0);
	if (in == (void *)(-1))
	{
		printf("Can't mapping in memory for input file (%s): %s\n", argv[1], strerror(errno));
		return EXIT_FAILURE;
	}

	// Size memory block
	size_t memoryLimit = atol(argv[3]);

	// Sorter
	SortingOfLines sort(memoryLimit);

	// Create initial block
	LinesBlock *r = new LinesBlock(in, in + size, true);

	clock_t cl1 = clock();

	// Sorting
	r = sort(r);

	clock_t cl2 = clock();
	printf("Sorted for %0.02f seconds\n", (float)(cl2-cl1)/CLOCKS_PER_SEC);
	munmap(in, size);

	// Validation result
	if (!r->valid)
	{
		delete r;

		printf("Fail\n");
		return EXIT_FAILURE;
	}

	// Rename temp file as output
	rename(r->mmap_file, argv[2]);

	delete r;

	printf("Done\n");
	return EXIT_SUCCESS;
}

