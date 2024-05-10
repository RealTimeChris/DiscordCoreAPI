#!/bin/bash
cmake -S ./ -B ./Build -DCMAKE_BUILD_TYPE=Release
cmake --build ./Build --config=Release
