#!/bin/bash

if [ "$#" -ne 2 ] ; then
  echo "Usage: $0 INPUT_DIRECTORY OUTPUT_DIRECTORY" >&2
  exit 1
fi

INPUT_DIR="$1"
OUTPUT_DIR="$2"

echo "Outputting shaders in directory: $OUTPUT_DIR"

vs_files=$(readlink -m "${INPUT_DIR}/*.vert")
fs_files=$(readlink -m "${INPUT_DIR}/*.frag")

for f in $vs_files
do
  base_filename=${f##*/}
  output=$(readlink -m "$OUTPUT_DIR/$base_filename.spv")
  glslc $f -o $output
done

for f in $fs_files
do
  base_filename=${f##*/}
  output=$(readlink -m "$OUTPUT_DIR/$base_filename.spv")
  glslc $f -o $output
done
