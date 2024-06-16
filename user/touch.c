#include <lib.h>

int main(int argc, char **argv) {

	if (argc != 2)
	{
		debugf("touch:缺少文件名\n");
		return -1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd >= 0) {
		debugf("touch:文件已存在\n");
		return -1;
	}

    fd = open(argv[1], O_CREAT);
	if (fd < 0) {
		printf("touch: cannot touch \'%s\': No such file or directory\n",argv[1]);
        return -1;
	}
	close(fd);
    return 0;
}

///////////////////////////// 实现文档 /////////////////////////////
/*
	1. 先调用user/lib/file.c中的open(path, O_RDONLY)打开文件，如果打开成功就说明文件已经存在，退出
	2. 如果打开失败，就再次调用open(path, O_CREAT)来创建这个文件，如果创建失败就打印提示信息
*/
///////////////////////////////////////////////////////////////////