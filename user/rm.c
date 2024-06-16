#include <lib.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		debugf("rm:缺少文件名\n");
	}

	struct Stat stat_buf;
	if (stat(argv[1], &stat_buf) >= 0) {
		printf("rm: cannot remove \'%s\': Is a directory\n", argv[1]);
	}

	int r = remove(argv[1]);
	if (r < 0) {
		printf("rm: cannot remove \'%s\': No such file or directory\n",argv[1]);
		return -1;
	}

	return 0;
}