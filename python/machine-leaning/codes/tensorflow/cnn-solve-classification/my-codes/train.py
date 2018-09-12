#! /usr/bin/env python

######################################################################
# Import
######################################################################
import sys
from data_helpers import *
import tensorflow as tf
from tensorflow.contrib import learn
import numpy as np
from text_cnn import CNN






######################################################################
# Params
######################################################################
# Define the positive file
tf.flags.DEFINE_string("positive_file", "data/rt-polaritydata/rt-polarity.pos", "negative file path")
tf.flags.DEFINE_string("negative_file", "data/rt-polaritydata/rt-polarity.neg", "positive file path")
tf.flags.DEFINE_float("dev_sample_percentage", 0.1, "Percentage of the training data to use for validation")

tf.flags.DEFINE_integer("num_filters", 128, "num of filters")
tf.flags.DEFINE_string("filter_sizes", "3,4,5", "Comma-separated filter sizes (default: '3,4,5')")
tf.flags.DEFINE_integer("num_classes", 2, "num of classes")
tf.flags.DEFINE_integer("num_steps", 200, "num of steps")
tf.flags.DEFINE_integer("embedding_size", 128, "embedding size")
tf.flags.DEFINE_integer("batch_size", 64, "batch size")
tf.flags.DEFINE_integer("log_every", 100, "batch size")


FLAGS = tf.flags.FLAGS
FLAGS(sys.argv) # 启用flags





######################################################################
# Load data
######################################################################
# 获得指定2个文件中的所有[一句话,一句话]以及对应的标签
X_text, y = load_data_and_labels(FLAGS.positive_file, FLAGS.negative_file)
# 转换[一句话,]=>词典id的列表
max_document_length = max([len(x.split(" ")) for x in X_text])
vocab_processor = learn.preprocessing.VocabularyProcessor(max_document_length)
X = np.array(list(vocab_processor.fit_transform(X_text)))

# 计算整个文本的陌生单词数
vocab_size = len(vocab_processor.vocabulary_)

# 分割训练集和测试集
dev_sample_index = -1 * int(FLAGS.dev_sample_percentage * float(len(y)))
X_train, X_dev = X[:dev_sample_index], X[dev_sample_index:]
y_train, y_dev = y[:dev_sample_index], y[dev_sample_index:]
print("Vocabulary Size: {:d}".format(len(vocab_processor.vocabulary_)))
print("Train/Dev split: {:d}/{:d}".format(len(y_train), len(y_dev)))



# 开启一个默认图
with tf.Graph().as_default():
    # 定义CNN模型结构(损失函数,准确度,优化器)
    cnn = CNN(
            filter_sizes = list(map(int, FLAGS.filter_sizes.split(','))),
            num_filters = FLAGS.num_filters,
            num_classes = FLAGS.num_classes,
            embedding_size = FLAGS.embedding_size,
            sequence_length = X.shape[1],
            batch_size = FLAGS.batch_size,
            vocab_size = vocab_size,
            )

    # 定义优化器和训练的op
    global_step = tf.Variable(0, name="global_step", trainable=False)
    optimizer = tf.train.AdamOptimizer(1e-3)
    grads_and_vars = optimizer.compute_gradients(cnn.loss) # 这里就是loss的op
    train = optimizer.apply_gradients(grads_and_vars, global_step=global_step)
    
    # 启动会话
    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config=tf.ConfigProto(gpu_options=gpu_options)
    sess = tf.Session(config=config)
    with sess.as_default():
        # 初始化变量
        tf.global_variables_initializer().run()
        # 获得
        batches = next_batch(list(zip(X_train, y_train)), FLAGS.batch_size, FLAGS.num_steps)
        # for(i:0=>num_steps)
        for batch in batches:
            # 获得X_train和y_train
            X_train, y_train = zip(*batch)
            # 训练CNN
            _, loss, total = sess.run([train,cnn.loss, cnn.total],feed_dict={
                cnn.features : X_train,
                cnn.labels : y_train
                })
            # 获得当前训练次数
            current_step = tf.train.global_step(sess, global_step)
            if current_step == 149:
                print('debug starts')
            # 如果训练到一定阶段
            if current_step % FLAGS.log_every == 0:
                # 打印CNN当前准确度
                print('step {0}:loss={1}\taccuracy={2}'.format(current_step,loss, total/FLAGS.batch_size))
