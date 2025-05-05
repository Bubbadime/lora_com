#!/bin/bash
trap my_exit INT

my_exit() {
	echo ""
	echo "Key Interrupt Received"
	echo $counter > index.txt
	exit
}

counter=$(cat index.txt)
while [ 1 -eq 1 ]; do
	echo Image $counter:
	fname=pic$counter.jpeg
	rpicam-still --immediate --encoding jpg -o $fname
	python im_mktmp.py $fname
	ssdv -e tmp.jpeg out.ssdv
	python fsnd.py $counter

	counter=$((counter + 1))
	echo $counter > index.txt
done

