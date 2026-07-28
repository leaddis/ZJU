#!/bin/bash

# Get the parent directory of the script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_FOLDER="$(dirname "$SCRIPT_DIR")"

for num in {1..5}
do 
    cd "${PARENT_FOLDER}/clevr-dataset-gen/image_generation/"
    bash gen.sh ${num}
    cd "${PARENT_FOLDER}/gaussian-grouping/"
    bash cal.sh clevr-${num}    
done