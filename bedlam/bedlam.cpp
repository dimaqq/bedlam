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
  { 2, 5, { dir(0,0,0), dir(1,0,0), dir(1,-1,0), dir(0,1,0), dir(0,1,1)}},
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

struct cube
{
  char data[6][6][6];

  cube(){ memset(data, 0, sizeof(data)); }
  bool safe(){ return !sentinel(); }
  bool full()
  {
    for (int x=0; x<6; x++)
      for (int y=0; y<6; y++)
	for (int z=0; z<6; z++)
	{
	  int c=0;
	  if (x==0 || x==5) c++;
	  if (y==0 || y==5) c++;
	  if (z==0 || z==5) c++;

	  if (c==0 && data[x][y][z]==0) return false;
	}
    return true;
  }
  bool sentinel()
  {
    for (int x=0; x<6; x++)
      for (int y=0; y<6; y++)
	for (int z=0; z<6; z++)
	{
	  int c=0;
	  if (x==0 || x==5) c++;
	  if (y==0 || y==5) c++;
	  if (z==0 || z==5) c++;

	  if (c>=1 && data[x][y][z]!=0) return true;
	}
    return false;
  }
  void print()
  {
    for (int x=0; x<6; x++)
    {
      printspaces(x*6);
      printf("layer %d%s\n", x, x==0?" (front)":x==5?" (back)":"");
      for (int y=0; y<6; y++)
      {
	printspaces(x*6);
	for (int z=0; z<6; z++)
	  printf("% 3d", (int) (data[x][y][z]));
	printf("\n");
      }
      printf("\n");
    }
  }
  bool fits(int piece_no, int dir, int xxx, int yyy, int zzz)
  {
    piece& p=piece_rots[piece_no][dir];
    for (int i=0; i<p.len; i++)
    {
      if (xxx+p.bits[i].x <= 0 || xxx+p.bits[i].x>=5 ||
	  yyy+p.bits[i].y <= 0 || yyy+p.bits[i].y>=5 ||
	  zzz+p.bits[i].z <= 0 || zzz+p.bits[i].z>=5)
	return false;
      if (data[xxx+p.bits[i].x][yyy+p.bits[i].y][zzz+p.bits[i].z])
	return false;
      data[xxx+p.bits[i].x][yyy+p.bits[i].y][zzz+p.bits[i].z]=p.name;
    }
    return true;
  }
};

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

void check_cube(cube c)
{
  if (c.full()) printf(" full");
  if (c.safe()) printf(" safe");

  printf("\n\n");
  c.print();
  exit(0);
}

void add_piece(cube ccc, int n)
{
  if (n>=PIECES)
  { check_cube(ccc); return; }

  for (int d=0; d<6*4; d++)
    for (int x=0; x<6; x++)
      for (int y=0; y<6; y++)
	for (int z=0; z<6; z++)
	{
	  cube c=ccc;
	  if (c.fits(n, d, x, y, z))
	    add_piece(c, n+1);
	  else
	    if (n>10) printf("%d ", n);
	}
}


int main(int arv, char** argv)
{
  prepare_piece_rots();
  cube c;
  add_piece(c, 0);
  return 0;
}
