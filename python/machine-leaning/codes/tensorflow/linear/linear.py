import tensorflow as tf
import numpy as np


# 定义features和labels的placeholder
features = tf.placeholder(tf.float32,[None,1])
labels = tf.placeholder(tf.float32,[None,1])
# 开启linear model's name_scope
with tf.name_scope('linear') as scope:
    # 定义k变量
    #k = tf.Variable(tf.random_normal([1]),name='k')
    k = tf.Variable(1.2,name='k')
    # 定义b变量
    #b = tf.Variable(tf.zeros([1]) + 0.1,name='b')
    b = tf.Variable(0.1,name='b')
    # 计算该模型的预测值
    y_predicted = k * features + b
    # 定义loss
    #loss = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = labels,logits = y_predicted))
    loss = tf.reduce_mean(tf.square(y_predicted - labels))
    # 定义train
    train = tf.train.GradientDescentOptimizer(0.02).minimize(loss)
# 获得训练集的特征值
X_train = np.linspace(-10, 10, 100) + np.random.random(100) * 0.01
X_train = np.expand_dims(X_train, 1)
# 获得训练集的标签
y_train = 1.1 * X_train + 0.2

gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
config=tf.ConfigProto(gpu_options=gpu_options)

with tf.Session(config=config) as sess:
    # 初始化全局变量
    sess.run(tf.global_variables_initializer())
    # 循环k轮
    for step in range(10000):
        # 训练模型:使用我们定义的训练集的特征值和标签
        sess.run(train,feed_dict = {features:X_train,labels:y_train})
        # 打印k和b
        print('step {0}:loss={3}\tk={1}\tb={2}'.format(step,sess.run(k),sess.run(b),sess.run(loss,feed_dict = {features:X_train,labels:y_train})))
