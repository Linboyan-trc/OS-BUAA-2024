#include <lib.h>

int main(int argc, char **argv) {
	if (argc != 2)
	{
		debugf("touch:缺少文件名\n");
		return -1;
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd >= 0)
	{
		debugf("touch:文件已存在\n");
		return -1;
	}

    fd = open(argv[1], O_CREAT);
	if (fd < 0)
	{
		printf("touch: cannot touch \'%s\': No such file or directory\n",argv[1]);
        return -1;
	}
    return 0;
}