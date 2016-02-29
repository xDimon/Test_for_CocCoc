/*
 * SortingOfLines.hpp
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: Dmitriy Khaustov aka Dиmon (khaustov.dm@gmail.com)
 */

#ifndef SRC_SORTINGOFLINES_HPP_
#define SRC_SORTINGOFLINES_HPP_

#include <stddef.h>

class LinesBlock;

class SortingOfLines
{
private:
	void *_ptrMemoryBlock;
	size_t _sizeMemoryBlock;

	static int compare(const void *l, const void *r);
	static inline int comparePtr(const void *l, const void *r)
	{
		return compare(*(const unsigned char **)l, *(const unsigned char **)r);
	}

	LinesBlock *merge(LinesBlock *l, LinesBlock *r);
	LinesBlock *othersort(LinesBlock *m, size_t count);
	LinesBlock *mergesort(LinesBlock *m);

public:
	SortingOfLines(size_t memoryLimit);
	virtual ~SortingOfLines();

	inline LinesBlock *operator()(LinesBlock *m)
	{
		return mergesort(m);
	}
};

#endif /* SRC_SORTINGOFLINES_HPP_ */
