#include<common.h>
static char *buffer;
void init_ftrace(const char *trace_file){
    if (trace_file == NULL){
        Log("No function trace elf file is given");
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

}