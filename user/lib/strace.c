#include <lib.h>
#include <env.h>
#include <printk.h>

void strace_barrier(u_int env_id) {
	int straced_bak = straced;
	straced = 0;
	while (envs[ENVX(env_id)].env_status == ENV_RUNNABLE) {
		syscall_yield();
	}
	straced = straced_bak;
}

void strace_send(int sysno) {
	if (!((SYS_putchar <= sysno && sysno <= SYS_set_tlb_mod_entry) ||
	      (SYS_exofork <= sysno && sysno <= SYS_panic)) ||
	    sysno == SYS_set_trapframe) {
		return;
	}

	// Your code here. (1/2)
         if (straced == 0) {
	 	//debugf("strace test passed!\n");   
	 	 return;
         } else {
		//debugf("strace test passed!\n");
		 int old_straced = straced;
	    	straced = 0;
	    	
	    	ipc_send(env->env_parent_id, sysno, NULL, 0);
	    	
	    	syscall_set_env_status(env->env_id, ENV_NOT_RUNNABLE);
	    	
	    	straced = old_straced;
         }
}

void strace_recv() {
	// Your code here. (2/2)
	 while(1) {
        	int sysno = 0;
        
        	sysno = ipc_recv((void *)env->env_ipc_from, (void *)env->env_ipc_dstva, (void *)env->env_ipc_perm);
		
			strace_barrier(env->env_ipc_from);
			
		recv_sysno(env->env_ipc_from, sysno);
			
		syscall_set_env_status(env->env_ipc_from, ENV_RUNNABLE);
			
		if (sysno == SYS_env_destroy) {
		return;
		}
	}
}
