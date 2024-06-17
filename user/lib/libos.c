#include <env.h>
#include <lib.h>
#include <mmu.h>

void exit(void) {
	// After fs is ready (lab5), all our open files should be closed before dying.
#if !defined(LAB) || LAB >= 5
	close_all();
#endif

	syscall_env_destroy(0);
	user_panic("unreachable code");
}

const volatile struct Env *env;
extern int main(int, char **);

void libmain(int argc, char **argv) {
	// set env to point at our env structure in envs[].
	env = &envs[ENVX(syscall_getenvid())];

	// call user main routine
	// 1. if has & , argc--, argv--
	// debugf("进来了吗1\n");
	// int has_background = 0;
	// if (argc >= 2) {
	// 	// debugf("进来了吗2\n");
	// 	int len = strlen(argv[argc-1]);
	// 	if (argv[argc-1][len-1] == '&') {
	// 		// debugf("进来了吗3\n");
	// 		// debugf("开始的参数%s\n",argv[argc-1]);
	// 		has_background = 1;
	// 		argv[argc-1][len-1] = '\0';
	// 		// debugf("最终的参数%s\n",argv[argc-1]);
	// 		// debugf("has_background = %d\n",has_background);
	// 	} 
	// }
	int r = main(argc, argv);
	// debugf("has_background = %d\n",has_background);

	//////////////////// 1. 进程退出前给父进程发消息 /////////////////////
	// printf("进程%x给父进程%x发送信息%d\n",env->env_id, env->env_parent_id,r);

	// if (has_background == 0) {
		// debugf("有ipc\n");
	// debugf("sleep内ipc_send什么情况 %x给父进程%x发信息\n",env->env_id,env->env_parent_id);
	if (syscall_get_kill_envid() != env->env_id) {
		ipc_send(env->env_parent_id, r, 0,0);
	}
	// } else {
	// 	// debugf("没有ipc\n");
	// }
	//////////////////////////////////////////////////////////////////

	// exit gracefully
	exit();
}
