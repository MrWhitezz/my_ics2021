#include<common.h>
static char *buffer;
uint32_t shoff = 0;
uint16_t shentsize = 0;
uint16_t shnum = 0;

uint32_t sh_type = 0;
uint32_t SYM_off = 0;
uint32_t SYM_size = 0;
uint32_t SYM_entsize = 0x10;
uint32_t SYM_num = 0;
uint32_t STR_off = 0;
uint32_t STR_size = 0;
bool is_find_STR = false;

uint32_t st_name = 0;
uint32_t st_value = 0;
uint32_t st_size = 0;
uint8_t st_info = 0;

static inline word_t buff_read(void *addr, int len) {
  switch (len) {
    case 1: return *(uint8_t  *)addr;
    case 2: return *(uint16_t *)addr;
    case 4: return *(uint32_t *)addr;
    default: assert(0); 
  }
}

void init_ftrace(const char *trace_file){
    if (trace_file == NULL){
        Log("No function trace elf file is given");
        assert(0);
    }
    FILE* fp = fopen(trace_file, "rb");
    Assert(fp, "Cannot open '%s'", trace_file);

    fseek(fp, 0, SEEK_END);
    long size_elf = ftell(fp);

    Log("The function trace file is %s, size = %ld", trace_file, size_elf);
    buffer = calloc(1, size_elf + 1);
    fseek(fp, 0, SEEK_SET);
    // test for open file and read
    if (!buffer) {
        fclose(fp);
        Log("memory alloc to function trace buffer fails");
        assert(0);
    }

    if (1 != fread(buffer, size_elf, 1, fp)){
        fclose(fp); free(buffer);
        Log("entire read from function trace elf fails");
    }

    fclose(fp);

    shoff = buff_read(buffer + 0x20, 4);
    shentsize = buff_read(buffer + 0x2e, 2); // normally shentsize = 0x28
    shnum = buff_read(buffer + 0x30, 2); 

    // search for offset of STR_TAB, SYM_TAB
    enum{SHT_SYMTAB = 2, SHT_STRTAB};
    for (int i = 0; i < shnum; ++i){
        sh_type = buff_read(buffer + shoff + i * shentsize + 0x4, 4);
        if (sh_type == SHT_SYMTAB){
           SYM_off = buff_read(buffer + shoff + i * shentsize + 0x10, 4);
           SYM_size = buff_read(buffer + shoff + i * shentsize + 0x14, 4);
           SYM_num = SYM_size / SYM_entsize;
           //printf("SYM_off = %x\n", SYM_off);
        }
        if (sh_type == SHT_STRTAB && !is_find_STR){
           is_find_STR = true;
           STR_off = buff_read(buffer + shoff + i * shentsize + 0x10, 4);
           STR_size = buff_read(buffer + shoff + i * shentsize + 0x14, 4); 
        }
    }


}

int call_times = 0;
char func_name[64];
void ftrace_print(word_t addr_caller, word_t addr_to, bool is_call){
    enum{STT_FUNC = 0x12}; // I don't know why???
    if (is_call){
        for (int i = 0; i < SYM_num; ++i){
            st_info = buff_read(buffer + SYM_off + i * SYM_entsize + 0xc, 1);
                    printf("st_info = %x\n", st_info);// not be executed
            if (st_info == STT_FUNC){
                st_name = buff_read(buffer + SYM_off + i * SYM_entsize + 0x0, 4);
                    printf("st_name = %x", st_name);// not be executed
                st_value = buff_read(buffer + SYM_off + i * SYM_entsize + 0x4, 4);
                st_size = buff_read(buffer + SYM_off + i * SYM_entsize + 0x8, 4);
                if (addr_to >= st_value && addr_to < st_value + st_size){
                    printf("st_name = %x", st_name);// not be executed
                    strcpy(func_name, buffer + STR_off + st_name);
                }
            }
        }    
    }
    if (is_call) ++call_times; else --call_times;
    printf("0x%08x: ", addr_caller);
    for (int i = 0; i < call_times; ++i) {putchar(' ');}
    if (is_call)
        printf("call %s@0x%08x\n", func_name, addr_to); 
    else
        printf("ret %s@0x%08x\n", func_name, addr_to); 

}