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
// #define bufsz 409600
// char bufp[bufsz]; // how fool you are

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

uintptr_t loader(PCB *pcb, const char *filename) { // temporarily ignore pcd; remove static
  Elf_Ehdr elf;
  Elf_Phdr phdr;
  int fd = fs_open(filename, 0, 0);
  if (fd == -1) return -1;
  fs_read(fd, &elf, sizeof(elf));
  // ramdisk_read(&elf, 0, sizeof(elf));
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
      assert(filesz <= memsz);
      int nr_page = (memsz + PGSIZE - 1) / PGSIZE;
      void *p_page = pg_alloc(nr_page);
      assert(pcb != NULL);
      for (int i = 0; i < nr_page; ++i){
        map(&pcb->as, ((void *)vaddr) + i * PGSIZE, p_page + i * PGSIZE, MMAP_READ | MMAP_WRITE);
      }
      fs_lseek(fd, offp, SEEK_SET);
      // fs_read(fd, bufp, filesz);
      // memcpy((void *)vaddr, bufp, filesz);
      fs_read(fd, (void *)vaddr, filesz);
      // printf("vaddr: %p\n", (void *)vaddr);
      // printf("e_entry: %p\n", (void *)e_entry);
      // printf("buf from %p to %p\n", bufp, bufp + bufsz);
      //ramdisk_read(bufp, offp, filesz);
      if (filesz < memsz) {memset((void *)vaddr + filesz, 0, memsz - filesz);}
    }
  }
  // printf("e_entry = 0x%08x\n", e_entry);
  return (uintptr_t)load_tmp;
  // return (uintptr_t)e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  // uintptr_t entry = loader(pcb, filename);
  // if (entry == -1) return;
  // Log("Jump to entry = %p", entry);
  // ((void(*)())entry) ();
  context_uload(pcb, filename, NULL, NULL);
  yield();
}

