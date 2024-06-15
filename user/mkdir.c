#include <lib.h>

int main(int argc, char **argv) {
    if (argc != 2)
	{
		debugf("mkdir:缺少目录名\n");
		return -1;
	}
	int fd;
	fd = open(argv[1], O_RDONLY);
	if (fd >= 0)
	{
		debugf("mkdir: cannot create directory \'%s\': File exists\n",argv[1]);
		return -1;
	}
	if (create(argv[1], FTYPE_DIR) < 0)
	{
		debugf("mkdir: cannot create directory \'%s\': No such file or directory\n",argv[1]);
        return -1;
	}
    return 0;
}