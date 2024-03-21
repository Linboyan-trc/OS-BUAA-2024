#include "elf.h"
#include <stdio.h>

/* Overview:
 *   Check whether specified buffer is valid ELF data.
 *
 * Pre-Condition:
 *   The memory within [binary, binary+size) must be valid to read.
 *
 * Post-Condition:
 *   Returns 0 if 'binary' isn't an ELF, otherwise returns 1.
 */
int is_elf_format(const void *binary, size_t size) {
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
	return size >= sizeof(Elf32_Ehdr) && ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
	       ehdr->e_ident[EI_MAG1] == ELFMAG1 && ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
	       ehdr->e_ident[EI_MAG3] == ELFMAG3;
}

/* Overview:
 *   Parse the sections from an ELF binary.
 *
 * Pre-Condition:
 *   The memory within [binary, binary+size) must be valid to read.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output the address of every section in ELF.
 */

int readelf(const void *binary, size_t size) {
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

	// Check whether `binary` is a ELF file.
	if (!is_elf_format(binary, size)) {
		fputs("not an elf file\n", stderr);
		return -1;
	}

	// Get the address of the section table, the number of section headers and the size of a
	// section header.
	const void *sh_table;
	Elf32_Half sh_entry_count;
	Elf32_Half sh_entry_size;
	/* Exercise 1.1: Your code here. (1/2) */
    // 1. binary是字节指针 + ehdr->e_shoff偏移量 得到 sh_table节头表指针
    // 2. 节头表条目数在ehdr->e_shnum中有
    // 3. 节头表每个条目的大小在ehdr->shentsize中有
    sh_table = binary + ehdr->e_shoff;
    sh_entry_count = ehdr->e_shnum;
    sh_entry_size = ehdr->e_shentsize;

	// For each section header, output its index and the section address.
	// The index should start from 0.
	for (int i = 0; i < sh_entry_count; i++) {
		const Elf32_Shdr *shdr;
		unsigned int addr;
		/* Exercise 1.1: Your code here. (2/2) */
		// 1. 每个条目用结构体Elf32_Shdr表示, 在每次遍历中都重新声明了一个Elf32_Shdr结构体指针
		// 2. sh_table是字节指针, 所以每个条目的结构体指针指向 = sh_table + i * 每个条目的大小
		// 3. 每个条目的地址存储在了Elf32_Shdr结构体中的sh_addr中
		shdr = sh_table + i * sh_entry_size;
		addr = shdr->sh_addr;

		printf("%d:0x%x\n", i, addr);
	}

	return 0;
}
