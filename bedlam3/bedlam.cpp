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

bool operator==(const dir& a, const dir& b)
{
  return (a.x == b.x && a.y == b.y && a.z == b.z);
}

int cmp(const dir& a, const dir& b)
{
  if (a.x > b.x) return 1;
  if (a.x < b.x) return -1;
  if (a.y > b.y) return 1;
  if (a.y < b.y) return -1;
  if (a.z > b.z) return 1;
  if (a.z < b.z) return -1;
  return 0;
}

bool operator>(const dir& a, const dir& b)
{ return cmp(a,b)>0; }

bool operator<(const dir& a, const dir& b)
{ return cmp(a,b)<0; }

struct piece
{
  short name;
  short len;
  dir bits[5];
  void rotx()
  {
    for (short i=0; i<len; i++)
      bits[i].rotx();
  }
  void roty()
  {
    for (short i=0; i<len; i++)
      bits[i].roty();
  }
  void rotz()
  {
    for (short i=0; i<len; i++)
      bits[i].rotz();
  }
};

bool operator==(const piece& a, const piece& b)
{
  if (a.len != b.len) return false;
  for (short i=0; i<a.len; i++)
    if (a.bits[i] != b.bits[i]) return false;
  return true;
}

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

struct rots
{
  short len;
  piece pieces[6*4];
};

rots piece_rots[PIECES];

//piece piece_rots[PIECES][6*4];

void printspaces(short n)
{ for (short i=0; i<n; i++) printf(" "); }

struct cube
{
  char data[4][4][4];

  cube(){ memset(data, 0, sizeof(data)); }
  bool full() const
  {
    for (short x=0; x<4; x++)
      for (short y=0; y<4; y++)
	for (short z=0; z<4; z++)
	  if (data[x][y][z]==0) return false;
    return true;
  }
  void print() const
  {
    for (short x=0; x<4; x++)
    {
      printspaces(x*6);
      printf("layer %d%s\n", x, x==0?" (front)":x==3?" (back)":"");
      for (short y=0; y<4; y++)
      {
	printspaces(x*6);
	for (short z=0; z<4; z++)
	  printf("% 3d", (int) (data[x][y][z]));
	printf("\n");
      }
      printf("\n");
    }
  }
  inline bool bound_hole(short x, short y, short z) const
  {
    if (x>0) if (data[x-1][y][z]==0) return false;
    if (x<3) if (data[x+1][y][z]==0) return false;
    if (y>0) if (data[x][y-1][z]==0) return false;
    if (y<3) if (data[x][y+1][z]==0) return false;
    if (z>0) if (data[x][y][z-1]==0) return false;
    if (z<3) if (data[x][y][z+1]==0) return false;
    return true;
  }
  inline bool no_bound_holes() const
  {
    for (short x=0; x<4; x++)
      for (short y=0; y<4; y++)
	for (short z=0; z<4; z++)
	  if (data[x][y][z]==0)
	    if (bound_hole(x,y,z)) return false;
    return true;
  }
  inline bool fits(short piece_no, short dir, short xxx, short yyy, short zzz)
  {
    piece& p=piece_rots[piece_no].pieces[dir];
    for (short i=0; i<p.len; i++)
    {
      if (xxx+p.bits[i].x < 0 || xxx+p.bits[i].x>3 ||
	  yyy+p.bits[i].y < 0 || yyy+p.bits[i].y>3 ||
	  zzz+p.bits[i].z < 0 || zzz+p.bits[i].z>3)
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
  for (short i=0; i<PIECES; i++)
  {
    piece p = pieces[i];
    piece_rots[i]=6*4;
    for (short r=0; r<4; r++)
    {
      short d=0;
      piece s=p;
      piece_rots[i].pieces[(d++)*4+r] = s;
      s.roty();
      piece_rots[i].pieces[(d++)*4+r] = s;
      s.roty();
      piece_rots[i].pieces[(d++)*4+r] = s;
      s.roty();
      piece_rots[i].pieces[(d++)*4+r] = s;
      s.roty();
      s.rotz();
      piece_rots[i].pieces[(d++)*4+r] = s;
      s.rotz();
      s.rotz();
      piece_rots[i].pieces[(d++)*4+r] = s;
      // rotate p along x
      p.rotx();
    }
  }
}

void reduce_piece_rots()
{
  for (short i=0; i<PIECES; i++)
  {
  }
}

void check_cube(const cube& c)
{
  if (c.full()) printf("full ****\n");

  c.print();
  //exit(0);
}

void add_piece(cube ccc, short n)
{
  //if (n<7)
  //{
  //printf("now: "); printspaces(n); printf("%d\n", n);
  //ccc.print();
  //}
  if (n>=PIECES)
  { check_cube(ccc); return; }

  for (short d=0; d<piece_rots[i]; d++)
    for (short x=0; x<4; x++)
      for (short y=0; y<4; y++)
	for (short z=0; z<4; z++)
	{
	  cube c=ccc;
	  if (c.fits(n, d, x, y, z))
	  {
	    if (c.no_bound_holes())
	      add_piece(c, n+1);
	  }
	  //if (n==12) c.print();
	}
}


int main(int arv, char** argv)
{
  prepare_piece_rots();
  reduce_piece_rots();
  cube c;
  //add_piece(c, 0);
  return 0;
}
