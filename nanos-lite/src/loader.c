#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
Elf32_Off phoff;
uint16_t phentsize, phnum;

static uintptr_t loader(PCB *pcb, const char *filename) { // temporarily ignore pcd and filename
  Elf_Ehdr elf;
  ramdisk_read(&elf, 0, sizeof(elf));
  assert(*(uint32_t *)elf.e_ident == 0x464c457f); // correct ELF MAGIC number
  phoff = elf.e_phoff;
  phentsize = elf.e_phentsize;
  phnum= elf.e_phnum;
  TODO();
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

