#include <args.h>
#include <lib.h>

void descend_mkdir(char *path) {
	int fd = mkdir(path);
	if (fd > 0) {
		return;
	}
	
	if (fd < 0) {
		// 1. find last '\'
		char newpath[1024];
		strcpy(newpath, path);
		int len = strlen(newpath);
		for(int i = len - 1;i >= 0;i--) {
			if(newpath[i] == 47) {
				newpath[i] = '\0';
				break;
			}
		}
		// 2. create father
		mkdir(newpath);
		// 3. create self
		mkdir(path);
	}
}

int main(int argc, char **argv) {

	debugf("mkdir:argc=%d, argv[0]=%s, argv[1]=%s\n",argc, argv[0],argv[1]);

	int p = 0;
	if (argv[1][0]=='-' && argv[1][1]=='p' && argv[1][2]=='\0') {
		argc--;
		p = 1;
	}

    if (argc != 2) {
		debugf("mkdir:缺少目录名\n");
		return -1;
	}
	
	struct Stat stat_buf;
	if (stat(argv[1+p], &stat_buf) >= 0) {
		if (p == 0) {
			printf("mkdir: cannot create directory \'%s\': File exists\n", argv[1+p]);
			return -1;
		} else {
			return 0;
		}
	}

    int fd = mkdir(argv[1+p]);
	if (fd < 0) {
		if (p == 0) {
			printf("mkdir: cannot create directory \'%s\': No such file or directory\n", argv[1+p]);
			return -1;
		} else {
			descend_mkdir(argv[1+p]);
			return 0;
		}
	}

    return 0;
}