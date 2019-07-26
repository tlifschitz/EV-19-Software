/*
 * Copyright © 2019 Embedded Artistry LLC.
 * License: MIT. See LICENSE file for details.
 */

#include <assert.h>
#include <malloc.h>
#include <memory.h>



__attribute__((weak)) void malloc_init(void)
{
	assert(0 && "malloc is not supported.");
}

__attribute__((weak)) void* malloc(size_t __attribute__((unused)) size)
{
	assert(0 && "malloc is not supported");
	return NULL;
}

__attribute__((weak)) void free(void* __attribute__((unused)) ptr)
{
	assert(0 && "malloc is not supported");
	while(1)
	{
		// Loop forever
	}
}
