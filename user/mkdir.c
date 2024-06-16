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
		descend_mkdir(newpath);
		// 3. create self
		descend_mkdir(path);
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

///////////////////////////// 实现文档 /////////////////////////////
/*
	1. 首先可能的指令类型有两种
		mkdir x
		mkdir -p x
		-p表示忽略错误信息，包括目录已存在时不打印提示信息，路劲不存在时递归创建路径
	2. 首先判断argv[1]是不是"-p\0"
		2.1 然后使用stat(path, &stat_buf)函数判断目录是否已经存在
			已经存在 + 无-p选项：打印提示信息
			已经存在 + -p选项：退出
		2.2 如果不存在
			先直接mkdir(path)
			mkdir(path)函数是自己写的，在user/lib/file.c中，并且需要在user/include/lib.h中声明
			这个函数就是调用file.c中的open()函数open(path, O_MKDIR)了一下
				实际上是与文件系统服务进程通信，调用了fs/serv.c中的serve_open()函数
				首先模仿O_CREAT的方法，直接复制一份，然后改成O_MKDIR
				然后创建完成后，如果是O_MKDIR，就要把f->f_type赋值为FTYPE_DIR
			这样可以完成单层目录的创建

			因为serv.c中的serve_open()中调用了fs/fs.c的file_create()，file_create()调用了walk_path()
			而walk_path()的原理是，如果路径中有一环不存在就会返回负值，因此文件系统服务进程自带的file_create()不能完成递归创建目录

			所以需要自己写一个descend_mkdir()函数
			该函数的递归出口是 fd = mkdir(path), 如果 fd >= 0 就是创建成功，说明此时在创建缺失路径的最顶层
			如果 fd < 0，就要去除最底层的目录，然后调用descend_mkdir()创建他的上一级目录，然后再重新调用descend_mkdir()创建自己

			最后根据有没有-p选项来选择在创建失败的时候是否要递归创建目录
*/
///////////////////////////////////////////////////////////////////