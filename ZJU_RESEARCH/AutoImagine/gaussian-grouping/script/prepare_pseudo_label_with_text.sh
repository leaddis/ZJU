#!/bin/bash


# Check if the user provided an argument
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <dataset_name>"
    exit 1
fi


dataset_name="$1"
prompt="$2"

scale=1
dataset_folder="data/$dataset_name"

if [ ! -d "$dataset_folder" ]; then
    echo "Error: Folder '$dataset_folder' does not exist."
    exit 2
fi



# 1. DEVA anything mask
cd Tracking-Anything-with-DEVA/

if [ "$scale" = "1" ]; then
    img_path="../data/${dataset_name}/images"
else
    img_path="../data/${dataset_name}/images_${scale}"
fi

# colored mask for visualization check
# ori: --size 480 \
# SAM_NUM_POINTS_PER_BATCH=1, chunk_size=1, SAM_NUM_POINTS_PER_SIDE=32
python demo/demo_with_text.py \
  --chunk_size 4 \
  --img_path "$img_path" \
  --amp \
  --temporal_setting semionline \
  --SAM_NUM_POINTS_PER_BATCH 4 \
  --size 480 \
  --output "./example/output_gaussian_dataset/${dataset_name}" \
  --DINO_THRESHOLD 0.40 \
  --max_missed_detection_count 0 \
  --prompt ${prompt} 

  
# 2. copy gray mask to the correponding data path
rm ./example/output_gaussian_dataset/${dataset_name}/object_mask -r

cp -r ./example/output_gaussian_dataset/${dataset_name}/Annotations ./example/output_gaussian_dataset/${dataset_name}/object_mask

python get_gray_image.py --path ./example/output_gaussian_dataset/${dataset_name} --prompt ${prompt}

cp -r ./example/output_gaussian_dataset/${dataset_name}/object_mask ../data/${dataset_name}/
cd ..