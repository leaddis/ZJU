import os
from PIL import Image


import argparse
parser = argparse.ArgumentParser()

parser.add_argument('--dataset', type=str, default='living-and-kitchen')
args = parser.parse_args()

dir2 = f'videos/{args.dataset}' 
dir1 = f'outputs/{args.dataset}'  
output_dir = f'renders/{args.dataset}' 

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

file_names = os.listdir(dir1)

for file_name in file_names:
    img1_path = os.path.join(dir1, file_name)
    img2_path = os.path.join(dir2, file_name)
    
    if os.path.exists(img2_path):
        img1 = Image.open(img1_path).convert("RGBA") 
        img2 = Image.open(img2_path).convert("RGBA")

        import numpy as np
        np_img = np.array(img1)
        np_img[:,:,1] = 0
        np_img[:,:,2] = 0
        img1 = Image.fromarray(np_img)

        alpha = 0.8 
        img2 = Image.blend(img1, img2, alpha)

        output_path = os.path.join(output_dir, file_name)
        output_path = os.path.join(output_dir, file_name)

        img2 = img2.convert("RGB") 
        img2.save(output_path, 'JPEG') 

        print(f"Saved blended image: {output_path}")
    else:
        print(f"No corresponding image in dir2 for {file_name}")
