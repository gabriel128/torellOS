#include <stdlib.h>
#include <stdio.h>
#include "fs.h"
#include "types.h"
#include "spinlock.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define T_EMPTY 0  // Free inode
#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

struct logheader {
  int n;
  int block[30];
};
char address[6+1];    //+1 for NUL
char binAddress[6*4+1];//+1 for NUL
void hexToBin(void);

/*
  [ boot block | superblock | log header + log  blocks | inode blocks  | free bit map  | data blocks ]
  0            1            2                          32              58              59

 */
int main() {
  int fd = open("fs.img", O_RDWR);
  struct stat buff;

  fstat(fd, &buff);

  void *img =  mmap(NULL, buff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  struct superblock *sp = (struct superblock *) (img + BSIZE);
  struct logheader *log = (struct logheader *) (img + BSIZE*2);

  struct dinode *inodes = (struct dinode *) (img + BSIZE*32);
  uchar *bitmapmem = (uchar *) (img + BSIZE * 58);

  struct dirent *dirent;

  // Transform bitmap
  uint bitmap[512*8];

  for(int i = 0; i < 512;i++) {
    uint byte = *(bitmapmem + i);
    for(int j=7; j >= 0; j--) {
      bitmap[j+(i*8)] = (byte >> j)%2;
    }
  }

  int i;
  // Inodes Checks
  for(i=0;i < (25*8);i++) {
    struct dinode *inode = inodes + i;

    // Type Check
    fprintf(stdout, "Checking  Inode types...\n");
    if(inode->type != T_EMPTY && inode->type != T_FILE && inode->type != T_DIR && inode->type != T_DEV) {
      fprintf(stderr, "[Error] Wrong Inode Type mothefucka, it was %d\n", inode->type);
      exit(1);
    }

    // Dot in directories point to themselve
    fprintf(stdout, "Checking Directories ...\n");

    if(inode->type == T_DIR) {
      struct dirent *dir = (struct dirent *) (img + BSIZE*(inode->addrs[0]));
      if(dir->inum != i) {
        fprintf(stderr, "[Error] Dot doesn't point to itself, dir inum: %d, inode num: %d \n", dir->inum, i);
        exit(1);
      }
    }

    // Data blocks mapping checks
    fprintf(stdout, "Checking Data consistency...\n");
    int j;
    for(j=0;j < 13;j++) {
      uint addr = inode->addrs[j];
      if(addr == 0)
        continue;

      if(inode->type != T_EMPTY && bitmap[addr-59] != 1) {
        fprintf(stderr, "[Error] Corrupt data block at addr %d, should be with data \n", addr);
        exit(1);
      } else if (inode->type == T_EMPTY && bitmap[addr-59] != 0){
        fprintf(stderr, "[Error] Corrupt data block at addr %d, should be empty \n", addr);
        exit(1);
      }
    }

    // Cross Reference check, seems like O(n^3) but it's actually O(25*25*12*31)
    fprintf(stdout, "Checking cross reference consistency...\n");
    if(inode->type != T_EMPTY && inode->type != T_DEV && i != 1) {
      int in_a_dir = 0;
      for(int j = 0; j < 25;j++) {
        struct dinode *inode2 = inodes + j;
        if(inode2->type == T_DIR) {
          for(int k=0; k < 12; k++) {
            // Not doing indirect blocks
            for(int l=0; l < 31;l++) {
              struct dirent *dir = (struct dirent *) (img + BSIZE*(inode2->addrs[k]))+l;
              if(dir->inum == i)
                in_a_dir = 1;
            }
          }
        }
      }
      if(in_a_dir == 0) {
        fprintf(stderr, "[Error] Inode not referenced in any directory, damn. Inode num: %d. Thanks Gabe\n", i);
        exit(1);
      }
    }
  }

  // Root directory checks
  fprintf(stdout, "Checking Root dir consistency...\n");
  struct dinode *root_inode = inodes+1;
  struct dirent *rootdir_dot = (struct dirent *) (img + BSIZE*(root_inode->addrs[0]));
  struct dirent *rootdir_dot_dot = (struct dirent *) (img + BSIZE*(root_inode->addrs[0]))+1;

  if(rootdir_dot->inum != 1 || rootdir_dot_dot->inum != 1) {
    fprintf(stderr, "[Error] Corrupt root . and .. should point to root. Thanks Gabe\n");
    exit(1);
  }

  //
  fprintf(stdout, "All gooood, Thanks Gabe!\n");

  return 0;
}
