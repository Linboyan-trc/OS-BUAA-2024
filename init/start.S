#include <asm/asm.h>
#include <mmu.h>

.text
EXPORT(_start)
.set at
.set reorder
/* Lab 1 Key Code "enter-kernel" */
	/* clear .bss segment */
	la      v0, bss_start
	la      v1, bss_end
clear_bss_loop:
	beq     v0, v1, clear_bss_done
	sb      zero, 0(v0)
	addiu   v0, v0, 1
	j       clear_bss_loop
/* End of Key Code "enter-kernel" */

clear_bss_done:
	/* disable interrupts */
	mtc0    zero, CP0_STATUS

	/* hint: you can refer to the memory layout in include/mmu.h */
	/* set up the kernel stack */
	/* Exercise 1.3: Your code here. (1/2) */
	/* 1. 设置栈顶位置，由mmu.h中可以看出栈顶在8004_0000, 也可以用宏KSTACKTOP */
	la sp, KSTACKTOP

	/* jump to mips_init */
	/* Exercise 1.3: Your code here. (2/2) */
	/* 2. 跳到初始化函数，本实验给定初始化函数为mips_init */
	j mips_init
