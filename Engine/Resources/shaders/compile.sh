#!/bin/zsh
output=%1
echo Compiling shaders...

#compile all {name}.vert and {name}.frag file in the same folder
#output result in output/{name}.spv
#get vulkan path in argument to find glsl compiler

mkdir -p $output

FILES=$(pwd)

N=0

for f in $FILES
do
    if [[ $file == *.frag ]] || [[ $file == *.vert ]]; then
		glslc $f -o $output/${filename%.*}.spv
		((N+=1))
    fi
done

echo $N shaders compiled
