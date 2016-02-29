/*
 * LinesBlock.hpp
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: Dmitriy Khaustov aka Dиmon (khaustov.dm@gmail.com)
 */

#ifndef SRC_LINESBLOCK_HPP_
#define SRC_LINESBLOCK_HPP_

#include <stddef.h>

class LinesBlock
{
public:
	/// Begin of block
	unsigned char *begin;
	/// End of block
	unsigned char *end;

	/// Pointer to memory block where maped temp files
	void *mmap_ptr;
	/// Size of memory block where maped temp files
	size_t mmap_size;
	/// Path to temp file
	char *mmap_file;

	bool readonly;

	bool valid;

	/// Creare by memory block
	LinesBlock(unsigned char *b, unsigned char *e, bool readonly = false);
	/// Create memory block by mapping tem file
	LinesBlock(size_t s);

	virtual ~LinesBlock();
};

#endif /* SRC_LINESBLOCK_HPP_ */
