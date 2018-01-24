#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <fcntl.h>

struct file_storage
{ 
  int fd;
  void* base;
  long mapsize;
  void exit_error(const char* why)
  { 
    perror(why);
    exit(1);
  }
  file_storage(const char* name, long size)
  { 
    fd = open(name, O_RDONLY);
    if (fd<0) exit_error("create");
    mapsize = (size+PAGE_SIZE-1)/PAGE_SIZE*PAGE_SIZE;
    printf("file: %s, req size: %lu, mmap: %lu\n", name, size, mapsize);
    //int rv = ftruncate(fd, mapsize);
    //if (rv<0) exit_error("truncate");
    base=mmap(0, mapsize, PROT_READ, MAP_SHARED, fd, 0);
    if (base==MAP_FAILED) exit_error("mmap");
    close(fd);
  }
  ~file_storage()
  { 
    munmap(base, mapsize);
  }
};

void printlong(long x)
{
  int tot=0;
  for (int i=63; i>=0; i--)
    printf(x&(1L<<i)?(tot++,"#"):".");
  printf(" :%d\n", tot);
}

int main(int ara, char** sdfg)
{
  file_storage fs("((9,10),(11,12))", 7075061760);
  int len=155043680;
  long *exp=(long*)fs.base;
  long check[64][2];
  for (int x=0; x<4; x++)
    for (int y=0; y<4; y++)
      for (int z=0; z<4; z++)
      {
	check[x*16+y*4+z][0]=(1L<<(x*16+y*4+z));
	long tmp=0;
	if (x>0) tmp |= (1L<<((x-1)*16+y+4+z));
	if (x<3) tmp |= (1L<<((x+1)*16+y+4+z));
	if (y>0) tmp |= (1L<<(x*16+(y-1)+4+z));
	if (y<3) tmp |= (1L<<(x*16+(y+1)+4+z));
	if (z>0) tmp |= (1L<<(x*16+y+4+z-1));
	if (z<3) tmp |= (1L<<(x*16+y+4+z+1));
	check[x*16+y*4+z][1]=tmp;
      }
  int culled=0;
  for (int i=0; i<len; i++)
    for (int c=0; c<64; c++)
      if ((~exp[i])&check[c][0])
	if ((exp[i]&check[c][1])==check[c][1])
	{
	  culled++;
	  break;
	}
  printf("culled: %d\n", culled);
  printf("******: %.1f%%\n", culled*100.0/len);
  return 0;
}
