import sys
from skimage.io import imread, imsave
from skimage.filters import threshold_otsu
from skimage.color import rgb2gray

#print("Hello, world!")

try:
    # コマンドライン引数から入力画像と出力画像のパスを取得
    input_image_path = sys.argv[1]
    output_image_path = sys.argv[2]

    # 画像の読み込みと二値化処理
    image = imread(input_image_path)
    if len(image.shape) == 3:
        image = rgb2gray(image)
    thresh = threshold_otsu(image)
    binary = image > thresh
    imsave(output_image_path, (binary * 255).astype('uint8'), check_contrast=False)
    print(thresh*255)
except Exception as e:
    print("An error occurred:", e, file=sys.stderr)
    sys.exit(1)

