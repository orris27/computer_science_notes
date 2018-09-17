'''
    介绍:自定义的loss函数,商品多生产1个,损失利润1元;如果少生产1个,就损失利润10元.所以自定义loss函数
    使用:python my-loss.py
'''
import tensorflow as tf
import numpy as np

with tf.name_scope('placeholder'):
    features = tf.placeholder(tf.float32,[None,2],name='features')
    labels = tf.placeholder(tf.float32,[None,1],name='labels')

def nn(inputs, output_dim, activator=None, scope_name=None):
    '''
        定义神经网络的一层
    '''
    # 定义权重的初始化器
    norm = tf.random_normal_initializer(stddev=1.0)
    # 定义偏差的初始化
    const = tf.constant_initializer(0.0)

    # 打开变量域,或者使用None
    with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE):
        # 定义权重
        W = tf.get_variable("W",[inputs.get_shape()[1],output_dim],initializer=norm)
        tf.add_to_collection('losses', tf.contrib.layers.l2_regularizer(.5)(W))

        # 定义偏差
        b = tf.get_variable("b",[output_dim],initializer=const)
        # 激活
        if activator is None:
            return tf.matmul(inputs,W)+b
        a = activator(tf.matmul(inputs,W)+b)
        # dropout
        # 返回输出值
        return a
    
y_predicted = nn(features,1,scope_name='nn')


# loss
mse_loss = tf.reduce_mean(tf.where(tf.greater(y_predicted,labels),1*(y_predicted-labels),10*(labels-y_predicted)))
tf.add_to_collection('losses',mse_loss)

loss = tf.add_n(tf.get_collection('losses'))
# global_step
global_step = tf.Variable(0)
# train
train = tf.train.AdamOptimizer(0.001).minimize(loss,global_step=global_step)

dataset_size = 1000
# construct X_train
X = np.random.rand(dataset_size,2)
# construct y_train
y = [[x1 + x2 + np.random.rand()/10.0 - 0.05]for (x1, x2) in X]

def next_batch(data, batch_size, num_steps,shuffle=True):
    '''
        获得数据,一批一批的
        1. data: 列表
        2. batch_size: 一批数据的大小
        3. num_steps: 总共迭代整个数据的次数
        4. shuffle:是否洗牌
    '''
    
    # 转换data为numpy array
    data = np.array(data)
    # 获得1个epoch有多少个batch_size
    num_batches = int((len(data) - 1)/batch_size) + 1
    # for(i:0=>epoch的次数)
    for i in range(num_steps):
        # 如果洗牌的话
        if shuffle:
            # 洗牌
            shuffle_indices = np.random.permutation(np.arange(len(data))) # 会将[0, len(data))的整数洗牌
            data = data[shuffle_indices]
        # for(i:0=>1个epoch中batch的个数)
        for i in range(num_batches):
            # 计算起始index
            start_index = i * batch_size
            # 计算结束index
            end_index = min((i+1)*batch_size, len(data))
            # yield出列表
            yield data[start_index:end_index]

batch_size = 16
num_epochs = 100
batches = next_batch(list(zip(X, y)), batch_size, num_epochs)


gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
config=tf.ConfigProto(gpu_options=gpu_options)
with tf.Session(config=config) as sess:
    sess.run(tf.global_variables_initializer())

    # for(i:0=>num_epochs)
    for batch in batches:
        X_train, y_train = zip(*batch) # 这里的X和y已经是训练集中的[batch_size,xxx]的二维矩阵了
        X = np.array(X_train)
        y = np.array(y_train)
        _, step, loss1 = sess.run([train, global_step, loss],feed_dict={features:X_train,labels:y_train})
        if (step + 1) % 100 == 0:
            print('step {0}:loss={1}'.format(step+1,loss1))
