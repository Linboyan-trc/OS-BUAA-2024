#include <lib.h>

int main(int argc, char **argv) {

    if (argc != 2)
	{
		debugf(1, "mkdir:缺少目录名\n");
		return -1;
	}
	int fd;
	fd = open(argv[1], O_RDONLY);
	if (fd >= 0)
	{
		printf(1, "mkdir: cannot create directory \'%s\': File exists\n",argv[1]);
		return -1;
	}
    fd = open(argv[1], O_CREAT);
	if (fd < 0)
	{
		printf(1, "mkdir: cannot create directory \'%s\': No such file or directory\n", argv[1]);
        return -1;
	}

    return 0;
}