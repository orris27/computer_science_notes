import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data

features=tf.placeholder(tf.float32,[None,784])
labels=tf.placeholder(tf.float32,[None,10])
keep_prob=tf.placeholder(tf.float32)

a0=tf.reshape(features,[-1,28,28,1])

# Layer1 (conv+pooling)

# W
W1=tf.Variable(tf.truncated_normal([5,5,1,32],stddev=0.1))
# b
b1=tf.Variable(tf.zeros([32])+0.1)
# activate
a1=tf.nn.relu(tf.nn.conv2d(a0,W1,strides=[1,1,1,1],padding='SAME')+b1)
# pooling
a1_pool=tf.nn.max_pool(a1,ksize=[1,2,2,1],strides=[1,2,2,1],padding='VALID')

# a1_pool is [-1,14,14,32]

# Layer2 (conv+pooling)

# W
W2=tf.Variable(tf.truncated_normal([5,5,32,64],stddev=0.1))
# b
b2=tf.Variable(tf.zeros([64])+0.1)
# activate
a2=tf.nn.relu(tf.nn.conv2d(a1_pool,W2,strides=[1,1,1,1],padding='SAME')+b2)
# pooling
a2_pool=tf.nn.max_pool(a2,ksize=[1,2,2,1],strides=[1,2,2,1],padding='VALID')

# a2_pool is [-1,7,7,64]

#tranisition from conv to fully_connected
a2_pool_tran=tf.reshape(a2_pool,[-1,7*7*64])

# Layer3 (fully_connected)

# W
# W3=tf.Variable(tf.random_normal([7*7*64,1024],stddev=0.1))
W3=tf.Variable(tf.random_normal([7*7*64,1024],stddev=1/tf.sqrt(1024.)))
# b
b3=tf.Variable(tf.zeros([1024])+0.1)
# activate
a3=tf.nn.sigmoid(tf.matmul(a2_pool_tran,W3)+b3)
# dropout
a3_dropout=tf.nn.dropout(a3,keep_prob)

# Layer4 (fully_connected)

# W
W4=tf.Variable(tf.random_normal([1024,10],stddev=0.1))
# b
b4=tf.Variable(tf.zeros([10])+0.1)
# activate
y_predicted=tf.nn.softmax(tf.matmul(a3_dropout,W4)+b4)

# loss
cross_entropy=-tf.reduce_mean(tf.reduce_sum(labels*tf.log(y_predicted)))
train=tf.train.AdamOptimizer(1e-3).minimize(cross_entropy)

# find the target
mnist=input_data.read_data_sets('MNIST_data/',one_hot=True)
X_test=mnist.test.images
y_test=mnist.test.labels
len_test=len(y_test)

# fight
with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    for epoch in range(1000):
        X_train,y_train=mnist.train.next_batch(100)
        sess.run(train,feed_dict={features:X_train,labels:y_train,keep_prob:0.3})

        # check accuracy
        if epoch % 50 == 0:
            y_pre=sess.run(y_predicted,feed_dict={features:X_test,labels:y_test,keep_prob:1})
            total=tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_pre,axis=1),tf.argmax(y_test,axis=1)),tf.int16))
            print("epoch {0}: {1}/{2}".format(epoch,sess.run(total,feed_dict={features:X_test,labels:y_test,keep_prob:1}),len_test))

