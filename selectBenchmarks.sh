#!/bin/bash

# read path of build (argument given by Makefile), debug/prod
DIRECTORY=$1
ARGUMENTS=${@:2}


# read in available benchmarks
for arg in `ls -1 $DIRECTORY | sed '/\(.*\)\..*/d';`
do
	(( i++ ))
	var[i]=$arg
done

if [ ${#var[*]} -eq 1 ]
	then

	echo 'Executing Benchmark:' ${var[1]} $ARGUMENTS

	# execute selected Benchmark
	$DIRECTORY/${var[1]} $ARGUMENTS
	DONE=1
	#
fi

while [ ! $DONE ]
do
	echo
	echo 'Available Benchmarks:'
	echo

	for x in $(seq $i)
	do
		echo $x':' ${var[$x]}
	done

	echo
	echo 'Execute? (default = 1)'
	echo '[ 0 to exit ]'
	echo

	# select benchmarks
	read SELECTION

	if [ ! $SELECTION ]
		then
		SELECTION=1
	fi

	if [ $SELECTION -eq 0 ]
		then
		DONE=0
	fi

	if [ $SELECTION -eq 42 ]
		then
		echo '...'
		sleep 2
		echo
		sleep 1
		echo
	fi

	for x in $(seq $i)
	do
		if [ $SELECTION -eq $x ]
			then

			echo 'Executing Benchmark:' ${var[$x]} $ARGUMENTS

			# execute selected Benchmark
			$DIRECTORY/${var[$x]} $ARGUMENTS
			DONE=1
			#
		fi
	done

	if [ ! $DONE ]
		then
		echo "Benchmark number $SELECTION is not available."
		sleep 1
	else
		echo
	fi

done