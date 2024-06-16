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

///////////////////////////// 实现文档 /////////////////////////////
/*
	1. 首先根据可能的情况
		rm x
		rm -r x
		rm -rf x
		判断是否有-r或者-rf，然后对没有选项，有-r选项，有-rf选项分别处理
	2.1 有-r选项
		user/lib/file.c中的remove(path)函数实际上与文件系统服务进程进行通信，然后调用fs/fs.c中的file_remove()函数
		该函数可以删除文件，也可以删除目录
		因此直接调用remove(argv[2])即可实现递归删除
		如果删除失败则打印提示信息
	2.2 有-rf选项
		直接删除即可
		无论成功失败都不打印提示信息
	2.3 没有选项
		要先判断需要删除的是否是目录
		首先fd = open (path, O_RDONLY)打开该文件，获取文件描述符下标
		然后fd_lookup(fd, &fd_)寻找文件描述符，需要引入<fd.h>库
		然后把&fd_强制转换为struct Filefd *ffd;
		然后获取ffd->f_file.f_type，判断ffd->f_file.f_type == FTYPE_DIR
		如果是目录就删除失败，打印提示信息
		否则就说明是文件类型，删除即可，删除失败打印提示信息
*/
///////////////////////////////////////////////////////////////////