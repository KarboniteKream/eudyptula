#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>

#define FAT_IOCTL_SET_VOLUME_LABEL _IOW('r', 0x14, char *)

int main(int argc, char **argv)
{
	if (argc < 3)
		return 1;

	int fd = open(argv[1], O_NONBLOCK);
	char *argp = argv[2];

	if (fd < 0)
		return 1;

	int ret = ioctl(fd, FAT_IOCTL_SET_VOLUME_LABEL, argp);
	printf("ret=%d, errno=%d\n", ret, errno);

	return 0;
}
