#!/usr/bin/python
import exps

#print "Acquired expressions"
#for k in exps.exps:
#  print k, ":", len(exps.exps[k])

def int_to_tuple(i):
  return tuple(reversed([i&(1<<bits) and 1 or 0 for bits in range(64)]))

def tuple_to_int(t):
  t = tuple(reversed(t))
  return sum([t[bits]<<bits for bits in range(64)])

def fold_tuple_twice(t):
  return tuple([t[2*b] + t[2*b+1] for b in range(len(t)/2)])

def add_tuples(a,b):
  return tuple(map(int.__add__, a, b))

def uniform_tuple(a):
  for i in a:
    if i != a[0]:
      return False
  return True

uni_ = uniform_tuple
add_ = add_tuples
int_ = tuple_to_int
tup_ = int_to_tuple
fold = fold_tuple_twice

ex = [["".join(map(str, int_to_tuple(j))) for j in exps.exps[i]] for i in sorted(exps.exps)]

results = []

def has_prefix(i, pfx):
  for b in ex[i]:
    if b.startswith(pfx):
      return True

import datetime, pprint
def next(prefixes):
  if len(prefixes[0])==64:
    print "DONE", prefixes
    results.append(("DONE", prefixes))
  if len(prefixes[0])>21:
    print datetime.datetime.now()
    pprint.pprint(prefixes)
  for i in range(len(ex)):
    if has_prefix(i, prefixes[i]+"1"):
      for j in range(len(ex)):
        if j!=i:
          if not has_prefix(j, prefixes[j]+"0"):
            break
      else:
        next([prefixes[j]+(j==i and "1" or "0") for j in range(len(ex))])

#ex = []
#for piece in sorted(exps.exps):
#  rv = {}
#  for e in exps.exps[piece]:
#    rv[tup_(e)] = None
#
#  while len(rv.keys()[0])>1:
#    tmp = rv
#    rv = {}
#    for i in tmp:
#      rv.setdefault(fold(i), {})
#      rv[fold(i)][i] = tmp[i]
#
#  ex.append(rv)

#print "---"
#for i in rv:
# print "".join(map(str, i))
# for j in rv[i]:
#   print " "+"".join(map(str, j))
#   for k in rv[i][j]:
#     print "  "+"".join(map(str, k))
#     for l in rv[i][j][k]:
#       print "   "+"".join(map(str, l))

#def do_next(ex, next):
#  #if len(next[0])>=2:
#    #print next
#    #print "DONE 2", taken, discarded
#    #return
#  if len(next[0])>=64:
#    print next
#    print "DONE 64"
#    return
#  newex = [ex[i][next[i]] for i in range(len(ex))]
#  #print next
#  #pprint.pprint(newex, depth=1)
#  key0 = newex[0].keys()[0]
#  #print "key0", key0
#  empty = [0 for i in key0]
#
#  test_set(newex, 0, empty, [])
#
#taken = 0
#discarded = 0
#
#def test_set(ex, i, part, next):
#  if i>=len(ex):
#    if uni_(part):
#      global taken
#      taken += 1
#      #print "done", part
#      do_next(ex, next)
#    else:
#      global discarded
#      discarded += 1
#      #print ".",
#    return
#
#  for k in ex[i]:
#    #print k
#    test_set(ex, i+1, add_(part, k), next+[k])
#
##test_set(ex, 0, (0,), [])
#
#print "taken", taken, "discarded", discarded
#

#def expand_tuple(t):
#  addo = [0]*(64/len(t)-1)
#  rv = []
#  for i in t:
#    rv += addo
#    rv += [i]
#  return tuple(rv)

#sieves = {}
#for r in range(6):
#  i = 2**r
#  sieves[i] = int(("1"*i+"0"*i)*(64/i/2), 2)
#
#print "Calculated sieves"
#for k in sieves: print k, ":", repr(sieves[k])
#
#def fold_int_basic(i, fold):
#  target = 2**64 - 1
#  shift = fold
#  sieve = sieves[fold]
#  return ((i & sieve)>>shift) + (i & (sieve>>shift))
