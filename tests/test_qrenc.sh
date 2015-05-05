#!/bin/sh -e

COMMAND=../qrencode
TEXT="hello"
TARGET_DIR="test_images"

$COMMAND -t SVG -o $TARGET_DIR/svg.svg $TEXT
$COMMAND -t SVG --rle -o $TARGET_DIR/svg-rle.svg $TEXT
$COMMAND -t SVG --svg-path -o $TARGET_DIR/svg-path.svg $TEXT
$COMMAND -t SVG --rle --svg-path -o $TARGET_DIR/svg-rle-path.svg $TEXT
