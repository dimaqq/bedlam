#include <stdio.h>

struct bit
{
  int x,y,z;
  bit(int a, int b, int c): x(a), y(b), z(c)
  {}
  bit(): x(0), y(0), z(0)
  {}
};

struct piece
{ int len; bit bits[5]; };

piece pieces[] = 
{
  /*0*/ {4, {bit(0,0,0), bit(1,0,0), bit(1,1,0), bit(0,0,1)}},
  /*1*/ {5, {bit(0,0,0), bit(1,0,0), bit(1,0,-1), bit(0,1,0), bit(0,1,1)}},
  /*2*/ {5, {bit(0,0,0), bit(1,0,0), bit(-1,0,0), bit(0,1,0), bit(0,1,1)}},
  /*3*/ {5, {bit(0,0,0), bit(1,0,0), bit(-1,0,0), bit(0,1,0), bit(1,0,1)}},
  /*4*/ {5, {bit(0,0,0), bit(-1,0,0), bit(0,1,0), bit(0,0,1), bit(1,0,1)}},
  /*5*/ {5, {bit(0,0,0), bit(-1,0,0), bit(-2,0,0), bit(0,1,0), bit(0,1,1)}},
  /*6*/ {5, {bit(0,0,0), bit(1,0,0), bit(1,0,1), bit(0,1,0), bit(-1,1,0)}},
  /*7*/ {5, {bit(0,0,0), bit(1,0,0), bit(1,1,0), bit(-1,0,0), bit(-1,0,-1)}},
  /*8*/ {5, {bit(0,0,0), bit(1,0,0), bit(-1,0,0), bit(0,1,0), bit(0,0,1)}},
  /*9*/ {5, {bit(0,0,0), bit(1,0,0), bit(2,0,0), bit(0,1,0), bit(0,0,1)}},
  /*10*/ {5, {bit(0,0,0), bit(1,0,0), bit(1,-1,0), bit(0,1,0), bit(-1,1,0)}},
  /*11*/ {5, {bit(0,0,0), bit(1,0,0), bit(1,-1,0), bit(-1,0,0), bit(0,1,0)}},
  /*12*/ {5, {bit(0,0,0), bit(1,0,0), bit(-1,0,0), bit(0,1,0), bit(0,-1,0)}}
};


bit rotx(bit i)
{
  return bit(i.x, i.z, -i.y);
}

bit roty(bit i)
{
  return bit(-i.z, i.y, i.x);
}

bit rotz(bit i)
{
  return bit(i.y, -i.x, i.z);
}

piece piece_rotx(piece arg)
{
  piece rv;
  rv.len = arg.len;
  for (int i=0; i<arg.len; i++)
    rv.bits[i]=rotx(arg.bits[i]);
  return rv;
}

piece piece_roty(piece arg)
{
  piece rv;
  rv.len = arg.len;
  for (int i=0; i<arg.len; i++)
    rv.bits[i]=roty(arg.bits[i]);
  return rv;
}

piece piece_rotz(piece arg)
{
  piece rv;
  rv.len = arg.len;
  for (int i=0; i<arg.len; i++)
    rv.bits[i]=rotz(arg.bits[i]);
  return rv;
}

int expl[13] = {0};
unsigned long exps[13][500] = {0};

void add_rot_xyz(int i, piece a, int x, int y, int z)
{
  unsigned long e=0;
  for (int n=0; n<a.len; n++)
  {
    if (a.bits[n].x+x<0 || a.bits[n].x+x>3) return;
    if (a.bits[n].y+y<0 || a.bits[n].y+y>3) return;
    if (a.bits[n].z+z<0 || a.bits[n].z+z>3) return;
    e |= 1UL<<((a.bits[n].x+x)*16+(a.bits[n].y+y)*4+a.bits[n].z+z);
  }
  for (int n=0; n<expl[i]; n++)
    if (exps[i][n] == e) return;
  exps[i][expl[i]++] = e;
}

void add_rot(int i, piece a)
{
  for (int x=0; x<4; x++)
    for (int y=0; y<4; y++)
      for (int z=0; z<4; z++)
	add_rot_xyz(i, a, x,y,z);
}

#define pos(x,y,z) (x*16+y*4+z)

unsigned long total=0;

int main(int argc, char** argv)
{
  for (int i=0; i<13; i++)
  {
    if (i==4)
    {
      add_rot(i, pieces[i]);
    }
    else
    {
      piece b = pieces[i];
      for (int r=0; r<4; r++)
      {
	add_rot(i,b);
	add_rot(i,piece_roty(b));
	add_rot(i,piece_roty(piece_roty(b)));
	add_rot(i,piece_roty(piece_roty(piece_roty(b))));
	add_rot(i,piece_rotz(b));
	add_rot(i,piece_rotz(piece_rotz(piece_rotz(b))));
	b = piece_rotx(b);
      }
    }
  }

  for (int i=0; i<13; i++)
    printf("%d: %d\n", i, expl[i]);

  recur(0,0,0);
  printf("total: %ld\n", total);
}
