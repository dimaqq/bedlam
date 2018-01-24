#!/usr/bin/python
import sys

if "-psyco" in sys.argv:
  try:
    import psyco
    psyco.profile()
  except:
    print 'Psyco not found, ignoring it'

if "-print" in sys.argv:
  mod_print=True
else:
  mod_print=False

limit=6
for s in sys.argv:
  if s.startswith("-limit="):
    limit = int(s[len("-limit="):])

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
    return [-y,x,z]
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
  if True:
    for r in range(4):
      add_rot(i,b)
      add_rot(i,roty(b))
      add_rot(i,roty(roty(b)))
      add_rot(i,roty(roty(roty(b))))
      add_rot(i,rotz(b))
      add_rot(i,rotz(rotz(rotz(b))))
      b=roty(b)

# print a long in binary (base 2)
# print ''.join([str(int(j&(1L<<63-i)!=0)) for i in range(0,64)])
def bin(x):
  return ''.join([str(int(x&(1L<<63-i)!=0)) for i in range(0,64)])

# for i in exps:
#   print i, len(exps[i])
#   for j in exps[i]:
#     # print "%016x" % j
#     print ''.join([str(int(j&(1L<<63-i)!=0)) for i in range(0,64)])

all = [0, 1, 4, 5, 16, 17, 20, 21]
order = []
if limit>len(all): limit = len(order)
counter = 0
cut=False
cutoff=False

def finish(cubeexp, used):
  if mod_print:
    print ''.join([(63-i) in order and "1" or "0" for i in range(0,64)]), ": all"
    print ''.join([(63-i) in order[:limit] and "1" or "0" for i in range(0,64)]), ": mask"
    print bin(cubeexp), ": cube", len(used)
    for i in used:
      print bin(used[i]), ":", i
    print
  global counter, cut
  counter+=1
  if type(cutoff) is int and counter > cutoff: cut=True; print "cut!"

def recur(cubeexp, used, iter):
  if cut: return
  if iter>=limit: finish(cubeexp, used); return
  if cubeexp&(1L<<order[iter]): recur(cubeexp, used, iter+1); return
  for i in exps:
    if cut: return
    if i not in used.keys():
      for j in exps[i]:
        if cut: return
        if j&(1L<<order[iter]) and not j & cubeexp:
	  newcubeexp = j | cubeexp
	  newused = used.copy()
          newused[i]=j
	  newiter = iter+1
          recur(newcubeexp, newused, newiter)

if mod_print:
  for i in exps:
    print i, len(exps[i])

#simple solver
#recur(0L, {}, 0)
#print "totals: ", counter

all = range(0,64)
# dynamic solver - chooses the order on line
totallimit=8
iter=0
totalorder=[]
while iter<totallimit:
  d = {}
  cutoff=False
  for o in all:
    if o not in totalorder:
      # cook order, counter, limit
      order = totalorder+[o]
      limit = len(order)
      counter=0
      cut=False
      recur(0L, {}, 0)
      print o, "gives", counter
      d[counter] = o
      if type(cutoff) is int: cutoff = min([cutoff, counter])
      else: cutoff=counter
      print "new cutoff", cutoff
  next = d[min(d.keys())]
  print totalorder, d, "chose", next
  totalorder+=[next]
  iter+=1
