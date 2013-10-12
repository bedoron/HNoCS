#!/bin/bash

while [ 1 ]; do 
	sleep 60s
	cat /proc/meminfo > ~/health.txt
done
