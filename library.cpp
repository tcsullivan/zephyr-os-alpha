#include <library.hpp>

void strncpy(char *dst, const char *src, int cnt)
{
	int i = 0;
	do {
		dst[i] = src[i];
	} while (++i < cnt && src[i] != 0);
}

int strcpy(char *dst, const char *src)
{
	int i = 0;
	while (*dst && *src)
		*dst++ = *src++, i++;

	return i;
}

char *itoa(int n, int base)
{
	static char buf[16];
	char *p = buf + 16;

	*--p = '\0';
	do {
		*--p = "0123456789abcdef"[n % base];
	} while (n /= base);

	return p;
}
