/*
 * LinesBlock.cpp
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: Dmitriy Khaustov aka Dиmon (khaustov.dm@gmail.com)
 */

#include "LinesBlock.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

LinesBlock::LinesBlock(unsigned char *b, unsigned char *e, bool ro)
{
	begin = b;
	end = e;
	mmap_ptr = reinterpret_cast<void *>(-1);
	mmap_size = 0;
	mmap_file = 0;
	readonly = ro;
	valid = true;
};

LinesBlock::LinesBlock(size_t size)
{
	begin = 0;
	end = 0;
	mmap_ptr = reinterpret_cast<void *>(-1);
	mmap_size = 0;
	mmap_file = 0;
	readonly = false;
	valid = false;

	if (!size)
	{
		printf("Can't create zero size block\n");
		return;
	}

	char fn[16] = "tmp_XXXXXX";
	int fd = mkostemp(fn, O_RDWR | O_NOATIME);
	if (fd < 0)
	{
		printf("Can't create temp file: %s\n", strerror(errno));
		return;
	}
	if (lseek(fd, size - 1, SEEK_SET) == -1)
	{
		remove(fn);
		printf("Can't expand temp file: %s\n", strerror(errno));
		return;
	}
	char z = '\0';
	if (write(fd, &z, sizeof(z)) == -1)
	{
		remove(fn);
		printf("Can't expand temp file: %s\n", strerror(errno));
		return;
	}

	mmap_ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	if (mmap_ptr == (void *)(-1))
	{
		remove(fn);
		printf("Can't memory mapping temp file: %s\n", strerror(errno));
		return;
	}
	mmap_size = size;
	mmap_file = strdup(fn);

	begin = static_cast<unsigned char *>(mmap_ptr);
	end = begin + size;
	valid = true;

	return;
}

LinesBlock::~LinesBlock()
{
	if (mmap_ptr != reinterpret_cast<void *>(-1))
	{
		munmap(mmap_ptr, mmap_size);
	}
	remove(mmap_file);
};


