#include "header.h"


void create_MMF()
{
    int ret;
    struct stat st;
    /*size_t len_file, len;

    if ((fd_mmf = open(LOG_FILE_NAME,O_RDWR | O_CREAT, FILEMODE)) < 0)
    {
        perror("\033[91mError in file opening");
        exit(1);
    }
    if ((ret = fstat(fd_mmf, &st)) < 0) {
        perror("\033[91mError in fstat");
        exit(1);
    }

    len_file = st.st_size;
    // expand dst size by seeking to the last byte
    lseek(fd_mmf, 20480 - 1, SEEK_SET);
    // put something there
    write(fd_mmf, "" , 1);
    if ((addr_mmf = mmap(NULL,20480,PROT_READ|PROT_WRITE,MAP_SHARED,fd_mmf,0)) == MAP_FAILED)
    {
        perror("\033[91mError in mmap");
        exit(1);
    }
    offset = 0;
    printf("ok\n");*/
    if ((fd_mmf = open(LOG_FILE_NAME, O_RDWR | O_CREAT, 0600)) < 0){
        perror("\033[91mError in file opening");
        exit(1);
    }
    //write(fd_mmf, "" , 1);
    if ((ret = fstat(fd_mmf, &st)) < 0) {
        perror("\033[91mError in fstat");
        exit(1);
    }
    len_file = st.st_size;
    lseek(fd_mmf, len_file - 1, SEEK_SET);
    write(fd_mmf, "" , 1);

    //lseek(fd_mmf, len_file - 1, SEEK_SET);


    if ((addr_mmf = mmap(0, len_file, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mmf, 0)) == MAP_FAILED){
        perror("\033[91mError in mmap");
        exit(1);
    }
}

void write_to_mmf (char* buffer, int buffer_size) {
  len_file += buffer_size;
  if ((addr_mmf = mremap(addr_mmf, offset, len_file, MREMAP_MAYMOVE)) == MAP_FAILED) {
      perror("\033[91mError extending mapping");
      exit(1);
  }
  if (ftruncate(fd_mmf, len_file) != 0){
      perror("\033[91mError extending file");
      exit(1);
  }
  lseek(fd_mmf, offset - 1, SEEK_SET);
  write(fd_mmf, "" , 1);

  memcpy(addr_mmf + offset, buffer, buffer_size);
  offset += buffer_size;
  #ifdef DEBUG
  printf("##################### DEBUG #######################\n");
  printf("MMF - buffer: %s\nMMF - offset: %d\n", buffer, offset);
  printf("################## DEBUG END ###################\n");
  #endif
  /*int len;
  len = len_file;
  len_file += buffer_size;

  /*if ((addr = mremap(addr, len, len_file, MREMAP_MAYMOVE)) == MAP_FAILED) {
      perror("\033[91mError extending mapping");
      return EXIT_FAILURE;
  }*/
  /*if (ftruncate(fd_mmf, len_file) != 0){
      perror("\033[91mError extending file");
      perror(1);
  }
  memcpy(addr_mmf+len, buffer, len_file - len);
  printf( "Val:%s\n",addr_mmf ) ;*/ //Checking purpose
}
