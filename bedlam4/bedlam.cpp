#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

struct piece_expression
{
  int name;
  int len;
  long exp[6*4*64];
  piece_expression(): name(0), len(0) { memset(exp, 0, sizeof(exp)); }
  bool has(long arg) const
  {
    for (int i=0; i<len; i++)
      if (exp[i]==arg) return true;
    return false;
  }
  void add(long arg)
  {
    exp[len]=arg;
    len++;
  }
};

long express(piece& p, int xxx, int yyy, int zzz)
{
  long rv=0;
  for (int i=0; i<p.len; i++)
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

void prepare_piece_exp()
{
  for (int n=0; n<PIECES; n++)
  {
    piece_exp[n].name=pieces[n].name;
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
  for (int i=0; i<piece_exp[pno].len; i++)
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
    printf("%02d: %d\n", i, piece_exp[i].len);
  }
  fflush(0);
}

int main(int arv, char** argv)
{
  prepare_piece_rots();
  prepare_piece_exp();
  //exp_stats();
  //cube c;
  add_piece(0L, 0);
  return 0;
}
