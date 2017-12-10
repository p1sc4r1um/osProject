#include "header.h"


void create_MMF()
{
    int ret;
    size_t len_file, len;
    struct stat st;
    if ((fd_mmf = open(LOG_FILE_NAME,O_RDWR | O_CREAT, FILEMODE)) < 0)
    {
        perror("Error in file opening");
        exit(1);
    }
    if ((ret = fstat(fd_mmf, &st)) < 0) {
        perror("Error in fstat");
        exit(1);
    }

    len_file = st.st_size;
    // expand dst size by seeking to the last byte
    lseek(fd_mmf, 20480 - 1, SEEK_SET);
    // put something there
    write(fd_mmf, "" , 1);
    if ((addr_mmf = mmap(NULL,20480,PROT_READ|PROT_WRITE,MAP_SHARED,fd_mmf,0)) == MAP_FAILED)
    {
        perror("Error in mmap");
        exit(1);
    }
    offset = 0;
    printf("ok\n");
}

void write_to_mmf (char* buffer, int buffer_size) {
  memcpy(addr_mmf + offset, buffer, buffer_size);
  offset += buffer_size;
  #ifdef DEBUG
  printf("##################### DEBUG #######################\n");
  printf("MMF - buffer: %s\nMMF - offset: %d\n", buffer, offset);
  printf("################## DEBUG END ###################\n");
  #endif
}
