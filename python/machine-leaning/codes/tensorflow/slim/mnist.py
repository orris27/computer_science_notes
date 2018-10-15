'''
    Usage: Prepare "MNIST_data/" directory & python mnist.py
'''
import tensorflow as tf
import numpy as np
import tensorflow.contrib.slim as slim
from tensorflow.examples.tutorials.mnist import input_data

features = tf.placeholder(tf.float32,[None,784])
labels = tf.placeholder(tf.float32,[None,10])
keep_prob = tf.placeholder(tf.float32)

inputs_reshaped = tf.reshape(features, [-1, 28, 28, 1])
# layer1: cnn
conv1 = slim.conv2d(inputs_reshaped, num_outputs=32, kernel_size=[5, 5], padding='SAME', scope="layer1_conv")
conv1_pooling = slim.max_pool2d(conv1, kernel_size=[2, 2],stride=2,scope="layer1_pooling")
# layer2: cnn
conv2 = slim.conv2d(conv1_pooling, num_outputs=64, kernel_size=[5, 5], padding='SAME', scope="layer2_conv")
conv2_pooling = slim.max_pool2d(conv2, kernel_size=[2, 2],stride=2,scope="layer2_pooling")

# reshape
conv2_reshape = slim.flatten(conv2_pooling, scope="reshape")

# layer3: nn
nn3 = slim.fully_connected(conv2_reshape, num_outputs=500, activation_fn=tf.nn.sigmoid, scope="layer3") # activation_fn is default to ReLU
# layer4: nn
#nn4 = slim.fully_connected(nn3, num_outputs=10, scope="layer4")
#y_predicted = slim.fully_connected(nn3, num_outputs=10, activation_fn=tf.nn.softmax, scope="layer4")
y_predicted = slim.fully_connected(nn3, num_outputs=10, activation_fn=None, scope="layer4")

#y_predicted = nn4

# calc loss
# loss
####################################################################################################################
# Do not use the loss above!! Otherwise, the params will not be updated
####################################################################################################################
#loss=-tf.reduce_mean(tf.reduce_sum(labels*tf.log(y_predicted)))
loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=y_predicted, labels=tf.argmax(labels, 1)))




#loss = cross_entropy

#train=tf.train.AdamOptimizer(1e-3).minimize(cross_entropy)
train=tf.train.GradientDescentOptimizer(0.01).minimize(loss)



# find the target
mnist=input_data.read_data_sets('/home/user/dataset/MNIST_data/',one_hot=True)
X_test=mnist.test.images
y_test=mnist.test.labels
len_test=len(y_test)


#train, y_predicted = inference(features, labels)

# fight
with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    for epoch in range(1000):
        X_train,y_train=mnist.train.next_batch(100)
        sess.run(train,feed_dict={features:X_train,labels:y_train,keep_prob:0.3})

        # check accuracy
        if epoch % 50 == 0:
            y_pre = sess.run(y_predicted,feed_dict={features:X_test,labels:y_test,keep_prob:1})
            total=tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_pre,axis=1),tf.argmax(y_test,axis=1)),tf.int16))
            print("epoch {0}: {1}/{2}".format(epoch,sess.run(total,feed_dict={features:X_test,labels:y_test,keep_prob:1}),len_test))
