    
## 1. TensorFlow
1. 定义features和labels
```
with tf.name_scope('placeholder'):
    #  [[特征值0,特征值1,特征值2,...,特征值783],
    #   [第二个实例],
    #   [第三个实例],]
    features = tf.placeholder(tf.float32,[None,784], name='features') 
    #  [[是分类0的可能性,是分类1的可能性,是分类2的可能性,...,是分类9的可能性],
    #   [第二个实例],
    #   [第三个实例]]
    labels = tf.placeholder(tf.float32,[None,10], name='labels')
    keep_prob = tf.placeholder(tf.float32, name='keep_prob')
learning_rate = tf.Variable(1e-3)
```
2. NN/CNN
    1. CNN
        1. <方法1> 函数
        ```
        def _conv(self, inputs, kernel_width, kernel_height, output_filters, stride_width, stride_height, scope_name):
            input_filters = inputs.shape[-1]

            with tf.variable_scope(scope_name,reuse=tf.AUTO_REUSE) as scope:
                W = tf.get_variable("W", [kernel_width,kernel_height,input_filters,output_filters], initializer=tf.contrib.layers.xavier_initializer_conv2d())

                b = tf.get_variable("b", [output_filters], initializer=tf.constant_initializer(0.1))

                a = tf.nn.relu(tf.nn.bias_add(tf.nn.conv2d(inputs,W,strides = [1,stride_width,stride_height,1],padding = 'SAME'), b))

            return a

        def _max_pooling(self, inputs, kernel_width, kernel_height, stride_width, stride_height):
            return tf.nn.max_pool(inputs,ksize = [1,kernel_width,kernel_height,1],strides = [1,stride_width,stride_height,1],padding = 'SAME', name='pooling')


        ########################################################################################################
        # 使用范例
        ########################################################################################################
        with tf.variable_scope("layer1"):
            conv1_1 = self._conv(inputs, 3, 3, 64, 1, 1, "conv1_1")

            conv1_2 = self._conv(conv1_1, 3, 3, 64, 1, 1, "conv1_2")

            pool1 = self._max_pooling(conv1_2, 2, 2, 1, 1)
        ```
        2. <方法2> 代码块
        + conv2d
            1. a0:输入的4维矩阵
            2. W1:`[5,5]`/`[filter_size,5]`是窗口的大小;`[1]`是输入的厚度;`[32]`/`[num_filters]`是输出的厚度
            3. strides
            4. padding
                1. 'VALID':最后结果的shape=`[batch_size,a0的2nd维度 - 窗口大小的1st维度 + 1, a0的3rd维度 - 窗口大小的2nd维度 + 1, 输出的厚度]`
                2. 'SAME':最后结果的shape=a0.shape
        + max_pool
            1. a1
            2. ksize:将中间2个维度大小的矩阵变成`1*1`的矩阵
            3. strides:pooling会重新作用于使用过的点,除非我们步长设置的和ksize一样.
            4. padding
                1. VALID:如果不够pooling的话,就不管了.比如对于`[1,2,3,1]`使用`{ksize=[1,2,2,1],strides=[1,2,2,1]}`时结果为`[1,1,1,1]`(第3列被抛弃了)
                2. SAME:只有需要补全的时候才补全.比如对于`[1,2,3,1]`使用`{ksize=[1,2,2,1],strides=[1,2,2,1]}`时结果为`[1,1,2,1]`(补全成`[1,2,4,1]`)
        ```
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
        ```
    2. 过渡卷积神经网路到全连接神经网络,改变特征值的形状
        1. TensorFlow的reshape:不能使用在普通变量
        ```
        # 改变适应卷积神经网络的形状到适应全连接神经网络的形状.[-1,7,7,64]  = > [-1.7*7*64]
        a2_pool_tran = tf.reshape(a2_pool,[-1,7*7*64])
        ```
        2. numpy的reshape:如果是和张量无关的话,就使用np.reshape
        ```
        a2_pool_tran = np.reshape(a2_pool,(batch_size,1))
        ```
    3. 定义全连接神经网络的一层,并使用dropout
        1. 方法1
            + 中间层节点个数:1024
            + 输入:`[-1,7*7*64]`
        ```
        # W = tf.Variable(tf.random_normal([输入值中每行有多少个,节点个数/输出值中每行有多少个],stddev = 0.1))
        W = tf.Variable(tf.random_normal([7*7*64,1024],stddev = 1/tf.sqrt(1024.)))
        # b
        b = tf.Variable(tf.zeros([1024])+0.1)
        # activate
        a1 = tf.nn.sigmoid(tf.matmul(a0_pool_tran,W)+b)
        # dropout
        a1_dropout = tf.nn.dropout(a1,keep_prob)
        ```
        2. 方法2
        ```
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
            
        # 使用方法如下所示
        def discriminator(self,inputs,dim):
            '''
                定义判别器的模型
            '''
            # 第0层全连接神经网络:节点数=dim,激活函数=tf.tanh,scope=d0
            a0 = self._nn(inputs,dim*2,tf.tanh,'d0')
            # 第1层全连接神经网络:节点数=dim,激活函数=tf.tanh,scope=d1
            a1 = self._nn(a0,dim*2,tf.tanh,'d1')
            # 第2层全连接神经网络:节点数=dim,激活函数=tf.tanh,scope=d2
            a2 = self._nn(a1,dim*2,tf.tanh,'d2')
            # 第3层全连接神经网络:节点数=dim,激活函数=tf.tanh,scope=d3
            #a3 = self._nn(a2,dim*2,tf.tanh,'d3') 
            # 返回判别器的预测给分:节点数=1,激活函数=tf.sigmoid,形状=[batch_size,1]
            y_predicted = self._nn(a2,1,tf.nn.sigmoid,'d3')
            return y_predicted
        ```
        3. 方法3:[fully_connected官方文档](https://www.tensorflow.org/api_docs/python/tf/contrib/layers/fully_connected)
        ```
        y_predicted = tf.contrib.layers.fully_connected(output,1,activation_fn=None)
        ```
    4. [神经网络和卷积神经网络的实现](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/cnn)
    5. 多种不同类型的窗口大小下CNN
    ```
    # 定义保存pooling结果的列表
    outputs = []
    # filter_sizes:[2,3,4]这样的
    for filter_size in filter_sizes:
        # 执行卷积操作
        with tf.name_scope('conv-{0}'.format(filter_size)):
            # W:窗口大小(filter_size,embedding_size),当前filter层个数,输出filter层个数
            W = tf.Variable(tf.truncated_normal([filter_size,embedding_size,1,num_filters],stddev = 0.1), name='W')
            # b:输出的filter层的个数
            b = tf.Variable(tf.zeros([num_filters])+0.1, name='b')
            # conv2d + relu:[batch_size,time_step,embedding_size,1] => [batch_size,time_step - filter_size + 1,1,num_filters]
            a = tf.nn.relu(tf.nn.conv2d(a0,W,strides = [1,1,1,1],padding = 'VALID')+b, name='conv2d-relu')
            # pooling:[batch_size,time_step - filter_size + 1,1,num_filters] => [batch_size,1,1,num_filters]
            a_pool = tf.nn.max_pool(a,ksize = [1,sequence_length - filter_size + 1,1,1],strides = [1,1,1,1],padding = 'VALID', name='pooling')
            # 添加pooling结果到列表中
            outputs.append(a_pool)

    # 拼接pooling结果的列表为一个结果: 多个[batch_size,1,1,num_filters] => [batch_size,1,1,num_filters*n]
    a1 = tf.concat(outputs,-1)
    # reshape成1个2维矩阵: [batch_size,1,1,num_filters*n] => [batch_size,1*1*num_filters*n]
    a1 = tf.reshape(a1, [batch_size, 1*1*num_filters*len(filter_sizes)])

    # 全连接层进行分类: [batch_size,1*1*num_filters*n] => [batch_size, num_classes]
    y_predicted = self.nn(a1,num_classes,tf.nn.softmax,'fully-connected')

    # 定义损失函数
    with tf.name_scope('loss'):
        self.loss = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = self.labels,logits = y_predicted))
    ```


3. loss代价函数(损失函数)
    1. sigmoid_cross_entropy_with_logits
        + labels:正确的y
        + logits:预测的y
        + 优化器:AdamOptimizer
    ```
    loss = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = labels,logits = y_predicted))
    ```
    2. sparse_softmax_cross_entropy_with_logits
        + 注意
            1. logits.shape是[batch_size, num_classes] (dtype=tf.float)，labels.shape必须是[batch_size] (dtype=tf.int)
            2. 使用前不能经过softmax.即y_predicted没有经过softmax处理
            3. output: a tensor of the same shape as labels
            4. tf.reduce_sum() && tf.reduce_mean(): if the shape of labels is (700,), then tf.reduce_sum() is the 700 times the length of tf.reduce_mean()
    ```
    cross_entropy = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = labels,logits = y_predicted))
    tf.add_to_collection('losses',cross_entropy)
    
    # 如果使用的labels为[batch_size, num_classes]格式的话,用tf.argmax缩减第二个维度
    #cross_entropy = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = tf.argmax(labels,1),logits = y_predicted)) 
    ```
    3. mean_squared_error
        + 差的平方和的平均值
    ```
    # labels 和 predictions 都是一维数组
    loss = tf.losses.mean_squared_error(labels=self.labels,predictions=self.y_predicted)
    ```
    4. 自带学习率衰减
        + var_list:类似于`self.d_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='disc')`
            + tf.GraphKeys.TRAINABLE_VARIABLES是字符串类型,为`trainable_variables`
            + tf.get_collection返回的是变量的列表
            + GraphKeys
                1. `GLOBAL_VARIABLES`
                2. `LOCAL_VARIABLES`
                3. `MODEL_VARIABLES`
                4. `TRAINABLE_VARIABLES`
                5. `SUMMARIES`
                6. `QUEUE_RUNNERS`
                7. `MOVING_AVERAGE_VARIABLES`
                8. `REGULARIZATION_LOSSES`
        + self.d_loss:代价函数
        + self.learning_rate:0.03等
    ```
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

    
    self.d_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='disc')
    self.d_train  = self.optimizer(self.d_loss, self.d_params, self.learning_rate)
    ```
    5. more concrete examples
        1. perplexity
            1. labels: `[batch_size * num_steps]`
            2. y_predicted: `[batch_size * num_steps, vocab_size]`
        ```
        self.loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = self.labels,logits = y_predicted))
        params = tf.trainable_variables()
        opt = tf.train.GradientDescentOptimizer(learning_rate) # learning_rate = 1.0
        gradients = tf.gradients(self.loss * num_steps, params) # reduce (self.loss * num_steps)
        clipped_gradients, norm = tf.clip_by_global_norm(gradients,max_grad_norm) # max_grad_norm = 5
        self.train = opt.apply_gradients(zip(clipped_gradients, params))
        
        ##########################################################################
        # ... The self.loss only calculates the perplexity in that batch
        ##########################################################################
        total_loss = 0.0
        step = 0
        while batches:
            total_loss += loss
            step += 1
            perplexity = np.exp(total_loss / step) # => correct perplexity
        ```
    6. Policy Gradients下取值只有{0, 1}的Action. (完整代码看30-4的CartPole问题解决的代码)
    ```
    with tf.name_scope("loss"):
        loglik = tf.log((false_labels - probability) ** 2)
        loss = -tf.reduce_mean(loglik * advantages) # 这里的advantages是经过标准化的Discounted Future Rewards
    ```
    7. l2_loss: `output = sum(t ** 2) / 2`
        + warning: data type of the input should be float
    ```
    a
    #-----------------------------------------------------
    # array([-3., -2., -1.,  0.,  1.,  2.,  3.])
    #-----------------------------------------------------

    tf.nn.l2_loss(a).eval()
    #-----------------------------------------------------
    # 14.0  # <= (3 ** 2 + 2 ** 2 + 1 ** 2 ) * 2 / 2
    #-----------------------------------------------------

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
            1. 特征值:images (numpy array)
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
    2. iris
    ```
    from sklearn import datasets
    
    iris = datasets.load_iris()

    print(iris)
    # knn.fit(iris.data, iris.target) # iris.data => [batch_size, features_dims]; iris.target => [batch_size]
    ```
6. 改变形状
```
#features = tf.placeholder(tf.float32,[None,784])
a0 = tf.reshape(features,[-1,28,28,1])
```


7. get_variable
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
    4. get_variable的第一个参数使用变量的scope和使用字符串
        1. 使用字符串
        ```
        import tensorflow as tf
        with tf.variable_scope("scope"):
            tf.get_variable("w",shape=[1])#这个变量的name是 scope/w
            with tf.variable_scope("scope"):
                tf.get_variable("w", shape=[1]) #这个变量的name是 scope/scope/w
        # 这两个变量的名字是不一样的，所以不会产生冲突
        ```
        2. 使用变量
        ```
        import tensorflow as tf
        with tf.variable_scope("yin"):
            tf.get_variable("w",shape=[1])
            scope = tf.get_variable_scope()#这个变量的name是 scope/w
            with tf.variable_scope(scope):#这种方式设置的scope，是用的外部的scope
                tf.get_variable("w", shape=[1])#这个变量的name也是 scope/w
        # 两个变量的名字一样，会报错
        ```
    5. 获得变量域的名字(`original_name_scope`会返回字符串)
        + 用在variable_scope的第一个参数是string or VariableScope
    ```
    with tf.variable_scope("foo", reuse=tf.AUTO_REUSE):
        print(tf.get_variable_scope())
        print(tf.get_variable_scope().original_name_scope)
        print(tf.get_variable_scope().name)
        with tf.variable_scope("foo1", reuse=tf.AUTO_REUSE):
            print(tf.get_variable_scope())
            print(tf.get_variable_scope().original_name_scope)
            print(tf.get_variable_scope().name)
    #-------------------------------------------------------------------------------------
    # <tensorflow.python.ops.variable_scope.VariableScope object at 0x7efec9986390>
    # foo/
    # foo
    # <tensorflow.python.ops.variable_scope.VariableScope object at 0x7fe9bb2875c0>
    # foo/foo1/
    # foo/foo1
    #-------------------------------------------------------------------------------------

    ```

8. 复制s1变量域内的所有可训练的变量到s1变量域内
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
9. [get_collection和assing的复制变量域的实现](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/collection)


10. 我自己的命名规范
    1. `batch_size`:第一个维度,表示一个特征值/标签的实例个数=单次训练的实例个数
    2. 维度:1开始的下标
    3. 训练次数
        1. `num_pretrain_steps`
        2. `num_steps`或`num_epochs`: 表示训练整个数据集的个数
        3. `num_batches`: 表示训练1个batch的次数
    4. CNN
        1. `filter_size`:窗口的宽
        2. `filter_sizes`:窗口的宽的列表
        3. `num_filters`:输出的filter层的个数
    5. 间隔
        1. `evalute_every`
    6. 分类问题
        1. `num_classes`:分多少类
        

11. [自己模仿写的正态分布的GAN](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/gan)

12. 随机数
    1. 均匀分布下的随机生成
    ```
    # 生成X_train:获取区间为[-5,5)的平均分布下随机产生的形状为[batch_size]的值
    X_train = (np.random.random(batch_size) - 0.5) * 10.0

    ```
    2. 间隔地随机生成
    ```
    data_range = 8
    a = np.linspace(-data_range, data_range, batch_size) + np.random.random(batch_size) * 0.01
    ```
    3. 正态分布下的随机生成
    ```
    # 定义正态分布的均值
    mu = 4
    # 定义正态分布的方差
    sigma = 0.5
    
    # 获得正态分布下随机生成的形状为[batch_size,1]的数据
    samples = np.random.normal(self.mu, self.sigma, batch_size)
    ```
13. 数学函数
    1. 正态分布的概率密度函数
        1. self.mu:均值
        2. self.sigma:方差
    ```
    from scipy.stats import norm
    # 定义正态分布的均值
    mu = 4
    # 定义正态分布的方差
    sigma = 0.5

    # 生成y_train:正态分布的概率密度函数(X_train)
    y_train = norm.pdf(X_train, loc=self.mu, scale=self.sigma)
    ```
14. 保存和还原会话
    1. 保存
        1. 保存会话里的所有变量
            + `+=`赋值的变量不会被保存
            + `saver = tf.train.Saver()`的max_to_keep=5:如果调用save的话,只有最近的max_to_keep次的结果会被保存
                + 如果用i循环`[0,10)`并且每个循环都调用save,默认使用5的话,就只有5,6,7,8,9会被保存,前面的都被删除
                + 如果想要保存所有save调用的结果的话,那么设置为0或者None
                ```
                saver = tf.train.Saver(max_to_keep=0)
                ```
            + `saver.save`的global_step可以更改存储模型的文件名
            ```
            saver.save(sess, 'my-model', global_step=0)    ==> filename: 'my-model-0'
            ...
            saver.save(sess, 'my-model', global_step=1000) ==> filename: 'my-model-1000'
            #-------------------------------------------------------------------------------------
            # -1000.data-00000-of-00001
            # -1000.index
            # -1000.meta
            # checkpoint
            #-------------------------------------------------------------------------------------
            ```
        ```
        W1 = tf.Variable(tf.truncated_normal([1],stddev = 0.1))

        saver = tf.train.Saver()
        with tf.Session(config=config) as sess:
            sess.run(tf.global_variables_initializer())
            print(sess.run(W1))
            saver.save(sess,"ckpt/")  # 如果使用目录来存储,最后一定要加入/
            #saver.save(sess,"ckpt/1.ckpt") 
        ```
        2. 保存会话里的部分变量
            1. 存储:<key,value>
                + key默认是value的名字(即带有scope的名字)
                + value就是变量的值
                + restore的时候根据<key,value>来恢复内容
            ```
            v1 = tf.Variable(..., name='v1')
            v2 = tf.Variable(..., name='v2')

            # 方法1
            saver = tf.train.Saver({'v1': v1, 'v2': v2})

            # 方法2
            saver = tf.train.Saver([v1, v2]) # 如果使用列表的话,效果同字典的上面用法

            # 方法3
            saver = tf.train.Saver({v.op.name: v for v in [v1, v2]})
            ```
            2. 检查:`print_tensors_in_checkpoint_file`仅仅打印指定位置下存储的变量对应的`<key,value>`,并不恢复这些变量的值
            ```
            from tensorflow.python.tools.inspect_checkpoint import print_tensors_in_checkpoint_file
            print_tensors_in_checkpoint_file("ckpt/", None, True) # 这样就是恢复test-ckpt里默认的checkpoint
            #print_tensors_in_checkpoint_file("ckpt/model-2", None, True) # model-2是上次使用save时的参数"test-ckpt/model-2.ckpt"
            ```

    2. 还原
        1. 还原整个会话里的变量
        ```
        W1 = tf.Variable(tf.truncated_normal([1],stddev = 0.1))

        saver = tf.train.Saver()
        with tf.Session(config=config) as sess:
            saver.restore(sess,"ckpt/1.ckpt")
            print(sess.run(W1))
        ```
        2. 还原整个graph,而不需要自己重新定义tensor变量
            + 可以用其meta来还原整个graph
            + 如果保存是"ckpt/"的话,".meta"是隐藏文件
            + 如果前面定义了重复名字的tensor,就会使用该tensor
        ```
        saver = tf.train.import_meta_graph("ckpt/model.ckpt.meta")
        v2 = tf.Variable(3.14,name='v2')

        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config = tf.ConfigProto(gpu_options=gpu_options)
        with tf.Session(config=config) as sess:
            saver.restore(sess,"ckpt/model.ckpt")
            print(sess.run(tf.get_default_graph().get_tensor_by_name("v1:0")))
            print(sess.run(tf.get_default_graph().get_tensor_by_name("v2:0"))) # 返回的是3.14

        ```
        3. 恢复最后一次save的模型:`tf.train.latest_checkpoint('ckpt/')`<=>`ckpt = tf.train.get_checkpoint_state('ckpt/')`下的`ckpt.model_checkpoint_path`(ckpt是checkpoint里面所有信息的字典)
        ```
        ckpt_latest = tf.train.latest_checkpoint('ckpt/') # model比如说是'ckpt/-9'(表示第global_step=9时save到'ckpt/').如果没有的话,model=None.常用这个来判断是否能恢复模型
        if ckpt_latest:
            saver.restore(sess,ckpt_latest)
            global_step = int(ckpt_latest.split('/')[-1].split('-')[-1])
            #print(sess.run(W1))
        ```
    3. 保存结果
    ```
    ckpt/
    ├── 1.ckpt.data-00000-of-00001
    ├── 1.ckpt.index
    ├── 1.ckpt.meta
    └── checkpoint
    ```
    4. 保存十次save的所有结果
    ```
    import tensorflow as tf

    W1 = tf.Variable(tf.truncated_normal([1],stddev = 0.1))
    update = tf.assign_add(W1,[1])


    saver = tf.train.Saver(max_to_keep=0) # 保存所有save结果

    gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config = tf.ConfigProto(gpu_options=gpu_options)

    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())

        for i in range(10):
            sess.run(update)
            print(sess.run(W1))
            saver.save(sess,"ckpt/",global_step=i)
    #------------------------------------------------------------------------------------------------
    # ckpt/
    # ├── -0.data-00000-of-00001
    # ├── -0.index
    # ├── -0.meta
    # ├── -1.data-00000-of-00001
    # ├── -1.index
    # ├── -1.meta
    # ├── -2.data-00000-of-00001
    # ├── -2.index
    # ├── -2.meta
    # ├── -3.data-00000-of-00001
    # ├── -3.index
    # ├── -3.meta
    # ├── -4.data-00000-of-00001
    # ├── -4.index
    # ├── -4.meta
    # ├── -5.data-00000-of-00001
    # ├── -5.index
    # ├── -5.meta
    # ├── -6.data-00000-of-00001
    # ├── -6.index
    # ├── -6.meta
    # ├── -7.data-00000-of-00001
    # ├── -7.index
    # ├── -7.meta
    # ├── -8.data-00000-of-00001
    # ├── -8.index
    # ├── -8.meta
    # ├── -9.data-00000-of-00001
    # ├── -9.index
    # ├── -9.meta
    # └── checkpoint
    #------------------------------------------------------------------------------------------------
    ```


15. 生成全是0的变量
    1. 定义变量
    ```
    W = tf.Variable(tf.zeros(3),name='weights',dtype=tf.float32)
    ```
    2. 定义张量
    ```
    tf.zeros([3, 4], tf.int32) # 默认类型是tf.float32
    tf.zeros(3) #< =>tf.zeros([3])
    ```
    
    
    
16. GPU
    1. tf.ConfigProto的参数
        1. log_device_placement=True : 日志中记录每个节点会被安排在哪个设备上.如果False可以减少日志量(生产环境使用)
        2. allow_soft_placement=True ： 允许TF将GPU上的运算放到CPU上;如果False一般就报错(设置True的话移植性强)
            1. 放到CPU上运算的条件
                1. 运算无法在GPU上进行
                2. 没有GPU资源.(比如指定第二个GPU,却没有)
                3. 运算输入包含对CPU计算结果的引用
        ```
        tf.ConfigProto(log_device_placement=True,allow_soft_placement=True)
        ```
    2. 控制GPU资源使用率
        1. 刚一开始分配少量的GPU容量，然后按需慢慢的增加
            + 不会释放内存，所以会导致碎片
        ```
        config = tf.ConfigProto()
        config.gpu_options.allow_growth = True
        session = tf.Session(config=config, ...)
        ```
        2. 设置每个GPU应该拿出多少容量给进程使用，0.4代表40%
        ```
        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config = tf.ConfigProto(gpu_options=gpu_options)
        session = tf.Session(config=config, ...)
        ```
    3. 控制使用哪块GPU
    ```
    ~/ CUDA_VISIBLE_DEVICES=0  python your.py#使用GPU0
    ~/ CUDA_VISIBLE_DEVICES=0,1 python your.py#使用GPU0,1
    #注意单词不要打错

    #或者在 程序开头
    os.environ['CUDA_VISIBLE_DEVICES'] = '0' #使用 GPU 0
    os.environ['CUDA_VISIBLE_DEVICES'] = '0,1' # 使用 GPU 0，1
    ```
    4. 指定设备
        + 设备在TensorFlow中的命名是`/gpu:n`或者`/cpu:n`
        + "/cpu:0"一般会认为是所有CPU;但"/gpu:0"会认为就是第一个GPU,而第二个GPU就是"/gpu:1"
    ```
    with tf.device("/gpu:0"):
        # ...
    ```
    
17. rnn
    1. dynamic_rnn
        1. `[batch_size, num_steps]` => embedding层(`[vocab_size, embedding_size]`) => `[batch_size, num_steps, embedding_size]` => dynamic_rnn => outputs(`[batch_size, num_steps, lstm_size]`), final_state
            + `[有几句话, 每句话最多几个单词]` => embedding层 => `[有几句话, 每句话最多几个单词, 一个单词对应多长的词向量]` => dynamic_rnn => `[有几句话, 每句话最多几个单词, RNN内部有多少个节点]`, `(最后一个节点的c输出, 最后一个节点的h输出)`
            + `embedding_lookup`: 最终结果就是在最后增加一维,这个维度的长度是embedding_size.如果输入是1维或2维都没有关系
        2. embedding_size(一个单词转化后的词向量长度) 和 lstm_size(RNN隐藏层的节点个数) 的关系: embedding_size和lstm_size不需要相等,但是如果相等的话,就可以共享embedding层和softmax层的权重了
        3. 参数
            1. initial_state
                 1. None: 默认会初始化为0.源代码中是`state = cell.zero_state(batch_size, dtype)`
                 2. zero_state或者dynamic_rnn的返回值
             2. sequence_length: `[batch_size]`形状. `a[i]`表示这个batch里的第i个句子的有效单词长度.(由于padding的缘故会小于embedding_size).如果你要输入三句话，且三句话的长度分别是5,10,25,那么sequence_length=`[5,10,25]`.最后的outputs在第一个句子的第5个step后全为0
        4. 返回值
            1. final_state
                1. `final_state[0]`: cell_state的结果 => 提供给forget gate, 而不是作为输入提供给input gate, cell gate和output gate
                2. `final_state[1]`: hidden_state的结果 => 提供给下一个时间步长的input gate, cell gate 和 output gate
            2. outputs
                1. time_major
                    1. False(Default):
                        1. shape: `[batch_size, num_steps, lstm_size]`
                        2. 1个结论:如果time_major=False,那么`output = tf.squeeze(outputs[:,-1,:])`<=>`final_state[1]`
                        3. 注意:使用tf.squeeze后,output的shape就`<unknown>`了,所以如果后续需要output的shape的话,可以考虑去掉tf.squeeze,直接用这个 `outputs[:,-1,:]`
                    2. True:
                        1. shape
                            + inputs 和 outputs 都为`[max_time, batch_size, lstm_size]`
                        2. 1个结论:如果time_major=True,那么:[time_major为真的处理](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/examples/dynamic-time-major.py)

                2. 后续可以考虑全连接层.比如W为`[lstm_size,10]`,b为`[10]`,然后用矩阵乘法来解决.`y_predicted=tf.nn.softmax(tf.matmul(final_state[1],weights)+bias)`
    ```
    train_times = 28
    embedding_size = 28
    lstm_size = 100
    # reshape
    inputs = tf.reshape(features,[-1,train_times,embedding_size])
    # defines the lstm_cell
    lstm_cell = tf.contrib.rnn.BasicLSTMCell(lstm_size)
    # dropout
    lstm_cell = tf.nn.rnn_cell.DropoutWrapper(lstm_cell, output_keep_prob=0.5)
    # map.  
    outputs,final_state = tf.nn.dynamic_rnn(lstm_cell,inputs,dtype=tf.float32)
    ```
    2. BasicLSTMCell.[原理图](https://github.com/orris27/orris/blob/master/python/machine-leaning/images/BasicLSTMCell.png)
        1. state_is_tuple
            + True: state是一个tuple:(c=array([[]]), h=array([[]]):其中c代表Ct的最后时间的输出，h代表Ht最后时间的输出，h是等于最后一个时间的output的.即state=(c, h)
            + False: state是一个由c和h拼接起来的张量，`state=tf.concat(1,[c,h])`。在运行时，则返回2值，一个是h，还有一个state
        2. forget_bias:forget门的bias,如果为1的话,就表示刚开始我们不能忘记上一次输入.推荐填1.0(其实就是默认)
    ```
    #tf.nn.rnn_cell.BasicLSTMCell(num_units, forget_bias, input_size, state_is_tupe=Flase, activation=tanh)
    cell = tf.nn.rnn_cell.BasicLSTMCell(num_units, forget_bias=1.0, input_size=None, state_is_tupe=Flase, activation=tanh)
    #num_units:图一中ht的维数，如果num_units=10,那么ht就是10维行向量
    #forget_bias：遗忘门的初始化偏置
    #input_size:[batch_size, max_time, size]。假设要输入一句话，这句话的长度是不固定的，max_time就代表最长的那句话是多长，size表示你打算用多长的向量代表一个word，即embedding_size（embedding_size和size的值不一定要一样）
    #图三向上指的ht称为output
    #此函数返回一个lstm_cell，即图一中的一个A
    ```
    3. 运行RNN:
        1. tf.nn.dynamic_rnn
        ```
        tf.nn.dynamic_rnn(cell, inputs, sequence_length=None, initial_state=None,dtype=None,time_major=False)
        ```
        2. 自己循环train_times次
        ```
        outputs = []
        states = initial_states
        with tf.variable_scope("RNN"):
            for time_step in range(max_time):
                if time_step>0:tf.get_variable_scope().reuse_variables()#LSTM同一曾参数共享，
                (cell_out, state) = lstm_cell(inputs[:,time_step,:], state)
                outputs.append(cell_out)
        ```
        3. call: execute LSTM cell once
            1. outputs: `[batch_size, lstm_size]`
        ```
        outputs, state = self.dec_lstm_cell.call(state=state,inputs=embedded_chars)
        ```
        
    4. Deep RNN: 不能用同一层循环体
    ```
    def create_lstm_cell(lstm_size, output_keep_prob):
        lstm_cell=tf.contrib.rnn.BasicLSTMCell(lstm_size,state_is_tuple=True)
        lstm_cell = tf.nn.rnn_cell.DropoutWrapper(lstm_cell, output_keep_prob=output_keep_prob)
        return lstm_cell

    stacked_lstm = tf.nn.rnn_cell.MultiRNNCell([create_lstm_cell(FLAGS.lstm_size, FLAGS.keep_prob) for _ in range(num_layers)])
    
    # 下面都是错误的
    #stacked_lstm = tf.nn.rnn_cell.MultiRNNCell([lstm_cell] * 2)
    #stacked_lstm = tf.nn.rnn_cell.MultiRNNCell([lstm_cell for _ in range(num_layers)])
    ```
    5. more concrete examples
        1. embedding + LSTM + softmax (一定要指定scope的intializer!!)
        ```
        class PTBModel(object):

            def __init__(self,num_steps, lstm_size, vocab_size, num_layers, share_emb_and_softmax, is_training, learning_rate, max_grad_norm,batch_size):
                ###############################################################################
                # attr
                ###############################################################################
                with tf.name_scope('placeholder'):
                    self.features = tf.placeholder(tf.int32,[None,num_steps], name='features') 
                    self.labels = tf.placeholder(tf.int32,[None,num_steps], name='labels')
                    self.keep_prob = tf.placeholder(tf.float32, name='keep_prob')
                self.num_steps = num_steps
                self.lstm_size = lstm_size
                self.vocab_size = vocab_size
                self.num_layers = num_layers
                self.share_emb_and_softmax = share_emb_and_softmax
                self.is_training = is_training
                self.learning_rate = learning_rate
                self.max_grad_norm = max_grad_norm

                ###############################################################################
                # embedding
                ###############################################################################
                embedding_size = lstm_size
                W_embedding = tf.get_variable(name="W_embedding", shape=[vocab_size,embedding_size])
                embedded_chars = tf.nn.embedding_lookup(W_embedding, self.features)

                if is_training:
                    embedded_chars = tf.nn.dropout(embedded_chars,FLAGS.embedding_keep_prob)
                ###############################################################################
                # lstm
                ###############################################################################
                def create_lstm_cell(lstm_size, output_keep_prob):
                    lstm_cell=tf.contrib.rnn.BasicLSTMCell(lstm_size,state_is_tuple=True)
                    lstm_cell = tf.nn.rnn_cell.DropoutWrapper(lstm_cell, output_keep_prob=output_keep_prob)
                    return lstm_cell

                lstm_cell = tf.nn.rnn_cell.MultiRNNCell([create_lstm_cell(lstm_size, self.keep_prob) for _ in range(num_layers)])
                outputs,final_state = tf.nn.dynamic_rnn(lstm_cell,embedded_chars,dtype=tf.float32)


                ###############################################################################
                # softmax => [batch_size, num_steps, vocab_size]
                ###############################################################################
                if share_emb_and_softmax:
                    W_softmax = tf.transpose(W_embedding)
                else:
                    W_softmax = tf.get_variable(name='W_softmax',shape=[lstm_size, vocab_size])
                b_softmax = tf.get_variable(name="b_softmax", shape=[vocab_size])
                outputs = tf.reshape(outputs, [-1, lstm_size])
                y_predicted = tf.matmul(outputs,W_softmax) + b_softmax

                ###############################################################################
                # loss + train
                ###############################################################################
                self.loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = tf.reshape(self.labels, [-1]),logits = y_predicted))

                if not is_training:
                    return 

                params = tf.trainable_variables()
                opt = tf.train.GradientDescentOptimizer(learning_rate)
                gradients = tf.gradients(self.loss * num_steps, params) # depends on your actual model
                clipped_gradients, norm = tf.clip_by_global_norm(gradients,max_grad_norm)
                self.train = opt.apply_gradients(zip(clipped_gradients, params))
        initializer=tf.random_uniform_initializer(-0.05, 0.05)
        with tf.variable_scope('model',reuse=False,initializer=initializer):
            train_model = PTBModel(FLAGS.train_num_steps, FLAGS.lstm_size, FLAGS.vocab_size, FLAGS.num_layers,FLAGS.share_emb_and_softmax, True, FLAGS.learning_rate, FLAGS.max_grad_norm,FLAGS.train_batch_size)
        # train_model.train
        ```
18. 获得形状
```
a = tf.zeros([3,2])

with tf.Session() as sess:
    print(sess.run(tf.shape(a)[0]))
```
19. 返回一个一模一样的op,并且自己是个运算op.[参考文档](https://blog.csdn.net/hu_guan_jie/article/details/78495297)
    + y本身可以是创建好的变量或者是完全新的变量
```
y = tf.identity(x)
```
20. 执行某个op前先执行特定的op:`tf.control_dependencies`
    + `y = tf.identity(x)`不能改成`y = x`,因为后者不会创建op,所以就不会走control的线路
```
# 每次输入y的值的时候,都会向上搜索.发现y是tf.identity(x)运算出来的,所以就计算tf.identity(x).但是这个op在tf.control_dependencies下面,所以要先执行它的内容,即x_plus_1的op.所以就会输出2.0,3.0,4.0,5.0,6.0

x = tf.Variable(1.0)
y = tf.Variable(0.0)
x_plus_1 = tf.assign_add(x, 1)

with tf.control_dependencies([x_plus_1]):
    y = tf.identity(x)
init = tf.initialize_all_variables()

with tf.Session() as session:
    init.run()
    for i in range(5):
        print(y.eval())
```
21. feed_dict
    1. 只在自己的sess.run内起作用,其他地方不起作用
    2. 可以临时修改变量的值,不仅仅是placeholder
```
import tensorflow as tf
y = tf.Variable(1)
b = tf.identity(y)
with tf.Session() as sess:
    tf.global_variables_initializer().run()
    print(sess.run(b,feed_dict={y:3})) #使用3 替换掉
    #tf.Variable(1)的输出结果，所以打印出来3 
    #feed_dict{y.name:3} 和上面写法等价

    print(sess.run(b))  #由于feed只在调用他的方法范围内有效，所以这个打印的结果是 1
```
22. Normalization
    + 直接使用在conv2d或者pooling层后面
    + 参数:公式参考:[tensorflow学习笔记(十二):Normalization](https://blog.csdn.net/u012436149/article/details/52985303)
        1. depth_radius: 就是公式里的n/2
        2. bias : 公式里的k
        3. input: 将conv2d或pooling 的输出输入就行了[batch_size, height, width, channels]
        4. return :前后形状不变

```
a1_pool = tf.nn.local_response_normalization(a1_pool, depth_radius=None, bias=None, alpha=None, beta=None, name=None)
```

23. tensorboard
    + 步骤
        1. 流程图
            1. 定义记录graph的writer:`tf.summary.FileWrite`
            2. 命名变量:`tf.name_scope`等
        2. 画图
            1. 定义记录graph的writer:`tf.summary.FileWrite`
            2. 添加需要记录的变量:`tf.summary.scalar`
            3. 合并第2步骤添加的变量:`tf.summary.merge_all()`
            4. 在循环的时候,计算这些变量(或者合并后的变量)的值,并且给这些变量打点:`sess.run`+`writer.add_summary`
    1. 简单的画流程图
    ```
    log_dir = 'logs/'
    with tf.Session(config=config) as sess, tf.summary.FileWriter(log_dir,sess.graph) as writer:
        pass
    ####################################################################################
    # 命令行
    ####################################################################################
    tensorboard --logdir=.
    ```
    2. 使用tensorboard中的统计图
    ```
    cross_entropy = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = labels,logits = y_predicted))
    tf.summary.scalar('loss',cross_entropy)
    
    # 合并所有要记录的点
    merged = tf.summary.merge_all()
    log_dir = 'logs/'
    with tf.Session(config=config) as sess, tf.summary.FileWriter(log_dir,sess.graph) as writer:
        #...
        for global_step in range(xxx):
            _,summary = sess.run([train, merged],feed_dict = {features:X_train,labels:y_train,keep_prob:0.5})
            # 记录loss的结果
            writer.add_summary(summary,global_step)

    ```


24. 梯度: [第三方文档](https://blog.csdn.net/u012436149/article/details/53006953)
    1. 计算梯度: 本质上, 因为我们最后的作用对象就是第二个参数,所以形状就是第二个参数形状,并且类型是float
        1. 如果xs是列表:计算ys对每个xs里的x的偏导并返回.结果的shape等于第二个参数的shape
        ```
        a = tf.constant(2.)
        b = 2 * (a**3) + 3 * a + 3
        grad = tf.gradients(b, [a, b])
        #------------------------------------------------
        # [27.0, 1.0]
        #------------------------------------------------
        ```
        2. 如果ys是列表,则计算y对x的梯度,并且对于每个x,将y的梯度简单相加作为这个x的值
        ```
        a = tf.constant(2.)
        b = 2 * (a**3) + 3 * a + 3
        grad = tf.gradients([2*a, a, b, b], a)
        #-------------------------------------------------------------
        # [57.0]
        #-------------------------------------------------------------
        ```
        4. [PTB数据集语言模型训练](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/ptb)
        5. [Seq2Seq中英翻译简单模型训练]
            1. 2个不同lstm_cell的dynamic_rnn需要用不同的scope分开
            2. forward函数定义了新的op,所以要在session前调用forward,而不是在global_variables_initializer之后甚至在epoch循环里
            3. 基本变量定义在init中,train和loss定义在forward里,并作为函数返回获得
            4. NLP中使用tf.reduce_sum(tf.nn.sparse_softmax_cross_entropy_with_logtis())获得cost,然后实际打印的损失值是`cost / 单词个数`(PTB中直接是reduce_mean,但Seq2Seq则是根据reduce_sum(mask)来获得);而训练用的loss是`cost / tf.to_float(batch_size)`


    2. 处理梯度
        + 如果函数是`y = tf.clip_by_value(x, 0, 5)`,那么`[0,5]`内返回1,而其他范围内返回0
        + 汇总一个batch的梯度进行应用. 不需要取平均值. (完整代码看30-4的CartPole问题解决的代码)
        ```
        # init grads
        grads_buf = sess.run(params)
        for index, grad in enumerate(grads_buf):
            grads_buf[index] = grad * 0

        #for each instance:
            curr_gradients = sess.run(gradients, feed_dict = {...})
            for i, curr_grads in enumerate(curr_gradients):
                grads_buf[i] += curr_grads
            
            #if num_instances % batch_size == 0:
                sess.run(train, feed_dict = {grad1: grads_buf[0], grad2: grads_buf[1]}) # apply.(In this case, only 2 vars)
                
                # reset grads
                for index, grad in enumerate(grads_buf):
                    grads_buf[index] = grad * 0
        ```
    3. 应用梯度
    4. 总结
    ```
    params = tf.trainable_variables()
    opt = tf.train.AdamOptimizer(learning_rate)
    gradients = tf.gradients(loss, params)

    max_gradient_norm = 5 # 这里的5是我随便填的
    clipped_gradients, norm = tf.clip_by_global_norm(gradients,max_gradient_norm)

    train = opt.apply_gradients(zip(clipped_gradients, params))
    ```


25. clip
    1. clip_by_value
    ```
    W = tf.Variable(tf.truncated_normal([20,2],stddev = 0.1))
    W1 = tf.clip_by_value(W,0,1)
    ```
    2. clip_by_norm
    ```
    W = tf.Variable([3,4],dtype=tf.float32)
    W1 = tf.clip_by_norm(W,5.1)
    W2 = tf.clip_by_norm(W,5)
    W3 = tf.clip_by_norm(W,4.9)
    ```
    3. clip_by_global_norm
    ```
    params = tf.trainable_variables()
    opt = tf.train.AdamOptimizer(learning_rate)
    gradients = tf.gradients(loss, params)

    max_gradient_norm = 5
    clipped_gradients, norm = tf.clip_by_global_norm(gradients,max_gradient_norm)

    train = opt.apply_gradients(zip(clipped_gradients, params))

    ```
26. name_scope & variable_scope变量名字
    1. 打印变量名字
    ```
    with tf.variable_scope('foo', reuse=tf.AUTO_REUSE):
        print('---------------------------------------------------------------------')
        W = tf.Variable([3,4],dtype=tf.float32)
        b = tf.Variable([3,4],dtype=tf.float32,name='b')
        print(W.name)
        print(b.name)
        b = tf.Variable([3,4],dtype=tf.float32,name='b')
        print(b.name)
        print('---------------------------------------------------------------------')

    #---------------------------------------------------------------------
    # foo/Variable:0
    # foo/b:0
    # foo/b_1:0
    #---------------------------------------------------------------------

    ```
    2. name_scope:只不影响get_variable
    ```
    with tf.name_scope("foo1"):
        var = tf.Variable(1.32,dtype=tf.float32)
        var_get = tf.get_variable('var_get',[1])
        add = tf.add(var,3)
        print('Variable:',var.name)
        print('get_variable:',var_get.name)
        print(add.name)
    #---------------------------------------------------------------------------------
    # Variable: foo1/Variable:0
    # get_variable: var_get:0
    # foo1/Add:0
    #---------------------------------------------------------------------------------
    ```
    3. 清除name_scope
        + 不能清除`variable_scope`
    ```
    with tf.name_scope("foo1"):
        var = tf.Variable(1.32,dtype=tf.float32)
        print(var.name)
        with tf.name_scope(None):
            var1 = tf.Variable(1.32,dtype=tf.float32)
            print(var1.name)
    #---------------------------------------------------------------------------------
    # foo1/Variable:0
    # Variable:0
    #---------------------------------------------------------------------------------
    ```
27. 框架
```
variables
...
ops
...
summary_op
...
merge_all_summarie
saver
init_op

with tf.Session() as sess:
    writer = tf.tf.train.SummaryWriter()
    sess.run(init)
    saver.restore()
    for ...:
        train
        merged_summary = sess.run(merge_all_summarie)
        writer.add_summary(merged_summary,i)
    saver.save
```

28. supervisor
```
# 定义op
a = tf.Variable(1)
b = tf.Variable(1)
c = tf.add(a,b)
update = tf.assign(a,c)

# 添加summary的内容
tf.summary.scalar("a",a)
# 合并summary
merged_summary_op = tf.summary.merge_all()
# 定义初始化变量op
init_op = tf.initialize_all_variables()
# 定义存放summary和checkpoint的目录
logdir = os.path.splitext(__file__)[0]+'/'
# 构造supervisor
sv = tf.train.Supervisor(logdir=logdir,init_op=init_op)
# 获取supervisor的saver
saver = sv.saver

gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
config=tf.ConfigProto(gpu_options=gpu_options)

# 开启视图,并自动恢复summary和 checkpoint
with sv.managed_session(config=config) as sess:
    # 循环训练
    for i in range(10):
        print(sess.run(update))
        # 保存
        if i + 1 == 10:
            # 保存summary
            merged_summary = sess.run(merged_summary_op)
            sv.summary_computed(sess, merged_summary,global_step=i)
            # 保存checkpoint
            saver.save(sess,logdir,global_step=i)
```

29. 常见值
    1. 随机tensor,用于赋值给Variable的第一个参数
    ```
    # 正态分布
    tf.random_normal(shape, mean=0.0, stddev=1.0, dtype=tf.float32, seed=None, name=None)

    # 正态分布,但如果摇出来的值离平均值超过2个标准差,句重新摇
    tf.truncated_normal(shape, mean=0.0, stddev=1.0, dtype=tf.float32, seed=None, name=None)

    # 均匀分布
    tf.random_uniform(shape, minval=0, maxval=None, dtype=tf.float32, seed=None, name=None)

    tf.random_shuffle(value, seed=None, name=None)

    tf.random_crop(value, size, seed=None, name=None)

    tf.multinomial(logits, num_samples, seed=None, name=None)

    # Gamma分布
    tf.random_gamma(shape, alpha, beta=None, dtype=tf.float32, seed=None, name=None)

    tf.set_random_seed(seed)
    ```
    2. 常量tensor
    ```
    tf.zeros(shape, dtype=tf.float32, name=None)

    tf.zeros_like(tensor, dtype=None, name=None)

    tf.ones(shape, dtype=tf.float32, name=None)

    tf.ones_like(tensor, dtype=None, name=None)

    # 产生一个全部为value的数组
    tf.fill(dims, value, name=None)

    tf.constant(value, dtype=None, shape=None, name='Const')
    ```
    3. initializer
    ```
    tf.constant_initializer(value=0, dtype=tf.float32)
    
    tf.random_normal_initializer(mean=0.0, stddev=1.0, seed=None, dtype=tf.float32)
    
    tf.truncated_normal_initializer(mean=0.0, stddev=1.0, seed=None, dtype=tf.float32)
    
    tf.random_uniform_initializer(minval=0, maxval=None, seed=None, dtype=tf.float32)
    
    tf.uniform_unit_scaling_initializer(factor=1.0, seed=None, dtype=tf.float32)
    
    tf.zeros_initializer(shape, dtype=tf.float32, partition_info=None)
    
    tf.ones_initializer(dtype=tf.float32, partition_info=None)
    
    tf.orthogonal_initializer(gain=1.0, dtype=tf.float32, seed=None)
    ```
30. 代码实战
    1. [构建TF代码](https://blog.csdn.net/u012436149/article/details/53843158)
    2. [自定义loss函数.表示商品生产数量的预测值和实际值导致的利润](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/my-loss/my-loss.py)
    3. [利用RNN实现基于前n个sinx的值判断下一个sinx的值](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/sin/sin.py)
    4. [Policy Gradients解决CartPole问题](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/reinforcement/cartpole.py)
        + CartPole问题中初始化权重的代码: `W1 = tf.get_variable("W1", shape=[env_dims, hidden_size], initializer=tf.contrib.layers.xavier_initializer())`
    5. [sklearn实现决策树](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/sklearn/decision-tree/all_electronics.py)
    6. [skleanr实现iris的knn解法](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/sklearn/knn/iris.py)
    7. [TensorFlow实现VGGNet,评测forward(inference)和backward(training)的耗时](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/vggnet)
    8. [TensorFlow实现MNIST数据集的CNN识别](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/cnn)
    9. [slim基于CNN实现MNIST数字识别](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/slim/readme.md)
31. 空

32. 设置随机数的种子
```
tf.set_random_seed(1)
#x = tf.random_normal(shape=[1,3,3,1])
```

33. train_op
    1. 方法1
    ```
    self.train = tf.contrib.layers.optimize_loss(self.loss,tf.train.get_global_step(),optimizer="Adagrad",learning    _rate=self.learning_rate)
    ```
    2. 方法2
    ```
    train = tf.train.AdamOptimizer(learning_rate).minimize(loss, global_step=global_step)
    ```

34. 同时计算多个tensor
    + tf.group返回op
    + tf.tuple返回tensor的列表
```
w = tf.Variable(1)
mul = tf.multiply(w, 2)
add = tf.add(w, 2)
group = tf.group(mul, add)
tuple = tf.tuple([mul, add])
# sess.run(group)和sess.run(tuple)都会求Tensor(add)
#Tensor(mul)的值。区别是，tf.group()返回的是`op`
#tf.tuple()返回的是list of tensor。
#这样就会导致，sess.run(tuple)的时候，会返回 Tensor(mul),Tensor(add)的值.
#而 sess.run(group)不会
```
35. 条件计算
    1. tf.cond
        + tf.cond:如果第一个参数为真,就调用第二个参数;否则调用第三个参数
        + 第一个参数不能是简单的True和Flase,必须是`tensorflow.python.framework.ops.Tensor`.这里的x和y由于都是tensor,所以返回tensor的bool
    ```
    z = tf.multiply(x, y)
    #result = tf.cond(x < y, lambda: tf.add(x, z), lambda: tf.square(y))
    result = tf.cond(tf.less(x, y), lambda: tf.add(x, z), lambda: tf.square(y))
    ```
    2. tf.case:根据字典选择函数,否则使用default里的函数
        + exclusive=True:字典里至多1个为True,否则就报错
    ```
    import tensorflow as tf

    x = tf.constant(0)
    y = tf.constant(1)
    z = tf.constant(2)

    def f1(): return tf.constant(1)
    def f2(): return tf.constant(2)
    def f3(): return tf.constant(3)

    result = tf.case({tf.less(x, y): f1, tf.less(x, z): f2},
             default=f3, exclusive=False)

    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config=tf.ConfigProto(gpu_options=gpu_options)

    with tf.Session(config=config) as sess:
        tf.global_variables_initializer().run()
        print(sess.run(result))

    ```

36. 循环计算
    1. while_loop函数原型
    ```
    #tf.while_loop(cond, body, loop_vars, shape_invariants=None, parallel_iterations=10, back_prop=True, swap_memory=False, name=None)
    ```
    2. 理解:只要`cond(*loop_vars)`是正确的,就一直执行`body(*loop_vars)`
    ```
    loop_vars = [...]
    while cond(*loop_vars):
        loop_vars = body(*loop_vars)    
    ```
    3. 使用
    ```
    import tensorflow as tf

    a = tf.get_variable("a", dtype=tf.int32, shape=[], initializer=tf.ones_initializer())
    b = tf.constant(2)
    f = tf.constant(6)

    # Definition of condition and body
    def cond(a, b, f):
        return a < 4

    def body(a, b, f):
        # do some stuff with a, b
        a = a + 1
        return a, b, f

    # Loop, 返回的tensor while 循环后的 a，b，f
    a, b, f = tf.while_loop(cond, body, [a, b, f])

    with tf.Session() as sess:
        tf.global_variables_initializer().run()
        res = sess.run([a, b, f])
        print(res)
    ```


37. 计算梯度
    + `intial_learning_rate×(decay_rate)^(global_step÷decay_steps)`:`0.1×(0.9)^(6÷10)`
    + global_step设置为learning_rate的初始值,一般情况下都是从0开始,后续每次用train来运行learning_rate都会增加step,为1,2,3
    + `staircase`: [原理图](https://pic2.zhimg.com/80/v2-8c944c984b11a929d703d60ce36e29fc_hd.jpg)
        1. True:`(global_step÷decay_steps)`转换为整数.(图中的阶梯)
            + 用途:如果decay_steps周期刚好为1个完整的训练集,那么相同的训练集就会有相同的学习率.
        2. False(默认):`(global_step÷decay_steps)`为浮点数.(图中的光滑曲线)
```
global_step = tf.Variable(0, trainable=False, name='global_step')

initial_learning_rate = 0.1 #初始学习率

learning_rate = tf.train.exponential_decay(initial_learning_rate,
                                           global_step=global_step,
                                           decay_steps=10,decay_rate=0.9)
```

38. TensorFlow的Debug
```
# 第一行： 引包
import tensorflow as tf
from tensorflow.python import debug as tf_debug

a = tf.Variable(1)
b = tf.Variable(2)
update = tf.add(a,b)

sess = tf.Session()
# 初始化的 sess 没必要加上 debug wrapper
sess.run(tf.global_variables_initializer())

# 第二行，给 session 加个 wrapper
debug_sess = tf_debug.LocalCLIDebugWrapperSession(sess=sess)
debug_sess.run(update) # 用 加了 wrapper 的 session，来代替之前的 session 做训练操作

################################################################################################
# 命令行
################################################################################################
python -m test
# python test.py # 2个都可以

```
39. FIFOQueue
    1. 创建1个队列
        1. capacity:容量
        2. dtypes:如果是`[tf.string,tf.int64]`的话,表示每个元素都是`(tf.string,tf.int64)`类型
    ```
    queue = tf.FIFOQueue(capacity=100, dtypes=[tf.string, tf.int64])
    ```
    2. 添加元素到队列
        1. 单线程入队
            1. `[b'hello', 1]`
            2. `[b'world', 2]`
            3. `[b'orris', 3]`
            4. `[b'hello', 1]`
        ```
        # enqueue_many 的写法，两个元素放在两个列表里。
        en_m = queue.enqueue_many([['hello', 'world', 'orris'], [1, 2, 3]])
        queue_init = queue.enqueue_many([[1, 10],]) # shape为[1]的时候,这样才能插入2个数据
        # enqueue 的写法
        en = queue.enqueue(['hello', 1])
        ```
        2. 多线程入队
        ```
        queue = tf.FIFOQueue(capacity=5, dtypes=tf.int32)
        enqueue_ops = [queue.enqueue_many([[i + 1],]) for i in range(5)]

        dequeue_op = queue.dequeue()
        qr = tf.train.QueueRunner(queue,enqueue_ops)
        tf.train.add_queue_runner(qr)

        with tf.Session() as sess:
            sess.run(tf.global_variables_initializer())

            coord = tf.train.Coordinator()
            threads = tf.train.start_queue_runners(sess,coord)

            for i in range(5):
                print(sess.run(dequeue_op))

            coord.request_stop()
            coord.join(threads)
        ```
    3. 出队列
    ```
    deq = queue.dequeue()
    ```
    4. 查看队列大小
    ```
    print(sess.run(queue.size()))
    ```
    5. 获得队列
    ```
    with tf.Session() as sess:
    sess.run(en_m)
    print(sess.run(deq))
    ```

40. 变量集合.GraphKeys, collection
    1. 获得变量集合
        1. <方法1> get_collection
        ```
        self.d_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='disc')
        tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES)
        ```
        2. <方法2> GraphKeys
        ```
        params = tf.global_variables()
        params = tf.trainable_variables() # 必须可训练的变量在这条语句前定义,否则获取不到
        ```
    2. 打印变量集合
        1. 打印全局变量的每个变量
        ```
        for var in tf.global_variables():
            print(var)
        ```
        2. 打印全局变量
        ```
        print(sess.run(tf.global_variables()))
        #print(sess.run(tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES)))
        ```
    3. 添加变量到指定变量集合中
    ```
    tf.add_to_collection('losses', tf.contrib.layers.l2_regularizer(scale)(W))
    ```
    4. 对整个集合进行操作
    ```
    a = tf.Variable(4)
    tf.add_to_collection('my-collection', a)
    b = tf.Variable(6)
    tf.add_to_collection('my-collection', b)

    print(sess.run(tf.add_n(tf.get_collection('my-collection')))) # 10.比如添加4和6进去,结果就是返回10
    ```


41. 将词典id表示的单词组成的文本转换成文本对应的词向量:tf.nn.embedding_lookup
```
# 示例代码
import tensorflow as tf
import numpy as np
 
sess = tf.InteractiveSession()
 
embedding = tf.Variable(np.identity(5,dtype=np.int32))
input_ids = tf.placeholder(dtype=tf.int32,shape=[None])
input_embedding = tf.nn.embedding_lookup(embedding,input_ids)
 
sess.run(tf.initialize_all_variables())
print(sess.run(embedding))
#[[1 0 0 0 0]
# [0 1 0 0 0]
# [0 0 1 0 0]
# [0 0 0 1 0]
# [0 0 0 0 1]]
print(sess.run(input_embedding,feed_dict={input_ids:[1,2,3,0,3,2,1]}))
#[[0 1 0 0 0]
# [0 0 1 0 0]
# [0 0 0 1 0]
# [1 0 0 0 0]
# [0 0 0 1 0]
# [0 0 1 0 0]
# [0 1 0 0 0]]
```
42. 拼接tensor
    + 使用`+`也可以拼接普通的列表.如`[1, 2]+[3]=>[1, 2, 3]`
    + np.concatenate和tf.concat完全相同
```
t1 = [[1, 2, 3],
      [4, 5, 6]]
t2 = [[7, 8, 9], 
      [10, 11, 12]]
tf.concat([t1, t2], 0)  # 如果axis=0,直接在最外层上下拼接
#np.concatenate([t1,t2],0)
# [[1, 2, 3],
   [4, 5, 6],
   [7, 8, 9],
   [10, 11, 12]]
tf.concat([t1, t2], 1)  # 如果axis=1,在最内层左右拼接
#np.concatenate([t1,t2],1)
# [[1, 2, 3, 7, 8, 9],
   [4, 5, 6, 10, 11, 12]]
```


43. 交换矩阵的2个维度
```
A = np.array([[1,2,3],[4,5,6]])
x = tf.transpose(A, [1,0])
 
B = np.array([[[1,2,3],[4,5,6]]])
y = tf.transpose(B, [2,1,0]) # 交换1st维度和3rd维度
with tf.Session() as sess:
    print(A)
    print(sess.run(x))
    print(B)
    print(sess.run(y))

```
44. split
```
import tensorflow as tf;
import numpy as np;
 
A = [[1,2,3],[4,5,6]]
x = tf.split(1, 3, A)
 
with tf.Session() as sess:
    c = sess.run(x)
    for ele in c:
        print ele

```


45. flags
    1. 定义flags并使用
    ```
    import sys

    tf.flags.DEFINE_float("learning_rate", 0.01, "learning rate")
    tf.flags.DEFINE_float("regularization_scale", 0.0001, "scale of regularization")
    
    tf.flags.DEFINE_integer("num_filters", 128, "num of filters")
    tf.flags.DEFINE_integer("num_classes", 2, "num of classes")
    tf.flags.DEFINE_integer("num_epochs", 200, "num of epochs")
    tf.flags.DEFINE_integer("embedding_size", 128, "embedding size")
    tf.flags.DEFINE_integer("batch_size", 64, "batch size")
    tf.flags.DEFINE_integer("save_every", 100, "save_every")
    tf.flags.DEFINE_integer("log_every", 100, "log_every")


    tf.flags.DEFINE_string("filter_sizes", "3,4,5", "Comma-separated filter sizes (default: '3,4,5')")
    
    tf.flags.DEFINE_boolean("log_device_placement", False, "Log placement of ops on devices")
    tf.flags.DEFINE_boolean("allow_soft_placement", True, "Allow device soft device placement")
    tf.flags.DEFINE_boolean("shuffle", False, "The batch data should shuffled or not")

    FLAGS = tf.flags.FLAGS
    FLAGS(sys.argv) # 启用flags
    print(FLAGS.dev_sample_percentage)
    print(FLAGS.filter_sizes)
    print(list(map(int, FLAGS.filter_sizes.split(','))))
    ```
    2. 打印flags的内容
    ```
    for attr, value in sorted(FLAGS.__flags.items()):
        print("{}={}".format(attr.upper(), value))
    ```
46. 转换`[一句话,一句话,..]`为`[[单词1的id,单词2的id,...],[单词1的id,单词2的id,...], ...]`字典id的列表,并且自动填充
```
import numpy as np
from tensorflow.contrib import learn


X_text = ['I have cute balls', 'He loves balls']

max_document_length = max([len(x.split(" ")) for x in X_text])
vocab_processor = learn.preprocessing.VocabularyProcessor(max_document_length)
X = np.array(list(vocab_processor.fit_transform(X_text)))

print(X)
#---------------------------------------------------------------------------------
# [[1 2 3 4]
#  [5 6 4 0]]
#---------------------------------------------------------------------------------
print(len(vocab_processor.vocabulary_)) # 所有不同单词的个数+1,包括一个0用来填充,不表示任何单词
#---------------------------------------------------------------------------------
# 7
#---------------------------------------------------------------------------------
```

47. 切割训练集和测试集
```
# FLAGS.dev_sample_percentage = 0.1
# 比如len(y)为10662,那么dev_sample_index就是-1066
dev_sample_index = -1 * int(FLAGS.dev_sample_percentage * float(len(y)))
X_train, X_dev = X[:dev_sample_index], X[dev_sample_index:]
y_train, y_dev = y[:dev_sample_index], y[dev_sample_index:]
print("Vocabulary Size: {:d}".format(len(vocab_processor.vocabulary_)))
print("Train/Dev split: {:d}/{:d}".format(len(y_train), len(y_dev)))
```


48. 转换词典id的二维矩阵(`[[单词1的id,单词2的id,...],[单词1的id,单词2的id,...], ...]`)为embedding的三维矩阵(转换每个单词本身为一个一维的矩阵)
    + W:`[不同单词个数, embedding_size]`
    + input_x:`[batch_size, 单句话的最大单词个数或time_step]`:必须是tf.int32类型!!!
    + 输出:`[batch_size, 单句话的最大单词个数或time_step, embedding_size]`:(一个单词`shape=[]`=>一个embedding矩阵`shape=[embedding_size]`)
```
print(x) # 必须是tf.int32类型
# [[1 2 3 4]
#  [5 6 4 0]]
print(len(vocab_processor.vocabulary_))
# 7

vocab_size = len(vocab_processor.vocabulary_)
embedding_size = 128
input_x = x

W = tf.Variable(
    tf.random_uniform([vocab_size, embedding_size], -1.0, 1.0),
    name="W")
embedded_chars = tf.nn.embedding_lookup(W, input_x) # [7, 128], [2, 4]

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    print(sess.run(embedded_chars)) # shape为[2, 4, 128]


#---------------------------------------------------------------------------
# [[[-0.98232985 -0.52155375  0.5751116  ... -0.6340177  -0.84810543
#    0.17815971]
#  [-0.7253678  -0.8297632  -0.20531392 ...  0.25289154  0.36266613
#    0.75570035]
#  [ 0.91721225 -0.9421947  -0.9394753  ...  0.76013064 -0.8321254
#    0.3380263 ]
#  [-0.9852202  -0.60237265 -0.7277608  ...  0.28419566  0.1563003
#   -0.8882804 ]]

# [[-0.9926841  -0.14970827  0.01962733 ...  0.87411165 -0.87654376
#   -0.2559495 ]
#  [ 0.66079235 -0.46869588  0.4234197  ...  0.38469195 -0.7251394
#    0.04596043]
#  [-0.9852202  -0.60237265 -0.7277608  ...  0.28419566  0.1563003
#   -0.8882804 ]
#  [ 0.6936033   0.97441864  0.5263803  ...  0.3178606   0.4688716
#    0.2274158 ]]]
#---------------------------------------------------------------------------
```

49. 增加维度
+ `np.expand_dims`也是同理
+ `tf.expand_dims(a, axis)`:指添加的1在什么位置
    + 比如(2,3)
        + axis=0 => (1,2,3)
        + axis=1 => (2,1,3)
        + axis=-1或2 => (2,3,1)
```
embedded_chars_expanded = tf.expand_dims(embedded_chars, -1)
```

50. 使用global_step:
    1. 使用global_step
        + `global_step`会在指定该global_step的op执行时会自动+1
    ```
    with tf.Session() as sess:
        global_step = tf.Variable(0, trainable=False, name="global_step")
        optimizer = tf.train.AdamOptimizer(1e-3)
        grads_and_vars = optimizer.compute_gradients(cnn.loss) # 这里就是loss的op
        train_op = optimizer.apply_gradients(grads_and_vars, global_step=global_step)

        #....

        current_step = tf.train.global_step(sess, global_step) # 如果直接用sess.run(global_step)的话,和这个current_step是相等的值
    ```
    2. 获得global_step
        1. <方法1>
        ```
        global_step = ckpt_latest.split('/')[-1].split('-')[-1]
        ```
        2. <方法2> 调用函数寻找名字为"global_step:0"的函数
        ```
        global_step = tf.train.get_global_step()
        ```
51. 创建graph,并设置为默认的graph
```
# 执行和tensor无关的操作,比如加载数据集

with tf.Graph().as_default():
    # 定义各种tensor(比如说定义CNN的模型)
    sess = tf.Session(config=session_conf)
    with sess.as_default():
        # 执行session的内容
```

52. [CNN解决文本分类问题](https://github.com/orris27/orris/blob/master/python/machine-leaning/images/cnn%E8%A7%A3%E5%86%B3%E6%96%87%E6%9C%AC%E5%88%86%E7%B1%BB.png)
    1. [别人的代码](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/cnn-solve-classification/others-codes)
    2. [我的代码](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/cnn-solve-classification/my-codes)


53. tf.stack 和 tf.unstack
    1. tf.stack:拼接一组矩阵
        + 0维感觉就是直接第一个参数
        + 1维的话,就是纵向拼接
    ```
    x = tf.constant([1, 4])
    y = tf.constant([2, 5])
    z = tf.constant([3, 6])
    tf.stack([x, y, z])  
    # [[1, 4], [2, 5], [3, 6]] (Pack along first dim.)
    tf.stack([x, y, z], axis=1)  
    # [[1, 2, 3], 
    #  [4, 5, 6]]
    ```
    2. tf.unstack:拆分一组矩阵.和tf.stack是逆过程
    ```
    sess.run(a)
    #---------------------------------------------------------------------------
    # array([[1, 2, 3],
    #   [4, 5, 6]], dtype=int32)
    #---------------------------------------------------------------------------

    sess.run(tf.unstack(a,axis=0))
    #---------------------------------------------------------------------------
    # [array([1, 2, 3], dtype=int32), array([4, 5, 6], dtype=int32)]
    #---------------------------------------------------------------------------
    
    
    sess.run(tf.unstack(a,axis=1))
    #---------------------------------------------------------------------------
    # [array([1, 4], dtype=int32),
    #  array([2, 5], dtype=int32),
    #  array([3, 6], dtype=int32)]
    #---------------------------------------------------------------------------

    ```
    3. ta.stack(TensorArray的stack):将TensorArray的元素叠起来当做1个Tensor输出
        + 0维感觉就是直接第一个参数
        + 1维的话,就是纵向拼接
    ```
    x = tf.constant([1, 4])
    y = tf.constant([2, 5])
    z = tf.constant([3, 6])
    tf.stack([x, y, z])  
    # [[1, 4], [2, 5], [3, 6]] (Pack along first dim.)
    tf.stack([x, y, z], axis=1)  
    # [[1, 2, 3], 
    #  [4, 5, 6]]
    ```
    4. ta.unstack(Tensor的unstack):输入Tensor,输出一个新的TensorArray对象.和tf.stack是逆过程
    5. ta.write(index, value, name=None) 指定index位置写入Tensor
    6. ta.read(index, name=None) 读取指定index位置的Tensor
    7. 定义TensorArray:主要是用来做while_loop等操作
    ```
    from tensorflow.python.ops import tensor_array_ops, control_flow_ops
    sub_feature = tensor_array_ops.TensorArray(dtype=tf.float32, size=self.sequence_length/self.step_size,
                                               dynamic_size=False, infer_shape=True, clear_after_read=False)
    ```
54. tf.squeeze:删除维度为1的维度
```
tf.squeeze(tf.zeros([1,2,3,4,1,5]))
#<tf.Tensor 'Squeeze:0' shape=(2, 3, 4, 5) dtype=float32>
```



55. tf.nn.l2_normalize(x, dim, epsilon=1e-12, name=None):计算矩阵的l2范化结果
    + 计算行/列的作为一维向量的模,然后矩阵里的每个元素除以这个模就可以了.最后输出的形状与原来的形状是相同的
    + 具体的计算过程参考[这个博客](https://blog.csdn.net/abiggg/article/details/79368982)
    1. 按列计算
    ```
    import tensorflow as tf
    input_data = tf.constant([[1.0,2,3],[4.0,5,6],[7.0,8,9]])

    output = tf.nn.l2_normalize(input_data, dim = 0)
    with tf.Session() as sess:
    print sess.run(input_data)
    print sess.run(output)
    #----------------------------------------------------------
    # [[1. 2. 3.]
    # [4. 5. 6.]
    # [7. 8. 9.]]
    # [[0.12309149 0.20739034 0.26726127]
    # [0.49236596 0.51847583 0.53452253]
    # [0.86164045 0.82956135 0.80178374]]
    #----------------------------------------------------------
    ```
    2. 按行计算
    ```
    import tensorflow as tf
    input_data = tf.constant([[1.0,2,3],[4.0,5,6],[7.0,8,9]])

    output = tf.nn.l2_normalize(input_data, dim = 1)
    with tf.Session() as sess:
    print sess.run(input_data)
    print sess.run(output)
    #----------------------------------------------------------
    # [[1. 2. 3.]
    # [4. 5. 6.]
    # [7. 8. 9.]]
    # [[0.26726124 0.5345225 0.8017837 ]
    # [0.45584232 0.5698029 0.6837635 ]
    # [0.5025707 0.5743665 0.64616233]]
    #----------------------------------------------------------
    ```


56. [训练学习一元线性函数的代码](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/linear/linear.py)
    1. `step 99:loss=nan    k=[nan] b=[nan]`
        1. 不能使用tf自带的`tf.nn.sigmoid_cross_entropy_with_logits`
        2. 使用`tf.square`.因为直接对`y_predicted-labels`求平均值的话,得到的结果可能是负数
        3. k和b接近真实的k和b.因为如果远离太多的话,得到的loss就会很大,有可能就溢出
        4. X_train的batch_size小点.理由同上
        4. 学习率设置小一点.理由同上.如果学习率太大,一下子k和b就会偏离真实的k和b很多
    2. 初始化`[64,1]`的numpy矩阵
        1. <方法1> 构造[64,]的矩阵,然后扩展这个矩阵
        ```
        X_train = np.linspace(-10, 10, 64) + np.random.random(64) * 0.01
        X_train = np.expand_dims(X_train, 1)
        ```
        2. <方法2> 使用numpy的newaxis直接构造`[64,1]`矩阵
        ```
        X_train = np.linspace(-10, 10, 64)[:,np.newaxis] + np.random.random([64,1]) * 0.01
        ```
    3. 损失函数
        1. 均方误差
            + 条件
                1. y_predicted.shape == labels.shape
                2. shape=`[batch_size,num_classes]`或者`[batch_size]`等都可以
        ```
        loss = tf.reduce_mean(tf.square(y_predicted - labels)) # 适用于回归等
        ```

57. 文件操作
    + 接口:不需要sess.run
        1. `tf.gfile.FastGFile`
        2. 打开后的文件:`<class 'tensorflow.python.platform.gfile.FastGFile'>`
            1. `f.read()`: bytes
            2. `f.readlines()`
    1. 打开文件,并打印每一行
        1. 方法1
        ```
        with tf.gfile.FastGFile('test.py') as f:
            print(f.read())
        ```
        2. 方法2
        ```
        with tf.gfile.FastGFile('test.py') as f:
        for line in f.readlines():
            sys.stdout.write(line)
            sys.stdout.flush()
        ```
    2. 图像
        1. 读取图片 和 写入图片. [代码和图形类型转化的示意图](https://github.com/orris27/orris/blob/master/python/machine-leaning/images/tf-image-type.png)
        ```
        import tensorflow as tf
        import matplotlib.pyplot as plt

        image_raw = tf.gfile.FastGFile('/home/orris/Pictures/1.jpeg','rb').read()

        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config = tf.ConfigProto(gpu_options=gpu_options)

        with tf.Session(config=config) as sess:
            image = tf.image.decode_jpeg(image_raw)
            print(image.eval())

            plt.figure()
            plt.imshow(image.eval())
            plt.show()


            encoded_image = tf.image.encode_jpeg(image)

            with tf.gfile.FastGFile('output.jpeg', 'wb') as f:
                f.write(encoded_image.eval())
        ```
58. 数据预处理
    1. 给定一个一维列表,按`[batch_size]`的大小输出
        1. <方法1> tf.train.batch
            1. tf.train.slice_input_producer:
                1. 参数
                    1. tensor_list
                    2. num_epochs: 决定遍历多少轮整个数据.遍历结束后就会返回`tf.errors.OutOfRangeError`
                        + 注意:需要在创建`slice_input_producer`后初始化local变量!!
                        + None:表示一直循环
                    3. shuffle
                        1. True: 每个batch都会是洗牌后的结果,所以可能会重复.但是如果规定了num_epochs后,每个元素必然会出现num_epochs
                        2. False: 完全按照顺序输出.比如`[0,1,2,3,4]`两个两个输出就是`[0,1]`,`[2,3]`,`[4,0]`,...
                2. 返回值:input_queue可以理解为和参数的第一个元素相同,表面上操作的是一个元素,实际上会对整个列表产生影响
            2. [使用tf.train.batch实现按批输出图片的程序](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/batch-images/batch-images.py)
                + `tf.image.resize_image_with_crop_or_pad`:图片太大,选中间部分;图片太小,周围填充0(黑色).
            3. [使用tf.train.batch实现按批输出图片的程序2,解决yield下threads无法join的](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/batch-images/batch-images2.py)
        ```
        import tensorflow as tf

        def next_batch(sess, l, elm_type, shuffle, num_epochs, batch_size, num_threads,capacity):
            # 转换普通的list为tf能识别的类型
            l = tf.cast(l,elm_type)

            # 创建队列
            input_queue = tf.train.slice_input_producer([l], shuffle=shuffle,num_epochs=num_epochs) # if there are 2 elms in the 1st param,the next sentence uses '[1]' to get that param
            # 获取队列的第一个元素
            l = input_queue[0]

            #####################################################################################################
            # 这里可以将l当做一个元素进行操作,最终会影响整个队列.
            # 比如如果是image的filename的话,可以用读内容,decode,标准化等
            #
            # 比如
            # image_contents = tf.read_file(input_queue[0])
            # image = tf.image.decode_jpeg(image_contents,channels=3) # image={shape:(?, ?, 3),dtype:uint8}
            # image = tf.image.resize_image_with_crop_or_pad(image,image_h,image_w) # image={shape:(208, 208, 3),dtype:uint8}
            # #image = tf.image.per_image_standardization(image) # image={shape:(208, 208, 3),dtype:float32}
            # #image = tf.cast(image,tf.float32)
            #####################################################################################################


            # 获取batch对象.l_batch={shape:(batch_size,xxx)}
            l_batch = tf.train.batch([l],batch_size=batch_size,num_threads=num_threads,capacity=capacity)

            sess.run(tf.local_variables_initializer()) # initialize num_epochs

            # 启动线程
            coord = tf.train.Coordinator()
            threads = tf.train.start_queue_runners(coord=coord)
            try:
                while not coord.should_stop():
                    # get & process the batch
                    yield sess.run([l_batch])

            except tf.errors.OutOfRangeError:
                print("done!")
            finally:
                coord.request_stop()

            coord.join(threads)

        # input
        l = [i for i in range(64)]

        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config = tf.ConfigProto(gpu_options=gpu_options)

        with tf.Session(config=config) as sess:
            for l_batch in next_batch(sess, l, elm_type=tf.int32, shuffle=True, num_epochs=2, batch_size=2, num_threads=64, capacity=256):
                print(l_batch)
                #--------------------------------------------------------
                # [array([11,  6], dtype=int32)]
                # [array([27, 39], dtype=int32)]
                # [array([55, 61], dtype=int32)]
                # [array([47, 57], dtype=int32)]
                # [array([31,  3], dtype=int32)]
                # ...
                #--------------------------------------------------------
        ```
        2. <方法2> Dataset的API
            1. 循环输出.
            ```
            ds = tf.data.Dataset.from_tensor_slices((X_train,y_train))
            ds = ds.repeat().shuffle(1000).batch(FLAGS.batch_size)
            X, y = ds.make_one_shot_iterator().get_next()
            ######################################################################
            # X,y 为tensor,不能作为feed_dict的值使用.但可以直接作为features和labels使用
            ######################################################################
            ```
            2. 全部输出,不循环.batch_size=1.(全都循环完了如果还`X.eval()`就报错)
            ```
            ds = tf.data.Dataset.from_tensor_slices((X_test,y_test))
            ds = ds.batch(1)
            X, y = ds.make_one_shot_iterator().get_next()
            ```

    2. words => indices, dictionary, reverse_dictionary. 完整代码参考[skip-gram代码](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/word2vec.py/skip-gram.py)
        + `['I', 'am', 'a', 'boy']` => [23, 10, 2, 33], {'I': 23, 'am': 10,..}, {23: 'I', 10:'am', ...}
    ```
    def words_to_indices(words):
        # count words and filter the most common words
        count = collections.Counter(words).most_common(vocab_size - 1)
        # add <unk>
        freq = [['<unk>', -1]]
        freq.extend(count)

        dictionary = dict()
        # calc the dictionary
        for i in range(vocab_size):
            dictionary[freq[i][0]] = i

        # calc the reverse_dictionary
        reverse_dictionary = dict(zip(dictionary.values(), dictionary.keys()))

        # define a indices
        indices = []
        # create indices
        for word in words:
            if word in dictionary:
                indices.append(dictionary[word])
            else:
                indices.append(0)

        return indices, dictionary, reverse_dictionary

    indices, dictionary, reverse_dictionary = words_to_indices(words)
    del words
    ```
    3. Skip-Gram: indices => X, y. 完整代码参考[skip-gram代码](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/word2vec.py/skip-gram.py)
        + `[23, 10, 2, 33]` => `[10, 10, 2, 2 ,...]`, `[[23], [2], [10], [33], ...]`
    ```
    data_index = 0

    # define generate_batch function
    def generate_batch(indices, batch_size):

        global data_index
        len_indices = len(indices)

        X = np.ndarray(shape=(batch_size,), dtype=np.int32)
        y = np.ndarray(shape=(batch_size, 1), dtype=np.int32)

        dq_len = skip_window * 2 + 1
        # create a deque with length of "skip_window * 2 + 1"
        dq = collections.deque(maxlen=dq_len)
        # init dq
        for _ in range(dq_len):
            dq.append(indices[data_index])
            data_index = (data_index + 1) % len_indices

        sub = 0
        for _ in range(batch_size // num_skips):

            # obtain the feature: dq[skip_window]
            # randomly choose num_skips elms from 0 to skip_window*2+1(not included) except skip_window
            # 1. create a list <choices>:[0, skip_window*2+1)
            choices = [i for i in range(skip_window * 2 + 1)]
            # 2. remove skip_window
            choices.pop(skip_window)
            # 3. while len(choices) != num_skips:
            while len(choices) != num_skips:
                # 1. randomly determine a elm: [0, len(choices))
                choice = np.random.randint(0, len(choices))
                # 2. remove it
                choices.pop(choice)

            # obtain the label
            for choice in choices:
                X[sub] = dq[skip_window]
                y[sub][0] = dq[choice]
                sub += 1

            # move the dq
            dq.append(indices[data_index])
            data_index = (data_index + 1) % len_indices

        return X, y
        
    # ...   
    for step in range(num_epochs):
        # get X,y from generate_batch
        X, y = generate_batch(indices, batch_size)
        _, loss1 = sess.run([train, loss], feed_dict = {features:X, labels:y})
        # ...
    ```
59. 验证
    1. 使用tf.nn.in_top_k
        1. 参数
            1. predictions:`[batch_size,num_classes]`
            2. targets:`[batch_size]`.dtype=int.表示正确的分类是第几类.
            3. k: Number of top elements to look at for computing precision.
    ```
    import tensorflow as tf;
 
    A = [[0.8,0.6,0.3], [0.1,0.6,0.4]] # A的结果分类应该是0,1.
    B = [1, 1]
    out = tf.nn.in_top_k(A, B, 1)
    with tf.Session() as sess:
        sess.run(tf.initialize_all_variables())
        print sess.run(out)
    # 输出[False,True]
    ```
    2. 实战使用tf.nn.in_top_k判断准确率
        + 注意:如果是在feed训练集的情况下的话,算出来的是batch_size里的结果
        + 其他用途:可以用来预测image的标签.将image作为batch_size为1喂食进模型中,然后这个sess.run返回的accuracy得到的就是预测的准确值
    ```
    with tf.name_scope('accuracy') as scope:
        correct = tf.nn.in_top_k(y_predicted,labels,1)
        accuracy = tf.reduce_mean(tf.cast(correct,tf.float32))
    ```

60. Graph
    1. 使用建议: 构建图的时候,使用下面的代码,这样看起来会更清晰吧
    ```
    graph = tf.Graph()
    with graph.as_default():
        pass
    ```
    2. 获得某个变量所在的Graph,并且判断是不是当前默认的Graph
    ```
    print(v1.graph is tf.get_default_graph())
    ```
    3. 创建新的视图,并在该视图下计算.(不同Graph的张量和运算都不共享)
        + 在指定的graph对象的as_default下做的定义,都写在这个graph对象里面.
        + tf.Session可以指定使用的计算图
        + `with tf.variable_scope('',reuse=True):`可以在默认的variable_scope里启动reuse
    ```
    import tensorflow as tf

    g1 = tf.Graph()
    with g1.as_default():
        v = tf.get_variable("v",shape=[1],initializer=tf.zeros_initializer)

    g2 = tf.Graph()
    with g2.as_default():
        v = tf.get_variable("v",shape=[1],initializer=tf.ones_initializer)

    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config=tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config,graph=g1) as sess:
        sess.run(tf.global_variables_initializer())
        with tf.variable_scope('',reuse=True):
            print(sess.run(tf.get_variable("v")))

    with tf.Session(config=config,graph=g2) as sess:
        sess.run(tf.global_variables_initializer())
        with tf.variable_scope('',reuse=True):
            print(sess.run(tf.get_variable("v")))
    ```
    4. 导出元图为json格式
    ```
    import tensorflow as tf
    import numpy as np

    a = tf.Variable(1.0,name='a')
    b = tf.Variable(2.0,name='b')

    add = tf.add(a,b,name='add')

    saver = tf.train.Saver()

    saver.export_meta_graph("ckpt/.meta.json",as_text=True)
    ```

61. eval:如果当前有默认的session的话,就会启用该session;或者使用参数里的session;否则报错
    1. 创建默认的session
        1. 方法1
        ```
        with tf.Session() as sess:
            print(var.eval())
        ```
        2. 方法2
        ```
        with tf.Session().as_default() as sess:
            print(var.eval())
        ```
        3. 方法3
        ```
        sess.as_default()
        print(var.eval())
        ```        
        4. 方法4
        ```
        print(var.eval(session=sess))
        ```

62. `initialized_value()`:获得初始值,可以用来将自己的初始值设置成和其他变量一样
```
import tensorflow as tf

a = tf.Variable(3.14)
b = tf.Variable(a.initialized_value() * 2)

    sess.run(tf.global_variables_initializer())
    print(sess.run(b)) # 6.28
```
63. `var.initializer`:可以单独初始化var
```
import tensorflow as tf

a = tf.Variable(3.14)
b = tf.Variable(a.initialized_value() * 2)

with tf.Session() as sess:
    sess.run(b.initializer)
    print(sess.run(b))
```
63. `tf.matmul`和`*`:前者矩阵乘法,后者点积
64. 更改tensor的形状
```
a = tf.Variable(tf.random_normal([2,3]))
b = tf.Variable(tf.random_normal([2,2]))

update = tf.assign(a,b,validate_shape=False) # a的形状还是[2,3],但输出就是[2,2]形状,但又不能和[2,2]进行相加等
```
65. 条件判断
    1. `tf.greater`:基于元素来返回`v1>v2`
    ```
    a = tf.constant([1.,2.,3.,4.])
    b = tf.constant([4.,3.,2.,1.])

    result = tf.greater(a,b) # [False False  True  True]
    ```
    2. `tf.where`:根据第一个参数的真值来返回第二个参数或者第三个参数(基于元素级别)
    ```
    a = tf.constant([1.,2.,3.,4.])
    b = tf.constant([4.,3.,2.,1.])

    result = tf.where(tf.greater(a,b),a, b) # [4. 3. 3. 4.]
    ```
66. 正则化
    + 使用:`优化对象loss=损失函数+正则化系数*正则化函数(W)`.正则化主要用来降低模型复杂度从而防止过拟合.而模型复杂度一般只由权重(W)决定
        1. 'losses'变量集合:添加所有W和实际损失函数到该集合中,对整个集合的变量进行加法操作得到优化对象
        ```
        #################################################################################################
        # 将权重和损失函数加入到集合"losses"中,然后将集合相加得到最后要优化的对象
        #################################################################################################
        W = tf.get_variable("W",[inputs.get_shape()[1],output_dim],initializer=norm)
        tf.add_to_collection('losses', tf.contrib.layers.l2_regularizer(.5)(W)) # 一般scale取0.0001
        cross_entropy = tf.reduce_mean(tf.where(tf.greater(y_predicted,labels),1*(y_predicted-labels),10*(labels-y_predicted)))
        tf.add_to_collection('losses',cross_entropy)
        loss = tf.add_n(tf.get_collection('losses'))
        ```
    1. l1正则化:`scale*(|w0,0|+|w0,1|+|w1,0|+|w1,1|)`(`||`为绝对值函数).`tf.contrib.layers.l1_regularizer`
        1. 计算公式不可导
        2. 计算结果比较稀疏(元素为0的多)=>用来特征值筛选
    ```
    W = tf.constant([[1.,-2.],[-3.,4.]])
    sess.run(tf.contrib.layers.l1_regularizer(.5)(W)) # 7.5 = 0.5 * (1 + |-2| + |-3| + |4|) / 2
    ```
    2. l2正则化:`scale*(|w0,0|^2+|w0,1|^2+|w1,0|^2+|w1,1|^2)/2`.`tf.contrib.layers.l2_regularizer`
        1. 计算公式可导
        2. 计算结果比较不稀疏(元素为0的少).(因为l2正则化不会将0.001这样很小的数字继续调整为0)
    ```
    W = tf.constant([[1.,-2.],[-3.,4.]])
    sess.run(tf.contrib.layers.l2_regularizer(.5)(W)) # 7.5 = 0.5 * (1^2 + |-2|^2 + |-3|^2 + |4|^2) / 2

    #loss = tf.reduce_mean(tf.where(tf.greater(y_predicted,labels),1*(y_predicted-labels),10*(labels-y_predicted))) + tf.contrib.layers.l2_regularizer(.5)(W)
    ```
67. 滑动平均值:
    + tf.train.ExponentialMovingAverage
        1. `shadow_variable = decay * shadow_variable + (1 - decay) * variable`
            1. shadow_variable:值=`sess.run(ema.average([v1]))`
                + shadow_variable会作为1个global_variable存在.命名格式为"v/ExponentialMovingAverage:0",是不可训练的
            2. variable:值=`sess.run([v1])`
            3. decay:值=`min{decay, (1 + num_updates) / (10 + num_updates)}`
        2. `__init__`
            1. 参数
                1. decay:一般取接近1的数字,比如0.99
                2. num_updates
    + 用途:训练时使用原来的变量(W和b),但验证或者测试的时候,使用滑动平均值
    1. 定义1个变量,查看不同step下这个变量的滑动平均值
    ```
    import tensorflow as tf
    import numpy as np

    v = tf.Variable(0.0)
    step = tf.Variable(0)

    ema = tf.train.ExponentialMovingAverage(decay=0.99,num_updates=step)
    maintain_averages = ema.apply([v]) # 执行apply会将variable通过上述公式更新到shadow_variable

    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.4)
    config=tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())
        print('variable={0}\tshadow_variable={1}'.format(sess.run(v),sess.run(ema.average(v))))
        # 0,0 初始情况shadow_variable == variable

        sess.run(tf.assign(v,5))
        sess.run(maintain_averages)
        print('variable={0}\tshadow_variable={1}'.format(sess.run(v),sess.run(ema.average(v))))
        # 5.0, 4.5 
        # shadow_variable = 0.1 * 0 + (1 - 0.1) * 5  (因为decay = min(0.99,(1+0)/(10+0)))


        sess.run(tf.assign(v,10))
        sess.run(tf.assign(step,1000))
        sess.run(maintain_averages)
        print('variable={0}\tshadow_variable={1}'.format(sess.run(v),sess.run(ema.average(v))))
        # 10.0, 4.555 
        # shadow_variable = 0.99 * 4.5 + (1 - 0.99) * 10 (因为decay = min(0.99, (1 + 10000) / (10 + 10000)))
    ```
    2. 保存变量值和其滑动平均值,但恢复视图时只恢复该变量对应的滑动平均值
        + 技巧
            1. Saver的词典重命名
            2. 影子变量的命名格式或者`tf.train.ExponentialMovingAverage`提供的`variables_to_restore`方法:返回`{"影子变量名":实际变量,}`的词典
    ```
    #####################################################################################################
    # Save
    #####################################################################################################

    import tensorflow as tf
    import numpy as np

    v = tf.Variable(0.0,name='v')

    ema = tf.train.ExponentialMovingAverage(decay=0.99)
    maintain_averages = ema.apply([v]) # 执行apply会将variable通过上述公式更新到shadow_variable

    saver = tf.train.Saver()

    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.4)
    config=tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())

        sess.run(tf.assign(v,10))
        sess.run(maintain_averages)
        print('variable={0}\tshadow_variable={1}'.format(sess.run(v),sess.run(ema.average(v))))
        # variable=10.0 shadow_variable=0.09999990463256836
        saver.save(sess,'ckpt/')


    #####################################################################################################
    # Restore
    #####################################################################################################
    import tensorflow as tf
    import sys
    import os

    v = tf.Variable(3.14,name='v')

    saver = tf.train.Saver({'v/ExponentialMovingAverage':v})
    #ema = tf.train.ExponentialMovingAverage(decay=0.99)
    #saver = tf.train.Saver(ema.variables_to_restore())

    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config=tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())
        saver.restore(sess,"ckpt/")
        print(sess.run(v)) # 0.099999905
    ```


68. 保存必要的计算节点,而将其他变量转换成常数
    1. 保存
        1. 获取当前的计算图
        2. 转换当前计算图内的某个计算节点
        3. 写入转换结果到保存的文件里
    ```
    import tensorflow as tf
    from tensorflow.python.framework import graph_util

    a = tf.Variable(1.0,name='a')
    b = tf.Variable(2.0,name='b')

    result = tf.add(a,b, name='add')

    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.4)
    config=tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())

        print(result.name)

        graph_def = tf.get_default_graph().as_graph_def()

        output_graph_def = graph_util.convert_variables_to_constants(sess,graph_def,['add'])

        with tf.gfile.GFile("ckpt/model.pb","wb") as f:
            f.write(output_graph_def.SerializeToString())
    ```
    2. 恢复
        1. 构造GraphDef对象
        2. 解析文件内容到GraphDef对象中
        3. 导入GraphDef对象内的某个计算节点到当前的计算图中,并设置对应的张量
    ```
    import tensorflow as tf

    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config=tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())

        with tf.gfile.FastGFile('ckpt/model.pb','rb') as f:
            graph_def = tf.GraphDef()
            graph_def.ParseFromString(f.read())

        result = tf.import_graph_def(graph_def,return_elements=["add:0"])

        print(sess.run(result))

    ```

69. 查看`data-*****-of-*****`保存的所有变量和对应的维度
```
reader = tf.train.NewCheckpointReader('ckpt/')

variables_dict = reader.get_variable_to_shape_map() # "{变量名:变量的维度,}" 的词典

for variable_name in variables_dict:
    print(variable_name,variables_dict[variable_name])

```

70. tfrecord
    1. 保存图像
    ```
    import tensorflow as tf
    from PIL import Image
    import os 
    import numpy as np

    data_path = '../dogs-cats/dataset/train/'
    output_filename = 'images.tfrecords'

    def _int64_feature(value):
        return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))

    def _bytes_feature(value):
        return tf.train.Feature(bytes_list=tf.train.BytesList(value=[value]))

    count = 0

    # create a writer to record this big event!
    writer = tf.python_io.TFRecordWriter(output_filename)

    # for file in data_path:
    list_dirs = os.walk(data_path) 
    for root, dirs, files in list_dirs: 
        for f in files: 
            if f.endswith('jpg') and count < 1000: # 这里的count是为了控制图片数量,不然一个tfrecord文件太大...
                # obtain image_raw
                img = Image.open(os.path.join(root,f))
                image_raw = img.tobytes()



                # get label
                label = f.split('.')[0]
                if label == 'dog':
                    label = 1
                else:
                    label = 0

                example = tf.train.Example(features=tf.train.Features(feature={
                    'height':_int64_feature(img.height),
                    'width':_int64_feature(img.width),
                    'channels':_int64_feature(len(img.getbands())),
                    'label':_int64_feature(label),
                    'image_raw':_bytes_feature(image_raw)}))
                # serialize the example and write it to the file
                writer.write(example.SerializeToString())

                count += 1

    # close the writer and have fun!
    writer.close()

    ```
    2. 读取图像
        1. reader.read && reader.read_up_to(一次性读取多个样例)
        2. tf.parse_single_example && tf.parse_example(解析多个样例)
    ```
    import tensorflow as tf
    import matplotlib.pyplot as plt
    import numpy as np

    # create a reader 
    reader = tf.TFRecordReader()

    # open the file
    filename_queue = tf.train.string_input_producer(
            ["./images.tfrecords"])
    # read one example at one time
    _, serialized_example = reader.read(filename_queue)
    # get the dictionary object
    features = tf.parse_single_example(
            serialized_example,
            features = {
                'image_raw': tf.FixedLenFeature([],tf.string),
                'height': tf.FixedLenFeature([],tf.int64),
                'width': tf.FixedLenFeature([],tf.int64),
                'channels': tf.FixedLenFeature([],tf.int64),
                'label': tf.FixedLenFeature([],tf.int64),
                })

    # extract the image
    image_raw_op = tf.decode_raw(features['image_raw'],tf.uint8)
    # extract the label
    label_op = tf.cast(features['label'],tf.int32)
    # extract the pixel
    height_op = tf.cast(features['height'],tf.int32)
    width_op = tf.cast(features['width'],tf.int32)
    channels_op = tf.cast(features['channels'],tf.int32)

    gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.5)
    config = tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        coord = tf.train.Coordinator()
        threads = tf.train.start_queue_runners(sess=sess,coord=coord)

        try:
            i = 0
            while not coord.should_stop() and i < 5:
                # iterator
                image_raw, label, height, width, channels = sess.run([image_raw_op,label_op,height_op,width_op,channels_op])
                image = np.reshape(image_raw,[height,width,channels])

                plt.figure()
                plt.imshow(image)
                plt.show()

                i += 1

        except tf.errors.OutOfRangeError:
            print("done!")
        finally:
            coord.request_stop()

        coord.join(threads)

    ```
    
71. 图像预处理
    1. 大小
        1. 伸缩: 4种图像大小调整算法,不会相差太远,所以随机选择
        ```
        # convert image dtype to tf.float
        image = tf.image.convert_image_dtype(image,dtype=tf.float32)
        # resize (requires float type)
        image = tf.image.resize_images(image,[300,300],method=np.random.randint(4))
        ```
        2. 裁剪 或 填充
        ```
        image = tf.image.resize_image_with_crop_or_pad(image,100,100) # 截取中间部分
        image = tf.image.resize_image_with_crop_or_pad(image,300,300) # 填充黑色
        image = tf.image.central_crop(image,0.5) # 裁剪0.5比例的中间内容
        ```
    2. 翻转
    ```
    image = tf.image.flip_up_down(image) # 上下翻转
    image = tf.image.flip_left_right(image) # 左右翻转
    image = tf.image.transpose_image(image) # 沿对角线翻转
    image = tf.image.random_flip_up_down(image) # 50%概率上下翻转
    image = tf.image.random_flip_left_right(image) # 50%概率左右翻转
    ```
    3. 色相,亮度,对比度和色度: 色相,亮度,对比度和色度的调整可能会使元素超出`[0.0,1.0)`范围,所以需要在最后一个操作后截断
    ```
    #####################################################################################
    # 色相
    #####################################################################################
    image = tf.image.convert_image_dtype(image,dtype=tf.float32)
    image = tf.image.adjust_hue(image,0.5)
    # image = tf.image.random_hue(image,0.5) # 随机调整亮度,范围是[0,0.5)
    image = tf.clip_by_value(image,0.0,1.0)    
    
    
    #####################################################################################
    # 亮度
    #####################################################################################
    image = tf.image.convert_image_dtype(image,dtype=tf.float32)
    image = tf.image.adjust_brightness(image,-0.5)
    # image = tf.image.random_brightness(image,0.5) # 随机调整亮度,范围是[-0.5,0.5)
    image = tf.clip_by_value(image,0.0,1.0)
    
    
    #####################################################################################
    # 对比度
    #####################################################################################
    image = tf.image.convert_image_dtype(image,dtype=tf.float32)
    image = tf.image.adjust_contrast(image,-0.5)
    # image = tf.image.random_contrast(image,0.5, 5) # 随机调整对比度,范围是原来的对比度*[0.5,5)
    image = tf.clip_by_value(image,0.0,1.0)    

    #####################################################################################
    # 色度
    #####################################################################################
    image = tf.image.convert_image_dtype(image,dtype=tf.float32)
    image = tf.image.adjust_saturation(image,0.5)
    # image = tf.image.random_saturation(image,0.5, 1.5) 
    image = tf.clip_by_value(image,0.0,1.0)    
    ```
    4. 标准化
        1. 亮度均值<=0
        2. 方差<=1
    ```
    image = tf.image.convert_image_dtype(image,dtype=tf.float32)
    image = tf.image.per_image_standardization(image)
    image = tf.clip_by_value(image,0.0,1.0)
    ```
    5. 标注框: 标注重点信息
        1. 画出标注框
        ```
        #image = tf.image.resize_image_with_crop_or_pad(image,50,50) # 方便观看设置裁剪了
        image = tf.image.convert_image_dtype(image,dtype=tf.float32)
        image = tf.expand_dims(image,0) # 画批注框要求为4维的图片,第一维度是batch_size

        bbox = tf.constant([[[0.05,0.05,0.9,0.7],[0.35,0.47,0.5,0.56]]]) # 分别代表[ymin,xmin,ymax,xmax],为相对位置

        image = tf.image.draw_bounding_boxes(image,bbox) # image[0]就是画出标注图的图
        ```
        2. 随机截取包含标注框40%的内容
        ```
        bbox = tf.constant([[[0.05,0.05,0.9,0.7],[0.35,0.47,0.5,0.56]]])

        begin, size, bbox_for_draw = tf.image.sample_distorted_bounding_box(
                tf.shape(image),bounding_boxes=bbox,min_object_covered=0.4) # 0.4 <=> 40%

        image = tf.image.convert_image_dtype(image,dtype=tf.float32)
        image = tf.slice(image,begin,size)
        ```
    6. [图像预处理代码](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/preprocess/preprocess.py)
    
    
72. Coordinator
    + [完整的Coordinator管理Python线程的代码](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/coordinator/coord.py)
```
def work(coord, i):
    # while haven't receive stop info
    while not coord.should_stop():
        # do sth
        coord.request_stop()

coord = tf.train.Coordinator()
threads = [threading.Thread(target=work,args=(coord,i)) for i in range(5)]
for thread in threads:
    thread.start()
coord.join(threads)
```

73. QueueRunner.[输入文件的队列原理图](https://github.com/orris27/orris/blob/master/python/machine-leaning/images/tf-files-queue.png)
```
def preprocess(filename):
    # create a reader 
    reader = tf.TFRecordReader()

    # read one example at one time
    _, serialized_example = reader.read(filename)
    # get the dictionary object
    features = tf.parse_single_example(
            serialized_example,
            features = {
                'i': tf.FixedLenFeature([],tf.int64),
                'j': tf.FixedLenFeature([],tf.int64),
                })

    # extract the i
    i = tf.cast(features['i'],tf.int32)
    # extract the j
    j = tf.cast(features['j'],tf.int32)
    # return features
    return i, j

# open the files
# form a file list
filenames = tf.train.match_filenames_once('data.tfrecords-*')
# construct a string input queue
filename_queue = tf.train.string_input_producer(filenames,shuffle=False)

# preprocess a filename
i, j = preprocess(filename_queue)

gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
config = tf.ConfigProto(gpu_options=gpu_options)
# start session
with tf.Session(config=config) as sess:
    # init vars
    sess.run(tf.local_variables_initializer()) # tf.train.match_filenames_once需要

    print(sess.run(filenames))
    
    coord = tf.train.Coordinator()
    threads = tf.train.start_queue_runners(sess=sess, coord=coord)

    # get i & j
    for _ in range(6):
        print(sess.run([i,j]))

    coord.request_stop()
    coord.join(threads)
```


74. Dataset方法
    1. 基于python list创建dataset
    ```
    import tensorflow as tf

    input_data = [1,2,3,5,8]
    dataset = tf.data.Dataset.from_tensor_slices(input_data)

    iterator = dataset.make_one_shot_iterator()

    x = iterator.get_next()

    y = x * x

    gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config = tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        for i in range(len(input_data)):
            print(sess.run(y))
    ```
    2. 基于文本文件创建dataset
    ```
    import tensorflow as tf

    input_files = ['simple.py','text.py']

    dataset = tf.data.TextLineDataset(input_files)

    iterator = dataset.make_one_shot_iterator()

    line = iterator.get_next()

    gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config = tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        for i in range(40):
            print(sess.run(line))
    ```
    3. 基于tfrecord创建dataset
        1. tfrecord文件名已经确定
        ```
        import tensorflow as tf

        def parser(record):

            features = tf.parse_single_example(
                    record,
                    features = {
                        'image_raw': tf.FixedLenFeature([],tf.string),
                        'height': tf.FixedLenFeature([],tf.int64),
                        'width': tf.FixedLenFeature([],tf.int64),
                        'channels': tf.FixedLenFeature([],tf.int64),
                        'label': tf.FixedLenFeature([],tf.int64),
                        })
            return features['height'],features['width']

        input_files = ['images.tfrecords']
        dataset = tf.data.TFRecordDataset(input_files)

        dataset = dataset.map(parser)

        iterator = dataset.make_one_shot_iterator()

        height, width = iterator.get_next()

        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config = tf.ConfigProto(gpu_options=gpu_options)
        with tf.Session(config=config) as sess:
            for i in range(3):
                print(sess.run([height,width]))
        ```
        2. tfrecord文件名是placeholder形式
            + `sess.run(iterator.initializer,feed_dict={input_files:'images.tfrecords'})`: If we call this method, then the return value of `get_next()` will start from the first elm. => An alternative to `repeat` method
        ```
        import tensorflow as tf

        def parser(record):
            features = tf.parse_single_example(
                    record,
                    features = {
                        'image_raw': tf.FixedLenFeature([],tf.string),
                        'height': tf.FixedLenFeature([],tf.int64),
                        'width': tf.FixedLenFeature([],tf.int64),
                        'channels': tf.FixedLenFeature([],tf.int64),
                        'label': tf.FixedLenFeature([],tf.int64),
                        })
            return features['height'],features['width']

        input_files = tf.placeholder(tf.string)
        dataset = tf.data.TFRecordDataset(input_files)

        dataset = dataset.map(parser)

        iterator = dataset.make_initializable_iterator()

        height, width = iterator.get_next()

        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config = tf.ConfigProto(gpu_options=gpu_options)
        with tf.Session(config=config) as sess:
            sess.run(iterator.initializer,feed_dict={input_files:'images.tfrecords'})
            while True:
                try:
                    print(sess.run([height,width]))
                except tf.errors.OutOfRangeError:
                    break
        ```
    4. Create dataset by zipping together the given datasets
        + Do not use 'x.eval()'
    ```
    sess = tf.InteractiveSession()
    sess.as_default()

    d1 = tf.data.Dataset.from_tensor_slices([1,2,3,4, 5])
    d2 = tf.data.Dataset.from_tensor_slices([10,20,30,40,50])

    d = tf.data.Dataset.zip((d1,d2))

    iterator = d.make_one_shot_iterator()

    ###############################################################################
    # 1st method
    ###############################################################################
    x = iterator.get_next()
    for i in range(5):
        print(sess.run(x))

    ###############################################################################
    # 2nd method
    ###############################################################################
    x, y = iterator.get_next()
    for i in range(5):
        print(sess.run([x, y]))
    ```
    5. 数据集的处理
        1. 单独处理image而不处理label.(这里简单的通过height和width来解决)
            1. 单独处理dataset里的一个元素
            2. shuffle
            2. batch && padded_batch
            3. num_epochs
        ```
        import tensorflow as tf

        def parser(record):

            features = tf.parse_single_example(
                    record,
                    features = {
                        'image_raw': tf.FixedLenFeature([],tf.string),
                        'height': tf.FixedLenFeature([],tf.int64),
                        'width': tf.FixedLenFeature([],tf.int64),
                        'channels': tf.FixedLenFeature([],tf.int64),
                        'label': tf.FixedLenFeature([],tf.int64),
                        })
            return features['height'],features['width']

        def f(x,y,z):
            return x*z,y

        input_files = tf.placeholder(tf.string)
        dataset = tf.data.TFRecordDataset(input_files)
        
        #################################################################################
        dataset = dataset.map(parser)
        dataset = dataset.map(lambda x,y:f(x,y,100))
        dataset = dataset.shuffle(100) # =tf.train.shuffle_batch's min_after_dequeue
        batch_size = 16
        dataset = dataset.batch(batch_size)
        num_epochs = 3
        dataset = dataset.repeat(num_epochs)
        #################################################################################
    
        
        iterator = dataset.make_initializable_iterator()

        height, width = iterator.get_next()

        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config = tf.ConfigProto(gpu_options=gpu_options)
        with tf.Session(config=config) as sess:
            sess.run(iterator.initializer,feed_dict={input_files:'images.tfrecords'})
            while True:
                try:
                    print(sess.run([height,width]))
                except tf.errors.OutOfRangeError:
                    break
        ```
        2. padded_batch: expand the dims of one instance to the specified or maximum length of all elms in that batch.
        ```
        d = tf.data.Dataset.from_tensor_slices([[1,2,2,3],[1,2,3,4],[3,4,5,7],[5,6,7,8]])
        it = d.make_one_shot_iterator()
        d = d.repeat(100)
        d = d.padded_batch(2, tf.TensorShape([5]))
        it = d.make_one_shot_iterator()

        x = it.get_next()
        #array([[1, 2, 2, 3, 0],
               [1, 2, 3, 4, 0]], dtype=int32)
        x
        #array([[3, 4, 5, 7, 0],
               [5, 6, 7, 8, 0]], dtype=int32)

        ```


75. reduce函数
> [TensorFlow下reduce数学接口介绍](https://www.jianshu.com/p/7fbce28e85a4)


76. Define a tensor that does nothing
```
tf.no_op()
```

77. tf.string_split: split elms in the 1st argument based on delimiter to a list of single word
```
tf.string_split(['hello world ',' a b c '],' ').values.eval()
# array([b'hello', b'world', b'a', b'b', b'c'], dtype=object)
```

78. InteractiveSession
```
sess = tf.InteractiveSession()
sess.as_default() # normal session does not make sense 

tf.string_split(['hello world ',' a b c '],' ').eval()
# show results...
```

79. Type Conversion
    1. Interface
    ```
    tf.string_to_number
    tf.to_double # tf.float64
    tf.to_float # tf.float32
    tf.to_bfloat16
    tf.to_int32
    tf.to_int64
    tf.cast
    tf.bitcast
    tf.saturate_cast # a safer cast
    ```
    2. Convert tensor string to tensor number
        + ~~tf.cast~~
    ```
    tf.string_to_number('12312',tf.int32).eval()
    #12312
    ```
    3. Convert tf.int to tf.float
    ```
    loss = cost / tf.to_float(batch_size)
    ```

80. Calculate the number of elms of a variable
```
a.shape
TensorShape([Dimension(3), Dimension(4)])

tf.size(a).eval()
# 12
```

81. tf.sequence_mask
```
In [6]: tf.sequence_mask([4.,3.,1.],maxlen=4,dtype=tf.float32).eval()
Out[6]: 
array([[1., 1., 1., 1.],
       [1., 1., 1., 0.],
       [1., 0., 0., 0.]], dtype=float32)

```


82. tf.sequence_mask实现消除padding计算出来的loss
```
#####################################################################
# loss + train
#####################################################################
cross_entropy = tf.nn.sparse_softmax_cross_entropy_with_logits(labels = tf.reshape(trg_label, [-1]),logits = y_predicted)
label_weights = tf.sequence_mask(trg_len, maxlen=tf.shape(trg_label)[1],dtype=tf.float32)
label_weights = tf.reshape(label_weights, [-1])
cost = tf.reduce_sum(cross_entropy * label_weights) # cost is our original loss

############################################################################################
# Loss for printing
############################################################################################
# NLP: We concern this loss value
cost_per_token = cost / tf.reduce_sum(label_weights) # divide by the number of words

############################################################################################
# Loss for optimizing
############################################################################################
loss = cost / tf.to_float(batch_size)

params = tf.trainable_variables()
opt = tf.train.GradientDescentOptimizer(learning_rate)
gradients = tf.gradients(loss, params) # depends on your actual model
clipped_gradients, norm = tf.clip_by_global_norm(gradients,max_grad_norm)
train = opt.apply_gradients(zip(clipped_gradients, params))
```

83. TensorArray
    1. `__init__`
        1. clear_after_read: If true, clear the values after reading them
    ```
    init_array = tf.TensorArray(dtype=tf.int32, size=0, dynamic_size=True, clear_after_read=False)
    ```
    2. read: read the value at location index in the TensorArray
    ```
    trg_input = trg_id.read(step)
    ```
    3. write: write value into index index of the TensorArray
        1. Returns: a TensorArray object with flow that ensures the write occurs. Use the return value for the subsequent operations
    ```
    init_array = init_array.write(0,SOS_ID)
    ```
84. show TF version
```
tf.__version__
```

85. get the rank of a tensor
```
tf.rank([[4,3,5],[4,2,5]]).eval()
2

tf.rank([3,4]).eval()
1

tf.rank(3).eval()
0
```


86. tf.slice: Extract a slice from a tensor
    1. input
    2. begin: `begin[i]` represents the offset of the i-th dimenstion you start to extract
    3. size: `size[i]` represents the number of elms of the i-th dimension you want to extract
```
t = tf.constant([[[1, 1, 1], [2, 2, 2]],
                 [[3, 3, 3], [4, 4, 4]],
                 [[5, 5, 5], [6, 6, 6]]])
                 
tf.slice(t, [1, 0, 0], [1, 1, 3])  # [[[3, 3, 3]]]
tf.slice(t, [1, 0, 0], [1, 2, 3])  # [[[3, 3, 3],
                                   #   [4, 4, 4]]]
tf.slice(t, [1, 0, 0], [2, 1, 3])  # [[[3, 3, 3]],
                                   #  [[5, 5, 5]]]
```

87. replicate i-th dimension serveral times
    + requirement: `tf.rank(input) == tf.rank(multiples)`
```
tf.tile([[0,1],[2,3]],[0,0]).eval()
array([], shape=(0, 0), dtype=int32)

tf.tile([[0,1],[2,3]],[1,0]).eval()
array([], shape=(2, 0), dtype=int32)

tf.tile([[0,1],[2,3]],[1,1]).eval()
array([[0, 1],
       [2, 3]], dtype=int32)

tf.tile([[0,1],[2,3]],[1,2]).eval()
array([[0, 1, 0, 1],
       [2, 3, 2, 3]], dtype=int32)

tf.tile([[0,1],[2,3]],[2,1]).eval()
array([[0, 1],
       [2, 3],
       [0, 1],
       [2, 3]], dtype=int32)
```

88. parse string
    1. return the given argument as a unicode string
    ```
    filename = 'hello.zip'

    with zipfile.ZipFile(filename) as f:
        print(f.read(f.namelist()[0]))
        #--------------------------------------------------------------------------
        # b'hello\nworld\nmy\nname\nis\norris\n'
        #--------------------------------------------------------------------------

        print(tf.compat.as_str(f.read(f.namelist()[0])))
        #--------------------------------------------------------------------------
        # hello
        # world
        # my
        # name
        # is
        # orris
        #
        #--------------------------------------------------------------------------
        
        print(tf.compat.as_str(f.read(f.namelist()[0])).split())
        #--------------------------------------------------------------------------
        # ['hello', 'world', 'my', 'name', 'is', 'orris']
        #--------------------------------------------------------------------------
    ```
89. tf.matmul: transpose_b
    + if transpose_b is true, transpose b before multiplication
```
tf.matmul(a,b.transpose(),transpose_b=True).eval()
#--------------------------------------------------------------------------
# array([[2.]])
#--------------------------------------------------------------------------

tf.matmul(a,b).eval()
#--------------------------------------------------------------------------
# array([[2.]])
#--------------------------------------------------------------------------

```

90. NCE loss
```
# ...

num_sampled = 64
with tf.name_scope("nce"):
    # define NCE loss
    nce_weights = tf.Variable(tf.truncated_normal([vocab_size, embedding_size], stddev=1.0 / math.sqrt(embedding_size)))
    nce_biases = tf.Variable(tf.zeros([vocab_size]))

    loss = tf.reduce_mean(tf.nn.nce_loss(weights = nce_weights, # [num_classes, embedding_size]
                                         biases = nce_biases, # [num_classes] 
                                         inputs = embedded_chars, # [batch_size, embedding_size]
                                         labels = labels, # [batch_size, num_true]; 
                                         num_sampled = num_sampled, # the number of negative samples
                                         num_classes = vocab_size)) # num_classes
#train = tf.train.GradientDescentOptimizer(1.0).minimize(loss)
```

91. 计算Discounted Future Rewards: `r = r0 + r1 * γ ** 1 + r2 * γ ** 2 + ... + rn * γ ** n`
```
def discount(rewards, gamma):
    '''
        [1, 2, 3] => [1 + 2 * gamma + 3 * gamma ** 2, 2 + 3 * gamma, 3]
    '''
    discounted_rewards = np.zeros_like(rewards)
    discounted_value = 0

    for ri in reversed(range(len(rewards))):
        discounted_value = discounted_value * gamma + rewards[ri]
        discounted_rewards[ri] = discounted_value
    return discounted_rewards


drs = []

# 对于单个实例,保存每个时间点的reward到一个数组中
drs.append(reward)

if done:
    vdrs = np.vstack(drs)
    vdiscounted_rewards = discount(vdrs, gamma)
    drs = [] # reset
    def standardize(l):
        l -= np.mean(l)
        l /= np.std(l)
        return l
    vdiscounted_rewards = standardize(vdiscounted_rewards)
```
### 1-1. slim
基本使用: 完整代码参看30-8处的内容
```
import tensorflow.contrib.slim as slim


features = tf.placeholder(tf.float32,[None,784])
labels = tf.placeholder(tf.float32,[None,10])
keep_prob = tf.placeholder(tf.float32)

inputs_reshaped = tf.reshape(features, [-1, 28, 28, 1])

conv1 = slim.conv2d(inputs_reshaped, num_outputs=32, kernel_size=[5, 5], padding='SAME', scope="layer1_conv")
conv1_pooling = slim.max_pool2d(conv1, kernel_size=[2, 2],stride=2,scope="layer1_pooling")

conv2 = slim.conv2d(conv1_pooling, num_outputs=64, kernel_size=[5, 5], padding='SAME', scope="layer2_conv")
conv2_pooling = slim.max_pool2d(conv2, kernel_size=[2, 2],stride=2,scope="layer2_pooling")

conv2_reshape = slim.flatten(conv2_pooling, scope="reshape")

nn3 = slim.fully_connected(conv2_reshape, num_outputs=1024, activation_fn=tf.nn.sigmoid, scope="layer3") # activation_fn is default to ReLU

y_predicted = slim.fully_connected(nn3, num_outputs=10, activation_fn=None, scope="layer4")

####################################################################################################################
# Do not use the loss above!! Otherwise, the params will not be updated
####################################################################################################################
#loss=-tf.reduce_mean(tf.reduce_sum(labels*tf.log(y_predicted)))
loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=y_predicted, labels=tf.argmax(labels, 1)))

train=tf.train.GradientDescentOptimizer(0.01).minimize(loss)
```
## 2. Bazel
```
cat BUILD 
#-----------------------------------------------------------------------
py_library(
    name = "hello_lib",
    srcs = [
        "hello_lib.py",
    ]
)

py_binary(
    name = "hello_main",
    srcs = [
        "hello_main.py",
    ],
    deps = [
        ":hello_lib",
    ],
)
#-----------------------------------------------------------------------

cat hello_lib.py 
#-----------------------------------------------------------------------
def print_hello_world():
    print("Hello World")
#-----------------------------------------------------------------------


cat hello_main.py 
#-----------------------------------------------------------------------
import hello_lib
hello_lib.print_hello_world()
#-----------------------------------------------------------------------



cat WORKSPACE  # 空文件
#-----------------------------------------------------------------------
#-----------------------------------------------------------------------

bazel build :hello_main
#-----------------------------------------------------------------------
total 40
drwxr-xr-x  2 orris orris 4096 Sep 16 23:29 ./
drwxrwxr-x 31 orris orris 4096 Sep 16 21:58 ../
lrwxrwxrwx  1 orris orris   88 Sep 16 23:29 bazel-bazel -> /home/orris/.cache/bazel/_bazel_orris/8d1219feedf870d498a844115318f209/execroot/__main__/
lrwxrwxrwx  1 orris orris  115 Sep 16 23:29 bazel-bin -> /home/orris/.cache/bazel/_bazel_orris/8d1219feedf870d498a844115318f209/execroot/__main__/bazel-out/k8-fastbuild/bin/
lrwxrwxrwx  1 orris orris  120 Sep 16 23:29 bazel-genfiles -> /home/orris/.cache/bazel/_bazel_orris/8d1219feedf870d498a844115318f209/execroot/__main__/bazel-out/k8-fastbuild/genfiles/
lrwxrwxrwx  1 orris orris   98 Sep 16 23:29 bazel-out -> /home/orris/.cache/bazel/_bazel_orris/8d1219feedf870d498a844115318f209/execroot/__main__/bazel-out/
lrwxrwxrwx  1 orris orris  120 Sep 16 23:29 bazel-testlogs -> /home/orris/.cache/bazel/_bazel_orris/8d1219feedf870d498a844115318f209/execroot/__main__/bazel-out/k8-fastbuild/testlogs/
-rw-r--r--  1 orris orris  207 Sep 16 23:29 BUILD
-rw-r--r--  1 orris orris   50 Sep 16 21:59 hello_lib.py
-rw-r--r--  1 orris orris   47 Sep 16 21:58 hello_main.py
-rw-r--r--  1 orris orris    0 Sep 16 21:59 WORKSPACE
#-----------------------------------------------------------------------

```
## 3. Numpy
1. 随机数
    1. 均匀分布: uniform distribution
        1. `np.random.RandomState.rand`=`np.random.rand`:返回给定形状的`[0,1)`下均匀分布的随机数的数组
        ```
        #np.random.RandomState(1).rand(3,2)
        np.random.rand(3,2)
        #----------------------------------------------------------
        # array([[ 0.14022471,  0.96360618],  #random
        #        [ 0.37601032,  0.25528411],  #random
        #        [ 0.49313049,  0.94909878]]) #random
        #----------------------------------------------------------
        ```
        2. randint:
        ```
        np.random.randint(1,4)
        #----------------------------------------------------------
        # 2
        #----------------------------------------------------------

        np.random.randint(1,4)
        #----------------------------------------------------------
        # 3
        #----------------------------------------------------------

        np.random.randint(1,4)
        #----------------------------------------------------------
        # 1
        #----------------------------------------------------------

        ```
    2. normal distribution
        1. `np.random.randn`: return values with the shape (d0, d1, d2, ...) from normal distribution
        ```
        np.random.randn(3,2)
        #----------------------------------------------------------
        # array([[ 1.17430369,  1.15183693],
        #        [-1.48240233, -0.08084986],
        #        [ 1.70741579, -0.3597359 ]])
        #----------------------------------------------------------

        ```
2. 转为python的slice切割出来的数组的每个元素为单个数组
```
a = np.array([1,2,3,4])
a[0:3,np.newaxis]
#-------------------------------
# array([[1],
#        [2],
#        [3]])
#-------------------------------

```
3. 转化numpy array为字节流
    + 存储图像,如MNIST
```
l = np.array([1,2,3])
l.tostring()
#b'\x01\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00'
```
4. argsort: return the indices that will sort the array
    1. sort
    ```
    data
    #---------------------------------------
    # array([2, 0, 1, 4, 3])
    #---------------------------------------

    data.argsort()
    #---------------------------------------
    # array([1, 2, 0, 4, 3]) # means that "data[1], data[2], data[0], data[4], data[3]" will be correct answer
    #---------------------------------------

    data[data.argsort()]
    #---------------------------------------
    # array([0, 1, 2, 3, 4])
    #---------------------------------------
    ```
    2. sort reversely
    ```
    (-data).argsort()
    #---------------------------------------
    # array([3, 4, 0, 2, 1])
    #---------------------------------------

    data[(-data).argsort()]
    #---------------------------------------
    # array([4, 3, 2, 1, 0])
    #---------------------------------------
    ```
5. 转换向量为单位向量(模=1的向量)
    1. 一维
    ```
    def unitize_1d(vector):
        return vector / np.sqrt(np.sum(np.square(vector),keepdims=True))

    vector = np.array([1,2,3,4,5])
    unitize_1d(vector)
    #--------------------------------------------------------------------------------
    # array([0.13483997, 0.26967994, 0.40451992, 0.53935989, 0.67419986])
    #--------------------------------------------------------------------------------
    ```
    2. 二维: `[batch_size, xxx]`
    ```
    def unitize_2d(vectors):
        return vectors / np.sqrt(np.sum(np.square(vectors), axis=1, keepdims=True))
    # TensorFlow version
    #def unitize_2d(vectors): 
    #   return vectors / tf.sqrt(tf.reduce_sum(tf.square(vectors), axis=1, keepdims=True))


    l = np.array([[1,2,3,4,5],[5,4,3,2,1],[3,4,5,6,7]]) 
    unitize_2d(l)
    #--------------------------------------------------------------------------------
    # array([[0.13483997, 0.26967994, 0.40451992, 0.53935989, 0.67419986],
    #        [0.67419986, 0.53935989, 0.40451992, 0.26967994, 0.13483997],
    #        [0.25819889, 0.34426519, 0.43033148, 0.51639778, 0.60246408]])
    #--------------------------------------------------------------------------------
    ```
6. np.ndarray
    1. construct
        1. np.arrary: `array(object, dtype=None, copy=True, order='K', subok=False, ndmin=0)`
        ```
        a = np.array(3.4)
        ```
        2. np.ndarray: `ndarray(shape, dtype=float, buffer=None, offset=0, strides=None, order=None)`

        ```
        a = np.ndarray(shape=(3,3), dtype=np.int32)
        ```
        3. np.zeros_like: return an array of zeros with the same shape and type as a given array
        ```
        a = [[1,2,3], [4,5,6]]

        np.zeros_like(a)
        #--------------------------------------------
        # array([[0, 0, 0],
        #        [0, 0, 0]])
        #--------------------------------------------
        ```
        4. np.stack & np.vstack
        ```
        a = [i for i in range(5)]                                                                                                    

        np.stack(a)
        #------------------------------------------------
        # array([0, 1, 2, 3, 4])
        #------------------------------------------------

        np.vstack(a)                                                                                                                 
        #------------------------------------------------
        # array([[0],
        #        [1],
        #        [2],
        #        [3],
        #        [4]])
        #------------------------------------------------
        ```


## 4. plt
`import matplotlib.pyplot as plt`
1. 画画
    1. 文字
    ```
    plt.figure() # start
    plt.text(0.3,0.4,"hello")
    plt.show() # end
    ```
    2. 曲线图
    ```
    plt.figure()
    #plt.plot(y_predicted,label='predictions') # y_predicted是一维数组
    x = np.linspace(-1.0, 1.0, 50)
    y = 2 * x ** 2+ 1
    plt.plot(x, y)
    plt.show()
    ```
    3. 散点图:plt.scatter
    4. 图像
    ```
    plt.figure()
    plt.imshow(image) # [height,width] || [height,width,channels] || etc
    plt.show()
    
    ```
2. 显示
    1. 显示在外部
    ```
    plt.show()
    ```
    2. 保存在图像文件中: 注意: show和savefig都会导致图像清空,所以不能同时调用,否则只有第一个才能成功
    ```
    plt.savefig("pic.png")
    ```
3. 画面大小
```
plt.figure(figsize=(18,18)) # set the size of the figure
plt.text(0.3,0.4,"hello")
plt.show()

```
4. annotation
    1. `s`: annotation
    2. `xytext`: the position of the annotation
    3. `xy`: the position which the annotation points to
    4. `arrowprops`: the style of the arrow. refer to [annotate-documentation](https://matplotlib.org/api/_as_gen/matplotlib.pyplot.annotate.html)
```
plt.figure()
plt.annotate(s="annotation",  xytext=(0.8, 0.8), xy=(0.2, 0.2), arrowprops=dict(arrowstyle="->"))
plt.show()
```
## 5. PIL
1. 基本操作
```
from PIL import Image

img = Image.open('1.jpg')
img.tobytes()
img.size
img.height
img.width
len(img.getbands())
```


## 6. Collections
1. 统计词频: Counter是dict的一个子类
    1. 统计词频
        1. 文本文件
        ```
        counter = collections.Counter()
        with codecs.open(raw_data,'r','utf-8') as f:
            for line in f:
                for word in line.strip().split():
                    counter[word] += 1
        ```
        2. list
        ```
        a = [ch for ch in 'apple']
        a
        #----------------------------------------------------
        # ['a', 'p', 'p', 'l', 'e']
        #----------------------------------------------------

        c = Counter(a)
        c
        #----------------------------------------------------
        # Counter({'a': 1, 'e': 1, 'l': 1, 'p': 2})
        #----------------------------------------------------

        c = Counter(a).most_common(2)
        c
        #----------------------------------------------------
        # [('p', 2), ('a', 1)]
        #----------------------------------------------------
        ```
    2. 简单实践
        1. 手动while循环统计词频
        ```
        from collections import Counter

        c = Counter()

        for ch in 'apple':
            #c[ch] = c[ch] + 1 # 默认c[某个字符]为0
            c[ch] += 1
        c
        #----------------------------------------------------
        # Counter({'a': 1, 'e': 1, 'l': 1, 'p': 2})
        #----------------------------------------------------
        ```
        2. 将需要统计的单词列表交给Counter的构造函数 + 选出前2个出现次数最多的内容
        ```
        a = [ch for ch in 'apple']
        a
        #----------------------------------------------------
        # ['a', 'p', 'p', 'l', 'e']
        #----------------------------------------------------

        c = Counter(a)
        c
        #----------------------------------------------------
        # Counter({'a': 1, 'e': 1, 'l': 1, 'p': 2})
        #----------------------------------------------------

        c = Counter(a).most_common(2)
        c
        #----------------------------------------------------
        # [('p', 2), ('a', 1)]
        #----------------------------------------------------
        ```
        
2. bidirectional queue
    1. create a bidirectional queue: `dq = collections.deque(maxlen=3)`
    2. enqueue: `dq.append(3)`
    3. get: `dq[1]`
    4. dequeue: 
        2. `dq.popleft()`: the most left elm
        1. `dq.pop()`: the most right elm
```
dq = collections.deque(maxlen=3)

dq
#----------------------------------------------------------
# deque([])
#----------------------------------------------------------

dq.append(1)

dq
#----------------------------------------------------------
# deque([1])
#----------------------------------------------------------

dq.append(2)
dq.append(3)

dq
#----------------------------------------------------------
# deque([1, 2, 3])
#----------------------------------------------------------

dq.append(4)

dq
#----------------------------------------------------------
# deque([2, 3, 4])
#----------------------------------------------------------

dq[0]
#----------------------------------------------------------
# 2
#----------------------------------------------------------
```
## 7. codecs
1. 打开文件,utf8格式
```
with codecs.open(raw_data,'r','utf-8') as f:
    for line in f:
        for word in line.strip().split():
            counter[word] += 1
```
## 8. NLP
0. 思路
    1. Get the dict: {word: index}
    2. Get the dict: {index: word}
    3. Translate the word into the index
1. 普通的文本文件 => 干净的一行一句话的文本文件
    1. English
    ```
    wget https://raw.githubusercontent.com/moses-smt/mosesdecoder/master/scripts/tokenizer/tokenizer.perl
    perl ./tokenizer.perl -no-escape -l en < ./train.en > train.txt.en
    ```
    2. Chinese: 以字为单位切割汉字
        + "我们将用一些影片来讲述一些深海里的故事。" => "我 们 将 用 一 些 影 片 来 讲 述 一 些 深 海 里 的 故 事。"
    ```
    sed 's/ //g; s/\B/ /g' ./train.tags.zh-en.zh > train.txt.zh
    ```
2. 一行一句话的文本文件 => 一行一个高频词的文本文件(后续将行号作为该单词的编号)
    + 表示如下
    ```
    I am a boy.               a
    You are a girl.    =>     boy
    study                     girl
    ```
    + `<unk>`:稀有词; `<eos>`:语句结束标记符
```
import codecs
import collections
from operator import itemgetter

raw_data = 'simple-examples/data/ptb.train.txt'
vocab_output = 'ptb.vocab'

#########################################################################################################################
 aer banknote berlitz calloway centrust cluett fromstein gitano guterman hydro-quebec ipo kia memotec mlx nahb punts rake regatta rubens sim snack-food ssangyong swapo wachter 
 pierre <unk> N years old will join the board as a nonexecutive director nov. N 
 mr. <unk> is chairman of <unk> n.v. the dutch publishing group 
    ||
Counter({'the': 50770, '<unk>': 45020, 'N': 32481, 'of': 24400, 'to': 23638, 'a': 21196, 'in': 18000, 'and': 17474, "'s": 9784, 'that': 8931, 'for': 8927, '$': 7541, 'is': 7337, 'it': 6112, 'said': 6027, 'on': 5650, 'by': 4915, 'at': 4894, 'as': 4833, 'from': 4724, 'million': 4627, 'with': 4585 })
#########################################################################################################################

counter = collections.Counter()
with codecs.open(raw_data,'r','utf-8') as f:
    for line in f:
        for word in line.strip().split():
            counter[word] += 1

#########################################################################################################################
Counter({'the': 50770, '<unk>': 45020, 'N': 32481, 'of': 24400, 'to': 23638, 'a': 21196, 'in': 18000, 'and': 17474, "'s": 9784, 'that': 8931, 'for': 8927, '$': 7541, 'is': 7337, 'it': 6112, 'said': 6027, 'on': 5650, 'by': 4915, 'at': 4894, 'as': 4833, 'from': 4724, 'million': 4627, 'with': 4585 })
    ||
[('the', 50770), ('<unk>', 45020), ('N', 32481), ('of', 24400), ('to', 23638), ('a', 21196), ('in', 18000), ('and', 17474), ("'s", 9784), ('that', 8931), ('for', 8927), ('$', 7541), ('is', 7337), ('it', 6112), ('said', 6027), ('on', 5650), ('by', 4915), ('at', 4894), ('as', 4833), ('from', 4724), ('million', 4627), ('with', 4585), ('mr.', 4326), ('was', 4073), ('be', 3923), ('are', 3914), ('its', 3846), ('he', 3632), ('but', 3541), ('has', 3494), ('an', 3477), ("n't", 3388), ('will', 3270), ('have', 3245), ('new', 2793), ('or', 2704), ('company', 2680), ('they', 2562), ('this', 2438), ('year', 2379), ('which', 2362), ('would', 2308), ('about', 2220), ('says', 2092), ('more', 2065), ('were', 2009), ('market', 2005), ('billion', 1881), ('his', 1852), ('had', 1850)]
#########################################################################################################################
sorted_word_to_cnt = sorted(counter.items(),key=itemgetter(1),reverse=True)


#########################################################################################################################
[('the', 50770), ('<unk>', 45020), ('N', 32481), ('of', 24400), ('to', 23638), ('a', 21196), ('in', 18000), ('and', 17474), ("'s", 9784), ('that', 8931), ('for', 8927), ('$', 7541), ('is', 7337), ('it', 6112), ('said', 6027), ('on', 5650), ('by', 4915), ('at', 4894), ('as', 4833), ('from', 4724), ('million', 4627), ('with', 4585), ('mr.', 4326), ('was', 4073), ('be', 3923), ('are', 3914), ('its', 3846), ('he', 3632), ('but', 3541), ('has', 3494), ('an', 3477), ("n't", 3388), ('will', 3270), ('have', 3245), ('new', 2793), ('or', 2704), ('company', 2680), ('they', 2562), ('this', 2438), ('year', 2379), ('which', 2362), ('would', 2308), ('about', 2220), ('says', 2092), ('more', 2065), ('were', 2009), ('market', 2005), ('billion', 1881), ('his', 1852), ('had', 1850)]
    ||
['the', '<unk>', 'N', 'of', 'to', 'a', 'in', 'and', "'s", 'that', 'for', '$', 'is', 'it', 'said', 'on', 'by', 'at', 'as', 'from', 'million', 'with', 'mr.', 'was', 'be', 'are', 'its', 'he', 'but', 'has', 'an', "n't", 'will', 'have', 'new', 'or', 'company', 'they', 'this', 'year', 'which', 'would', 'about', 'says', 'more', 'were', 'market', 'billion', 'his', 'had', 'their', 'up', 'u.s.', 'one', 'than', 'who',
#########################################################################################################################
sorted_words = [x[0] for x in sorted_word_to_cnt]

# 直接将<eos>添加到高频词列表中,而不是添加到文本文件中
sorted_words = ["<eos>"] + sorted_words

with codecs.open(vocab_output,'w','utf-8') as file_output:
    for word in sorted_words:
        file_output.write(word + '\n')
```
2. 一行一个高频词的文本文件 => 一行一句int话的文本文件
    + 表示如下
    ```
    a             23  40 55  123 0
    boy    =>     30  59 20  394 58 0
    girl          129 38 492 0
    ```
```
import codecs
import sys

raw_data = 'simple-examples/data/ptb.train.txt'
vocab = 'ptb.vocab'
output_data = 'ptb.train'

with codecs.open(vocab,'r','utf-8') as f_vocab:
    vocab = [word.strip() for word in f_vocab.readlines()]

word_to_id = {k:v for (k,v) in zip(vocab,range(len(vocab)))}

def get_id(word):
    return word_to_id[word] if word in word_to_id else word_to_id['<unk>']

with codecs.open(raw_data,'r','utf-8') as f_input, codecs.open(output_data,'w','utf-8') as f_output:
    for line in f_input.readlines():
        new_line = ' '.join([str(get_id(word)) for word in (line.strip().split() + ['<eos>'])]) + '\n'
        f_output.write(new_line)
```
3. 一行一句int话的文本文件 => `[num_batches, batch_size, num_steps]`的int列表
    + 表示如下
    ```
    23  40 55  123 0
    30  59 20  394 58 0   =>    [num_batches, batch_size, num_steps]
    129 38 492 0
    ```
    + 支持PTB等可以全部载入内存的小数据集
```
'''
    int_text => [num_batches, batch_size, num_steps]
'''
import tensorflow as tf
import numpy as np

def make_batches(data_file, batch_size, num_steps):
    # put together a large sentence
    with open(data_file,'r') as f:
        sentence = ' '.join([line.strip() for line in f.readlines()])
    # split into [int, ]
    data = np.array(sentence.strip().split())
    # calculate number of batches (len(data) - 1, because we need an extra elm)
    num_batches = (len(data) - 1 ) // (batch_size * num_steps)

    ###############################################################################
    # features
    ###############################################################################
    # capture the first (num_batches * batch_size * num_steps) elms
    features = data[:(num_batches * batch_size * num_steps)]
    # split into num_batches batches => (num_batches, batch_size * num_steps)
    features = np.split(features, num_batches)
    # reshape => [num_batches, batch_size, num_steps]
    features = np.reshape(features, [num_batches, batch_size, num_steps])
     
     
    ###############################################################################
    # label
    ###############################################################################
    # capture the first elm to (num_batches * batch_size * num_steps + 1) elms
    labels = data[1:(num_batches * batch_size * num_steps + 1)]
    # split into num_batches batches => (num_batches, batch_size * num_steps)
    labels = np.split(labels, num_batches)
    # reshape => [num_batches, batch_size, num_steps]
    labels = np.reshape(labels, [num_batches, batch_size, num_steps])

    # return zip (features,label)
    return list(zip(features,labels))


train_batches = data_loader.make_batches(
        FLAGS.train_data,FLAGS.train_batch_size,FLAGS.train_num_steps)
# ...
for X, y in batches:
    # ...

```
4. dict: {word: index} => dict: {index: word}
```
reverse_dictionary = dict(zip(dictionary.values(), dictionary.keys()))
```
## 9. sklearn
1. TSEN 降维: 这里的参数是根据word2vec来设计的,所以可能不太正确
``` 
from sklearn.manifold import TSNE
tsne = TSNE(perplexity=30, n_components=2, init='pca', n_iter=5000)

matrix = np.array([[1,2,3,4,5], [6,7,8,9,10]])

low_dim_embs = tsne.fit_transform(matrix)
#-------------------------------------------------------------
# array([[ 5949.602,     0.   ],
#        [-5949.602,     0.   ]], dtype=float32)
#-------------------------------------------------------------
```
2. one-hot编码离散数据
    1. 根据词典编码
    ```
    from sklearn.feature_extraction import DictVectorizer

    vec = DictVectorizer()
    l = [{"gender":"boy", "age": 17}, {"gender":"girl","age":18}, {"gender":"girl","age":20}]

    vec.fit_transform(l).toarray()
    #----------------------------------------------------------------
    # array([[17.,  1.,  0.],
    #        [18.,  0.,  1.],
    #        [20.,  0.,  1.]])
    #----------------------------------------------------------------

    vec.get_feature_names()
    #----------------------------------------------------------------
    # ['age', 'gender=boy', 'gender=girl']
    #----------------------------------------------------------------
    ```
    
    2. 根据一维列表编码: 类似于`embedding_lookup`一样,直接在`[dims]`的形状上增加一维度成为`[dims, num_classes]`
    ```
    from sklearn import preprocessing

    lb = preprocessing.LabelBinarizer()
    lb.fit_transform(["yes", "no", "yes"])
    
    #----------------------------------------------------------------
    # array([[1],
    #        [0],
    #        [1]])
    #----------------------------------------------------------------

    lb.fit_transform(["yes", "no", "yes", "unk"])
    #----------------------------------------------------------------
    # array([[0, 0, 1],
    #        [1, 0, 0],
    #        [0, 0, 1],
    #        [0, 1, 0]])
    #----------------------------------------------------------------

    ```
3. Decision Tree: 完整代码参看30-5的DecisionTree代码
```
clf = tree.DecisionTreeClassifier(criterion="entropy")
clf = clf.fit(features,labels) # features: [batch_size, features_dims]; labels: [batch_size, num_classes]
print(features)
#--------------------------------------------------------------
# [[0. 0. 1. 0. 1. 1. 0. 0. 1. 0.]
#  [0. 0. 1. 1. 0. 1. 0. 0. 1. 0.]
#  [1. 0. 0. 0. 1. 1. 0. 0. 1. 0.]
#  [0. 1. 0. 0. 1. 0. 0. 1. 1. 0.]
#  [0. 1. 0. 0. 1. 0. 1. 0. 0. 1.]
#  [0. 1. 0. 1. 0. 0. 1. 0. 0. 1.]
#  [1. 0. 0. 1. 0. 0. 1. 0. 0. 1.]
#  [0. 0. 1. 0. 1. 0. 0. 1. 1. 0.]
#  [0. 0. 1. 0. 1. 0. 1. 0. 0. 1.]
#  [0. 1. 0. 0. 1. 0. 0. 1. 0. 1.]
#  [0. 0. 1. 1. 0. 0. 0. 1. 0. 1.]
#  [1. 0. 0. 1. 0. 0. 0. 1. 1. 0.]
#  [1. 0. 0. 0. 1. 1. 0. 0. 0. 1.]
#  [0. 1. 0. 1. 0. 0. 0. 1. 1. 0.]]
#--------------------------------------------------------------
print(labels)
#--------------------------------------------------------------
# [[0]
#  [0]
#  [1]
#  [1]
#  [1]
#  [0]
#  [1]
#  [0]
#  [1]
#  [1]
#  [1]
#  [1]
#  [1]
#  [0]]
#--------------------------------------------------------------

# predict
y_predicted = clf.predict(features)
print(y_predicted)
#--------------------------------------------------------------
# [0 0 1 1 1 0 1 0 1 1 1 1 1 0]
#--------------------------------------------------------------
```


4. KNN
```
from sklearn import neighbors
from sklearn import datasets

knn = neighbors.KNeighborsClassifier()

iris = datasets.load_iris()

print(iris)

knn.fit(iris.data, iris.target) # iris.data => [batch_size, features_dims]; iris.target => [batch_size]

#predictedLabel = knn.predict([[0.1, 0.2, 0.3, 0.4]])
predictedLabel = knn.predict(iris.data)
print(predictedLabel)
```

5. SVM
    1. introduction
    ```
    from sklearn import svm

    points = np.r_[np.random.randn(20,2) + [2,2], np.random.randn(20,2) - [2,2]]
    labels = [0] * 20 + [1] * 20

    clf = svm.SVC(kernel = 'linear')
    clf.fit(points, labels)

    ################################################################################
    # 假如超平面是用b + w0 * x0 + w1 * x1 + w2 * x2 + ... + wn * xn = 0 表示的
    # clf.coef_表示所有超平面的集合, 而clf.coef_的每个元素即clf.coef_[i]表示这个超平面对应的所有w的集合
    # 而clf.intercpet则表示所有超平面的b的集合
    # 因此对于第一个超平面,权重集合: clf.coef_[0]; 偏差: clf.intercept_ (不是偏差集合,因为不需要多一个维度)
    # 从下面的代码可以看出,这个代码的特征值只有2个,为x0,x1. 所以超平面为 -0.095 + (-0.644) * x0 + (-0.381) * x1 = 0
    # 如果要画图,就默认x1为y,而x0为x, 则得到 y = - (-0.644) / (-0.381) * x0 - (-0.095) / (-0381)
    #                                 即 w = clf.coef_[0], b = clf.intercept_[0]
    #                                    y = - w[0] / w[1] * x - b / w[1] 
    
    p clf.coef_
    #--------------------------------------------------------------
    # array([[-0.64453299, -0.38101331]]) 
    #--------------------------------------------------------------
    
    clf.intercept_
    #--------------------------------------------------------------
    # array([-0.09549856])
    #--------------------------------------------------------------
    
    #################################################################################
    # Draw Hyperplane
    #################################################################################
    w = clf.coef_[0]
    b = clf.intercept_[0]
    x = np.linspace(-5.0,5.0, 30)
    y = - w[0] / w[1] * x - b / w[1]

    plt.figure()
    plt.plot(x, y)
    for point in points:
        plt.scatter(point[0], point[1])
    plt.show()

    ```

## 10. gym
1. usage
```
import gym

env = gym.make("CartPole-v0")

observation = env.reset()
#env.render() # paint the picture

observation, reward, done, info  = env.step(action)
# env state  reward  True/False _

env.close()
```




## 11. csv
1. read lines: 自动根据逗号分割成字符串列表
    + `next` cannot be used if the open mode is `bytes`
```
reader = csv.reader(open('AllElectronics.csv','r'))


#for line in reader: # print all lines
#    print(line)
#-----------------------------------------------------------------------------------
# RID,age,income,student,credit_rating,class_buys_computer
# 1,youth,high,no,fair,no
# 2,youth,high,no,excellent,no
#-----------------------------------------------------------------------------------


next(reader)
#-----------------------------------------------------------------------------------
# ['RID', 'age', 'income', 'student', 'credit_rating', 'class_buys_computer']
#-----------------------------------------------------------------------------------

next(reader)
#-----------------------------------------------------------------------------------
# ['1', 'youth', 'high', 'no', 'fair', 'no']
#-----------------------------------------------------------------------------------

next(reader)
#-----------------------------------------------------------------------------------
# ['2', 'youth', 'high', 'no', 'excellent', 'no']
#-----------------------------------------------------------------------------------


```


## 12. python
1. 如果是`__main__`的话
```
if __name__ == '__main__': 
```
2. 文件名
    1. 获取当前文件名
    ```
    print(__file__)
    ```
    2. 截取文件名的前缀和后缀
    ```
    os.path.splitext(file)[0] # 文件名
    os.path.splitext(file)[-1] # 文件的扩展名
    ```
3. 随机生成0,1的大小为5的numpy矩阵
    1. a: 1-D array-like or int(<=> np.arange(a))
    2. size: output shape
    3. replace: allows repeatition
```
np.array(np.random.choice(2, size=(5,)))
np.random.choice(5, 5, replace=False)
#-------------------------------------------------
# array([4, 2, 3, 0, 1])
#-------------------------------------------------

np.random.choice(5, 5, replace=True)
#-------------------------------------------------
# array([3, 3, 2, 1, 3])
#-------------------------------------------------

```

4. 组装字符串的列表.将字符串的内容分隔后,转换成整数,然后组成列表
```
list(map(int, '1,2,3'.split(',')))
```
5. 处理txt文件,获得字符串列表与标签,并且批量输出:[embedding从普通文本转化到词向量矩阵的转换流程图](https://github.com/orris27/orris/blob/master/python/machine-leaning/images/embedding.png)
    + 输入:txt文件(一行算作一句话,作为一个元素)
    ```
    it's nice to see piscopo again after all these years , and chaykin and headly are priceless . 
    provides a porthole into that noble , trembling incoherence that defines us all . 
    ```
    + 输出的X:一句话的列表
    ```
    ['which combined scenes of a japanese monster flick with canned shots of raymond burr commenting on the monster 's path of destruction", 'the thing looks like a made for home video quickie', "enigma is well made , but it 's just too dry and too placid"]

    ```
```
import re
import numpy as np


def clean_str(string):
    """
    Tokenization/string cleaning for all datasets except for SST.
    """
    string = string.strip()
    string = re.sub(r"[^A-Za-z0-9(),!?\'\`]", " ", string)
    string = re.sub(r"\'s", " \'s", string)
    string = re.sub(r"\'ve", " \'ve", string)
    string = re.sub(r"n\'t", " n\'t", string)
    string = re.sub(r"\'re", " \'re", string)
    string = re.sub(r"\'d", " \'d", string)
    string = re.sub(r"\'ll", " \'ll", string)
    string = re.sub(r",", " , ", string)
    string = re.sub(r"!", " ! ", string)
    string = re.sub(r"\(", " \( ", string)
    string = re.sub(r"\)", " \) ", string)
    string = re.sub(r"\?", " \? ", string)
    string = re.sub(r"\s{2,}", " ", string)
    return string.strip().lower()


def load_data_and_labels(positive_data_file,negative_data_file):
    # 打开文件字符串
    positive_data = open(positive_data_file,"rb").read().decode('utf-8')
    negative_data = open(negative_data_file,"rb").read().decode('utf-8')

    # 分隔成列表
    positive_data = positive_data.split('\n')[:-1]
    negative_data = negative_data.split('\n')[:-1]
    
    # 清洗列表中的每个字符串
    positive_data = [clean_str(row) for row in positive_data]
    negative_data = [clean_str(row) for row in negative_data]

    # 构造特征值
    features = positive_data + negative_data
    
    # 构造标签
    positive_labels = [[0,1] for _ in positive_data]
    negative_labels = [[1,0] for _ in negative_data]

    labels = np.concatenate([positive_labels, negative_labels], 0)

    return [features,labels]
    


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


X_text, y = load_data_and_labels('./data/rt-polaritydata/rt-polarity.pos','./data/rt-polaritydata/rt-polarity.neg')
# [一句话,] => [词典id,]
# [词典id,] => [词向量,] (X_text => X)
# 可以在这里将X和y分割成训练集和测试集
batches = next_batch(list(zip(X_trainset, y_trainset)), FLAGS.batch_size, FLAGS.num_steps)
for batch in batches:
    X_train, y_train = zip(*batch) # 这里的X和y已经是训练集中的[batch_size,xxx]的二维矩阵了
    X_train = np.array(X_train)
    y_train = np.array(y_train)
    # ... # 自由训练把~~
``` 

6. 洗牌
    + data:列表
    + `l[[3,5]]`会取出第3个和第5个元素(0为下标)并组成列表.
        + 下面的内容会返回`[4, 5]`
        ```
        data = np.array([1,2,3,4,5,6,7,8])
        print(data[[3,4]]) #[4, 5]
        ```
    1. 方法1:只适用于numpy array
    ```
    import numpy as np

    shuffle_indices = np.random.permutation(np.arange(len(data))) # 会将[0, len(data))的整数洗牌.比如说shuffle_indices会变成[2 4 6 7 1 0 3 5]
    shuffled_data1 = data1[shuffle_indices]
    shuffled_data2 = data2[shuffle_indices] # data{1,2}会保持相同行
    ```
    2. 方法2
    ```
    np.random.shuffle(data)
    ```
9. 计算当前时间戳
```
import time

time.time()
#1536759799.8907976

int(time.time())
#1536759813

```
10. 文件OS
    1. 计算绝对路径
    ```
    os.path.abspath('.')
    # '/home/orris/fun/tensorflow'
    ```
    2. 遍历某个目录下的所有文件,并打印文件名
    ```
    for filename in os.listdir(file_dir):
        print(filename)
    ```
    3. 拼接目录和文件名
    ```
    for filename in os.listdir(file_dir):
        print(filename)
        print(os.path.join(file_dir,filename))
    ```
11. 生成`[0,10)`的整数的列表
```
x = np.arange(9.0)
#---------------------------------------------------------------
# array([0., 1., 2., 3., 4., 5., 6., 7., 8.])
#---------------------------------------------------------------

np.arange(0, 5, dtype=np.int32)
#---------------------------------------------------------------
# array([0, 1, 2, 3, 4], dtype=int32)
#---------------------------------------------------------------
```

12. 分割列表:
    1. 根据下标进行切割
    ```
    x = np.arange(8.0)
    np.split(x, [3, 5, 6, 10], axis=0) # axis表示从0开始切割
    #--------------------------------------------------------------------------------
    # [array([ 0.,  1.,  2.]),
    #  array([ 3.,  4.]),
    #  array([ 5.]),
    #  array([ 6.,  7.]),
    #  array([], dtype=float64)]
    #--------------------------------------------------------------------------------
    ```
    2. 分割成k份:
        + a是`[m,n]`的矩阵,第二个参数是k的话:
            1. axis=0: `[k, m/k, n]`
            2. axis=1: `[k, m, n/k]`
    ```
    a.shape
    # (15, 28)

    np.array(np.split(a,3,axis=0)).shape
    # (3, 5, 28)

    np.array(np.split(a,7,axis=1)).shape
    # (7, 15, 4)
    ```

13. python2的UTF8声明
```
#-*- coding:utf-8 -*-
#coding=utf-8
#coding:utf-8
```

14. 字符串列表 <=> 字符串:split和join
```
' '.join(["1","2","3","4"])
#'1 2 3 4'

'1 2 3 4'.split(' ')
#['1', '2', '3', '4']
```
15. map:对第二个参数里的每个元素执行第一个函数,返回迭代器(可以用list转换成列表)
    + 这里的`int`充当函数
```
l
# ['1', '2', '3', '4']

map(int,l)
# <map at 0x7f111e328b00>

list(map(int,l))
# [1, 2, 3, 4]
```
16. lambda:定义一个简单的函数,返回函数对象本身
    1. 1个参数
    ```
    f = lambda x:x+1
    f(2)
    # 3
    ```
    2. 多个参数
    ```
    f = lambda x,y:x+y
    f(1,2)
    #3

    ```
    3. 没有参数
    ```
    f = lambda :7

    f()
    7
    ```
17. 遍历目录下的内容
```
import os 

list_dirs = os.walk('.') 
for root, dirs, files in list_dirs: 
    for d in dirs: 
        print(os.path.join(root, d))
    for f in files: 
        print(os.path.join(root, f))
```
18. 设置环境变量
```
os.environ["CUDA_VISIBLE_DEVICES"] = "2"
```

19. download: (import `urllib.request` instead of `urllib`
```
import urllib.request
import sys
import os

def download(url):
    filename = url.split('/')[-1]
    if os.path.exists(filename):
        return filename

    def show_progress(block_num, block_size, total_size):
        progress = float(block_num * block_size) / float(total_size) * 100.0
        sys.stdout.write('\r>> Downloading %s %.1f%%' % (filename, progress))
        sys.stdout.flush()

    filename, _ = urllib.request.urlretrieve(url, filename, show_progress)
    print()
    return filename

url = 'http://cn.wordpress.org/wordpress-3.1-zh_CN.zip'
filename = download(url)
print(filename)
# wordpress-3.1-zh_CN.zip
```
20. 查看文件属性: os.stat
```
filename = 'Python-2.7.5.tar.bz2'

statinfo = os.stat(filename)
print(statinfo)
# os.stat_result(st_mode=33188, st_ino=1449407, st_dev=2067, st_nlink=1, st_uid=1000, st_gid=1000, st_size=12147710, st_atime=1538840255, st_mtime=1538840612, st_ctime=1538840612)

print(statinfo.st_size)
# 12147710
```
21. zipfile
    1. unzip a file
    ```
    import zipfile
    
    with zipfile.ZipFile(filename) as f:
        print(f.namelist()) # type: list
    # ['ptb/', 'ptb/orris/', 'ptb/orris/__pycache__/', 'ptb/orris/__pycache__/data_loader.cpython-36.pyc', 'ptb/orris/.git/', 'ptb/orris/.git/refs/', 'ptb/orris/.git/refs/heads/', 'ptb/orris/.git/refs/heads/master', 'ptb/orris/.git/refs/heads/loss', 'ptb/orris/.git/refs/tags/', 'ptb/orris/.git/logs/', 'ptb/orris/.git/logs/refs/', 'ptb/orris/.git/logs/refs/heads/',]
    ```
22. dict
    1. reverse dictionary: {word, index} => {index, word}
    ```
    reverse_dictionary = dict(zip(dictionary.values(), dictionary.keys()))
    ```
    2. check if a value is in the keys
    ```
    dictionary = {"apple":350, "banana":500}

    "apple" in dictionary
    #--------------------------------------
    # True
    #--------------------------------------

    "apple1" in dictionary
    #--------------------------------------
    # False
    #--------------------------------------

    ```
23. list
    1. extend: Cannot use `a = a.extend(...)` which will assign NoneType to a
    ```
    a = [[1,2]]
    a.extend([[2,3],[3,4]])

    a
    #-----------------------------------------------------
    # [[1, 2], [2, 3], [3, 4]]
    #-----------------------------------------------------

    ```
    2. remove && pop: remove => value; pop => index
    ```
    k = [10,20,30,40]

    k.remove(10)
    k
    #--------------------
    # [20, 30, 40]
    #--------------------

    k.pop(1)
    #--------------------
    # 30
    #--------------------

    k
    #--------------------
    # [20, 40]
    #--------------------

    ```
    3. reversed
    ```
    [j for j in reversed([i for i in range(10)])]
    #------------------------------------------------
    # [9, 8, 7, 6, 5, 4, 3, 2, 1, 0]
    #------------------------------------------------
    ```
    4. concat
    ```
    [0] * 3 + [1] * 3
    #------------------------------------------------
    # [0, 0, 0, 1, 1, 1]
    #------------------------------------------------

    [0, 0, 0] + [1, 1, 1]
    #------------------------------------------------
    # [0, 0, 0, 1, 1, 1]
    #------------------------------------------------

    ```
24. Python里面的三目运算符`?:`
```
action = 1 if np.random.random() < probability1 else 0
```

25. 计时
```
time.time() #<=> clock()
```
