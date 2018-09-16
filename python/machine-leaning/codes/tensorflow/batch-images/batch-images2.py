'''
    用法同batch-images.py,只是把next_batch拿出来,解决最后threads不join的问题
'''
import tensorflow as tf
import matplotlib.pyplot as plt
import sys
import data_loader


image_list, label_list = data_loader.get_files('dataset/train/')

tf.flags.DEFINE_integer("image_h", 208, "image height")
tf.flags.DEFINE_integer("image_w", 208, "image width")
tf.flags.DEFINE_boolean("shuffle", False, "The batch data should shuffled or not")
# 如果num_epochs不是None的话,那么每次sess.run对应的image_batch等就会减1(注意:是运行才减1!!)
tf.flags.DEFINE_integer("num_epochs", None, "num of epochs")
tf.flags.DEFINE_integer("batch_size", 3, "batch size")
tf.flags.DEFINE_integer("num_threads", 32, "number of threads")
tf.flags.DEFINE_integer("capacity", 256, "capacity")


FLAGS = tf.flags.FLAGS
FLAGS(sys.argv) # 启用flags



# 转换普通的list为tf能识别的类型
image_list = tf.cast(image_list,tf.string)
label_list = tf.cast(label_list,tf.int32)

# 创建队列
input_queue = tf.train.slice_input_producer([image_list, label_list], shuffle=FLAGS.shuffle,num_epochs=FLAGS.num_epochs) # if there are 2 elms in the 1st param,the next sentence uses '[1]' to get that param


# get image's matrix
image_contents = tf.read_file(input_queue[0])
image = tf.image.decode_jpeg(image_contents,channels=3)
# the following 2 ways of resizing both make sense
image = tf.image.resize_image_with_crop_or_pad(image,FLAGS.image_h,FLAGS.image_w)

# get label
label = input_queue[1]

# 获取batch对象
image_batch, label_batch = tf.train.batch([image,label],batch_size=FLAGS.batch_size,num_threads=FLAGS.num_threads,capacity=FLAGS.capacity)

if FLAGS.num_epochs:
    sess.run(tf.local_variables_initializer()) # initialize num_epochs


gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
config = tf.ConfigProto(gpu_options=gpu_options)

with tf.Session(config=config) as sess:
    # 启动线程
    coord = tf.train.Coordinator()
    threads = tf.train.start_queue_runners(coord=coord)
    try:
        while not coord.should_stop():
            # get & process the batch
            images, labels = sess.run([image_batch, label_batch])
            for i in range(FLAGS.batch_size):
                print("label: %d" % labels[i])
                plt.imshow(images[i, :, :, :])
                plt.show()
            break

    except tf.errors.OutOfRangeError:
        print("done!")
    finally:
        coord.request_stop()

    coord.join(threads)


