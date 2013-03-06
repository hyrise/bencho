#!/bin/bash
 
file="settings.conf"
 
declare -a flags=('PROD' 'PAPI' 'VERBOSE_BUILD' 'GNUPLOT' 'PYPLOT')

# create default settings if none exists
if [ ! -f $file ]; then
    echo "PROD=0" > $file
    echo "PAPI=0" >> $file
    echo "VERBOSE_BUILD=0" >> $file
    echo "GNUPLOT=0" >> $file
    echo "PYPLOT=0" >> $file
    #echo "PREFETCH=0" >> $file
    #echo "THREAD_AFFINITY=0" >> $file
    #echo "TREX_SSE=0" >> $file
fi

# check if new flags came with a new version, adds them if nessecary
for flag in "${flags[@]}"
do
    found=0
    while read myline
    do
        if [[ ${myline%%=*} == ${flag} ]]; then
            found=1
        fi
    done < $file
    if [[ $found -eq 0 ]]; then
        echo "${flag}=0" >> $file
    fi
done
 
# read in settings file
i=0
while read myline
do
    (( i++ ))
    # split line
    var[i]=${myline%%=*}
    val[i]=${myline##*=}
     
    # echo ?
    if [ $1 ]; then
        printf ${var[$i]}"="${val[$i]}", "
    fi
 
done < $file
 
# set new values
if [ $1 ]; then
    echo
else
    rm $file
    c=1
    while [[ $c -le $i ]]; do
        printf ${var[$c]}"="${val[$c]}"? : "
        read -n 1 answer
         
        if [ $answer ]; then
            echo ${var[$c]}"="$answer >> $file
            echo
        else
            echo ${var[$c]}"="${val[$c]} >> $file
        fi
        (( c++ ))
    done
fi