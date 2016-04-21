#!/usr/bin/env python

from __future__ import print_function
import sys
import os
import time
import subprocess
import csv

_INPUT_DIR = "./random3SAT"
__OUTPUT_FILE = "output.txt"

if len(sys.argv) != 2:
	print("Usage: comparator [max_var_number]")
	sys.exit(0)
_MAX_VAR_NUM = int(sys.argv[1])
f = open(__OUTPUT_FILE, 'w')
f.truncate()
for filename in os.listdir("./random3SAT"):
	var_num = int(filename.split("-")[1].split("-")[0])
	if var_num > _MAX_VAR_NUM:
		continue

	start_p = time.time()
	process_p = subprocess.Popen("picosat < random3SAT/{}".format(filename), stdout=subprocess.PIPE, shell=True)
	output_p = (process_p.communicate()[0]).split()[1]
	end_p = time.time()

	print(filename + " " + str(end_p-start_p))
f.close()
