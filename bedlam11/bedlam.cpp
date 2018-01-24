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

void setup_exps()
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
}

#include <assert.h>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

struct k2 { char v[2]; };
struct k4 { char v[4]; };
struct k8 { char v[8]; };

bool operator<(const k2& a, const k2& b) { return memcmp(a.v, b.v, 2)<0; }
bool operator<(const k4& a, const k4& b) { return memcmp(a.v, b.v, 4)<0; }
bool operator<(const k8& a, const k8& b) { return memcmp(a.v, b.v, 8)<0; }

int sum(const k2& a){int rv=0; for (int i=0; i<2; i++) {rv+=a.v[i];} return rv;}
int sum(const k4& a){int rv=0; for (int i=0; i<4; i++) {rv+=a.v[i];} return rv;}
int sum(const k8& a){int rv=0; for (int i=0; i<8; i++) {rv+=a.v[i];} return rv;}

k2 operator+(const k2& a, const k2& b)
{ k2 rv; for (int i=0; i<2; i++) { rv.v[i] = a.v[i]+b.v[i]; } return rv; }
k4 operator+(const k4& a, const k4& b)
{ k4 rv; for (int i=0; i<4; i++) { rv.v[i] = a.v[i]+b.v[i]; } return rv; }
k8 operator+(const k8& a, const k8& b)
{ k8 rv; for (int i=0; i<8; i++) { rv.v[i] = a.v[i]+b.v[i]; } return rv; }

typedef map< k8,vector<unsigned long> > map8;
typedef map< k4,map8 > map4;
typedef map< k2,map4 > map2;
typedef map< int,map2 > mapi;

mapi tree;

char nth(unsigned long value, int nth, int ord)
{
  char rv=0;
  for (int i=0; i<64; i+=nth)
  {
    unsigned long sh = value >> i;
    rv += (sh & (1<<(nth-ord-1)))?1:0;
  }
  //printf("nth: 0x%016lx, %d, %d: %d\n", value, nth, ord, (int)rv);
  return rv;
}

void setup_tree()
{
  for (int i=0; i<13; i++)
  {
    for (int j=0; j<expl[i]; j++)
    {
      unsigned long value = exps[i][j];
      k2 i2;
      for (int b=0; b<2; b++) i2.v[b]=nth(value, 2, b);
      k4 i4;
      for (int b=0; b<4; b++) i4.v[b]=nth(value, 4, b);
      k8 i8;
      for (int b=0; b<8; b++) i8.v[b]=nth(value, 8, b);
      assert(sum(i2)==sum(i4));
      assert(sum(i4)==sum(i8));
      vector<unsigned long>& v = tree[i][i2][i4][i8];
      if (find(v.begin(), v.end(), value)==v.end()) v.push_back(value);
    }
  }
}

unsigned long total=0;
unsigned long cull_low=0;
unsigned long cull_high=0;

int main(int argc, char** argv)
{
  setup_exps();
  setup_tree();

  for (int i=0; i<13; i++)
  {
    printf("%d: %d\n", i, expl[i]);
    for (map2::const_iterator i2=tree[i].begin();
	i2!=tree[i].end(); i2++)
    {
      printf("(%d, %d)\n", (int) i2->first.v[0], (int) i2->first.v[1]);
      for (map4::const_iterator i4=tree[i][i2->first].begin();
	  i4!=tree[i][i2->first].end(); i4++)
      {
	/*
	printf("(%d, %d, %d, %d)\n",
	    (int) i4->first.v[0],
	    (int) i4->first.v[1],
	    (int) i4->first.v[2],
	    (int) i4->first.v[3]);
	    */
	for (map8::const_iterator i8=tree[i][i2->first][i4->first].begin();
	    i8!=tree[i][i2->first][i4->first].end(); i8++)
	{
	  /*
	  printf("(%d, %d, %d, %d, %d, %d, %d, %d)\n",
	      (int) i8->first.v[0],
	      (int) i8->first.v[1],
	      (int) i8->first.v[2],
	      (int) i8->first.v[3],
	      (int) i8->first.v[4],
	      (int) i8->first.v[5],
	      (int) i8->first.v[6],
	      (int) i8->first.v[7]);
	      */
	}
      }
    }
  }

  //recur(0,0,0);
  printf("total: %ld\n", total);
}
