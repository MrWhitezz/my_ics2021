#include<common.h>
void init_ftrace(const char *trace_file){
    if (trace_file == NULL){
        Log("No function trace elf file is given");
    }
    FILE* fp = fopen(trace_file, "rb");
    Assert(fp, "Cannot open '%s'", trace_file);
}