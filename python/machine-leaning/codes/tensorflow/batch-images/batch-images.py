'''
    介绍:存放在"dataset/train/"下的猫狗大战数据集
    使用:python batch-images.py
    结果:显示出若干图片
'''
import os
import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf

def get_files(file_dir):
    '''
        Get the image list and label list.
        [<filename1>,      [0,
         <filename2>,       1, 
          ...,             ..,
         <filenameN>]       0]
    '''
    # 定义保存filename的list
    image_list = []
    # 定义保存label的list
    label_list = []
    # 遍历目录下的每个文件
    for filename in os.listdir(file_dir):
        fullname = os.path.join(file_dir,filename)
        # 根据文件名推送到filename的list
        image_list.append(fullname)
        # 确定label的值:cat=0 && dog=1
        if filename.split('.')[0] == 'cat':
            label_list.append(0)
        # 根据文件名推送到label的list
        else:
            label_list.append(1)

    shuffle_indices = np.random.permutation(np.arange(len(image_list)))
    image_list = np.array(image_list)
    label_list = np.array(label_list)
    image_list = image_list[shuffle_indices]
    label_list = label_list[shuffle_indices]
    # 返回image_list 和 label_list
    return image_list, label_list

def next_batch(sess, image_list, label_list, image_h, image_w, shuffle, num_epochs, batch_size, num_threads,capacity):
    # 转换普通的list为tf能识别的类型
    image_list = tf.cast(image_list,tf.string)
    label_list = tf.cast(label_list,tf.int32)
    
    # 创建队列
    input_queue = tf.train.slice_input_producer([image_list, label_list], shuffle=shuffle,num_epochs=num_epochs) # if there are 2 elms in the 1st param,the next sentence uses '[1]' to get that param

    
    # get image's matrix
    image_contents = tf.read_file(input_queue[0])
    image = tf.image.decode_jpeg(image_contents,channels=3)
    # the following 2 ways of resizing both make sense
    image = tf.image.resize_image_with_crop_or_pad(image,image_h,image_w)
    #image = tf.image.resize_images(image, [image_h, image_w], method=tf.image.ResizeMethod.NEAREST_NEIGHBOR)

    # 不用转换成float32类型,因为plt.imshow在float类型下会出现"蓝色"?的图片,下面2种方法都会转换成float类型
    #image = tf.image.per_image_standardization(image)
    #image = tf.cast(image,tf.float32)

    # get label
    label = input_queue[1]

    # 获取batch对象
    image_batch, label_batch = tf.train.batch([image,label],batch_size=batch_size,num_threads=num_threads,capacity=capacity)

    if num_epochs:
        sess.run(tf.local_variables_initializer()) # initialize num_epochs

    # 启动线程
    coord = tf.train.Coordinator()
    threads = tf.train.start_queue_runners(coord=coord)
    try:
        while not coord.should_stop():
            # get & process the batch
            yield sess.run([image_batch, label_batch])

    except tf.errors.OutOfRangeError:
        print("done!")
    finally:
        coord.request_stop()

    coord.join(threads)
    
image_list, label_list = get_files('dataset/train/')

gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
config = tf.ConfigProto(gpu_options=gpu_options)

with tf.Session(config=config) as sess:
    i = 0
    batch_size = 3

    for images,labels in next_batch(sess, image_list, label_list, image_h=208, image_w=208, shuffle=False, num_epochs=None, batch_size=batch_size, num_threads=32,capacity=256):

        for i in range(batch_size):
            print("label: %d" % labels[i])
            plt.imshow(images[i, :, :, :])
            plt.show()
        break
