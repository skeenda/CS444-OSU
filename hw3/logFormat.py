import sys

f = open(sys.argv[1])
output = open('pretty_log.txt', 'w+')

for line in f:
	edit = line[:-1]
	output.write(edit + '\\\\\hline\n')

f.close()
output.close()
