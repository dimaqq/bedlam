#!/usr/bin/python

try:
  import psyco
  psyco.full()
  pass
except:
  print "Continuing without psyco"
  pass

pieces = {
  0: [ [0,0,0], [1,0,0], [1,1,0], [0,0,1]],
  1: [ [0,0,0], [1,0,0], [1,0,-1], [0,1,0], [0,1,1]],
  2: [ [0,0,0], [1,0,0], [-1,0,0], [0,1,0], [0,1,1]],
  3: [ [0,0,0], [1,0,0], [-1,0,0], [0,1,0], [1,0,1]],
  4: [ [0,0,0], [-1,0,0], [0,1,0], [0,0,1], [1,0,1]],
  5: [ [0,0,0], [-1,0,0], [-2,0,0], [0,1,0], [0,1,1]],
  6: [ [0,0,0], [1,0,0], [1,0,1], [0,1,0], [-1,1,0]],
  7: [ [0,0,0], [1,0,0], [1,1,0], [-1,0,0], [-1,0,-1]],
  8: [ [0,0,0], [1,0,0], [-1,0,0], [0,1,0], [0,0,1]],
  9: [ [0,0,0], [1,0,0], [2,0,0], [0,1,0], [0,0,1]],
  10: [ [0,0,0], [1,0,0], [1,-1,0], [0,1,0], [-1,1,0]],
  11: [ [0,0,0], [1,0,0], [1,-1,0], [-1,0,0], [0,1,0]],
  12: [ [0,0,0], [1,0,0], [-1,0,0], [0,1,0], [0,-1,0]]}

def rotx(a):
  if type(a[0]) is int:
    x,y,z=a
    return [x,z,-y]
  else:
    rv=[]
    for i in a:
      rv.append(rotx(i))
    return rv

def roty(a):
  if type(a[0]) is int:
    x,y,z=a
    return [-z,y,x]
  else:
    rv=[]
    for i in a:
      rv.append(roty(i))
    return rv

def rotz(a):
  if type(a[0]) is int:
    x,y,z=a
    return [y,-x,z]
  else:
    rv=[]
    for i in a:
      rv.append(rotz(i))
    return rv

exps={}

def add_rot_xyz(i,a,x,y,z):
  #print i, len(a), x,y,z
  e=0
  for n in a:
    xxx,yyy,zzz=n
    #print "  ", xxx+x, yyy+y, zzz+z
    if xxx+x<0 or xxx+x>3: return
    if yyy+y<0 or yyy+y>3: return
    if zzz+z<0 or zzz+z>3: return
    e|=1L<<((xxx+x)*16+(yyy+y)*4+zzz+z)
  if e not in exps[i]:
    exps[i].append(e)

def add_rot(i,a):
  if i not in exps: exps[i]=[]
  for x in range(4):
    for y in range(4):
      for z in range(4):
	add_rot_xyz(i,a,x,y,z)

for i in pieces:
  b=pieces[i]
  if i == 4:
    # avoid solutions differing only in rotation of the whole cube
    # by fixing rotations of any one asymmetric piece
    add_rot(i,b)
  else:
    for r in range(4):
      add_rot(i,b)
      add_rot(i,roty(b))
      add_rot(i,roty(roty(b)))
      add_rot(i,roty(roty(roty(b))))
      add_rot(i,rotz(b))
      add_rot(i,rotz(rotz(rotz(b))))
      b=rotx(b)

#------------------------------------------------------------------------------#

def bitmap_to_tuple(e):
  rv = []
  for i in range(0,64):
    rv.append( e&(1L<<i) and 1 or 0)
  return tuple(rv)

def tuple_to_bitmap(e):
  rv = 0
  for i in range(0,64):
    rv |= (long(e[i])<<i)
  return rv

def collapse(e, i):
  for _ in range(i):
    # fold in half
    e = tuple(map(int.__add__, e[len(e)/2:], e[:len(e)/2]))
  return e

explevels={}
for k in exps:
  explevels[k]={}
  for i in range(0,7):
    explevels[k][i]=[]
  for e in exps[k]:
    for i in range(0,7):
      c = collapse(bitmap_to_tuple(e), i)
      if c not in explevels[k][i]:
        explevels[k][i].append(c)

tree={}

def tree_insert(key, path):
  t = tree[key]
  for i in path:
    if i not in t:
      t[i] = {}
    t = t[i]

for k in explevels:
  print k, len(exps[k]), ":",
  for l in explevels[k]:
    print len(explevels[k][l]),
  print

print

for k in exps:
  tree[k] = {}
  for e in exps[k]:
    path = []
    r = range(7)
    r.reverse()
    for i in r:
      path.append(collapse(bitmap_to_tuple(e), i))
    tree_insert(k, path+[e])

def allkeys(d, depth):
  if depth==0:
    return d.keys()
  rv = []
  for i in d:
    rv += allkeys(d[i], depth-1)
  return rv

for k in tree:
  print k, len(exps[k]), ":",
  for i in range(7):
    print len(allkeys(tree[k], i)),
  print

print

def superblah(ls, lev):
  if lev==128:
    print "XXsolution"
    for yy in range(len(ls)):
      print yy, len(ls[yy].keys()), "%016x" % ls[yy].keys()[0]
    return
  #print map(dict.keys, ls), lev
  match=[None]*13
  def iter(acc, i):
    if i==13:
      superblah(match, lev*2)
      return
    for ex in ls[i]:
      ne = map(int.__add__, acc, ex)
      if True not in [xx>(64/lev) for xx in ne]:
        match[i]=ls[i][ex]
        iter(ne, i+1)
  iter([0]*lev, 0)

superblah([tree[i] for i in range(13)], 1)
