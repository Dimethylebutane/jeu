#!/bin/bash
output=$1
echo Compiling shaders...
echo $(pwd)
#compile all {name}.vert and {name}.frag file in the same folder
#output result in output/{name}.spv
#get vulkan path in argument to find glsl compiler

mkdir -p $output

FILES=$(pwd)/*

N=0

for f in $FILES
do
    if [[ $f == *.frag ]] || [[ $f == *.vert ]]
    then
		echo -$(basename ${f%.*})
		glslc $f -o $output/$(basename ${f%.*}).spv
		((N+=1))
    fi
done

echo $N shaders compiled
