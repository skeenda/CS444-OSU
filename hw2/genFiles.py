import os
import random


def write(i):
	f = open( str(i) + ".txt"", "a+")
	for x in range(randint(0, 50)):
		x = randint(0,500)
		f.write(x)
		f.close()

if __name__ == "__main__":
	fileNames = [test1,test2,test3,test4,test5]
	for i in fileNames:
		write(i)
		f = open(str(i) + ".txt")
		data = f.read()
		f.close()
		print (data + "\n")
