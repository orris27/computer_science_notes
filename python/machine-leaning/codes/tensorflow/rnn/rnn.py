import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data

train_times=28
n_inputs=28
lstm_size=100
n_classes=10
batch_size=100
layer_num = 2


# placeholder
features=tf.placeholder(tf.float32,[None,784])
labels=tf.placeholder(tf.float32,[None,10])

# reshape
inputs=tf.reshape(features,[-1,train_times,n_inputs])
# defines the lstm_cell
lstm_cell=tf.contrib.rnn.BasicLSTMCell(lstm_size,state_is_tuple=True)
# Dropout for lstm_cell
lstm_cell = tf.nn.rnn_cell.DropoutWrapper(lstm_cell, output_keep_prob=0.5)
# map
ouputs,final_state=tf.nn.dynamic_rnn(lstm_cell,inputs,dtype=tf.float32)


# W
weights=tf.Variable(tf.truncated_normal([lstm_size,n_classes]))
# b
bias=tf.Variable(tf.constant(0.1,shape=[n_classes]))
# final
y_predicted=tf.nn.softmax(tf.matmul(final_state[1],weights)+bias)

loss=tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(logits=y_predicted,labels=labels))
train=tf.train.AdamOptimizer(1e-4).minimize(loss)



mnist=input_data.read_data_sets('MNIST_data/',one_hot=True)
X_test=mnist.test.images
y_test=mnist.test.labels
len_test=len(y_test)


batch_total= mnist.train.num_examples // batch_size


epoches=30


# 控制使用GPU的量为0.9
gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.9)
config=tf.ConfigProto(gpu_options=gpu_options)

with tf.Session(config=config) as sess:
    sess.run(tf.global_variables_initializer())
    for epoch in range(epoches):
        for _ in range(batch_total):
            X_train, y_train = mnist.train.next_batch(batch_size)
            sess.run(train,feed_dict={features:X_train,labels:y_train})

        # check accuracy
        #if epoch % 50 == 0:
        y_pre=sess.run(y_predicted,feed_dict={features:X_test,labels:y_test})
        total=tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_pre,axis=1),tf.argmax(y_test,axis=1)),tf.int16))
        print("epoch {0}: {1}/{2}".format(epoch,sess.run(total,feed_dict={features:X_test,labels:y_test}),len_test))




