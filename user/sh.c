#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */

struct Job {
	int job_id; 
	char status [10]; 
	int env_id; 
	char cmd [100];
} jobs[100] = {0};



int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) {
		*s++ = 0;
	}
	if (*s == 0) {
		return 0;
	}

	if (strchr(SYMBOLS, *s)) {
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;

		///////////////// 2. 判断||和&& /////////////////
		if (t == '|' && (*s) == '|') {
			t = 1;
			*p1 = s;
			*s++ = 0;
			*p2 = s;
		} else if (t == '&' && (*s) == '&') {
			t = 2;
			*p1 = s;
			*s++ = 0;
			*p2 = s;
		}
		///////////////////////////////////////////////

		return t;
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		s++;
	}
	*p2 = s;
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

#define MAXARGS 128

int parsecmd(char **argv, int *rightpipe, int *need_ipc_send, int *need_ipc_recv, int *condi_or, int *condi_and, int *background) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int c = gettoken(0, &t);
		switch (c) {
		case 0:
			return argc;
		case 'w':
			if (argc >= MAXARGS) {
				debugf("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
			break;
		case '<':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: < not followed by word\n");
				exit();
			}
			// Open 't' for reading, dup it onto fd 0, and then close the original fd.
			// If the 'open' function encounters an error,
			// utilize 'debugf' to print relevant messages,
			// and subsequently terminate the process using 'exit'.
			/* Exercise 6.5: Your code here. (1/3) */
			//user_panic("< redirection not implemented");
			// 1. "<" 重定向
			// 2. 就是获取'<'之后的单词，打开对应的文件fd = open(t, O_RDONLY)
			// 3. 然后把这个文件复制到标准输入dup(fd, 0)
			// 4. 关闭文件即可close(fd)
			if((fd = open(t, O_RDONLY)) < 0) {
				debugf("failed to open %s\n");
				exit();
			}
			if((r = dup(fd, 0)) < 0) {
				debugf("failed to duplicate file to <stdin>\n");
				exit();
			}
			close(fd);
			break;
		case '>':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: > not followed by word\n");
				exit();
			}
			// Open 't' for writing, create it if not exist and trunc it if exist, dup
			// it onto fd 1, and then close the original fd.
			// If the 'open' function encounters an error,
			// utilize 'debugf' to print relevant messages,
			// and subsequently terminate the process using 'exit'.
			/* Exercise 6.5: Your code here. (2/3) */
			//user_panic("> redirection not implemented");
			if((fd = open(t, O_WRONLY | O_CREAT | O_TRUNC)) < 0) {
				debugf("failed to open %s\n");
				exit();
			}
			if((r = dup(fd, 1)) < 0) {
				debugf("failed to duplicate file to <stdout>\n");
				exit();
			}
			close(fd);
			break;
		///////////////////////////// 4. background ///////////////////////////////
		case '&':
			*background = 1;
			return argc;
			break;
		//////////////////////////////////////////////////////////////////////////
		case '|':;
			/*
			 * First, allocate a pipe.
			 * Then fork, set '*rightpipe' to the returned child envid or zero.
			 * The child runs the right side of the pipe:
			 * - dup the read end of the pipe onto 0
			 * - close the read end of the pipe
			 * - close the write end of the pipe
			 * - and 'return parsecmd(argv, rightpipe)' again, to parse the rest of the
			 *   command line.
			 * The parent runs the left side of the pipe:
			 * - dup the write end of the pipe onto 1
			 * - close the write end of the pipe
			 * - close the read end of the pipe
			 * - and 'return argc', to execute the left of the pipeline.
			 */
			int p[2];
			/* Exercise 6.5: Your code here. (3/3) */
			//user_panic("| not implemented");
			// 1. 创建管道
			if((r = pipe(p)) < 0) {
				debugf("failed to create pipe\n");
				exit();
			}
			// 2. 子进程
			// 2. 把子进程的fdnum,拷贝给fd[0]
			if((*rightpipe = fork()) == 0) {
				// printf("现在进入子进程\n");
				dup(p[0], 0);
				close(p[0]);
				close(p[1]);
				// printf("进程进入下一层parsecmd\n");
				return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, background);
			// 3. 父进程
			// 3. 把父进程的fdnum，拷贝给fd[1]
			} else {
				// printf("现在进入父进程\n");
				dup(p[1], 1);
				close(p[1]);
				close(p[0]);
				// printf("父进程返回argc为%d\n",argc);
				return argc;
			}
			break;
		///////////////////////////// 3. 对于 || 和 && /////////////////////////////
		case 1:;
			// 子进程
			if(fork() == 0){
				// printf("现在进入子进程\n");
				*need_ipc_send = 1;
				// printf("子进程进入下一层parsecmd\n");
				return argc;
			} else {
				// printf("现在进入父进程\n");
                ipc_recv(0,0,0);
                if (env->env_ipc_value != 0){
                    // 没成，就要spawn子进程
                    *need_ipc_recv = 1;
				    *condi_or = 1;
                    return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, background);
                } else {
                    while((c = gettoken(0, &t)) != 0){
						if (c == 1 || c == 2) {
							break;
						}
					}
                    *need_ipc_recv = 1;
                    if (c == 1) {
						*condi_or = 1;
					} else if (c == 2) {
						*condi_and = 1;
					}
                    return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, background);
                }
				// printf("父进程返回argc为%d\n",argc);
			}
			break;
		case 2:;
			if(fork() == 0){
				// printf("现在进入子进程\n");
				*need_ipc_send = 1;
				// printf("子进程进入下一层parsecmd\n");
				return argc;
			} else {
				// printf("现在进入父进程\n");
				ipc_recv(0,0,0);
				if (env->env_ipc_value == 0) {
					// 成了，就要spawn子进程
					*need_ipc_recv = 1;
					*condi_and = 1;
					return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, background);
				} else {
					while((c = gettoken(0, &t)) != 0){
						if (c == 1 || c == 2) {
							break;
						}
					}
					*need_ipc_recv = 1;
					if (c == 1) {
						*condi_or = 1;
					} else if (c == 2) {
						*condi_and = 1;
					}
					return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, background);
				}
				// printf("父进程返回argc为 %d \n",argc);
			}
			break;
		//////////////////////////////////////////////////////////////////////////
		}
	}

	return argc;
}

int atoi(char *s) {
    int ret = 0;
    while (*s) {
        ret = ret * 10 + (*s++ - '0');
    }
    return ret;
}

void runcmd(char *s) {
	char new[100] = {0};
	strcpy(new, s);
	gettoken(s, 0);

	if (strcmp(s,"jobs")==0) {
		syscall_get_jobs(1,0,NULL,NULL,0);
		exit();
	}
	// debugf("%s没有进入jobs\n",s);

	if (s[0] == 'f' && s[1] == 'g' && s[2] == ' ') {
		// 1. get envid
		int job_id = atoi(s+3);
		// 2. has envid
		int job_envid = syscall_get_job_envid(job_id, env->env_id);
		// 3. bring to front
		if (job_envid != -1) {
			int status = syscall_get_job_status(job_id);
			if (status == 1) {
				int tuoguanenvid = syscall_get_tuoguan(job_id);
				ipc_send(tuoguanenvid,99,0,0);
				ipc_recv(0,0,0);
			} else {
				printf("fg: (0x%08x) not running\n", job_envid);
			}
		} else {
			printf("fg: job (%d) do not exist\n", job_id);
		}

		exit();
	}

	if (s[0] == 'k' && s[1] == 'i' && s[2] == 'l' && s[3] == 'l' && s[4] == ' ') {
		int job_id = atoi(s+5);
		// debugf("工作id = %d\n",job_id);
		syscall_kill_job(job_id);
		// debugf("kill完毕\n");
		exit();
	}

	char *argv[MAXARGS];
	int rightpipe = 0;
	int need_ipc_send = 0;
	int need_ipc_recv = 0;
	int condi_or = 0;
	int condi_and = 0;
	int background = 0;
	int argc = parsecmd(argv, &rightpipe, &need_ipc_send, &need_ipc_recv, &condi_or, &condi_and, &background);
	if (argc == 0) {
		return;
	}
	argv[argc] = 0;

	///////////////// 查看条件执行 /////////////////
	// printf("一共有%d个指令, 并且rightpipe = %x\n",argc, rightpipe);
	// for(int i = 0;i < argc;i++) {
	// 	printf("%s\n",argv[i]);
	// }
	// printf("进程%x的rightpipe=%x, need_ipc_send=%x, need_ipc_recv=%d, condi_or=%d, condi_and=%d\n",env->env_id, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and);
	// printf("进程的background = %d\n",background);
	//////////////////////////////////////////////	

	int child = -1;
	if (need_ipc_recv != 0) {
		if (condi_or && env->env_ipc_value != 0) {
			child = spawn(argv[0], argv);
		} else if (condi_and && env->env_ipc_value == 0) {
			child = spawn(argv[0], argv);
		}
	} else {
		if (background == 0) {
			child = spawn(argv[0], argv);
		} else {
			// 1. 子进程处理sleep
			if(fork()==0){
				// debugf("由%x进程托管\n",env->env_id);
				int len = strlen(argv[argc-1]);
				argv[argc-1][len] = '&';
				argv[argc-1][len+1] = '\0';
				// 1. 创建子进程
				child = spawn(argv[0], argv);
				// 2. 写进后台
				syscall_get_jobs(2, child, "Running", new, env->env_id);
				// 3. 等待子进程结束：如果提前接收到99的值，就说明要回到前台
				ipc_recv(0,0,0);
				// debugf("我回来了,value = %d\n",env->env_ipc_value);
				if (env->env_ipc_value == 99) {
					int father = env->env_ipc_from;
					// debugf("%x让我fg,我是%x\n",father,env->env_id);
					ipc_recv(0,0,0);
					// wait(child);
					// debugf("等完了\n");
					syscall_get_jobs(3,child,NULL,NULL,0);
					// debugf("子进程内ipc_send什么情况 %x\n", father);
					ipc_send(father,0,0,0);
				} else if (env->env_ipc_value == 100){
					// debugf("我回来了2\n");
					syscall_get_jobs(3,child,NULL,NULL,0);
					exit();
				} else {
					syscall_get_jobs(3,child,NULL,NULL,0);
				}
			// 2. 父进程直接走人
			} else {

			}
		}
	}
	// printf("%s的进程id是%x\n",argv[0],child);
	close_all();
	if (background == 0) {
		if (child >= 0) {
			ipc_recv(0,0,0);
			//wait(child);
		} else {
			debugf("spawn %s: %d\n", argv[0], child);
		}
	} else {
		// debugf("%s\n",new);
		// syscall_get_jobs(2, child, "Running", new, NULL);
	}

	if (rightpipe) {
		wait(rightpipe);
	}

	if (need_ipc_send) {
		ipc_send(env->env_parent_id, env->env_ipc_value, 0,0);
	}

	exit();
}

void readline(char *buf, u_int n) {
	int r;
	for (int i = 0; i < n; i++) {
		if ((r = read(0, buf + i, 1)) != 1) {
			if (r < 0) {
				debugf("read error: %d\n", r);
			}
			exit();
		}
		if (buf[i] == '\b' || buf[i] == 0x7f) {
			if (i > 0) {
				i -= 2;
			} else {
				i = -1;
			}
			if (buf[i] != '\b') {
				printf("\b");
			}
		}
		if (buf[i] == '\r' || buf[i] == '\n') {
			buf[i] = 0;
			return;
		}
	}
	debugf("line too long\n");
	while ((r = read(0, buf, 1)) == 1 && buf[0] != '\r' && buf[0] != '\n') {
		;
	}
	buf[0] = 0;
}

char buf[1024];

void usage(void) {
	printf("usage: sh [-ix] [script-file]\n");
	exit();
}

int main(int argc, char **argv) {
	int r;
	int interactive = iscons(0);
	int echocmds = 0;
	printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	printf("::                                                         ::\n");
	printf("::                     MOS Shell 2024                      ::\n");
	printf("::                                                         ::\n");
	printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	ARGBEGIN {
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
	ARGEND

	if (argc > 1) {
		usage();
	}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[0], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[0], r);
		}
		user_assert(r == 0);
	}
	for (;;) {
		if (interactive) {
			printf("\n$ ");
		}
		readline(buf, sizeof buf);

		if (buf[0] == '#') {
			continue;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}
		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) {
			runcmd(buf);
			exit();
		} else {
			wait(r);
		}
	}
	return 0;
}

///////////////////////////// 实现文档 /////////////////////////////
/*
	1. 首先要清楚shell的调用流程
		首先在user/init.c中spwanl("sh.b","sh",NULL),开启了运行sh.b程序的进程
		然后sh.b中的main()函数负责不断从标准输入逐行读取输入的指令到char *buf
		然后使用runcmd(buf)来运行指令

		runcmd(buf)中调用parsecmd(argv, &rightpipe)来解析指令
		parsecmd(argv, &rightpipe)中不断循环调用gettoken()来解析命令
		gettoken()实际上调用_gettoken()来判断每次解析到的单词是是什么类型
			一共有三种类型
				0: 代表解析解结束
				t: 代表解析到特殊字符
					这里对于<,|,>,&,;,(,)直接返回ascii码就可以
					对于||和&&,也选择在_gettoken()中完成
						具体方式是识别到|或者&的时候，再多走一步判断下一个字符是不是|或者&
						对于||,我们返回1
						对于&&,我们返回2
						因为在ascii码表中这两个值被保留
				'w': 代表解析到一个单词
		然后再parsecmd()中增加能解析指令的case情况
			case 1: 代表读取到了||
			case 2: 代表读取到了&&
	2.	然后弄清楚管道的进程创建
		管道命令 ls | cat
		是由sh.b先fork()出一个子进程
			子进程去进一步parsecmd(),从而spawn出cat命令
			sh.b返回原进程，spawn出ls命令
			然后通过管道开关读写端实现ls把输出传递给cat
		
		条件指令可以采用一样的形式，只不是管道中，越靠前的指令由父进程创建，越靠后的指令由子进程创建
		条件指令中我们需要获取靠前指令的执行结果，并且返回给后面的指令，因为前面的指令要用子进程创建，然后子进程给父进程发送信息，父进程再决定后面的指令可不可以执行
		首先fork()出一个子进程，设置need_ipc_send,然后返回
			去spawn一个命令
			这个命令通过修改libos.c中的libmain,给子进程发送了直接结果的信息
			然后子进程通过if(child>0){ ipc_recv(0,0,0); }来等待spawn出的命令发送信息
			等到了之后，进入if(need_ipc_send)来给父进程发送信息

		然后回到parsecmd()中case 2的父进程分支
			父进程在此处ipc_recv(0,0,0)等待子进程发送信息
			结合env->env_ipc_value和现在是case 1 还是case 2 来决定是否要跳过下一个指令，直到找到下一个 case 1 或者 case 2
				跳过的方法就是在case内部调用c = gettoken(0,&t) != 0, 循环直到有 c == 1 或者 c == 2
				然后设置need_ipc_recv为1，这样返回runcmd的时候，才会进入结合condi_or或者condi_and来判断是否可以spawn这个指令
				然后设置condi_or或者condi_and其中一个为1
				然后继续parsecmd()

				这个时候往下走一层
				如果这一层只有一条指令，就会直接在case 'w'中返回，最终指令由父进程创建并执行
				如果这一层有两条指令，就会进入case 2，然后fork()新的子进程去执行中间那条指令，父进程继续ipc_recv(0,0,0)等待
				而且由于跳过了不允许执行的指令，因此有子进程的spawn出来的都是可以执行的指令，这个时候这些指令执行的返回值==0或者!=0再由子进程发送给父进程
				父进程再决定下一层的指令是创建还是跳过
				
*/
///////////////////////////////////////////////////////////////////
