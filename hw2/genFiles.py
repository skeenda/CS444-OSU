import os
import random


def write(i):
	f = open( str(i) + ".txt"", "a+")
	for x in range(randint(0, 50)):
		x = randint(0,500)
		f.write(x)
		f.close()

