/*
 * btac - print file in reverse order
 *
 * To the extent possible under law, Leah Neukirchen <leah@vuxu.org>
 * has waived all copyright and related or neighboring rights to this work.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* This code uses mmap, which gives better performance than e.g. pread on Linux.
   The buffer is reversed into another, by reading the source backwards and
   writing forwards.  Enabling the widest vector processing instruction set is
   recommended. */

#define N 32768
#define PREFETCH (32*N)

char src[N], dst[N];

static void
mem_rev(char *dst, const char *src, size_t len)
{
	for (size_t i = 0; i < len; i++)
		dst[i] = src[len-1-i];
}

int
main()
{
	struct stat st;
	fstat(0, &st);

	if (!S_ISREG(st.st_mode)) {
		errno = ESPIPE;
		perror("not a regular file");
		exit(1);
	}

	char *file = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, 0, 0);
	if (file == MAP_FAILED) {
		perror("mmap failed");
		exit(1);
	}

	off_t start = st.st_size;

	size_t blen = start % N ? start % N : N;
	while (start > 0) {
		start -= blen;

		if (start > PREFETCH)
			madvise(file + start - PREFETCH, PREFETCH,
			    MADV_WILLNEED);
		mem_rev(dst, file + start, blen);
		if (write(1, dst, blen) < (ssize_t)blen) {
			perror("short write");
			exit(1);
		}

		blen = N;
	}
}
