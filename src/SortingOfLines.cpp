/*
 * SortingOfLines.cpp
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: Dmitriy Khaustov aka Dиmon (khaustov.dm@gmail.com)
 */

#include "SortingOfLines.hpp"

#include <cstdio>
#include <cstdlib>

#include "LinesBlock.hpp"

SortingOfLines::SortingOfLines(size_t memoryLimit)
{
	_sizeMemoryBlock = memoryLimit;

	for (;;)
	{
		// Try allocate memory for size
		_ptrMemoryBlock = malloc(_sizeMemoryBlock);
		if (_ptrMemoryBlock)
		{
			if (_sizeMemoryBlock != memoryLimit)
			{
				printf("Unable to allocate requested memory, but less memory allocated: %lu bytes\n", _sizeMemoryBlock);
			}
			return;
		}
		// if fail, try again wish decreased size by 'golden ratio';
		_sizeMemoryBlock *= 0.62;
		if (_sizeMemoryBlock < (1<<14))
		{
			_sizeMemoryBlock = 0;
			printf("Can't allocate anough memory or requested too few memory. Will sorting only in mmaped files\n");
			return;
		}
	}
}

SortingOfLines::~SortingOfLines()
{
	free(_ptrMemoryBlock);
}

// Comparator for n-terminated lines
int SortingOfLines::compare(const void *bl, const void *br)
{
	const unsigned char *l = (const unsigned char *)bl;
	const unsigned char *r = (const unsigned char *)br;

	for (; *l == *r; l++, r++)
	{
		if (*l == '\n')
		{
			return 0;
		}
	}
	return (*l < *r) ? -1 : +1;
}

// Merging two block by order
LinesBlock *SortingOfLines::merge(LinesBlock *l, LinesBlock *r)
{
	// Block for result of merging
	LinesBlock *o = new LinesBlock((l->end - l->begin) + (r->end - r->begin));

	o->valid = o->valid && l->valid && r->valid;

	// Validation blocks
	if (!o->valid)
	{
		delete l;
		delete r;

		return o;
	}

	unsigned char *dst = o->begin;

	while (l->begin < l->end && r->begin < r->end)
	{
		if (compare(l->begin, r->begin) <= 0)
		{
			do
			{
				*dst++ = *l->begin;
			}
			while (*l->begin++ != '\n');
		}
		else
		{
			do
			{
				*dst++ = *r->begin;
			}
			while (*r->begin++ != '\n');
		}
	}
	while (l->begin < l->end)
	{
		*dst++ = *l->begin++;
	}
	while (r->begin < r->end)
	{
		*dst++ = *r->begin++;
	}

	delete l;
	delete r;

	return o;
}

// Sorting block im memory by 'QuickSorting' algorithm
LinesBlock *SortingOfLines::othersort(LinesBlock *m, size_t count)
{
	if (!m->valid)
	{
		return m;
	}

	unsigned char **ptr = (unsigned char **)_ptrMemoryBlock;

	unsigned char *src = m->begin;
	unsigned char *dst = (unsigned char *)_ptrMemoryBlock + sizeof(void *) * count;
	while (src < m->end)
	{
		// Save each ptr of line
		*ptr++ = dst;
		do
		{
			// Copy line to memory
			*dst++ = *src;
		}
		while (*src++ != '\n');
	}

	// Sorting
	qsort(_ptrMemoryBlock, count, sizeof(void *), comparePtr);

	// Recreate block as mapped temp file in memory
	if (m->readonly)
	{
		size_t size = m->end - m->begin;
		delete m;
		m = new LinesBlock(size);
	}

	ptr = (unsigned char **)_ptrMemoryBlock;
	dst = m->begin;

	for (size_t i = 0u; i < count; i++)
	{
		src = *ptr++;
		do
		{
			// Copy line to mmaped file
			*dst++ = *src;
		}
		while (*src++ != '\n');
	}

	return m;
}

// Sorting block by 'MergeSorting' algorithm
LinesBlock *SortingOfLines::mergesort(LinesBlock *m)
{
	if (!m->valid)
	{
		return m;
	}

	//
	if (m->begin + _sizeMemoryBlock > m->end)
	{
		int count = 0;
		for (unsigned char *p = m->begin; p < m->end; p++)
		{
			if (*p == '\n')
			{
				count++;
			}
		}
		if (m->begin + _sizeMemoryBlock - count * sizeof(void *) > m->end)
		{
			return othersort(m, count);
		}
	}

	unsigned char *mid;

	// Mid of block
	mid = m->begin + (m->end - m->begin) / 2;

	// Find begin of line to the right
	while (*mid++ != '\n') ;

	// Not found
	if (mid >= m->end)
	{
		// Mid of block
		mid = m->begin + (m->end - m->begin) / 2 - 1;

		// Find begin of line to the left
		while (mid >= m->begin && *mid != '\n') mid--;
		mid++;

		// Whole block is one line
		if (mid == m->begin)
		{
			return m;
		}
	}

	// Split block to two blocks
	LinesBlock *l = new LinesBlock(m->begin, mid, m->readonly);
	LinesBlock *r = new LinesBlock(mid, m->end, m->readonly);
	delete m;

	// Sorting each block
	l = mergesort(l);
	r = mergesort(r);

	// Merge blocks
	return merge(l, r);
}
