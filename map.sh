#!/bin/bash

#num of parameters
if (( $# < 2 )); then
	exit 1
fi
#check if file exists
if [ ! -f $1 ]; then
	exit 1
fi
# $@ is a list of parameters, we want to skip the first for easier iteration
skip_first=0
for parameter in $@
do
	#skip the first one
	if (($skip_first == 0 )) ; then	
		skip_first=1
		continue
	fi
	#check if string is purely numeric
	if ! [[ $parameter =~ ^[0-9]+$ ]] ; then
		exit 1;
	fi
	#check if parameter is in range
	if (( $parameter < 1 )) || (( $parameter > 999 )) ; then
		exit 1
	fi
done

skip_first=0
for parameter in $@
do
	#skip first
	if (($skip_first == 0)) ; then
		skip_first=1;
		continue
	fi
	#zero the answer
	answer=0;
	while read -r line;
	do
		#if line beginning matches parameter
		#done by removing longest match of sth ending with ; on the left
		if [ "${line%%;*}" == "$parameter" ]; then
			#skip num and first city
			#cut from beginning to ;
			line=${line#*;}
			line=${line#*;}
			#keep cutting while there is still a delimiter
			while [ $line != ${line#*;} ];
			do
				#num if what is left after all from right incl last delimiter is removed
				num=${line%%;*}
				answer=$(( num + answer ))
				#cut length,builtyear and city
				line=${line#*;}
				line=${line#*;}
				line=${line#*;}
			done
		fi
	#only now in the syntax the filename is passed!
	done < $1
	#check if the answer has increased
	if (($answer > 0)) ; then
		echo "$parameter:$answer"
	fi
done
exit 0
