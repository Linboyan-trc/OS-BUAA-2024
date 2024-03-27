void scanf_1_check() {
	printk("Running scanf_1_check\n");
	int num = 0;
	scanf("%d", &num);
	printk("Finished 1st scanf\n");
	printk("%d\n", num);
}

void mips_init(u_int argc, char **argv, char **penv, u_int ram_low_size) {
	scanf_1_check();
	halt();
}
