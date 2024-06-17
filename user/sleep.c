#include <lib.h>
#include <error.h>
#define ITER 10000
int atoi(char *s) {
    int ret = 0;
    while (*s) {
        ret = ret * 10 + (*s++ - '0');
    }
    return ret;
}
int main(int argc, char **argv) {

    int need_ipc_send = 0;
    int target = 0;
    int haschange = 0;

    int n = atoi(argv[1]);
    for (int t = 0; t < n * ITER; t++) {
        if (syscall_get_fg_target(env->env_id) != -1  && haschange == 0) {
            haschange = 1;
            target = syscall_get_fg_target(env->env_id);
            need_ipc_send = 1;
        }
        syscall_yield();
    }
    // debugf("我的进程%x\n",env->env_id);
    // debugf("我要发的人是%x\n",target);
    syscall_get_jobs(3,env->env_id,NULL,NULL,NULL);

    if (need_ipc_send == 1) {
        // debugf("我要发的人是%x\n",target);
        ipc_send(target,0,0,0);
    }
    return 0;
}