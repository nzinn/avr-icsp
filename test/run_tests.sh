#!/usr/bin/env bash

DIR="test/build"


for file in "$DIR"/*; do
  if [ -f "$file" ] && [ -x "$file" ]; then
      echo "Running test: $file"
      echo ""
      "$file"
      echo ""
  fi
done
