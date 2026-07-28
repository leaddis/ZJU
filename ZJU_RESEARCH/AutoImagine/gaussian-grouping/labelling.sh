dataset='dining-table'
iteration=15000

# Get the parent directory of the script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_FOLDER="$(dirname "$SCRIPT_DIR")"

data_path="${PARENT_FOLDER}/gaussian-grouping/data/${dataset}"
# output_path="${PARENT_FOLDER}/gaussian-grouping/output/${dataset}" # deprecated

zooming_file="${PARENT_FOLDER}/llm/tracking/searcher-zooming/output.txt"
loc_coord=$(head -n 1 "$zooming_file")

cd "${PARENT_FOLDER}/gaussian-grouping"

locating_file="${PARENT_FOLDER}/llm/tracking/searcher-locating/output.txt"
obj_coord=$(tail -n 1 "$locating_file")

log_and_run() {
  echo "[ $(basename "$0") ] -> $*"
  "$@"
}

# echo "$(basename "$0")"; python edit_object_removal.py -m output/${dataset} --iteration ${iteration} --operation skip --render_video --render_coord ${obj_coord}

cd "${PARENT_FOLDER}/segment-anything-2/notebooks/"


python get_mask.py --dataset ${dataset}

# test
python render.py --dataset ${dataset}

cp outputs/${dataset}/* ${data_path}/object_mask/
cp ${data_path}/train/ours/iteration_${iteration}/cam_infos.pkl ${data_path}/

cd "${PARENT_FOLDER}/gaussian-grouping"
python train.py -s data/${dataset} -r 1 -m output/${dataset} --config_file config/gaussian_dataset/train.json --train_split --train_labels --iteration ${iteration}

# test
log_and_run python edit_object_removal.py -m output/${dataset} --iteration ${iteration} --operation skip --render_all --render_obj 1 --render_coord ${loc_coord}
cp ${data_path}/train/ours/iteration_${iteration}/renders/00000.png ${PARENT_FOLDER}/llm/tracking/find-to-seg-obj.png
log_and_run python edit_object_removal.py -m output/${dataset} --iteration ${iteration} --operation skip --render_all --render_obj 0 --render_coord ${loc_coord}
cp ${data_path}/train/ours/iteration_${iteration}/renders/00000.png ${PARENT_FOLDER}/llm/tracking/find-to-seg-woobj.png
