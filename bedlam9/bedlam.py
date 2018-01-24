#!/usr/bin/python

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
    e|=1<<((xxx+x)*16+(yyy+y)*4+zzz+z)
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

freq = [0]*64
for i in exps:
  print i, len(exps[i])
  for j in exps[i]:
    #print "%016x" % j
    for i in range(0, 64):
      if j & (1<<i):
        freq[i] += 1
    pass

print

# bit frequency shows that corners are least popular places
print freq

print 
print

total=0

def pos(x,y,z):
  return x*16+y*4+z

order = ()
results = []

def recur_partial(bitmask, depth):
  if depth>=len(order):
    results.append(bitmask)
    return

  for e in exps[order[depth]]:
    if bitmask & e: continue
    recur_partial(bitmask|e, depth+1)

order = (0, 1,)
results = []

recur_partial(0L,0)
print len(results)
      
holes = (
    [[0,0,0]],
    [[0,0,0], [0,0,1]],
    [[0,0,0], [0,0,1], [0,0,2]],
    [[0,0,0], [0,0,1], [0,1,0]] )
      
def outline(hole): #direct neighbours
  tmp = []
  for h in hole:
    tmp.append((h[0]+1, h[1], h[2]))
    tmp.append((h[0]-1, h[1], h[2]))
    tmp.append((h[0], h[1]+1, h[2]))
    tmp.append((h[0], h[1]-1, h[2]))
    tmp.append((h[0], h[1], h[2]+1))
    tmp.append((h[0], h[1], h[2]-1))
  rv = 0L;
  for t in tmp:
    if True in [i<0 or i>3 for i in t]: continue
    if t in hole: continue
    rv |= (1L<<(t[0]*16+t[1]*4+t[2]))
  return rv

def move(bit, x,y,z):
  return [[b[0]+x, b[1]+y, b[2]+z] for b in bit]

htmp = []
hexps = []

def fits(b):
  for bit in b:
    for coord in bit:
      if coord<0 or coord>3: return False
  return True

def add_hole(b):
  global htmp, hexps
  for x in range(0,4):
    for y in range(0,4):
      for z in range(0,4):
	hole = move(b, x,y,z)
	#print hole, fits(hole)
        if not fits(hole): continue
	e = 0L
	for h in hole:
          e |= (1L<<(h[0]*16+h[1]*4+h[2]))
	#print "%016x" % e
        if e in htmp: continue
	#print "%016x" % e, "xxx"
	htmp.append(e)
        hexps.append( (e, outline(hole)) )

for hole in holes:
  b=hole
  for r in range(4):
    add_hole(b)
    add_hole(roty(b))
    add_hole(roty(roty(b)))
    add_hole(roty(roty(roty(b))))
    add_hole(rotz(b))
    add_hole(rotz(rotz(rotz(b))))
    b=rotx(b)

for h in hexps:
  print "%016x" % h[0]
  print "%016x" % h[1]
  print ""

print len(hexps)











#order = [ pos(0,0,0), pos(0,0,3), pos(0,3,0), pos(0,3,3),
#          pos(3,0,0), pos(3,0,3), pos(3,3,0), pos(3,3,3) ]
#
#order = [ pos(0,0,0), pos(0,0,1), pos(0,1,0), pos(0,1,1),
#          pos(1,0,0), pos(1,0,1), pos(1,1,0), pos(1,1,1) ]
#
#print order
#
#def recur(bitmask, depth, taken_mask):
#  global total
#  #if depth>=4:
#  if depth>=len(order):
#    print "bitmask: %016x, depth: %d, taken: %04x"% (bitmask, depth, taken_mask)
#    total+=1
#    return
#  for i in pieces:
#    if taken_mask & (1<<i):
#      continue
#    if bitmask & (1<<order[depth]):
#      recur(bitmask, depth+1, taken_mask)
#    else:
#      for r in exps[i]:
#        if bitmask & r:
#          continue
#        if (1<<order[depth]) & r:
#          recur(bitmask | r, depth+1, taken_mask | (1<<i))
#
#recur(0,0,0)
#print "total", total
#
#for i in exps[0]:
  #print "%016x" % i
