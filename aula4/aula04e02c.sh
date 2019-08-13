#!/bin/bash
function maior()
#return 0 if equal, 1 if $1<$2, 2 if $1>$2
{
	if [[ $1 -eq $2 ]] ; then return 0;
	elif [[ $1 -lt $2 ]] ; then return 1;
	else return 2
	fi
}
maior $1 $2

ret=$?
if (($ret==0));then
	echo "identical"
elif (($ret==1));then
	echo $2
else echo $1
fi

