'''write the captcha graphs to tfrecord'''
import os
import sys
import random
from PIL import Image
import numpy as np
import tensorflow as tf
'''get the captcha graphs'''
image_dir='captcha'
TEST_TOTAL=500
record_dir = 'captcha_tfrecord'

def record_exists(dir):
    for i in ['train','test']:
        if not tf.gfile.Exists(os.path.join(dir,i+'.tfrecords')):
            return False
    return True

def get_filenames():
    filenames=[]
    for file in os.listdir(image_dir):
        fullname=os.path.join(image_dir,file)
        filenames.append(fullname)
    return filenames


'''write to tfrecords'''


def int64_feature(values):
    if not isinstance(values, (tuple, list)):
        values = [values]
    return tf.train.Feature(int64_list=tf.train.Int64List(value=values))


def bytes_feature(values):
    return tf.train.Feature(bytes_list=tf.train.BytesList(value=[values]))


# get the tf.train.Example instances
def write_to_tfrecords(images, type):
    assert type in ['train', 'test']

    # get record filename
    with tf.python_io.TFRecordWriter(os.path.join(record_dir, type + '.tfrecords')) as writer:
        for index, filename in enumerate(images):
            try:
                sys.stdout.write('\r>> Progress %d/%d' % (index + 1, len(images)))
                sys.stdout.flush()
                # get the image
                image_data = Image.open(filename)
                # resize
                image_data = image_data.resize((224, 224))
                # 'L'
                image_data = np.array(image_data.convert('L'))
                # bytes
                image_data = image_data.tobytes()
                '''##write to the tfrecords'''
                '''###construct the tf.train.Example'''
                # get label
                label = os.path.basename(filename)[0:4]
                labels = []
                for i in label:
                    labels.append(int(i))

                example = tf.train.Example(features=tf.train.Features(feature={
                    'image': bytes_feature(image_data),
                    'label0': int64_feature(labels[0]),
                    'label1': int64_feature(labels[1]),
                    'label2': int64_feature(labels[2]),
                    'label3': int64_feature(labels[3]),
                }))
                writer.write(example.SerializeToString())
            except IOError as e:
                print('Could not import', filename)
                print('Error', e)
                print('Skip it\n')
    sys.stdout.write('\n')
    sys.stdout.flush()


if not tf.gfile.Exists(record_dir):
    tf.gfile.MkDir(record_dir)
if record_exists(record_dir):
    print('tfrecords exist')
else:
    images=get_filenames()
    # split the graphs to train_set and test_set
    random.seed(0)
    random.shuffle(images)
    images_train=images[TEST_TOTAL:]
    images_test=images[:TEST_TOTAL]



    with tf.Session() as sess:
        write_to_tfrecords(images_train,'train')
        write_to_tfrecords(images_test,'test')
