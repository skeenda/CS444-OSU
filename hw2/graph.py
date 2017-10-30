import numpy
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import sys
import pylab

if __name__ == "__main__":
	if(len(sys.argv) < 1):
		print "Need a filename as arg to run"
		exit()
	filename = sys.argv[1]
	points, idx, n = [], [], 0
	f = open(filename, "r+")
	data = f.read().split("\n")
	plt.figure()
	for i in data[420:460]:
		n += 1
		clean = i.split(": ")
		#print clean[3]
		idx.append(n)
		points.append(clean[3])
	plt.xlabel("Request value")
	plt.ylabel("Sector value")
	plt.plot(idx, points, 'r-')
	plt.savefig("output.png")
