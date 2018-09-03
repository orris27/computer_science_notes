import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data
# 获得输入:特征值和标签
features = tf.placeholder(tf.float32,[None,784]) 
labels = tf.placeholder(tf.float32,[None,10])
# 定义学习率
learning_rate = tf.Variable(1e-3)

# 第一层全连接神经网络:[-1,784]=>[-1,1024]
# 设置W
W1 = tf.Variable(tf.random_normal([784,1024],stddev = 1/tf.sqrt(1024.)))
# b:1024个节点
b1 = tf.Variable(tf.zeros([1024])+0.1)
# 运算:sigmoid
a1 = tf.nn.sigmoid(tf.matmul(features,W1)+b1)
# 
# 第二层全连接神经网络:[-1,1024] => [-1,100]
# W
W2 = tf.Variable(tf.random_normal([1024,100],stddev = 1/tf.sqrt(100.)))
# b:100
b2 = tf.Variable(tf.zeros([100])+0.1)
# 运算:sigmoid
a2 = tf.nn.sigmoid(tf.matmul(a1,W2)+b2)
# 

# 第三层全连接神经网络
# W
W3 = tf.Variable(tf.random_normal([100,10],stddev = 1/tf.sqrt(10.)))
# b:10
b3 = tf.Variable(tf.zeros([10])+0.1)
# 激活神经网络,并且获得预测的标签:softmax
y_predicted = tf.nn.softmax(tf.matmul(a2,W3)+b3)

# 定义代价函数
cross_entropy = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = labels,logits = y_predicted))
# 定义训练的张量
train = tf.train.AdamOptimizer(learning_rate).minimize(cross_entropy)
# 
mnist = input_data.read_data_sets('MNIST_data/',one_hot = True)            
X_test = mnist.test.images
# 获得测试集
y_test = mnist.test.labels
# 获得测试集里的总共个数
len_test = len(y_test)
# 启动session

with tf.Session() as sess:
    # 初始化变量
    sess.run(tf.global_variables_initializer())
    # 训练1000次
    for epoch in range(1000):
        # 获得100个训练集
        X_train,y_train = mnist.train.next_batch(100)
        # 训练
        sess.run(train,feed_dict = {features:X_train,labels:y_train})

        # 如果已经训练了50次的倍数,输出准确度
        if epoch % 50  ==  0:
            # 获得预测的标签
            y_pre = sess.run(y_predicted,feed_dict = {features:X_test,labels:y_test})
            # 计算这次的模型能用训练集预测出多少正确的
            total = tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_pre,axis = 1),tf.argmax(y_test,axis = 1)),tf.int16))
            predict_total = sess.run(total,feed_dict = {features:X_test,labels:y_test})
            # 输出正确的个数/全部
            print("epoch {0}: {1}/{2}".format(epoch,predict_total,len_test))
         

