#!/bin/bash

OUTPUT_DIR="$1"

echo "Outputting shaders in directory: $OUTPUT_DIR"

mkdir -p "$OUTPUT_DIR/assets/shaders"

glslc assets/shaders/shader.vert -o "$OUTPUT_DIR/assets/shaders/shader.vert.spv"
glslc assets/shaders/shader.frag -o "$OUTPUT_DIR/assets/shaders/shader.frag.spv"
glslc assets/shaders/diffuse.vert -o "$OUTPUT_DIR/assets/shaders/diffuse.vert.spv"
glslc assets/shaders/diffuse.frag -o "$OUTPUT_DIR/assets/shaders/diffuse.frag.spv"
glslc assets/shaders/default_curve.vert -o "$OUTPUT_DIR/assets/shaders/default_curve.vert.spv"
glslc assets/shaders/default_curve.frag -o "$OUTPUT_DIR/assets/shaders/default_curve.frag.spv"