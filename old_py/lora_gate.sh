#!/bin/bash
trap my_exit INT
my_exit() {
	echo ""
	echo "Key Interrupt Received"
	exit
}
echo 0 > index.txt
while [ 1 -eq 1 ]; do
	python frcv.py 0
	wait
	ssdv -d out.ssdv tmp.jpeg
done

