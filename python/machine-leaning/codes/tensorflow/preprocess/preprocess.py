'''
    intro: preprocess image
    usage: python preprocess.py
'''
import tensorflow as tf
import matplotlib.pyplot as plt
import numpy as np

# define function of distorting color
def distort_color(image, order=0):
    # define 1st order
    if order == 0:
        image = tf.image.random_hue(image,0.2)
        image = tf.image.random_brightness(image,32./255.) 
        image = tf.image.random_contrast(image,0.5, 1.5)
        image = tf.image.random_saturation(image,0.5, 1.5) 
    # define 2nd order
    elif order == 1:
        image = tf.image.random_brightness(image,32./255.) 
        image = tf.image.random_hue(image,0.2)
        image = tf.image.random_contrast(image,0.5, 1.5)
        image = tf.image.random_saturation(image,0.5, 1.5) 
    else:
        image = tf.image.random_hue(image,0.2)
        image = tf.image.random_contrast(image,0.5, 1.5)
        image = tf.image.random_brightness(image,32./255.) 
        image = tf.image.random_saturation(image,0.5, 1.5) 

    # clip by value
    image = tf.clip_by_value(image,0.0,1.0)    

    return image


# define function of preprocessing
def preprocess(image,height,width,bbox=None):
    # adjust bounding box
    if bbox is None:
        bbox = tf.constant([0.0,0.0,1.0,1.0],dtype=tf.float32,shape=[1,1,4])

    # convert image to float type
    if image.dtype != tf.float32:
        image = tf.image.convert_image_dtype(image,dtype=tf.float32)

    # slice the image
    begin, size, bbox_for_draw = tf.image.sample_distorted_bounding_box(
                    tf.shape(image),bounding_boxes=bbox) # 0.4 <=> 40%
    image = tf.slice(image,begin,size)

    # resize the image
    image = tf.image.resize_images(image,[height,width],method=np.random.randint(4))

    # filp left right
    image = tf.image.random_flip_left_right(image)
    # distort color
    image = distort_color(image,np.random.randint(3))

    # return final image
    return image



# get bytes of image
image_raw_data = tf.gfile.FastGFile('/home/orris/Pictures/1.jpeg','rb').read()

gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
config = tf.ConfigProto(gpu_options=gpu_options)

# start session
with tf.Session(config=config) as sess:
    # decode image
    image = tf.image.decode_jpeg(image_raw_data)
    plt.figure()
    plt.imshow(image.eval())
    plt.show()

    bbox = tf.constant([[[0.05,0.05,0.9,0.7],[0.35,0.47,0.5,0.56]]])

    # for(i:0=>6)
    for i in range(6):
        # preprocess image
        image = preprocess(image,100,100)
        # show the image
        plt.figure()
        plt.imshow(image.eval())
        plt.show()
            
