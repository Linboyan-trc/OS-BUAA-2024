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

int parsecmd(char **argv, int *rightpipe, int *need_ipc_send, int *need_ipc_recv, int *condi_or, int *condi_and) {
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
				return parsecmd(argv, rightpipe, NULL, NULL, NULL, NULL);
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
                    return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and);
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
                    return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and);
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
					return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and);
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
					return parsecmd(argv, rightpipe, need_ipc_send, need_ipc_recv, condi_or, condi_and);
				}
				// printf("父进程返回argc为 %d \n",argc);
			}
			break;
		//////////////////////////////////////////////////////////////////////////
		}
	}

	return argc;
}

void runcmd(char *s) {
	gettoken(s, 0);

	char *argv[MAXARGS];
	int rightpipe = 0;
	int need_ipc_send = 0;
	int need_ipc_recv = 0;
	int condi_or = 0;
	int condi_and = 0;
	int argc = parsecmd(argv, &rightpipe, &need_ipc_send, &need_ipc_recv, &condi_or, &condi_and);
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
	}
	// printf("%s的进程id是%x\n",argv[0],child);
	close_all();
	if (child >= 0) {
		ipc_recv(0,0,0);
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
