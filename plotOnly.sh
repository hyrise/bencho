#!/bin/bash

# read path of build (argument given by Makefile), debug/prod
DIRECTORY=$1

for arg in `ls -1 $DIRECTORY | sed '/\(.*\)\..*/d';`
do
	(( i++ ))
	var[i]=$arg
done

if [[ ${#var[*]} -ge 1 ]]; then
	$DIRECTORY/${var[1]} -plotonly
else
	echo 'No benchmark binary available to call the plotter!'
	echo
fi
