#!/usr/bin/env python

from __future__ import print_function
import sys
import re
import os
import time
import subprocess

_INPUT_DIR = "./random3SAT"
__OUTPUT_FILE = "output.txt"

if len(sys.argv) != 2:
	print("Usage: comparator [max_var_number]")
	sys.exit(0)
_MAX_VAR_NUM = int(sys.argv[1])
f = open(__OUTPUT_FILE, 'w')
f.truncate()
i = 0
totalDiff = 0
header = "timeSpent\tdecisions\tpropagations\tpropagations/timeSpent";
print(header)
for filename in os.listdir("./random3SAT"):
	i += 1
	var_num = int(filename.split("-")[1].split("-")[0])
	if var_num > _MAX_VAR_NUM:
		continue
	print(filename)
	start_m = time.time()
	process_m = subprocess.Popen("./mysat < random3SAT/{}".format(filename), stdout=subprocess.PIPE, shell=True)
	output_m = (process_m.communicate()[0])[:-1]
	myresult = output_m.split("-")[0]  #ignore printed stats
	end_m = time.time()

	start_p = time.time()
	process_p = subprocess.Popen("picosat < random3SAT/{}".format(filename), stdout=subprocess.PIPE, shell=True)
	output_p = (process_p.communicate()[0]).split()[1]
	end_p = time.time()
	mytime   = (end_m-start_m)
	picotime = (end_p-start_p)
	veredict = "  MATCH" if myresult == output_p else "DON'T MATCH"
	diff = (mytime/picotime)
	totalDiff += diff
	veredict += "(" + output_m[:3] + "). You're " + str(abs(diff))[:5] + " times " 
	veredict += "slower" #if diff > 0 else "slower"
	#str( round( (mytime - picotime)*1000, 5) ) + "ms"

	#print("%.2f" % round(a,2))
	print (veredict)
	print(filename, "-", "Own:", "[" + output_m + "]", mytime, "Picosat:", "[" + output_p + "]", picotime, file=f)

print("avg diff", totalDiff/i)
