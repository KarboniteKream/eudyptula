#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>

long eudyptula(unsigned int high_id, unsigned int low_id)
{
	return syscall(443, high_id, low_id);
}

int main(void)
{
	long ret;

	ret = eudyptula(0x2dfe, 0xa3d4488b);
	printf("2dfea3d4488b: ret=%ld, errno=%d\n", ret, errno);

	ret = eudyptula(0x123, 0x456);
	printf("123456: ret=%ld, errno=%d\n", ret, errno);

	return 0;
}
