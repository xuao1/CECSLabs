#include <stdint.h>
#include <iostream>
#include <assert.h>
#include "include/debug.h"

extern uint8_t pmem[];

// load a binary file into your cpu
uint64_t load_img(char *img_file){
    // Lab2 TODO: load the 'img_file' to the start of pmem, and return its size
    FILE *fp = fopen(img_file, "rb");
    assert(fp != NULL);
    
    fseek(fp, 0, SEEK_END); // 定位到文件末尾
    uint64_t file_size = ftell(fp); // 得到文件大小
    
    fseek(fp, 0, SEEK_SET); // 定位到文件开头
    fread(pmem, file_size, 1, fp); // 读取文件内容到 pmem 中
    
    fclose(fp);

    return file_size;
}
