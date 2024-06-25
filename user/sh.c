#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()#`"

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
		} else if (t == '>' && (*s) == '>') {
			t = 3;
			*p1 = s;
			*s++ = 0;
			*p2 = s;
		}
		///////////////////////////////////////////////

		return t;
	}

	if (*s == '\"') {
		*s++ = 0;
		*p1 = s;
		
		s++;
		while(*s != '\"') {
			s++;
		}

		*s++ = 0;
		*p2 = s;
		return 'w';
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

int parsecmd(char **argv, int *rightpipe, int *need_ipc_send, int *need_ipc_recv, int *condi_or, int *condi_and, int *bg, int *backquote, int *backpipe) {
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
		case '&':
			*bg = 1;
			if (fork() == 0) {
				*bg = 2;
				return argc;
			} else {
				return argc;
			}
		case '#':
			return argc;
		case ';':;
			int temp_r = 0;
			if ((temp_r = fork()) == 0) {
				return argc;
			} else {
				wait(temp_r);
				return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, bg, backquote, backpipe);
			}
			break;
		case '`':;
			// 2. 结束则返回
			if (*backquote == 1) {
				// debugf("现在在子进程:%s\n",argv[0]);
				return argc;
			}
			int p_quote[2];
			pipe(p_quote);
			// 1. 创建子进程执行`...`指令
			if ((r = fork()) == 0) {
				// 1.执行
				*backquote = 1;
				// 2. 管道
				// dup(p_quote[1], 1);
				// close(p_quote[1]);
				// close(p_quote[0]);
				return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, bg, backquote, &p_quote[1]);
			} else {
				// 2. 父进程读取执行结果，加入到argv
				close(p_quote[1]);
				char tempbuf[1024] = {0};
				int i_buf = 0;
				while (1) {
					int len = read(p_quote[0], tempbuf + i_buf, 1024 - i_buf);
					if (len <= 0) {
						break;
					}
					i_buf += len;
				}
				tempbuf[i_buf] = 0;
				close(p_quote[0]);
				// 2. 先等待子进程完成
				wait(r);
				argv[argc++] = tempbuf;
				// debugf("父进程读tempbuf读出来是:%s\n",tempbuf);
				// debugf("此时父进程的字符串是%c 或 %c\n",c, &t);
				// 3. 跳到下一个'`'
				while((c = gettoken(0, &t)) != '`') { }
			}
			// 3. 继续解析
			break;
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
				return parsecmd(argv, rightpipe, NULL, NULL, NULL, NULL, NULL, backquote, backpipe);
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
                    return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, bg, backquote, backpipe);
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
                    return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, bg, backquote, backpipe);
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
					return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, bg, backquote, backpipe);
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
					return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and, bg, backquote, backpipe);
				}
				// printf("父进程返回argc为 %d \n",argc);
			}
			break;
		//////////////////////////////////////////////////////////////////////////
		case 3:;
			// 1. 获取文件
			c = gettoken(0, &t);
			int fd_temp = open(t, O_APPEND | O_RDWR);
			if (fd_temp < 0) {
				fd_temp = open(t, O_CREAT);
				fd_temp = open(t, O_APPEND | O_RDWR);
			}
			// 2. 修改输出路径
			debugf("现在输出路径是%d\n",fd_temp);
			dup(fd_temp, 1);
			/////// test //////
			// int temp2 = open("aaa.txt", O_RDWR);
			// dup(temp2, 1);
			///////////////////
			break;
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

int isfg(char *s) {
	if (s[0]=='f' && s[1]=='g') {
		return 0;
	}
	return 1;
}

int iskill(char *s) {
	if (s[0]=='k' && s[1]=='i' && s[2]=='l' && s[3]=='l') {
		return 0;
	}
	return 1;
}

void runcmd(char *s) {
	/////////////////// copy s ///////////////////
	char news[128] = "\0";
	strcpy(news,s);
	//////////////////////////////////////////////	
	gettoken(s, 0);

	//////////////// bulid in cmd ////////////////
	if (strcmp(s,"jobs")==0) {
		syscall_print_jobs();

		syscall_add_history(news);
		exit();
	} else if (isfg(s) == 0) {
		int job_id = atoi(s+3);
		int envid = syscall_find_envid(job_id);
		if (envid != -1) {
			wait(envid);
		}

		syscall_add_history(news);
		exit();
	} else if (iskill(s) == 0) {
		int job_id = atoi(s+5);
		syscall_kill_job(job_id);

		syscall_add_history(news);
		exit();
	}
	//////////////////////////////////////////////	

	char *argv[MAXARGS];
	int rightpipe = 0;
	int need_ipc_send = 0;
	int need_ipc_recv = 0;
	int condi_or = 0;
	int condi_and = 0;
	int bg = 0;
	int backquote = 0;
	int backpipe = 0;
	int argc = parsecmd(argv, &rightpipe, &need_ipc_send, &need_ipc_recv, &condi_or, &condi_and, &bg, &backquote, &backpipe);
	if (argc == 0) {
		return;
	}
	argv[argc] = 0;

	//////////////// bulid in cmd ////////////////
	// 1. 执行历史指令
	// 2. 等待子进程结束再退出
	if (strcmp(argv[0],"history") == 0) {
		syscall_add_history(news);
		syscall_print_history();
		if (rightpipe) {
			// wait(rightpipe);
			exit();
		} else {
			exit();
		}
	}
	//////////////////////////////////////////////	

	///////////////////// bg /////////////////////
	if (bg == 1) {

		syscall_add_history(news);
		exit();
	}
	//////////////////////////////////////////////	

	///////////////// 查看条件执行 /////////////////
	// printf("一共有%d个指令, 并且rightpipe = %x\n",argc, rightpipe);
	// for(int i = 0;i < argc;i++) {
	// 	printf("%s\n",argv[i]);
	// }
	// printf("进程%x的rightpipe=%x, need_ipc_send=%x, need_ipc_recv=%d, condi_or=%d, condi_and=%d\n",env->env_id, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and);
	//////////////////////////////////////////////	

	int child = -1;
	if (need_ipc_recv != 0) {
		if (condi_or && env->env_ipc_value != 0) {
			child = spawn(argv[0], argv);
		} else if (condi_and && env->env_ipc_value == 0) {
			child = spawn(argv[0], argv);
		}
	} else {
		child = spawn(argv[0], argv);
		// debugf("创建ls子进程:%x\n",child);
		if (bg == 2) {
			syscall_add_jobs(child,news);
		}
	}
	// printf("%s的进程id是%x\n",argv[0],child);
	close_all();
	if (child >= 0) {
		// debugf("等待接受子进程消息\n");
		/////////////////// backquote ///////////////////
		if (rightpipe == 0 && backquote == 1) {
				// 2. 管道
				dup(backpipe, 1);
				close(backpipe);
				close(backpipe);
		}
		/////////////////////////////////////////////////
		ipc_recv(0,0,0);
		if (bg == 2) {
			syscall_finish_jobs(child);
		}
		//wait(child);
	} else {
		debugf("spawn %s: %d\n", argv[0], child);
	}

	if (rightpipe) {
		wait(rightpipe);
	}

	if (need_ipc_send) {
		ipc_send(env->env_parent_id, env->env_ipc_value, 0,0);
	}

	if (backquote == 0) {
		syscall_add_history(news);
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

///////////////////////////// 条件指令实现文档 /////////////////////////////
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

//////////////////////////////// 反引号实现文档 ///////////////////////////////////
/*
	1. 识别反引号
	2. 子进程执行反引号内指令
		传递管道
		为backquote 而且 rightpipe == 0的时候再复制dup(p_quote[1],1),把写端从stdout改成p)quote[1]
	3. 父进程读取执行结果
		父进程从dup(p_quote[0],0)把从stdin读改成从p_quote[0]读
		读取的时候因为管道只有32字节
		因此需要使用while(1) { read(p_quote[0], tempbuf + i_buf, 1024 - i_buf); }一段一段的读取，不能使用read()一次性读完
		读完之后作为argv[argc++] = tempbuf加入到参数中
		最后还要使父进程的gettoken走到下一个'`'
*/
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// 历史指令实现文档 ///////////////////////////////////
/*
	1. 使用内核实现历史指令，历史指令在内核中
	2. 首先考虑 history | cat的情况
		所有history的识别和执行要在parsecmd之后，这样有"|"的时候才会输出到p[1]
		而不是直接进以来就识别history
	3. 然后所有的指令退出前要加入到历史指令中
		在exit()前syscall_history_add()一下
		对于内建指令，更早exit()，也要更早history_add()
		对于`ls | cat | cat`这种指令，会在顶层 echo ... 123 的exit()之前history_add()
		因此只要是bakcquote == 1，那就不需要history_add()
	4. 对于history指令本身
		根据需求，需要在打印历史指令之前就history_add()
*/
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// 一行多指令实现文档 ///////////////////////////////////
/*
	1. ls ; ls
	2. 第一个ls开一个子进程去执行; 第二个ls父进程执行
*/
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////// 后台实现文档 ///////////////////////////////////
/*
    1.  通过parsecmd中增加case &，来修改background值
        有background值直接不ipc_recv(0,0,0),当前sh.b直接exit()
    2.  对于后台程序
        开一个子进程托管
        先关信息记录在内核中
    3.  然后fg和kill指令就调用内核指令去找到这个进程
        fg的话就让当前sh.b等待，直到子进程运行完毕
        kill的话直接杀死进程，结束sh.b
*/
///////////////////////////////////////////////////////////////////////////////

