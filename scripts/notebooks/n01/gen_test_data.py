import random

k = 10000 # number of distinct keys
v = 1000  # number of distinct values
n = 10000 # total number of records

for i in xrange(0, n):
	key = random.randint(0, k)
	value = random.randint(0, v)
	line = "key_{},value_{}".format(key,value)
	print line	
