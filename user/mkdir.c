#include <lib.h>

int main(int argc, char **argv) {

    if (argc != 2) {
		debugf("mkdir:缺少目录名\n");
		return -1;
	}
	
	struct Stat stat_buf;
	if (stat(argv[1], &stat_buf) >= 0) {
		printf("mkdir: cannot create directory \'%s\': File exists\n", argv[1]);
		return -1;
	}

    int fd = mkdir(argv[1]);
	if (fd < 0) {
		printf("mkdir: cannot create directory \'%s\': No such file or directory\n", argv[1]);
        return -1;
	}

    return 0;
}