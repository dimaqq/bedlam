#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <fcntl.h>

struct dir
{
  char x,y,z;
  dir(): x(0), y(0), z(0) {}
  dir(char xx, char yy, char zz): x(xx), y(yy), z(zz) {}
  void rotx()
  {
    char tmp=y;
    y=z;
    z=-tmp;
  }
  void roty()
  {
    char tmp=x;
    x=y;
    y=-tmp;
  }
  void rotz()
  {
    char tmp=y;
    y=x;
    x=-tmp;
  }
};

struct check_one
{
  long checks[64][2];
  check_one()
  {
    for (int x=0; x<4; x++)
      for (int y=0; y<4; y++)
	for (int z=0; z<4; z++)
	{
	  checks[x*16+y*4+z][0]=(1L<<(x*16+y*4+z));
	  long tmp=0;
	  if (x>0) tmp |= (1L<<((x-1)*16+y+4+z));
	  if (x<3) tmp |= (1L<<((x+1)*16+y+4+z));
	  if (y>0) tmp |= (1L<<(x*16+(y-1)+4+z));
	  if (y<3) tmp |= (1L<<(x*16+(y+1)+4+z));
	  if (z>0) tmp |= (1L<<(x*16+y+4+z-1));
	  if (z<3) tmp |= (1L<<(x*16+y+4+z+1));
	  checks[x*16+y*4+z][1]=tmp;
	}
  }
  bool operator()(long arg) const
  {
    for (int c=0; c<64; c++)
      if ((~arg)&checks[c][0])
	if ((arg&checks[c][1])==checks[c][1])
	  return false;
    return true;
  }
} check;

struct piece
{
  int name;
  int len;
  dir bits[5];
  void rotx()
  {
    for (int i=0; i<len; i++)
      bits[i].rotx();
  }
  void roty()
  {
    for (int i=0; i<len; i++)
      bits[i].roty();
  }
  void rotz()
  {
    for (int i=0; i<len; i++)
      bits[i].rotz();
  }
};

#define PIECES 13
piece pieces[PIECES] = 
{
  { 1, 4, { dir(0,0,0), dir(1,0,0), dir(1,1,0), dir(0,0,1), dir()}},
  //{ 2, 5, { dir(0,0,0), dir(1,0,0), dir(1,-1,0), dir(0,1,0), dir(0,1,1)}},
  { 2, 5, { dir(0,0,0), dir(1,0,0), dir(1,0,-1), dir(0,1,0), dir(0,1,1)}},
  { 3, 5, { dir(0,0,0), dir(1,0,0), dir(-1,0,0), dir(0,1,0), dir(0,1,1)}},
  { 4, 5, { dir(0,0,0), dir(1,0,0), dir(-1,0,0), dir(0,1,0), dir(1,0,1)}},
  { 5, 5, { dir(0,0,0), dir(-1,0,0), dir(0,1,0), dir(0,0,1), dir(1,0,1)}},
  { 6, 5, { dir(0,0,0), dir(-1,0,0), dir(-2,0,0), dir(0,1,0), dir(0,1,1)}},
  { 7, 5, { dir(0,0,0), dir(1,0,0), dir(1,0,1), dir(0,1,0), dir(-1,1,0)}},
  { 8, 5, { dir(0,0,0), dir(1,0,0), dir(1,1,0), dir(-1,0,0), dir(-1,0,-1)}},
  { 9, 5, { dir(0,0,0), dir(1,0,0), dir(-1,0,0), dir(0,1,0), dir(0,0,1)}},
  { 10, 5, { dir(0,0,0), dir(1,0,0), dir(2,0,0), dir(0,1,0), dir(0,0,1)}},
  { 11, 5, { dir(0,0,0), dir(1,0,0), dir(1,-1,0), dir(0,1,0), dir(-1,1,0)}},
  { 12, 5, { dir(0,0,0), dir(1,0,0), dir(1,-1,0), dir(-1,0,0), dir(0,1,0)}},
  { 13, 5, { dir(0,0,0), dir(1,0,0), dir(-1,0,0), dir(0,1,0), dir(0,-1,0)}},
};

piece piece_rots[PIECES][6*4];

void printspaces(int n)
{ for (int i=0; i<n; i++) printf(" "); }

void printlong(long x)
{
  int tot=0;
  for (int i=63; i>=0; i--)
    printf(x&(1L<<i)?(tot++,"#"):".");
  printf(" :%d\n", tot);
}

void prepare_piece_rots()
{
  for (int i=0; i<PIECES; i++)
  {
    piece p = pieces[i];
    for (int r=0; r<4; r++)
    {
      int d=0;
      piece s=p;
      piece_rots[i][(d++)*4+r] = s;
      s.roty();
      piece_rots[i][(d++)*4+r] = s;
      s.roty();
      piece_rots[i][(d++)*4+r] = s;
      s.roty();
      piece_rots[i][(d++)*4+r] = s;
      s.roty();
      s.rotz();
      piece_rots[i][(d++)*4+r] = s;
      s.rotz();
      s.rotz();
      piece_rots[i][(d++)*4+r] = s;
      // rotate p along x
      p.rotx();
    }
  }
}

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
    fd = open(name, O_RDWR|O_CREAT, S_IWUSR|S_IRUSR);
    if (fd<0) exit_error("create");
    mapsize = (size+PAGE_SIZE-1)/PAGE_SIZE*PAGE_SIZE;
    printf("file: %s, req size: %lu, mmap: %lu\n", name, size, mapsize);
    int rv = ftruncate(fd, mapsize);
    if (rv<0) exit_error("truncate");
    base=mmap(0, mapsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (base==MAP_FAILED) exit_error("mmap");
    close(fd);
  }
  ~file_storage()
  {
    munmap(base, mapsize);
  }
};

struct piece_expression
{
  char name[40];
  long len;
  long *exp;
  file_storage* fs;
  void setup_exp()
  {
    fs = new file_storage(name, len*sizeof(long));
    exp = (long*) fs->base;
  }
  piece_expression(long length=6*4*64)
  {
    len = length;
    snprintf(name, sizeof(name), "%s", tempnam(".", "()___"));
    setup_exp();
    memset(name, 0, sizeof(name));
    len = 0;
    memset(exp, 0, sizeof(exp));
  }
  ~piece_expression()
  {
    delete fs;
  }
  bool has(long arg) const
  {
    for (long i=0; i<len; i++)
      if (exp[i]==arg) return true;
    return false;
  }
  void add(long arg)
  {
    exp[len]=arg;
    len++;
  }
  piece_expression(piece_expression& a, piece_expression& b): len(0)
  {
    snprintf(name, sizeof(name), "(%s,%s)", a.name, b.name);
    len = a.len*b.len;
    setup_exp();
    len = 0;
    for (long x=0; x<a.len; x++)
      for (long y=0; y<b.len; y++)
	if (!(a.exp[x] & b.exp[y]))
	  if (check(a.exp[x]|b.exp[y]))
	    exp[len++]=(a.exp[x]|b.exp[y]);
    printf("reduction: %f\n", a.len*b.len*1.0/len);
    printstats();
  }
  piece_expression(const char* nnn)
  {
    snprintf(name, sizeof(name), "%s", nnn);
    len=1;
    setup_exp();
    exp[0]=0L;
  }
  void printstats()
  {
    printf("%s: %ld\n", name, len);
  }
};

long express(piece& p, int xxx, int yyy, int zzz)
{
  long rv=0;
  for (long i=0; i<p.len; i++)
  {
    int x=xxx+p.bits[i].x;
    int y=yyy+p.bits[i].y;
    int z=zzz+p.bits[i].z;
    if (x<0 || x>3 || y<0 || y>3 || z<0 || z>3) return 0;
    rv |= (1L<<(x*16+y*4+z));
  }
  return rv;
}

piece_expression piece_exp[PIECES];
//piece_expression piece_exp[16];

void prepare_piece_exp()
{
  for (int n=0; n<PIECES; n++)
  {
    snprintf(piece_exp[n].name, sizeof(piece_exp[n].name), "%d", pieces[n].name);
    for (int d=0; (n==4)?(d<1):(d<6*4); d++) // one bit fixes cube rotations
      for (int x=0; x<4; x++)
	for (int y=0; y<4; y++)
	  for (int z=0; z<4; z++)
	  {
	    long tmp=express(piece_rots[n][d], x,y,z);
	    if (tmp && !piece_exp[n].has(tmp))
	    {
	      //printf("%d %d %d %d %d ", n,d,x,y,z);
	      //printlong(tmp);
	      //printf("%016lx\n",tmp);
	      piece_exp[n].add(tmp);
	    }
	  }
    piece_exp[n].printstats();
  }
}

long state[PIECES];

void endspiel()
{
  static int combo=0;
  printf("combo %d", ++combo);
  long sum=0;
  for (int i=0; i<PIECES; i++)
    sum |=state[i];
  if (sum == -1L)
    printf(" (verified)");
  printf("\n");
  for (int i=0; i<PIECES; i++)
  {
    //printf("%02d: ", i);
    //printlong(state[i]);
    printf("0x%016lx\n", state[i]);
  }
  printf("\n");
  fflush(0);
}

void add_piece(long curr, int pno)
{
  //printf("*%02d* ", pno);
  //printlong(curr);
  if (pno>=PIECES) endspiel();
  for (long i=0; i<piece_exp[pno].len; i++)
  {
    long tmp = state[pno] = piece_exp[pno].exp[i];
    if ((tmp & curr)==0L)
      add_piece(tmp | curr, pno+1);
  }
}

void exp_stats()
{
  for (int i=0; i<PIECES; i++)
  {
    printf("%02d: %ld\n", i, piece_exp[i].len);
  }
  fflush(0);
}

void collapse_pieces()
{
  piece_expression dummy("X");
  piece_expression s0[7] =
  {
    piece_expression(piece_exp[0], piece_exp[1]),
    piece_expression(piece_exp[2], piece_exp[3]),
    piece_expression(piece_exp[4], piece_exp[5]),
    piece_expression(piece_exp[6], piece_exp[7]),
    piece_expression(piece_exp[8], piece_exp[9]),
    piece_expression(piece_exp[10], piece_exp[11]),
    piece_expression(piece_exp[12], dummy)
  };
  piece_expression s1[4] =
  {
    piece_expression(s0[0], s0[1]),
    piece_expression(s0[2], s0[3]),
    piece_expression(s0[4], s0[5]),
    piece_expression(s0[6], dummy),
  };
  piece_expression s2[2] =
  {
    piece_expression(s1[0], s1[1]),
    piece_expression(s1[2], s1[3])
  };
  piece_expression final(s2[0], s2[1]);
}

int main(int arv, char** argv)
{
  prepare_piece_rots();
  prepare_piece_exp();
  //exp_stats();
  //cube c;
  //add_piece(0L, 0);
  collapse_pieces();
  return 0;
}
