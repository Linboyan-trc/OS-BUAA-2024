#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ri(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
    [10] = handle_ri,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ri(struct Trapframe *tf){

	// 1. get instruction
	u_long addr = tf->cp0_epc;
	struct Page * pp = page_lookup(curenv->env_pgdir, addr, NULL);
	u_long kva = page2kva(pp) | (addr & 0xfff);
	u_int inst = *((u_int *)kva);

	// 2. process exception
	if (((inst & 0xfc000000)==0)&&((inst & 0x3f)==0x3f)) {

		u_int rs_g = (inst >> 21) & 0x1f;
		u_int rt_g = (inst >> 16) & 0x1f;
		u_int rd_g = (inst >> 11) & 0x1f;

		u_int rs = tf->regs[rs_g];
		u_int rt = tf->regs[rt_g];

		u_int rd = 0;
		for (u_int i = 0; i < 32; i += 8) {
		   u_int rs_i = rs & (0xff << i);
		   u_int rt_i = rt & (0xff << i);
		   if (rs_i < rt_i) {
		      rd = rd | rt_i;
		   } else {
		      rd = rd | rs_i;
		   }
		}

		tf->regs[rd_g] = rd;

	} else if(((inst & 0xfc000000)==0)&&((inst & 0x3f)==0x3e)) {

		u_int rs_g = (inst >> 21) & 0x1f;
		u_int rt_g = (inst >> 16) & 0x1f;
		u_int rd_g = (inst >> 11) & 0x1f;

		u_int rs = tf->regs[rs_g];
		u_int rt = tf->regs[rt_g];

		u_int rd = tf->regs[rd_g];
		addr = rs;
		pp = page_lookup(curenv->env_pgdir, addr, NULL);
		kva = page2kva(pp) | (addr & 0xfff);
		u_int tmp = *((u_int *)kva);
		if (*((u_int *)kva) == rt) {
		   *((u_int *)kva) = rd;
		}
		rd = tmp;

		tf->regs[rd_g] = rd;

	}

	// 3. next instruction
	tf->cp0_epc += 4;
}
