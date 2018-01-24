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
    char tmp=z;
    z=x;
    x=-tmp;
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
  piece_expression()
  {
    len = 0;
    memset(exp, 0, sizeof(exp));
  }
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
  void printstats()
  {
    printf("%d: %d\n", name, len);
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
//piece_expression piece_exp[16];

void prepare_piece_exp()
{
  for (int n=0; n<PIECES; n++)
  {
    piece_exp[n].name = pieces[n].name;
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

struct track_slot
{
  int piece;
  int exp;
}track[64]={{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},};

void finish(int slot, int used, long cube)
{
  printf("Finished!\n");
  if (slot==64) printf("slot check\n");
  if (used==0x1fff) printf("used check\n");
  if (cube==-1L) printf("cube check\n");
  for (int i=0; i<64; i++)
    printf("%d:%d ", track[i].piece, track[i].exp);
  printf("\n");
  for (int i=0; i<PIECES; i++)
    for (int s=0; s<64; s++)
      if (track[s].piece==i)
      {
	printf("%02d: ", i);
	printlong(piece_exp[i].exp[track[s].exp]);
      }
}

/*

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
*/

void process_slot(int slot, int used, long cube)
{
  if (slot>=64)
  {
    finish(slot, used, cube);
    printf("topped off\n\n");
    fflush(0);
    return;
  }
#define NEXT(i,n,s,u,c) \
  do {\
    track[slot].piece=i;\
    track[slot].exp=n;\
    process_slot(s,u,c);\
  } while(0)
  long bit=1L<<slot;
  if (cube & bit) NEXT(-1,-1,slot+1, used, cube);
  else
    for (int i=0; i<PIECES; i++)
      if (((1<<i)&used)==0)
	for (int n=0; n<piece_exp[i].len; n++)
	  if ((bit&piece_exp[i].exp[n])!=0&&(cube&piece_exp[i].exp[n])==0L)
	    NEXT(i,n,slot+1, used|(1<<i), cube|piece_exp[i].exp[n]);
}

/*
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
*/

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
  process_slot(0,0,0L);
  //exp_stats();
  //cube c;
  //add_piece(0L, 0);
  return 0;
}
