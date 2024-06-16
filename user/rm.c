#include <lib.h>
#include <fd.h>

int main(int argc, char **argv) {

	int isr = 0;
	int isrf = 0;
	if (argv[1][0] == '-' && argv[1][1] == 'r' && argv[1][2] == '\0') {
		argc--;
		isr = 1;
	} else if (argv[1][0] == '-' && argv[1][1] == 'r' && argv[1][2] == 'f' && argv[1][3] == '\0') {
		argc--;
		isrf = 1;
	}

	int r = 0;
	if (isr) {
		r = remove(argv[2]);
		if (r < 0) {
			printf("rm: cannot remove \'%s\': No such file or directory\n",argv[2]);
			return -1;
		}
	} else if (isrf) {
		remove(argv[2]);
		return 0;
	} else {
		if (argc != 2) {
			debugf("rm:缺少文件名\n");
			return -1;
		}

		// struct Stat stat_buf;
		// if (stat(argv[1], &stat_buf) >= 0) {
		// 	printf("rm: cannot remove \'%s\': Is a directory\n", argv[1]);
		// 	return -1;
		// }

		int fd = open(argv[1],O_RDONLY);
		if (fd >= 0) {
			struct Fd *fd_;
			fd_lookup(fd, &fd_);
			struct Filefd *fdd = (struct Filefd*)fd_;
			if (fdd->f_file.f_type == FTYPE_DIR) {
				printf("rm: cannot remove \'%s\': Is a directory\n", argv[1]);
				return -1;
			}
		}

		r = remove(argv[1]);
		if (r < 0) {
			printf("rm: cannot remove \'%s\': No such file or directory\n",argv[1]);
			return -1;
		}
	}

	return 0;
}