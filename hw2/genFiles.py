import os
from random import randint


def write(i):
	f = open( str(i) + ".txt", "a+")
	for x in range(randint(0, 50)):
		x = randint(0,500)
		f.write(str(x))
#	print ( "Wrote to:" + i)
	f.close()

if __name__ == "__main__":
	fileNames = ["test1","test2","test3","test4","test5"]
	while(True):
		for i in fileNames:
			write(i)
			f = open(str(i) + ".txt")
			data = f.read()
			f.close()
#			print ("Read from: " + i + "\n")
