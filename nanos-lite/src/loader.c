#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define ElfN_off Elf64_Off
# define ElfN_Addr Elf64_Addr
# define Elfsz    uint64_t
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define ElfN_off Elf32_Off
# define ElfN_Addr Elf32_Addr
# define Elfsz    uint32_t
#endif
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
ElfN_off phoff, offp;
ElfN_Addr vaddr, e_entry;
Elfsz filesz, memsz;
uint16_t phentsize, phnum;
#define bufsz 102400
char* bufp[bufsz];

int fs_open(const char *pathname, int flags, int mode);
int fs_close(int fd);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
#include "fs.h"

void load_tmp(){
  if (e_entry != 0){
    ((void(*)())e_entry) ();
  }
}

static uintptr_t loader(PCB *pcb, const char *filename) { // temporarily ignore pcd
  Elf_Ehdr elf;
  Elf_Phdr phdr;
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, &elf, sizeof(elf));
  //ramdisk_read(&elf, 0, sizeof(elf));
  assert(*(uint32_t *)elf.e_ident == 0x464c457f); // correct ELF MAGIC number
  phoff = elf.e_phoff;
  phentsize = elf.e_phentsize;
  phnum= elf.e_phnum;
  e_entry = elf.e_entry;
  assert(phnum >= 0 && phnum <= PN_XNUM);
  assert(phentsize == sizeof(phdr) || phentsize == 0);
  for (int i = 0; i < phnum; ++i){
    fs_lseek(fd, phoff + i * phentsize, SEEK_SET);
    fs_read(fd, &phdr, phentsize);
    //ramdisk_read(&phdr, phoff + i * phentsize, sizeof(phdr));
    if (phdr.p_type == PT_LOAD){
      vaddr = phdr.p_vaddr;
      filesz = phdr.p_filesz;
      memsz = phdr.p_memsz;
      offp = phdr.p_offset;
      assert(filesz <= memsz && filesz <= bufsz);
      fs_lseek(fd, offp, SEEK_SET);
      fs_read(fd, bufp, filesz);
      //ramdisk_read(bufp, offp, filesz);
      memcpy((void *)vaddr, bufp, filesz);
      if (filesz < memsz) {memset((void *)vaddr + filesz, 0, memsz - filesz);}
    }
  }
  // printf("e_entry = 0x%08x\n", e_entry);
  return (uintptr_t)load_tmp;
  // return (uintptr_t)e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

