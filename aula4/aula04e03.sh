#!/bin/bash

lista=(7 4 1 5 10)

for((i=0;i<${#lista[@]};i++)); do
	echo "${lista[i]}"
done | sort -n


