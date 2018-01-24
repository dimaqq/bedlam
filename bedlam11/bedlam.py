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

#freq = [0]*64
#for i in exps:
  #print i, len(exps[i])
  #for j in exps[i]:
    ##print "%016x" % j
    #for i in range(0, 64):
      #if j & (1<<i):
        #freq[i] += 1
    #pass
#
#print
#
## bit frequency shows that corners are least popular places
#print freq

print 
print

def pos(x,y,z):
  return x*16+y*4+z

e2={}
for i in exps:
  tmp=[]
  for e in exps[i]:
    exp=[0,0]
    for shift in range(0,64,2):
      ee = e>>shift
      exp[0] += (ee & 2 and 1 or 0)
      exp[1] += (ee & 1 and 1 or 0)
    if tuple(exp) not in tmp: tmp.append(tuple(exp))
  e2[i]=tmp

max_total=1L
for i in e2:
  print i, len(e2[i])
  max_total*=len(e2[i])
  #for e in e2[i]:
    #print e
print "max total (e2)", max_total

e4={}

for i in exps:
  tmp=[]
  for e in exps[i]:
    exp=[0,0,0,0]
    for shift in range(0,64,4):
      ee = e>>shift
      exp[0] += (ee & 0x8 and 1 or 0)
      exp[1] += (ee & 0x4 and 1 or 0)
      exp[2] += (ee & 0x2 and 1 or 0)
      exp[3] += (ee & 0x1 and 1 or 0)
    if tuple(exp) not in tmp: tmp.append(tuple(exp))
  e4[i]=tmp

max_total=1L
for i in e4:
  print i, len(e4[i])
  max_total*=len(e4[i])
  #for e in e4[i]:
    #print e
print "max total (e4)", max_total

total=0
cull_low=0
cull_high=0

def cull_e2(state, depth):
  global cull_low, cull_high
  for i in state:
    if i>32:
      cull_high+=1
      #print depth, state, "high"
      return True
    if i<32-(13-depth)*5:
      cull_low+=1
      #print depth, state, "low"
      return True
  return False

def recur_e2(state, depth):
  global total
  if cull_e2(state, depth):
    return
  if depth>=13:
    total+=1
    return
  for o in e2[depth]:
    new = map(int.__add__, state, o)
    recur_e2(new, depth+1)

def cull(state, depth):
  global cull_low, cull_high
  for i in state:
    if i>16:
      cull_high+=1
      return True
    if i<16-(13-depth)*5:
      cull_low+=1
      return True
  return False

def recur(state, depth):
  global total
  if cull(state, depth):
    return
  if depth>=0:
  #if depth>=10:
    total+=1
    return
  for o in e4[depth]:
    new = map(int.__add__, state, o)
    recur(new, depth+1)

recur_e2((0,0), 0)

print "results:", total, cull_low, cull_high
