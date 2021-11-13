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
Elf32_Addr vaddr;
uint32_t filesz, memsz;
uint16_t phentsize, phnum;
#define bufsz 2048
char* bufp[bufsz];

static uintptr_t loader(PCB *pcb, const char *filename) { // temporarily ignore pcd and filename
  Elf_Ehdr elf;
  Elf_Phdr phdr;
  ramdisk_read(&elf, 0, sizeof(elf));
  assert(*(uint32_t *)elf.e_ident == 0x464c457f); // correct ELF MAGIC number
  phoff = elf.e_phoff;
  phentsize = elf.e_phentsize;
  phnum= elf.e_phnum;
  assert(phnum > 0 && phnum <= PN_XNUM);
  assert(phentsize == sizeof(phdr));
  for (int i = 0; i < phnum; ++i){
    ramdisk_read(&phdr, phoff + i * phentsize, sizeof(phdr));
    if (phdr.p_type == PT_LOAD){
      vaddr = phdr.p_vaddr;
      filesz = phdr.p_filesz;
      memsz = phdr.p_memsz;
      assert(filesz <= memsz);
      assert(memsz <= bufsz);
    }
  }
  TODO();
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

