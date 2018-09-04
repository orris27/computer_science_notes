    
## 1. TensorFlow
1. 定义features和labels
```
#  [[特征值0,特征值1,特征值2,...,特征值783],
#   [第二个实例],
#   [第三个实例],]
features = tf.placeholder(tf.float32,[None,784]) 
#  [[是分类0的可能性,是分类1的可能性,是分类2的可能性,...,是分类9的可能性],
#   [第二个实例],
#   [第三个实例]]
labels = tf.placeholder(tf.float32,[None,10])
keep_prob = tf.placeholder(tf.float32)
learning_rate = tf.Variable(1e-3)
```
2. NN/CNN
    1. 定义卷积神经网络的一层
    ```
    # Layer1 (conv+pooling)
    
    # W:[5,5]是窗口的大小;[1]是输入的厚度;[32]是输出的厚度
    W1 = tf.Variable(tf.truncated_normal([5,5,1,32],stddev = 0.1))
    # b:[32]是输出的厚度
    b1 = tf.Variable(tf.zeros([32])+0.1)
    # activate:a0是输入的图像们;strides = [1,1,1,1]是步长,一般取这个值就OK了
    a1 = tf.nn.relu(tf.nn.conv2d(a0,W1,strides = [1,1,1,1],padding = 'SAME')+b1)
    # pooling:池化操作.就这样子就OK了 = >表示长宽缩小一半而厚度不变.
    a1_pool = tf.nn.max_pool(a1,ksize = [1,2,2,1],strides = [1,2,2,1],padding = 'VALID')

    # 输入=>输出: [-1,28,28,32]=>[-1,14,14,32]
    # a1_pool is [-1,14,14,32]
    ```
    2. 过渡卷积神经网路到全连接神经网络,改变特征值的形状
    ```
    # 改变适应卷积神经网络的形状到适应全连接神经网络的形状.[-1,7,7,64]  = > [-1.7*7*64]
    a2_pool_tran = tf.reshape(a2_pool,[-1,7*7*64])
    ```
    3. 定义全连接神经网络的一层,并使用dropout
        + 中间层节点个数:1024
        + 输入:`[-1,7*7*64]`
    ```
    # W3 = tf.Variable(tf.random_normal([输入值中每行有多少个,节点个数/输出值中每行有多少个],stddev = 0.1))
    W3 = tf.Variable(tf.random_normal([7*7*64,1024],stddev = 1/tf.sqrt(1024.)))
    # b
    b3 = tf.Variable(tf.zeros([1024])+0.1)
    # activate
    a3 = tf.nn.sigmoid(tf.matmul(a2_pool_tran,W3)+b3)
    # dropout
    a3_dropout = tf.nn.dropout(a3,keep_prob)
    ```
    4. [神经网络和卷积神经网络的实现](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/cnn)
3. 定义代价函数,并定义训练tensor
    + labels:正确的y
    + logits:预测的y
    + 优化器:AdamOptimizer
```
cross_entropy = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = labels,logits = y_predicted))
train = tf.train.AdamOptimizer(learning_rate).minimize(cross_entropy)
```

4. 训练
    + 整个训练集训练多少次:1000
    + 单次训练的实例个数:100
    + 学习率变化梯度:`0.97**epoch`
    + dropout:0.5
    + 训练集特征值:X_traion
    + 训练集标签:y_train
    + 什么时候输出准确度:训练次数是50的倍数
    + 测试集特征值:X_test
    + 测试集标签:y_test
    + 测试集dropout:1
    + 验证方式:预测正确了多少个
        + `tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_pre,axis = 1),tf.argmax(y_test,axis = 1)),tf.int16))`
    + 输出方式:当前是第几次训练:预测正确数/总共数量
        + `print("epoch {0}: {1}/{2}".format(epoch,predict_total,len_test))`
```
len_test = len(y_test)

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    for epoch in range(1000):
        tf.assign(learning_rate,1e-3*(0.97**epoch))
        X_train,y_train = mnist.train.next_batch(100)
        sess.run(train,feed_dict = {features:X_train,labels:y_train,keep_prob:0.5})

        # check accuracy
        if epoch % 50  ==  0:
            y_pre = sess.run(y_predicted,feed_dict = {features:X_test,labels:y_test,keep_prob:1})
            total = tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_pre,axis = 1),tf.argmax(y_test,axis = 1)),tf.int16))
            predict_total = sess.run(total,feed_dict = {features:X_test,labels:y_test,keep_prob:1})
            print("epoch {0}: {1}/{2}".format(epoch,predict_total,len_test))
```


5. 数据集
    1. MNIST_DATA
        1. 使用方式:直接把MNIST_DATA目录放到和程序同级目录的地方就OK了
        2. 数据格式
            1. 特征值
            ```
            [[特征值0,特征值1,...,特征值783],
             [第二个实例],
             [第三个实例]]
            ```
            2. 标签
            ```
            [[0,0,...,1,...,0], # 总共每一行有10个
             [第二个实例],
             [第三个实例]]            
            ```
        3. 获得
            1. 训练集特征值(X_train)
            ```
            from tensorflow.examples.tutorials.mnist import input_data
            
            mnist = input_data.read_data_sets('MNIST_data/',one_hot = True)            
            X_train,y_train = mnist.train.next_batch(100)
            ```
            2. 训练集标签(y_train):同上
            3. 测试集特征值(X_test)
            ```
            from tensorflow.examples.tutorials.mnist import input_data
            
            mnist = input_data.read_data_sets('MNIST_data/',one_hot = True)
            X_test = mnist.test.images
            ```
            4. 测试集标签(y_test)
            ```
            from tensorflow.examples.tutorials.mnist import input_data
            
            mnist = input_data.read_data_sets('MNIST_data/',one_hot = True)
            y_test = mnist.test.labels
            ```
        4. 下一批
        ```
        from tensorflow.examples.tutorials.mnist import input_data
            
        mnist = input_data.read_data_sets('MNIST_data/',one_hot = True)    
        X_train,y_train = mnist.train.next_batch(100)
        ```
6. 改变形状
```
#features = tf.placeholder(tf.float32,[None,784])
a0 = tf.reshape(features,[-1,28,28,1])
```


8. get_variable
    1. 初始化器
        1. 常数
        ```
        const = tf.constant_initializer(0.0)
        ```
        2. 正态分布产生的随机值
        ```
        norm = tf.random_normal_initializer(stddev=stddev)
        ```
    2. 重用变量,get_variable创建1个`foo/v`的变量
    ```
    with tf.variable_scope("foo", reuse=tf.AUTO_REUSE):
        norm = tf.random_normal_initializer(stddev=1.0)
        v = tf.get_variable("v", [1] ,initializer=norm)
    ```
    3. scope的with语句内部重用变量
    ```
    with tf.variable_scope("foo"):
        norm = tf.random_normal_initializer(stddev=1.0)
        v1 = tf.get_variable("v", [1] ,initializer=norm)

    with tf.variable_scope("foo") as scope:
        scope.reuse_variables()
        norm = tf.random_normal_initializer(stddev=1.0)
        v2 = tf.get_variable("v", [1] ,initializer=norm)
    ```


9. 复制s1变量域内的所有可训练的变量到s1变量域内
```
def scope_assign(src,dest,sess):
    '''
        赋值sess/src变量域内的值到sess/dest变量域内的值
    '''
    # 获得src内的所有可训练的变量
    src_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope=src)
    # 获得dest内的所有变量
    dest_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope=dest)
    
    # 获得现阶段src内张量的值
    src_params_run  = sess.run(src_params)
    # 复制src内的变量到dest的变量
    for i, v in enumerate(dest_params):
        sess.run(v.assign(src_params_run[i]))


#...
scope_assign('s1','s2',sess)
```
10. [get_collection和assing的复制变量域的实现](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/collection)


11. 我自己的命名规范
    1. batch_size:第一个维度,表示一个特征值/标签的实例个数=单次训练的实例个数
    2. 维度:1开始的下标
## 2. Python
1. 如果是`__main__`的话
```
if __name__ == '__main__': 
```
