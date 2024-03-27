void scanf_1_check() {
	printk("Running scanf_1_check\n");
	char arr[128];
	int dec,hex;
	char ch;
	scanf("%s%d%c%x",arr,&dec,&ch,&hex);
	printk("Finished 1st scanf\n");
	printk("%s\n%d\n%c\n%d\n", arr,dec,ch,hex);
}

void mips_init(u_int argc, char **argv, char **penv, u_int ram_low_size) {
	scanf_1_check();
	halt();
}
