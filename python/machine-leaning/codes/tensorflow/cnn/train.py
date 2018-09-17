#coding=utf8
'''
    使用:将MNIST_data放在指定位置(data_path),然后python train.py
    补充功能(只完成了部分)
    1. 移动Model到单独的py文件里
    2. 设置启动正则化的选项(训练时使用,验证/测试时关闭)
    3. 分离train.py和eval.py
    4. 周期地保存模型
    5. 添加滑动平均值(恢复的时候用滑动平均值的结果恢复变量)
    6. 添加FLAGS
'''
import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data
import sys

tf.flags.DEFINE_float("learning_rate", 0.002, "learning rate")
tf.flags.DEFINE_float("regularization_scale", 0.0001, "scale of regularization")

tf.flags.DEFINE_integer("num_classes", 2, "num of classes")
tf.flags.DEFINE_integer("num_epochs", 5, "num of epochs")
tf.flags.DEFINE_integer("embedding_size", 128, "embedding size")
tf.flags.DEFINE_integer("batch_size", 64, "batch size")
tf.flags.DEFINE_integer("save_every", 100, "save_every")
tf.flags.DEFINE_integer("log_every", 50, "log_every")

tf.flags.DEFINE_string("data_path", "/home/user/dataset/MNIST_data/", "MNIST data path")

FLAGS = tf.flags.FLAGS
FLAGS(sys.argv) # 启用flags

class Model(object):
    def __init__(self, learning_rate, regularizer=None):

        self.features = tf.placeholder(tf.float32,[None,784])
        self.labels = tf.placeholder(tf.float32,[None,10])
        self.keep_prob = tf.placeholder(tf.float32)
        self.regularizer = regularizer
        learning_rate = tf.Variable(learning_rate)


        a0 = tf.reshape(self.features,[-1,28,28,1])

        # Layer1 (conv+pooling+lrn)
        with tf.variable_scope('conv1',reuse=tf.AUTO_REUSE) as scope:
            # W:[5,5]是窗口的大小;[1]是输入的厚度;[32]是输出的厚度
            W = tf.get_variable("W", [5,5,1,32], initializer=tf.truncated_normal_initializer(stddev = 0.1))
            if self.regularizer:
                tf.add_to_collection('losses',self.regularizer(W))

            # b:[32]是输出的厚度
            b = tf.get_variable("b", [32], initializer=tf.constant_initializer(0.1))

            # activate:a0是输入的图像们;strides = [1,1,1,1]是步长,一般取这个值就OK了
            a = tf.nn.relu(tf.nn.conv2d(a0,W,strides = [1,1,1,1],padding = 'SAME')+b, name='conv2d-relu')
            #a = tf.nn.relu(tf.nn.bias_add(tf.nn.conv2d(a0,W1,strides = [1,1,1,1],padding = 'SAME'), b1))

            # pooling:池化操作.就这样子就OK了 = >表示长宽缩小一半而厚度不变.
            a_pool = tf.nn.max_pool(a,ksize = [1,2,2,1],strides = [1,2,2,1],padding = 'VALID', name='pooling')

            # lrn层
            a1 = tf.nn.lrn(a_pool,depth_radius=4,bias=1.0,alpha=0.001/9.0,beta=0.75,name='lrn')

            # 最后输出的shape可以通过print(a1.get_shape())查看

        # Layer2 (conv+pooling+lrn)
        with tf.variable_scope('conv2',reuse=tf.AUTO_REUSE) as scope:
            # W:[5,5]是窗口的大小;[1]是输入的厚度;[32]是输出的厚度
            W = tf.get_variable("W", [5,5,32,64], initializer=tf.truncated_normal_initializer(stddev = 0.1))
            if self.regularizer:
                tf.add_to_collection('losses',self.regularizer(W))

            # b:[32]是输出的厚度
            b = tf.get_variable("b", [64], initializer=tf.constant_initializer(0.1))

            # activate:a0是输入的图像们;strides = [1,1,1,1]是步长,一般取这个值就OK了
            a = tf.nn.relu(tf.nn.conv2d(a1,W,strides = [1,1,1,1],padding = 'SAME')+b, name='conv2d-relu')
            #a = tf.nn.relu(tf.nn.bias_add(tf.nn.conv2d(a0,W1,strides = [1,1,1,1],padding = 'SAME'), b1))

            # pooling:池化操作.就这样子就OK了 = >表示长宽缩小一半而厚度不变.
            a_pool = tf.nn.max_pool(a,ksize = [1,2,2,1],strides = [1,2,2,1],padding = 'VALID', name='pooling')

            # lrn层
            a2 = tf.nn.lrn(a_pool,depth_radius=4,bias=1.0,alpha=0.001/9.0,beta=0.75,name='lrn')

            # 最后输出的shape可以通过print(a_norm.get_shape())查看

        with tf.name_scope('transition'):
            #tranisition from conv to fully_connected
            a2_tran = tf.reshape(a2,[-1,7*7*64])

        # Layer3 (fully_connected)
        a3 = self.nn(a2_tran,1024,tf.nn.sigmoid,regularizer=self.regularizer,scope_name='fully1')

        # dropout
        a3_dropout = tf.nn.dropout(a3,self.keep_prob)

        # Layer4 (fully_connected)
        y_predicted = self.nn(a3_dropout,10,tf.nn.softmax,regularizer=self.regularizer,scope_name='fully2')

        with tf.name_scope('loss'):
            # loss
            cross_entropy = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = self.labels,logits = y_predicted))
            tf.add_to_collection('losses',cross_entropy)

            self.loss = tf.add_n(tf.get_collection('losses'))
            
            params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES)

            self.train  = self.optimizer(self.loss, params, learning_rate)

        with tf.name_scope('accuracy'):
            self.total = tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_predicted,axis = 1),tf.argmax(self.labels,axis = 1)),tf.int16))



    def nn(self, inputs, output_dim, activator=None, regularizer=None, scope_name=None):
        '''
            定义神经网络的一层
        '''
        # 定义权重的初始化器
        norm = tf.random_normal_initializer(stddev=1.0)
        # 定义偏差的初始化
        const = tf.constant_initializer(0.1)

        # 打开变量域,或者使用None
        with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE):
            # 定义权重
            W = tf.get_variable("W",[inputs.get_shape()[1],output_dim],initializer=norm)
            if regularizer:
                tf.add_to_collection('losses',regularizer(W))
            # 定义偏差
            b = tf.get_variable("b",[output_dim],initializer=const)
            # 激活
            if activator is None:
                return tf.matmul(inputs,W)+b
            a = activator(tf.matmul(inputs,W)+b)
            # dropout
            # 返回输出值
            return a

    def optimizer(self, loss, var_list, initial_learning_rate):
        '''
            var_list:要训练的张量集合.
        '''
        decay = 0.99
        num_decay_steps = 150
        global_step = tf.Variable(0)
        learning_rate = tf.train.exponential_decay(
            initial_learning_rate,
            global_step,
            num_decay_steps,
            decay,
            staircase=True
        )
        optimizer = tf.train.AdamOptimizer(learning_rate).minimize(
            loss,
            global_step=global_step,
            var_list=var_list
        )
        return optimizer


# find the target
mnist = input_data.read_data_sets(FLAGS.data_path,one_hot = True)
X_test = mnist.test.images
y_test = mnist.test.labels
len_test = len(y_test)

model = Model(learning_rate=FLAGS.learning_rate,regularizer= tf.contrib.layers.l2_regularizer(FLAGS.regularization_scale))

# Saver
saver = tf.train.Saver()

gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.7)
config=tf.ConfigProto(gpu_options=gpu_options,allow_soft_placement=True)
# fight
with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    for step in range(FLAGS.num_epochs * mnist.train.num_examples // 100):
        X_train,y_train = mnist.train.next_batch(100)
        _, loss = sess.run([model.train,model.loss],feed_dict = {model.features:X_train,model.labels:y_train,model.keep_prob:0.5})
        #step = tf.train.global_step(sess, global_step)

        # check accuracy
        if (step + 1) % FLAGS.log_every ==  0:
            #y_pre = sess.run(model.y_predicted,feed_dict = {model.features:X_test,model.labels:y_test,model.keep_prob:1})
            #total = tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_pre,axis = 1),tf.argmax(y_test,axis = 1)),tf.int16))
            print("epoch {0}: {1}/{2}\tloss={3}".format(step + 1,sess.run(model.total,feed_dict = {model.features:X_test,model.labels:y_test,model.keep_prob:1}),len_test, loss))

        # save model
        if (step + 1) % FLAGS.save_every ==  0:
            saver.save(sess,'ckpt/',global_step=step+1)
