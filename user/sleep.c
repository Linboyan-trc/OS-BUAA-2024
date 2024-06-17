#include <lib.h>
#define ITER 10000
int atoi(char *s) {
    int ret = 0;
    while (*s) {
        ret = ret * 10 + (*s++ - '0');
    }
    return ret;
}
int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    for (int t = 0; t < n * ITER; t++) {
        syscall_yield();
    }
    syscall_get_jobs(3,env->env_id,NULL,NULL,NULL);
    return 0;
}