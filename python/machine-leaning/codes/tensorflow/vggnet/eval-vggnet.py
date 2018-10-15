'''
    Usage: python eval-vggnet.py
    Intro: TensorFlow实现VGGNet,评测forward(inference)和backward(training)的耗时
'''
import math
import time
import numpy as np
import tensorflow as tf

class VGG():
    # define a conv op

    def _conv(self, inputs, kernel_width, kernel_height, output_filters, stride_width, stride_height, scope_name):
        input_filters = inputs.shape[-1]

        with tf.variable_scope(scope_name,reuse=tf.AUTO_REUSE) as scope:
            W = tf.get_variable("W", [kernel_width,kernel_height,input_filters,output_filters], initializer=tf.contrib.layers.xavier_initializer_conv2d())

            b = tf.get_variable("b", [output_filters], initializer=tf.constant_initializer(0.0))

            a = tf.nn.relu(tf.nn.bias_add(tf.nn.conv2d(inputs,W,strides = [1,stride_width,stride_height,1],padding = 'SAME'), b))

        return a

    def _max_pooling(self, inputs, kernel_width, kernel_height, stride_width, stride_height):
        return tf.nn.max_pool(inputs,ksize = [1,kernel_width,kernel_height,1],strides = [1,stride_width,stride_height,1],padding = 'SAME', name='pooling')

    def _nn(self, inputs, output_dim, activator=None, scope_name=None, regularizer=None):
        with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE):
            W = tf.get_variable("W",[inputs.get_shape()[1],output_dim],initializer=tf.random_normal_initializer(stddev=1.0))
            if regularizer:
                tf.add_to_collection('losses',regularizer(W))

            b = tf.get_variable("b",[output_dim],initializer=tf.constant_initializer(0.1))

            if activator is None:
                return tf.matmul(inputs,W)+b

            a = activator(tf.matmul(inputs,W)+b)
            return a





    def inference(self, inputs, keep_prob):
        # 1st layer: conv * 2 + pooling: [batch_size, 224, 224, 3] => [batch_size, 112, 112, 64]
        with tf.variable_scope("layer1"):
            #def _conv(inputs, kernel_width, kernel_height, output_filters, stride_width, stride_height):
            conv1_1 = self._conv(inputs, 3, 3, 64, 1, 1, "conv1_1")

            conv1_2 = self._conv(conv1_1, 3, 3, 64, 1, 1, "conv1_2")

            #def _max_pooling(inputs, kernel_width, kernel_height, stride_width, stride_height):
            pool1 = self._max_pooling(conv1_2, 2, 2, 1, 1)
        
        # 2nd layer: conv * 2 + pooling: [batch_size, 112, 112, 64] => [batch_size, 56, 56, 128]
        with tf.variable_scope("layer2"):
            #def _conv(inputs, kernel_width, kernel_height, output_filters, stride_width, stride_height):
            conv2_1 = self._conv(pool1, 3, 3, 128, 1, 1, 'conv2_1')

            conv2_2 = self._conv(conv2_1, 3, 3, 128, 1, 1, 'conv2_2')

            #def _max_pooling(inputs, kernel_width, kernel_height, stride_width, stride_height):
            pool2 = self._max_pooling(conv2_2, 2, 2, 1, 1)

        # 3rd layer: conv * 3 + pooling: [batch_size, 56, 56, 128] => [batch_size, 28, 28, 256]
        with tf.variable_scope("layer3"):
            #def _conv(inputs, kernel_width, kernel_height, output_filters, stride_width, stride_height):
            conv3_1 = self._conv(pool2, 3, 3, 256, 1, 1, 'conv3_1')

            conv3_2 = self._conv(conv3_1, 3, 3, 256, 1, 1, 'conv3_2')

            conv3_3 = self._conv(conv3_2, 3, 3, 256, 1, 1, 'conv3_3')

            #def _max_pooling(inputs, kernel_width, kernel_height, stride_width, stride_height):
            pool3 = self._max_pooling(conv3_3, 2, 2, 1, 1)

        # 4th layer: conv * 3 + pooling: [batch_size, 28, 28, 256] => [batch_size, 14, 14, 512]
        with tf.variable_scope("layer4"):
            #def _conv(inputs, kernel_width, kernel_height, output_filters, stride_width, stride_height):
            conv4_1 = self._conv(pool3, 3, 3, 512, 1, 1, 'conv4_1')

            conv4_2 = self._conv(conv4_1, 3, 3, 512, 1, 1, 'conv4_2')

            conv4_3 = self._conv(conv4_2, 3, 3, 512, 1, 1, 'conv4_3')

            #def _max_pooling(inputs, kernel_width, kernel_height, stride_width, stride_height):
            pool4 = self._max_pooling(conv4_3, 2, 2, 1, 1)

        # 5th layer: conv * 3 + pooling: [batch_size, 14, 14, 512] => [batch_size, 7, 7, 512]
        with tf.variable_scope("layer5"):
            #def _conv(inputs, kernel_width, kernel_height, output_filters, stride_width, stride_height):
            conv5_1 = self._conv(pool4, 3, 3, 512, 1, 1, 'conv5_1')

            conv5_2 = self._conv(conv5_1, 3, 3, 512, 1, 1, 'conv5_2')

            conv5_3 = self._conv(conv5_2, 3, 3, 512, 1, 1, 'conv5_3')

            #def _max_pooling(inputs, kernel_width, kernel_height, stride_width, stride_height):
            pool5 = self._max_pooling(conv5_3, 2, 2, 1, 1)

        # reshape: [batch_size, 7, 7, 512] => [batch_size, 7 * 7 * 512]
        reshape1 = tf.reshape(pool5,[-1,7*7*512])

        # 6th layer: nn: [batch_size, 7 * 7 * 512] => [batch_size, 4096]
        nn6 = self._nn(reshape1, 4096, tf.tanh, 'layer6')
        nn6_dropout = tf.nn.dropout(nn6,keep_prob)

        # 7th layer: nn: [batch_size, 4096] => [batch_size, 4096]
        nn7 = self._nn(nn6_dropout, 4096, tf.tanh, 'layer7')
        nn7_dropout = tf.nn.dropout(nn7,keep_prob)

        # 8th layer: nn: [batch_size, 4096] => [batch_size, 1000] => (argmax) => y_predicted
        nn8 = self._nn(nn7_dropout, 1000, None, 'layer8')
        nn8_softmax = tf.nn.softmax(nn8)

        y_predicted = tf.argmax(nn8_softmax, axis = 1)

        # calc the l2_loss as the loss
        loss = tf.nn.l2_loss(nn8)  
       
        # calc grads_op
        params = tf.trainable_variables()
        grads = tf.gradients(loss, params)
        

        return params, grads


# define a function to calc time
def eval_duration(sess, target, num_batches, feed_dict):
    # define num_steps_burn_in to burn in the program
    num_steps_burn_in = 10
    # init total_duration
    total_duration = 0
    # init total_square
    total_square = 0

    # for i in range(num_batches):
    for i in range(num_batches + num_steps_burn_in):
        # record current time
        start_time = time.time()
        # run & get the op
        _ = sess.run(target, feed_dict=feed_dict)
        # calc duration
        duration = time.time() - start_time
        if i >= num_steps_burn_in:
            # update total_duration
            total_duration += duration
            # update total_square
            total_square += duration * duration
            
            if i % 10 == 0:
                # print info
                print("step {0} sec:duration={1} sec".format(i - num_steps_burn_in, duration))

    mean_duration = total_duration / num_batches
    vr = total_square / num_batches - mean_duration * mean_duration
    # calc standard deviation
    sd = math.sqrt(vr)
    # print final info
    print("final: mean_duration={0} sec +/- {1} sec".format(mean_duration, sd))


def main():
    vgg = VGG()

    batch_size = 1

    graph = tf.Graph()
    with graph.as_default():
        # generate dataset
        X = tf.Variable(tf.random_normal([batch_size,224, 224, 3],stddev = 1/tf.sqrt(1024.)))
        
        keep_prob = tf.placeholder(tf.float32, name='keep_prob')

        y_predicted, grads = vgg.inference(X, keep_prob)

    # start session
    #gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.7)
    #config = tf.ConfigProto(gpu_options=gpu_options)
    #sess = tf.Session(config=config,graph=graph)
    with tf.Session(graph=graph) as sess:
        # init global variables
        sess.run(tf.global_variables_initializer())

        num_batches = 100
        # eval y_predicted
        print("Starting evaluating prediction...")
        eval_duration(sess, y_predicted, num_batches, {keep_prob:1.0})
        
        # eval grads_op
        print("Starting evaluating grads...")
        eval_duration(sess, grads, num_batches, {keep_prob:0.5})
    

if __name__ == "__main__":
    main()
