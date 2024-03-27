void scanf_1_check() {
	printk("Running scanf_1_check\n");
	char arr[128];
	int dec,hex;
	char ch;
	int cnt = scanf("%s%d%c%x",arr,&dec,&ch,&hex);
	printk("Finished 1st scanf\n");
	printk("%somg\n%domg\n%comg\n%domg\n%d\n", arr,dec,ch,hex,cnt);
}

void mips_init(u_int argc, char **argv, char **penv, u_int ram_low_size) {
	scanf_1_check();
	halt();
}
