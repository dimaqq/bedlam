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

// 443m
//int order[] = { pos(0,0,0), pos(0,0,1), pos(0,1,0), pos(0,1,1),
//                pos(1,0,0), pos(1,0,1), pos(1,1,0), pos(1,1,1) };

// 83m
//int order[] = { pos(0,0,0), pos(0,0,3), pos(0,3,0), pos(0,3,3),
//                pos(3,0,0), pos(3,0,3), pos(3,3,0), pos(3,3,3) };

int order[64] = { pos(0,0,0), pos(0,0,3), pos(0,3,0), pos(0,3,3),
                pos(3,0,0), pos(3,0,3), pos(3,3,0), pos(3,3,3), 
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };

unsigned long total=0;

void recur(unsigned long bitmask, int depth, int taken_mask)
{
  //if (depth>=sizeof(order)/sizeof(order[0]))
  if (depth>=6)
  {
    //printf("bitmask: %016lx, depth: %d, taken: %04x\n", bitmask, depth, taken_mask);
    total +=1;
    return;
  }

  for (int i=0; i<13; i++)
  {
    if (taken_mask & (1UL<<i))
      continue;
    if (bitmask & (1UL<<order[depth]))
      recur(bitmask, depth+1, taken_mask);
    else
      for (int r=0; r<expl[i]; r++)
      {
	if (bitmask & exps[i][r])
	  continue;
	if ((1UL<<order[depth]) & exps[i][r])
	  recur(bitmask | exps[i][r], depth+1, taken_mask | (1UL<<i));
      }
  }
}

struct cr
{
  long mask;
  long length;
  long* data;
  int* taken;
};

cr cross(int a, int b)
{
  long tota=0, totb=0;

  for (int i=0; i<13; i++)
    for (int r=0; r<expl[i]; r++)
    {
      if ((1UL<<a) & exps[i][r]) tota++;
      if ((1UL<<b) & exps[i][r]) totb++;
    }

  cr rv = { (1UL<<a)|(1UL<<b), 0, new long [tota*totb], new int [tota*totb] };

  for (int i=0; i<13; i++)
    for (int r=0; r<expl[i]; r++)
    {
      if (!((1UL<<a) & exps[i][r])) continue;

      if ((1UL<<b) & exps[i][r])
      {
	rv.data[rv.length] = exps[i][r];
	rv.taken[rv.length] = (1UL<<i);
	rv.length++;
	continue;
      }
      
      for (int j=0; j<13; j++)
      {
	if (j==i) continue;

	for (int p=0; p<expl[j]; p++)
	{
	  if (!((1UL<<b) & exps[j][p])) continue;

	  if ((1UL<<a) & (1UL<<b)) continue;

	  rv.data[rv.length] = exps[i][r] | exps[j][p];
	  rv.taken[rv.length] = (1UL<<i)|(1UL<<j);
	  rv.length++;
	}
      }
    }
  return rv;
}

cr dat[4];

void test_recur(unsigned long bitmask, int depth, int taken_mask)
{
  if (depth>=3)
  {
    total +=1;
    return;
  }

  for (long i=0; i<dat[depth].length; i++)
  {
    if (taken_mask & dat[depth].taken[i]) continue;
    if (bitmask & dat[depth].mask)
    {
      if (bitmask & dat[depth].mask == dat[depth].mask)
	recur(bitmask, depth+1, taken_mask);
      else
	printf("not imeplemented! %s:%d\n", __FILE__, __LINE__);
    }
    else
      recur(bitmask | dat[depth].data[i], depth+1, taken_mask | dat[depth].taken[i]);
  }
}

void test_cross()
{
  dat[0] = cross(order[0], order[1]);
  dat[1] = cross(order[2], order[3]);
  dat[2] = cross(order[4], order[5]);
  //dat[3] = cross(order[5], order[6]);
  for (int i=0; i<3; i++)
    printf("dat[%d].length = %ld;\n", i, dat[i].length);
}

bool belongs_to_order(int arg)
{
  for (int z=0; z<64; z++)
    if (order[z]==arg) return true;
  return false;
}

int does_not_belong_to_order(void)
{
  for (int j=0; j<64; j++)
    if (!belongs_to_order(j)) return j;
  return 0;
}

int main(int argc, char** argv)
{
  for (int i=8; i<64; i++)
    order[i] = does_not_belong_to_order();

  printf("order:");
  for (int i=0; i<64; i++)
    printf(" %d", order[i]);
  printf("\n\n");

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

  //recur(0,0,0);
  test_cross();
  test_recur(0,0,0);
  printf("total: %ld\n", total);
}
