import numpy
import matplotlib.pyplot as plt
import sys

if __name__ == "__main__":
	if(len(sys.argv) < 1):
		print "Need a filename as arg to run"
		exit()
	filename = sys.argv[1]
	points, idx, n = [], [], 0
	f = open(filename, "r+")
	data = f.read()
	values = data.split("\n")
	plt.figure()
	for i in values:
		n += 1
		idx.append(n)
		points.append(i)
	plt.xlabel("Request value")
	plt.ylabel("Sector value")
	plt.plt(idx, points, 'r-')
	plt.show()
