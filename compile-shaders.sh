#!/bin/bash

if [ "$#" -ne 2 ] ; then
  echo "Usage: $0 INPUT_DIRECTORY OUTPUT_DIRECTORY" >&2
  exit 1
fi

INPUT_DIR="$1"
OUTPUT_DIR="$2"

echo "Outputting shaders in directory: $OUTPUT_DIR"

mkdir -p "$OUTPUT_DIR/assets/shaders"

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

#glslc assets/shaders/uber.vert -o "$OUTPUT_DIR/assets/shaders/uber.vert.spv"
#glslc assets/shaders/uber.frag -o "$OUTPUT_DIR/assets/shaders/uber.frag.spv"
##glslc assets/shaders/parallax.vert -o "$OUTPUT_DIR/assets/shaders/parallax.vert.spv"
##glslc assets/shaders/parallax.frag -o "$OUTPUT_DIR/assets/shaders/parallax.frag.spv"
#
#glslc assets/shaders/debug.vert -o "$OUTPUT_DIR/assets/shaders/debug.vert.spv"
#glslc assets/shaders/debug.frag -o "$OUTPUT_DIR/assets/shaders/debug.frag.spv"
#glslc assets/shaders/shader.vert -o "$OUTPUT_DIR/assets/shaders/shader.vert.spv"
#glslc assets/shaders/shader.frag -o "$OUTPUT_DIR/assets/shaders/shader.frag.spv"
#glslc assets/shaders/diffuse.vert -o "$OUTPUT_DIR/assets/shaders/diffuse.vert.spv"
#glslc assets/shaders/diffuse.frag -o "$OUTPUT_DIR/assets/shaders/diffuse.frag.spv"
#glslc assets/shaders/default_curve.vert -o "$OUTPUT_DIR/assets/shaders/default_curve.vert.spv"
#glslc assets/shaders/default_curve.frag -o "$OUTPUT_DIR/assets/shaders/default_curve.frag.spv"
#
#glslc assets/shaders/default_curve.vert -o "$OUTPUT_DIR/assets/shaders/diffuse-color.vert.spv"
#glslc assets/shaders/default_curve.frag -o "$OUTPUT_DIR/assets/shaders/diffuse-color.frag.spv"
#
#glslc assets/shaders/albedo-spec-mapped.vert -o "$OUTPUT_DIR/assets/shaders/albedo-spec-mapped.vert.spv"
#glslc assets/shaders/albedo-spec-mapped.frag -o "$OUTPUT_DIR/assets/shaders/albedo-spec-mapped.frag.spv"