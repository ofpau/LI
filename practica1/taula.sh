#!/bin/bash

make
echo "file result timeSpent decisions propagations propagations/s";
for f in random3SAT/*.cnf; 
do 
	myres=$(./mysat < $f)
	#pico=$( { time picosat < $f; } 2>&1)
	echo "$f pau: $myres";
	#echo "pico: ${pico: -48:-30}"
done
