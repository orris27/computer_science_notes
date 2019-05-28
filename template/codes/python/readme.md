## 1. TensorFlow
1. 定义features和labels
```
with tf.name_scope('placeholder'):
    #  [[特征值0,特征值1,特征值2,...,特征值f783],
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

            # lrn层: 可以放到pooling前面或者后面,经验法则
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
    1. sigmoid_cross_entropy_with_logits: `tf.losses.sigmoid_cross_entropy`最终调用`tf.nn.xx`,基本一样
    + `tf.losses.sigmoid_cross_entropy(logits=logits, multi_class_labels=labels)` <=> `-1 * tf.reduce_mean(labels * tf.log(tf.sigmoid(logits)) + (1 - labels) * tf.log(1 - tf.sigmoid(logits)))`
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
    3. mean_squared_error: 差的平方和的平均值
        + `tf.losses.mean_squared_error(predictions=logits, labels=labels)` <=> `tf.reduce_mean((logits - labels) ** 2)`
        1. <方法1>
        ```
        # labels 和 predictions 都是一维数组
        loss = tf.losses.mean_squared_error(labels=self.labels,predictions=self.y_predicted)
        ```
        2. <方法2>
            1. y_predicted.shape == labels.shape
            2. shape=`[batch_size,num_classes]`或者`[batch_size]`等都可以
        ```
        #loss = tf.reduce_mean(tf.square(y_predicted - labels)) # 适用于回归等
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
        2. GAN: real_pred和fake_pred是Discriminator的输出,完整代码查看代码实战里的TF官网DCGAN实现
        ```
        def get_disc_loss(real_pred, fake_pred):
            real_loss = tf.losses.sigmoid_cross_entropy(multi_class_labels = tf.ones_like(real_pred), logits=real_pred)
            fake_loss = tf.losses.sigmoid_cross_entropy(multi_class_labels = tf.zeros_like(fake_pred), logits=fake_pred)
            return real_loss + fake_loss

        def get_gen_loss(fake_pred):
            fake_loss = tf.losses.sigmoid_cross_entropy(multi_class_labels = tf.ones_like(fake_pred), logits=fake_pred)
            return fake_loss
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
                1. <方法1>
                ```
                from tensorflow.examples.tutorials.mnist import input_data

                mnist = input_data.read_data_sets('MNIST_data/',one_hot = True)            
                X_train,y_train = mnist.train.next_batch(100)
                ```
                2. <方法2>
                ```
                (train_images, train_labels), (_, _) = tf.keras.datasets.mnist.load_data()

                train_images = train_images.reshape(train_images.shape[0], 28, 28, 1).astype('float32')
                train_images = (train_images - 127.5) / 127.5

                buffer_size = 60000
                train_dataset = tf.data.Dataset.from_tensor_slices(train_images).shuffle(buffer_size).batch(batch_size)
                
                # ...
                for images in train_dataset:
                    # ...
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
    3. COCO
        1. json
        + `info`: annotations['info'] => `{'description': 'This is stable 1.0 version of the 2014 MS COCO dataset.', 'url': 'http://mscoco.org', 'version': '1.0', 'year': 2014, 'contributor': 'Microsoft COCO group', 'date_created': '2015-01-27 09:11:52.357475'}`
        + `type`: annotations['type'] => `'captions'`
        + `images`: a list of image's infos. `annotations['images'][0]` => `{'license': 5, 'url': 'http://farm4.staticflickr.com/3153/2970773875_164f0c0b83_z.jpg', 'file_name': 'COCO_train2014_000000057870.jpg', 'id': 57870, 'width': 640, 'date_captured': '2013-11-14 16:28:13', 'height': 480}`
        + `annotations`: a list of image's annotations: `annotations['annotations'][0]` => `{'image_id': 318556, 'id': 48, 'caption': 'A very clean and well decorated empty bathroom'}`
        + NOTE: `'COCO_train2014_' + '%012d.jpg' % (image_id)` is the true filename rather than file_name.当然这里的文件名指的是直接`unzip train2014.zip`. 
        + NOTE2: `train2014.zip`总共有82783个图片,但`captions_train2014.json`总共只为16643个图片注释了,被注释的图片可能有多个注释,但是其他图片就没有caption了.
        ```
        annotation_file = './captions_train2014.json'
        with open(annotation_file, 'r') as f:
            annotations = json.load(f)
        #-----------------------------------------------------------
        # annotations.keys() => dict_keys(['info', 'images', 'licenses', 'type', 'annotations'])
        #-----------------------------------------------------------



        l = [['COCO_train2014_' + '%012d.jpg' % (annotation['image_id']), annotation['caption']] for image, annotation in zip(annotations['images'], annotations['annotations'])]
        #----------------------------------------------------------------------------------------------------
        # ['COCO_train2014_000000318556.jpg', 'A very clean and well decorated empty bathroom']
        # ['COCO_train2014_000000116100.jpg', 'A panoramic view of a kitchen and all of its appliances.']
        # ....
        #----------------------------------------------------------------------------------------------------

        
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
            + variables not saved:
                1. `+=`赋值的变量不会被保存
                2. placeholder
                3. output of op (`self.initial_state = tf.Variable(self.lstm_cell.zero_state(self.batch_size, tf.float32), trainable=False)`saved, while `self.initial_state = self.lstm_cell.zero_state(self.batch_size, tf.float32)` not saved)
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

        with tf.Session() as sess:
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

    with tf.Session() as sess:
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
        sess = tf.Session(config=config, ...)
        ```
        2. 设置每个GPU应该拿出多少容量给进程使用，0.4代表40%
        ```
        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config = tf.ConfigProto(gpu_options=gpu_options)
        sess = tf.Session(config=config, ...)
        ```
    3. 控制使用哪块GPU: 设置环境变量
        1. 命令行
        ```
        CUDA_VISIBLE_DEVICES=0  python your.py#使用GPU0
        CUDA_VISIBLE_DEVICES=0,1 python your.py#使用GPU0,1
        #注意单词不要打错
        ```
        2. 程序
        ```
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
        + `lstm_cell`在第一次调用`__call__`或者`dynamic_rnn`后会改变自己!!比如说`__call__`第一次调用的时候,使用的输入shape是`[batch_size, a]`的话,那么后续调用`__call__`或者`dynamic_rnn`时,输入的shape必须是`[batch_size, a]`或者`[batch_size, num_steps, a]`,这个a是不能改变的
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
26. scope: name_scope & variable_scope变量名字
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
    2. get_variable只受variable_scope影响,而Variable同时受name_scope和variable_scope影响.
        1. name_scope和variable_scope互相嵌套
        ```
        with tf.name_scope("123"):
            with tf.variable_scope("456"):
                with tf.name_scope("789"):
                    a = tf.Variable(3.14, name="a")
                    print(a.name) # 123/456/789/a:0
                    b = tf.get_variable("b", 1)
                    print(b.name) # 456/b:0


        ```
        2. 重复name_scope: 第二个scope开始加后缀`_1`等
        ```
        with tf.name_scope("123"):
            a = tf.Variable(3.14, name="a")
            print(a.name) # 123/a:0
            b = tf.get_variable("b", 1)
            print(b.name) # b:0

        with tf.name_scope("123"):
            a = tf.Variable(3.14, name="a")
            print(a.name) # 123_1/a:0
            #b = tf.get_variable("b", 1) # 不能定义
            #print(b.name)

        ```
        3. 重复variable_scope: 对于Variable而言第二个scope开始加后缀`_1`等,而对于get_variable而言,还是原来的那个.
        ```
        with tf.variable_scope("123"):
            a = tf.Variable(3.14, name="a")
            print(a.name) # 123/a:0
            b = tf.get_variable("b", 1)
            print(b.name) # 123/b:0

        with tf.variable_scope("123"):
            a = tf.Variable(3.14, name="a")
            print(a.name) # 123_1/a:0
            #b = tf.get_variable("b", 1)
            #print(b.name) # 123/b:0

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
    9. [slim基于LeNet-5实现MNIST数字识别](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/slim/readme.md)
    10. [TFLearn基于LeNet-5实现MNIST数字识别](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/tflearn)
    11. [Keras基于LeNet-5实现MNIST数字手写识别,使用Sequential和Model继承方法](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/keras)
    12. [Keras基于现成的ResNet50识别图片](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/keras/resnet-recognize.py)
    13. [TF基于attention的image caption](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/keras/image-caption.py)
    
    14. [Slim实现InceptionV3,评测耗时](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/cnn)
    15. [Slim实现ResNetV2,评测耗时](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/cnn)
    16. [TF官网DCGAN代码自己实现](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/gan/dcgan-mnist.py)
        1. 计算梯度的的时候,使用各自的GradientTape
        2. 应用梯度的时候,使用各自的Optimizer
        3. 生成图像检查的时候,注意Discriminator传入的real_images和fake_images形状是一样的,根据这一点,我们可以将fake_images还原成可以看的图片
        4. Evaluate和Train的时候,Dropout和BatchNormal表现不一样,必须通过training控制
        5. eager mode还是什么原因,总之MNIST输入必须使用tf.keras.datasets.mnist.load_data()这个API,得到的结果是ndarray,形状为[60000, 28, 28] 和 [60000,]
        6. 真实图片可以考虑手动切换到[-1, 1]的区域之间,之后再传入网络
        7. Discriminator中如果报错最后全连接层时说shape不对,是因为real_images和fake_images的shape不对,前者已经定义好Fully_connected层的结构了,所以如果fake_images不正确就会出现问题

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
+ 通用
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
+ NLP
    1. words => indices, dictionary, reverse_dictionary. 完整代码参考[skip-gram代码](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/word2vec.py/skip-gram.py)
        + `['I', 'am', 'a', 'boy']` => [23, 10, 2, 33], {'I': 23, 'am': 10,..}, {23: 'I', 10:'am', ...}
    ```
    words = ['I', 'am', 'a', 'boy', ...]
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

    indices, word2idx, idx2word = words_to_indices(words)
    del words
    ```

    2. Skip-Gram: indices => X, y. 完整代码参考[skip-gram代码](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/tensorflow/word2vec.py/skip-gram.py)
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
    
    
    3. sentences => indices
    ```
    train_captions = ['<sos> I am a boy <eos>', '<sos> I love a pen <eos>']
    top_k = 5 # 最后的结果会留下top_k-1个不同的数字

    tokenizer = tf.keras.preprocessing.text.Tokenizer(num_words=top_k, oov_token='<unk>', filters='!"#$%&()*+.,-/:;=?@[\]^_`{|}~ ')
    tokenizer.fit_on_texts(train_captions) # 让分词器适应该语料库

    tokenizer.texts_to_sequences(train_captions) # 使用已经被训练好的tokenizer去分词
    #-----------------------------------------------------------------------------------------------------------------------------
    # [[2, 3, 1, 4, 1, 1], [2, 3, 1, 4, 1, 1]]
    #-----------------------------------------------------------------------------------------------------------------------------

    tokenizer.get_config() # 查看tokenizer的属性
    #-----------------------------------------------------------------------------------------------------------------------------
    # {'num_words': 5,
    #  'filters': '!"#$%&()*+.,-/:;=?@[\\]^_`{|}~ ',
    #  'lower': True,
    #  'split': ' ',
    #  'char_level': False,
    #  'oov_token': '<unk>',
    #  'document_count': 2,
    #  'word_counts': '{"<sos>": 2, "i": 2, "am": 1, "a": 2, "boy": 1, "<eos>": 2, "love": 1, "pen": 1}',
    #  'word_docs': '{"<eos>": 2, "<sos>": 2, "boy": 1, "i": 2, "a": 2, "am": 1, "love": 1, "pen": 1}',
    #  'index_docs': '{"5": 2, "2": 2, "7": 1, "3": 2, "4": 2, "6": 1, "8": 1, "9": 1}',
    #  'index_word': '{"1": "<unk>", "2": "<sos>", "3": "i", "4": "a", "5": "<eos>", "6": "am", "7": "boy", "8": "love", "9": "pen"}',
    #  'word_index': '{"<unk>": 1, "<sos>": 2, "i": 3, "a": 4, "<eos>": 5, "am": 6, "boy": 7, "love": 8, "pen": 9}'}
    #-----------------------------------------------------------------------------------------------------------------------------

    tokenizer.word_index
    #-----------------------------------------------------------------------------------------------------------------------------
    # {'<unk>': 1,
    #  '<sos>': 2,
    #  'i': 3,
    #  'a': 4,
    #  '<eos>': 5,
    #  'am': 6,
    #  'boy': 7,
    #  'love': 8,
    #  'pen': 9}
    #-----------------------------------------------------------------------------------------------------------------------------
    ```
    
    
    
    
    4. indices => padded incdices: padding indices from the whole sentences
    ```
    a
    #-----------------------------------------------------------------------------------------------------------------------------
    # array([array([1, 2, 3, 4, 5]), array([2, 3, 4, 5, 6, 7, 8, 9])],
    #       dtype=object)
    #-----------------------------------------------------------------------------------------------------------------------------

    tf.keras.preprocessing.sequence.pad_sequences(a, padding='post') 
    #-----------------------------------------------------------------------------------------------------------------------------
    # array([[1, 2, 3, 4, 5, 0, 0, 0],
    #        [2, 3, 4, 5, 6, 7, 8, 9]], dtype=int32)
    #-----------------------------------------------------------------------------------------------------------------------------
    ```
    
+ 图像预处理: 1. `filename`: string => `image_raw`: bytes => `image`:(width, height, channels), dtype=int32 => `images`:(batch_size, width, height, channles), dtype=int32 => `dimages`:(batch_size, width, height, channels), dtype=float32 当然不是简单的类型转换啦 2. `img`用来表示其他类型的图像,如PIL结构的图像

    1. 单张图片
    ```
    import keras
    import numpy as np

    filename = 'elephant.jpg'
    img = keras.preprocessing.image.load_img(filename, target_size=(224, 224))
    image = keras.preprocessing.image.img_to_array(img)
    images = np.expand_dims(image, axis=0)
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
        
    config = tf.ConfigProto()
    config.gpu_options.allow_growth = True
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
    c = tf.placeholder(tf.float32, name="c")

    result = tf.add(a,b, name='add')
    ac = tf.add(a, c, name="ac")

    config = tf.ConfigProto()
    config.gpu_options.allow_growth = True
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())

        print(result.name)

        graph_def = tf.get_default_graph().as_graph_def()

        output_graph_def = graph_util.convert_variables_to_constants(sess,graph_def,['add', 'ac']) # It does not determine which nodes we can use

        with tf.gfile.GFile("ckpt/model.pb","wb") as f:
            f.write(output_graph_def.SerializeToString())

    ```
    2. 恢复
        1. 构造GraphDef对象
        2. 解析文件内容到GraphDef对象中
        3. 导入GraphDef对象内的某个计算节点到当前的计算图中,并设置对应的张量
    ```
    import tensorflow as tf

    config = tf.ConfigProto()
    config.gpu_options.allow_growth = True
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())

        # get graph_def objects
        with tf.gfile.FastGFile('ckpt/model.pb','rb') as f:
            graph_def = tf.GraphDef()
            graph_def.ParseFromString(f.read())

        # import graph_def
        c, ac = tf.import_graph_def(graph_def,return_elements=["c:0", "ac:0"]) # get nodes in the graph

        # run with fixed ops
        print(sess.run(ac, feed_dict={c:32.0})) # now we can operate with these nodes
    ```
    3. examples
        1. inception v3
        ```
        import tensorflow as tf
        import cv2

        config = tf.ConfigProto()
        config.gpu_options.allow_growth = True

        with tf.Session(config=config) as sess:
            sess.run(tf.global_variables_initializer())

            # get graph_def objects
            with tf.gfile.FastGFile('/zju_data0/dataset/models/tensorflow_inception_graph.pb','rb') as f:
                graph_def = tf.GraphDef()
                graph_def.ParseFromString(f.read())

            # import graph_def
            input_op, output_op = tf.import_graph_def(graph_def,return_elements=["DecodeJpeg:0", "softmax:0"]) # get nodes in the graph

            filename = '/zju_data0/dataset/train2014/COCO_train2014_000000000009.jpg'
            image_data = cv2.imread(filename)

            features = sess.run(output_op, feed_dict={input_op:image_data}) # (1, 1008) numpy array

            # run with fixed ops
            print(features)
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
+ `prefetch`: prefetch elms from the dataset in the buffer
    1. 基于python list创建dataset
        1. standard(not eager)
        ```
        import tensorflow as tf

        input_data = [1,2,3,5,8]
        dataset = tf.data.Dataset.from_tensor_slices(input_data)
        # dataset = dataset.prefetch(1)

        iterator = dataset.make_one_shot_iterator()

        x = iterator.get_next()

        y = x * x

        config = tf.ConfigProto()
        config.gpu_options.allow_growth = True
        with tf.Session(config=config) as sess:
            for i in range(len(input_data)):
                print(sess.run(y))
        ```
        2. eager mode: `dataset.__iter__()` is only supported when eager
        ```
        import tensorflow as tf

        config = tf.ConfigProto()
        config.gpu_options.allow_growth = True
        tf.enable_eager_execution(config=config)

        input_data = [i for i in range(100)]
        dataset = tf.data.Dataset.from_tensor_slices(input_data).shuffle(100).batch(64)
        # dataset = dataset.prefetch(1)


        num_epochs = 5

        for epoch in range(num_epochs):
            print('*'*30)
            print("epoch {0}".format(epoch))
            for inputs in dataset:
                print(inputs)
            print('*'*30)

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

92. Download: `tf.keras.utils.get_file`下载文件,并保存文件到cache里面(cache实际是`~/.keras/datasets`目录)
    1. 下载到cache的默认子目录下: 默认是`~/.keras/datasets/`
    ```
    url = "https://github.com/orris27/orris/raw/master/java/java.md"
    a = tf.keras.utils.get_file("test.md", origin=url)
    #---------------------------------------------------------------
    # Downloading data from https://github.com/orris27/orris/raw/master/java/java.md
    # 8192/5408 [=============================================] - 0s 0us/step
    #---------------------------------------------------------------

    a
    #---------------------------------------------------------------
    # '/home/orris/.keras/datasets/test.md'
    #---------------------------------------------------------------
    ``` 
    2. 下载到cache内指定的子目录下: 比如指定`cache_subdir`为`model`,那么就会保存到`~/.keras/model/`目录下
        1. 相对路径: 相对于`~/.keras/`
        ```
        url = "https://github.com/orris27/orris/raw/master/java/java.md"
        a = tf.keras.utils.get_file("test.md", origin=url, cache_subdir="model")

        #---------------------------------------------------------------
        # Downloading data from https://github.com/orris27/orris/raw/master/java/java.md
        # 8192/5408 [=============================================] - 0s 0us/step
        #---------------------------------------------------------------

        a
        #---------------------------------------------------------------
        # '/home/orris/.keras/model/test.md'
        #---------------------------------------------------------------
        ```
        2. 绝对路径: 
        ```
        import os

        url = "https://github.com/orris27/orris/raw/master/java/java.md"
        a = tf.keras.utils.get_file("test.md", origin=url, cache_subdir=os.path.abspath("."))
        #---------------------------------------------------------------
        # Downloading data from https://github.com/orris27/orris/raw/master/java/java.md
        # 8192/5408 [=============================================] - 0s 0us/step
        #---------------------------------------------------------------
        
        a
        #---------------------------------------------------------------
        # '/home/orris/fun/tensorflow/test.md'
        #---------------------------------------------------------------
        ```


93. 空

94. tf.py_func: wraps a python function and uses it as a tensor operation
```
def sqrt(x):
    return np.sqrt(x)

inp = tf.placeholder(tf.float32)
y = tf.py_func(sqrt, [inp], tf.float32)

with tf.Session() as sess:
    print(sess.run(y, feed_dict={inp:9}))
```

95. Environment test
    1. is_gpu_available: whether TensorFlow can access a gpu
    ```
    tf.test.is_gpu_available()
    #---------------------------------------------------------
    # True
    #---------------------------------------------------------
    ```
    2. is_built_with_cuda: Returns whether TensorFlow was built with CUDA (GPU) support.
    ```
    tf.test.is_built_with_cuda()
    #---------------------------------------------------------
    # True
    #---------------------------------------------------------
    ```
    
    
96. eager mode
    1. Basic use
    + `tfe.Variable`: in eager mode, `tf.Variable` is not supported, use `tfe.Variable` instead
    + It's okay to use `tf.reduce_mean`, `tf.square`, etc
    + `tf.random_normal` or `np.random.xxx` are nearly same.
    + 注意: 绝对不要将`optimizer = tf.train.AdamOptimizer()`放在epoch循环内,因为这样就会重复定义,而且在loss减少的很少...具体的原因是来自
    ```
    import tensorflow as tf

    config = tf.ConfigProto()
    config.gpu_options.allow_growth = True
    tf.enable_eager_execution(config=config) # add this code at the top of the program

    a = tf.constant([1,2,3,4])
    print(a)
    #---------------------------------------------------------------
    # tf.Tensor([1 2 3 4], shape=(4,), dtype=int32)
    #---------------------------------------------------------------

    print(a.numpy())
    #---------------------------------------------------------------
    # [1 2 3 4]
    #---------------------------------------------------------------

    b = tf.constant(3.14)

    print(b)
    #---------------------------------------------------------------
    # tf.Tensor(3.14, shape=(), dtype=float32)
    #---------------------------------------------------------------

    print(b.numpy())
    #---------------------------------------------------------------
    # 3.14
    #---------------------------------------------------------------

    print(type(b.numpy()))
    #---------------------------------------------------------------
    # <class 'numpy.float32'>
    #---------------------------------------------------------------
    ```
    2. module
    ```
    import tensorflow.contrib.eager as tfe
    ```
    
    3. training model in eager mode: `python xx.py` to execute the codes below
    ```
    import numpy as np
    import tensorflow as tf
    gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
    config = tf.ConfigProto(gpu_options=gpu_options)
    tf.enable_eager_execution(config=config)

    import tensorflow.contrib.eager as tfe



    learnin_rate = 0.01
    training_size = 1000
    #X_train = np.random.random([training_size]) * 10 
    X_train = tf.random_normal([training_size]) # 初始化X_train用tf和numpy都可以,毕竟只是训练用的数据.但是计算的时候最好使用TF的API
    noise = (np.random.random(training_size) - 0.5) * 0.01 
    y_train = X_train * 4.9 + noise + 1.1


    w = tfe.Variable(5.2)
    b = tfe.Variable(1.2)
    def inference(X_train, w, b):
        return X_train * w + b

    def get_loss(y_predicted, labels):
        return tf.reduce_mean(tf.square(y_predicted - labels))
        #return np.reduce_mean(np.square(y_predicted - labels)) # we cannot use numpy op

    num_epochs = 1000
    for step in range(num_epochs):
        with tf.GradientTape() as tape:
            y_predicted = inference(X_train, w, b) # tape里面必须包含能从w, b(参数们)一起追溯到loss的所有计算,所以这两行不能放在tap以外的代码段
            loss = get_loss(y_predicted, y_train)
        dw, db = tape.gradient(loss, [w, b])
        w.assign_sub(dw * learnin_rate)
        b.assign_sub(db * learnin_rate)
        if step % 10 == 0:
            print(w.numpy(), b.numpy())
    ```


97. noise
```
noise = tf.random_normal([NUM_EXAMPLES]) # eager mode's noise
noise = (np.random.random(training_size) - 0.5) * 0.01 # my noise for eager training
```



98. Image-Caption-based-on-SeqGAN
```
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 修改dataloader/MSCOCODataLoader.py下的这些内容为自己的目录
# annotation_dir = os.path.join(config.DATA_DIR, "MSCOCO", "annotation")
annotation_dir = os.path.join("/home/user/dataset", "MSCOCO", "annotation")
# image_feature_dir = os.path.join(config.DATA_DIR, "MSCOCO", "image_feature")
image_feature_dir = os.path.join("/home/user/dataset", "MSCOCO", "image_feature")
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 修改processor/image_processor.py下面内容
#IMG_MODEL_FILE = os.path.join(config.DATA_DIR, "models", "tensorflow_inception_graph.pb")
IMG_MODEL_FILE = os.path.join("/home/user/dataset", "models", "tensorflow_inception_graph.pb")

#data_dir = os.path.join(config.DATA_DIR, "MSCOCO")
data_dir = os.path.join("/home/user/dataset", "MSCOCO")

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 还是直接修改config.py的内容把
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



python image_processing.py # Create image_feature: "eval_img_feature.pkl", "test_img_feature.pkl", "train_img_feature.pkl"


#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 修改processor/text_processor.py里的
# WORD_VEC_DIM = 1008
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
python text_processing.py # Create :data/MSCOCO/word_embedding/{dictionary.pkl, embedding.pkl}"

mv data/MSCOCO/cache/cache.pkl data/MSCOCO/cache/cache.pkl.ori


cp dataloader/MSCOCODataLoader.py mscoco1.py
python mscoco1.py
rm mscoco1.py

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# gen_pre_train_epoch = 1
# dis_pre_train_epoch = 1
# adversarial_train_epoch = 1
# epoch_size = 2 # 第70行左右可以设置这里的参数,这样可以大大减少循环的次数
# log_every = 1
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 将generator/generator.py里的initial_state修改一下
# #_, self.initial_state = self.lstm_cell.call(self.image_input, self.zero_state)
# self.initial_state = self.lstm_cell(self.image_input, self.zero_state)
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

python training.py
python inference_py # we'll get the result
```


99. contribs:
    1. `avg_pool2d`
    ```
    weight, height = 3, 3
    images = np.random.uniform(size=[32, height, width, 3])

    tf.contrib.layers.avg_pool2d(images, [3, 3])                         
    #--------------------------------------------------------------------------------
    # <tf.Tensor 'AvgPool2D_2/AvgPool:0' shape=(32, 1, 1, 3) dtype=float64>
    #--------------------------------------------------------------------------------
    ```
    2. `convolution2d`: inputs, num_outpus, kernel_size, stride, padding(SAME), acitvation_fn(ReLu), normalizer_fn, weights_initializer, weights_regularizer, biases_initializer, biases_regularizer, reuse, variables_collection, outputs_collection, trainable, scope => `Conv/Relu`
    ```
    weight, height = 3, 3
    images = np.random.uniform(size=[32, height, width, 3])
    
    tf.contrib.layers.convolution2d(images, num_outputs=32, kernel_size=[3, 3])                  
    #--------------------------------------------------------------------------------
    # <tf.Tensor 'Conv/Relu:0' shape=(32, 3, 3, 32) dtype=float64>
    #--------------------------------------------------------------------------------
    
    weights = tf.contrib.framework.get_variables_by_name('weights')[0]                            

    weights_shape = weights.get_shape().as_list()                                                 

    weights                                                                                       
    #--------------------------------------------------------------------------------
    # <tf.Variable 'Conv/weights:0' shape=(3, 3, 3, 32) dtype=float64_ref>
    #--------------------------------------------------------------------------------

    weights_shape                                                                                 
    #--------------------------------------------------------------------------------
    # [3, 3, 3, 32]
    #--------------------------------------------------------------------------------

    ```
    3. `arg_scope`: 减少代码重用.
    ```
    weight, height = 3, 3
    images = np.random.uniform(size=[32, height, width, 3])
    
    with tf.contrib.framework.arg_scope([tf.contrib.layers.convolution2d], num_outputs=64, kernel_size=[3, 3]): 
        outputs = tf.contrib.layers.convolution2d(images)                                                            

    outputs                                                                                                          
    #--------------------------------------------------------------------------------
    # <tf.Tensor 'Conv_3/Relu:0' shape=(32, 3, 3, 64) dtype=float64>
    #--------------------------------------------------------------------------------

    ```
    
    4. `fully_connected`
    ```
    inputs = np.random.uniform(size=[64, 1024])                                        

    tf.contrib.layers.fully_connected(inputs, 10)                                      
    #--------------------------------------------------------------------------------
    # <tf.Tensor 'fully_connected/Relu:0' shape=(64, 10) dtype=float64>
    #--------------------------------------------------------------------------------
    ```
    
    
    5. `repeat`: 构建串联的相同网络. repeat(inputs, repetitions, layer, \*args, \*\*kwargs)
    ```
    inputs = np.random.uniform(size=[64, 1024])                                       

    tf.contrib.layers.repeat(inputs, 4, tf.contrib.layers.fully_connected, 100)
    #--------------------------------------------------------------------------------
    # <tf.Tensor 'Repeat/fully_connected_4/Relu:0' shape=(64, 100) dtype=float64>
    #--------------------------------------------------------------------------------

    ```
    
    6. `stack`: 构建串联的不同参数的网络. stack(inputs, layer, stack_args, \*\*kwargs).
    + 注意: 下面代码中的scope不是4个,但会自动添加后缀,为fc_1, fc_2等
    ```
    inputs = np.random.uniform(size=[64, 1024])                                       
    
    tf.contrib.layers.stack(inputs, tf.contrib.layers.fully_connected, [100, 50, 10, 2], scope='fc')                 
    #--------------------------------------------------------------------------------
    # <tf.Tensor 'fc/fc_4/Relu:0' shape=(64, 2) dtype=float64>
    #--------------------------------------------------------------------------------

    ```

100. Deconvolution
```
inputs = tf.random_uniform([1, 7, 7, 64])
conv = tf.keras.layers.Conv2DTranspose(32, (5, 5), strides=(1, 1), padding='same', use_bias=False)
outputs = conv(inputs)
#-------------------------------------------------------------------------------------------------------
# shape=(1, 7, 7, 32)
#-------------------------------------------------------------------------------------------------------
```
1. Operations
    1. get_operations
    ```

    c = tf.Variable(3.14)

    g.get_operations()
    #-----------------------------------------------------------------
    # [<tf.Operation 'a/Initializer/random_uniform/shape' type=Const>,
    #  <tf.Operation 'a/Initializer/random_uniform/min' type=Const>,
    #  <tf.Operation 'a/Initializer/random_uniform/max' type=Const>,
    #  <tf.Operation 'a/Initializer/random_uniform/RandomUniform' type=RandomUniform>,
    #  <tf.Operation 'a/Initializer/random_uniform/sub' type=Sub>,
    #  <tf.Operation 'a/Initializer/random_uniform/mul' type=Mul>,
    #  <tf.Operation 'a/Initializer/random_uniform' type=Add>,
    #  <tf.Operation 'a' type=VariableV2>,
    #  <tf.Operation 'a/Assign' type=Assign>,
    #  <tf.Operation 'a/read' type=Identity>,
    #  <tf.Operation 'Variable/initial_value' type=Const>,
    #  <tf.Operation 'Variable' type=VariableV2>,
    #  <tf.Operation 'Variable/Assign' type=Assign>,
    #  <tf.Operation 'Variable/read' type=Identity>]
    #-----------------------------------------------------------------
    ```
    2. get_operations的返回值Op的相关方法: 查看输入和输出
    ```
    add = a + b
    g.get_operations()[-1]
    #-----------------------------------------------------------------
    # <tf.Operation 'add' type=Add>
    #-----------------------------------------------------------------

    add_op = g.get_operations()[-1]

    add_op.inputs[:]
    #-----------------------------------------------------------------
    # [<tf.Tensor 'a/read:0' shape=(1,) dtype=float32>,
    #  <tf.Tensor 'b/read:0' shape=(2,) dtype=float32>]
    #-----------------------------------------------------------------


    add_op.outputs
    #-----------------------------------------------------------------
    # [<tf.Tensor 'add:0' shape=(2,) dtype=float32>]
    #-----------------------------------------------------------------

    ```

### 1-1. slim
1. Basic Usage: 完整代码参看30-8处的内容
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

###########-#########################################################################################################
# Do not use the loss above!! Otherwise, the params will not be updated
####################################################################################################################
#loss=-tf.reduce_mean(tf.reduce_sum(labels*tf.log(y_predicted)))
loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=y_predicted, labels=tf.argmax(labels, 1)))

train=tf.train.GradientDescentOptimizer(0.01).minimize(loss)
```


2. slim.arg_scope: 给指定函数的指定参数赋予默认值
```
# 设置slim.conv2d和slim.fully_connected两个函数的weights_regularizer参数的默认值为slim.l2_regularizer(weight_decay)
with slim.arg_scope([slim.conv2d, slim.fully_connected], weights_regularizer=slim.l2_regularizer(weight_decay))

```

3. Batch Normalization: 
    1. 将slim.batch_norm作为slim.conv2d的normalizer_fn的参数
    2. 设置slim.conv2d的normalizer_params参数
    + slim.batch_norm自身有activation_fn的参数,可以实现ResNet中输入前使用BN方法+激活,如`preact = slim.batch_norm(inputs, activateion_fn=tf.nn.relu, scope="preact")`
    ```
    def inception_v3_arg_scope(weight_decay=0.00004, stddev=0.1, batch_norm_var_collection='moving_vars'):
        batch_norm_params = {
                'decay': 0.9997,
                'epsilon': 0.001,
                'updates_collections': tf.GraphKeys.UPDATE_OPS,
                'variables_collections': {
                    'beta': None,
                    'gamma': None,
                    'moving_mean': [batch_norm_var_collection],
                    'moving_variance': [batch_norm_var_collection],
                    },
                }

        with slim.arg_scope([slim.conv2d, slim.fully_connected], weights_regularizer=slim.l2_regularizer(weight_decay)):
            with slim.arg_scope([slim.conv2d], weights_initializer=tf.truncated_normal_initializer(stddev=stddev),
                                               activation_fn=tf.nn.relu,
                                               normalizer_fn=slim.batch_norm,
                                               normalizer_params=batch_norm_params) as sc:
                return sc

    ```



### 1-2. TFLearn
1. Basic Usage
```
import tflearn
from tflearn.layers.core import input_data, fully_connected
from tflearn.layers.conv import conv_2d, max_pool_2d
from tflearn.layers.estimator import regression

import tflearn.datasets.mnist as mnist

X_train, y_train, X_test, y_test = mnist.load_data(data_dir="/home/user/dataset/MNIST_data/", one_hot=True)

X_train = X_train.reshape([-1, 28, 28, 1])
X_test = X_test.reshape([-1, 28, 28, 1])


# define a placholder to recv data
net = input_data(shape=[None, 28, 28, 1], name="input")

# layer1
net = conv_2d(net, nb_filter=32, filter_size=[5, 5], activation="relu", padding='same')
net = max_pool_2d(net, kernel_size=[2,2]) # strides default to be the same as kernel_size

# layer2
net = conv_2d(net, nb_filter=64, filter_size=[5, 5], activation="relu", padding='same')
net = max_pool_2d(net, kernel_size=[2,2])

####################################################################
# No need to reshape
####################################################################

# layer3
net = fully_connected(net, n_units=500, activation="relu")

# layer4
net = fully_connected(net, n_units=10, activation="softmax")

# calc optimizer

net = regression(net, optimizer='sgd', loss='categorical_crossentropy', learning_rate=0.01)

# create Deep NN
model = tflearn.DNN(net, tensorboard_verbose=0)

# train: just like "fit" in sklearn
model.fit(X_train, y_train, n_epoch=20, validation_set=([X_test, y_test]), show_metric=True, shuffle=True,)
```

### 1-3. Keras
1. Sequential
```
import keras
from keras.datasets import mnist
from keras import backend as K

num_classes = 10

########################################################################################
# Create Dataset
########################################################################################
img_width, img_height = 28, 28
(X_train, y_train), (X_test, y_test) = mnist.load_data()
if K.image_data_format() == 'channels_first':
    X_train = X_train.reshape(X_train.shape[0], 1, img_width, img_height)
    X_test = X_test.reshape(X_test.shape[0], 1, img_width, img_height)
    input_shape = (1, img_width, img_height)
else:
    X_train = X_train.reshape(X_train.shape[0], img_width, img_height, 1)
    X_test = X_test.reshape(X_test.shape[0], img_width, img_height, 1)
    input_shape = (img_width, img_height, 1)

# unitize X
X_train = X_train.astype('float32')
X_train /= 255
X_test = X_test.astype('float32')
X_test /= 255

# reshape y
y_train = keras.utils.to_categorical(y_train, num_classes)
y_test = keras.utils.to_categorical(y_test, num_classes)

########################################################################################
# Build Model
########################################################################################
model = keras.models.Sequential()

model.add(keras.layers.Conv2D(filters=32, kernel_size=[5, 5], padding='same', activation='relu', input_shape=input_shape))
model.add(keras.layers.MaxPooling2D(pool_size=[2,2], padding='same'))

model.add(keras.layers.Conv2D(filters=64, kernel_size=[5, 5], padding='same', activation='relu', input_shape=input_shape))
model.add(keras.layers.MaxPooling2D(pool_size=[2,2], padding='same'))

model.add(keras.layers.Flatten())

model.add(keras.layers.Dense(units=500, activation='relu'))
model.add(keras.layers.Dense(units=num_classes, activation='softmax'))

model.compile(loss=keras.losses.categorical_crossentropy, optimizer=keras.optimizers.SGD(), metrics=['accuracy'])

########################################################################################
# train
########################################################################################
model.fit(X_train, y_train, batch_size=128, epochs=20, validation_data=(X_test, y_test))




########################################################################################
# Evaluate
########################################################################################
score = model.evaluate(X_test, y_test)        
print("Test score:", score[0])
print("Test accuracy:", score[1])
```



2. `tf.keras.models.Model` <=> `tf.keras.Model`: [reference-website](https://www.tensorflow.org/api_docs/python/tf/keras/models/Model#__call__)
+ With the "functional API", where you start from Input, you chain layer calls to specify the model's forward pass, and finally you create your model from inputs and outputs
```
import tensorflow as tf

inputs = tf.keras.Input(shape=(3,))
x = tf.keras.layers.Dense(4, activation=tf.nn.relu)(inputs)
outputs = tf.keras.layers.Dense(5, activation=tf.nn.softmax)(x)
model = tf.keras.Model(inputs=inputs, outputs=outputs)
```



+ By subclassing the `Model` class: in that case, you should define your layers in `__init__` and you should implement the model's forward pass in `call`.
    1. basic
    + `my_model.variables`: get all the variables of the object
    ```
    import tensorflow as tf

    class MyModel(tf.keras.Model):

        def __init__(self):
            super(MyModel, self).__init__()
            self.dense1 = tf.keras.layers.Dense(4, activation=tf.nn.relu)
            self.dense2 = tf.keras.layers.Dense(5, activation=tf.nn.softmax)

        def call(self, inputs):
            x = self.dense1(inputs)
            return self.dense2(x)

    model = MyModel()
    ```
    
    2. MNIST classification
    + `super(YourClass, self).__init__()`: do not forget
    + `model.compile`: use `tf.keras` API, instead of `tf.nn.xxx` or `tf.train.AdamOptimizer`
    ```
    import keras
    import tensorflow as tf
    from keras.datasets import mnist
    from keras import backend as K

    num_classes = 10

    ########################################################################################
    # Create Dataset
    ########################################################################################
    # define image width & image height
    img_width, img_height = 28, 28
    # get mnist data
    (X_train, y_train), (X_test, y_test) = mnist.load_data()
    # reshape X
    if K.image_data_format() == 'channels_first':
        X_train = X_train.reshape(X_train.shape[0], 1, img_width, img_height)
        X_test = X_test.reshape(X_test.shape[0], 1, img_width, img_height)
        input_shape = (1, img_width, img_height)
    else:
        X_train = X_train.reshape(X_train.shape[0], img_width, img_height, 1)
        X_test = X_test.reshape(X_test.shape[0], img_width, img_height, 1)
        input_shape = (img_width, img_height, 1)

    X_train = X_train.astype('float32')
    X_train /= 255
    X_test = X_test.astype('float32')
    X_test /= 255

    y_train = keras.utils.to_categorical(y_train, num_classes)
    y_test = keras.utils.to_categorical(y_test, num_classes)

    ########################################################################################
    # Build Model
    ########################################################################################
    class LeNet(tf.keras.Model):
        def __init__(self):
            super(LeNet, self).__init__()

            self.conv1 = tf.keras.layers.Conv2D(filters=32, kernel_size=[5, 5], padding='same', activation='relu', input_shape=input_shape)
            self.pooling1 = tf.keras.layers.MaxPooling2D(pool_size=[2,2], padding='same')

            self.conv2 = tf.keras.layers.Conv2D(filters=64, kernel_size=[5, 5], padding='same', activation='relu', input_shape=input_shape)
            self.pooling2 = tf.keras.layers.MaxPooling2D(pool_size=[2,2], padding='same')

            self.flatten = tf.keras.layers.Flatten()

            self.dense1 = tf.keras.layers.Dense(units=500, activation='relu')
            self.dense2 = tf.keras.layers.Dense(units=num_classes, activation='softmax')

        def call(self, inputs):
            x = self.conv1(inputs)
            x = self.pooling1(x)

            x = self.conv2(x)
            x = self.pooling2(x)

            x = self.flatten(x)

            x = self.dense1(x)
            return self.dense2(x)

    model = LeNet()

    learning_rate = 1.0
    model.compile(loss=tf.keras.losses.categorical_crossentropy, optimizer=tf.keras.optimizers.SGD(), metrics=['accuracy'])

    ########################################################################################
    # train
    ########################################################################################
    model.fit(X_train, y_train, batch_size=128, epochs=20, validation_data=(X_test, y_test))

    ########################################################################################
    # Evaluate
    ########################################################################################
    score = model.evaluate(X_test, y_test)        
    print("Test score:", score[0])
    print("Test accuracy:", score[1])
    ```


If you subclass `Model`, you can optionally have a `training` argument (boolean) in `call`, which you can use to specify a different behavior in training and inference:
```
import tensorflow as tf

class MyModel(tf.keras.Model):

  def __init__(self):
    super(MyModel, self).__init__()
    self.dense1 = tf.keras.layers.Dense(4, activation=tf.nn.relu)
    self.dense2 = tf.keras.layers.Dense(5, activation=tf.nn.softmax)
    self.dropout = tf.keras.layers.Dropout(0.5)

  def call(self, inputs, training=False):
    x = self.dense1(inputs)
    if training:
      x = self.dropout(x, training=training)
    return self.dense2(x)

model = MyModel()
```


3. `tf.keras.applications`
    1. InceptionV3 (未完待续): [参考资料](https://keras.io/applications/#inceptionv3)
    + 类似于`keras.applications.inception_v3.InceptionV3(include_top=True, weights='imagenet', input_tensor=None, input_shape=None, pooling=None, classes=1000)`
    + default input shape: `[batch_size, 299, 299, 3]`
    + `include_top`: whether to include a fully connected layer at the top of the network
    + `weights`: pretraining on ImageNet
    + Returns: keras model
    ```
    def load_image(image_path):
        img = tf.read_file(image_path)
        img = tf.image.decode_jpeg(img, channels=3)
        img = tf.image.resize_images(img, (299, 299))
        img = tf.keras.applications.inception_v3.preprocess_input(img)
        return img, image_path
    
    image_model = tf.keras.applications.InceptionV3(include_top=False, weights='imagenet')
    new_input = image_model.input
    hidden_layer = image_model.layers[-1].output

    image_features_extract_model = tf.keras.Model(new_input, hidden_layer)

    for img, path in image_dataset:
    # img.shape=TensorShape([Dimension(16), Dimension(299), Dimension(299), Dimension(3)])

      batch_features = image_features_extract_model(img)
      # batch_features.shape=TensorShape([Dimension(16), Dimension(8), Dimension(8), Dimension(2048)])
    ```
    
    
    2. predict: 完整代码请查看30代码实战中的"Keras基于现成的ResNet50识别图片"
    + `ResNet50`, `InceptionV3`, etc all make senses
    ```
    import keras
    from keras.applications.resnet50 import ResNet50
    from keras.applications.resnet50 import preprocess_input, decode_predictions
    import numpy as np

    model = ResNet50(weights='imagenet')
    # ...

    dimages = preprocess_input(images) # suppose images: [1, width, height, 3], dtype=int32(0~255)

    y_predicted = model.predict(dimages) # y_predicted => (batch_size, 1000). 表示总共有1000个分类的one-hot
    
    
    print(decode_predictions(y_predicted)) # => [1, 5, 3]
    #----------------------------------------------------------------------------------------------------------
    # [[('n02504458', 'African_elephant', 0.63380444), ('n01871265', 'tusker', 0.27116048), ('n02504013', 'Indian_elephant', 0.094646156), ('n09428293', 'seashore', 5.843781e-05), ('n01704323', 'triceratops', 3.599315e-05)]]
    #----------------------------------------------------------------------------------------------------------
    
    
    print('Predicted:', decode_predictions(y_predicted, top=3)[0])
    #----------------------------------------------------------------------------------------------------------
    # Predicted: [(u'n02504013', u'Indian_elephant', 0.82658225), (u'n01871265', u'tusker', 0.1122357), (u'n02504458', u'African_elephant', 0.061040461)]
    #----------------------------------------------------------------------------------------------------------
    ```



4. attention implementation
```
class CNNEncoder(tf.keras.Model):
    def __init__(self, embedding_size):
        super(CNNEncoder, self).__init__()

        self.dense = tf.keras.layers.Dense(embedding_size, activation=tf.nn.relu)

    def call(self, inputs):
        return self.dense(inputs)

def gru(units):
    if tf.test.is_gpu_available():
        return tf.keras.layers.CuDNNGRU(units,
                                        return_sequences=True,
                                        return_state=True,
                                        recurrent_initializer='glorot_uniform')
    else:
        return tf.keras.layers.GRU(units,
                                   return_sequences=True,
                                   return_state=True,
                                   recurrent_activation='sigmod',
                                   recurrent_initializer='glorot_uniform')

class BahdanauAttention(tf.keras.Model):
    def __init__(self, wv_size):
        super(BahdanauAttention, self).__init__()
        self.V = tf.keras.layers.Dense(wv_size, activation=None)
        self.W = tf.keras.layers.Dense(wv_size, activation=None)
        self.U = tf.keras.layers.Dense(1, activation=None)
        

    def call(self, encoder_output, hidden_state):
        hidden_state_with_time = tf.expand_dims(hidden_state, axis=1)
        weights = tf.nn.softmax(self.U(tf.tanh(self.V(encoder_output) + self.W(hidden_state_with_time))), axis=1)
        context = tf.reduce_sum(weights * encoder_output, axis=1)
        return context

class RNNDecoder(tf.keras.Model):
    def __init__(self, embedding_size, wv_size, vocab_size):
        super(RNNDecoder, self).__init__()
        self.embedding = tf.keras.layers.Embedding(vocab_size, embedding_size)
        self.attention = BahdanauAttention(wv_size)
        self.gru_size = gru_size
        self.gru = gru(self.gru_size)
        self.dense1 = tf.keras.layers.Dense(self.gru_size, activation=None)
        self.dense2 = tf.keras.layers.Dense(vocab_size, activation=None)

    def call(self, decoder_input, encoder_output, hidden_state):
        decoder_input = self.embedding(decoder_input)
        context = self.attention(encoder_output, hidden_state)
        decoder_input = tf.concat([tf.expand_dims(context, axis=1), decoder_input], -1)
        output, hidden_state = self.gru(decoder_input)
        output = self.dense1(output)
        output = tf.reshape(output, [-1, output.shape[-1]])
        output = self.dense2(output)
        return output, hidden_state

    def reset_state(self, batch_size):
        return tf.zeros([batch_size, self.gru_size])

encoder = CNNEncoder(embedding_size)
decoder = RNNDecoder(embedding_size, units, vocab_size)

def calc_loss(logits, labels):
    mask = 1 - np.equal(labels, 0)
    loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = labels,logits = logits) * mask)
    return loss

optimizer = tf.train.AdamOptimizer()
for epoch in range(num_epochs):
    for (step, (dimages, padded_indices)) in enumerate(dataset):
        loss = 0
        hidden_state = decoder.reset_state(batch_size=padded_indices.shape[0])
        decoder_input = tf.expand_dims([tokenizer.word_index["<start>"]] * batch_size, axis=1)
        with tf.GradientTape() as tape:
            encoder_output = encoder(dimages)
            for curr_timestep in range(1, padded_indices.shape[1]):
                output, hidden_state = decoder(decoder_input, encoder_output, hidden_state)
                loss += calc_loss(logits=output, labels=padded_indices[:, curr_timestep])
                decoder_input = tf.expand_dims(padded_indices[:, curr_timestep], axis=1)
        grads = tape.gradient(loss, encoder.variables + decoder.variables)
        train = optimizer.apply_gradients(zip(grads, encoder.variables + decoder.variables))
        log_every = 20
        if step % log_every == 0:
            print("batch {2} step {0}: loss={1}".format(step, loss.numpy() / (int)(padded_indices.shape[1]), epoch))
```

5. tf.one_hot
```
print(sess.run(tf.one_hot([2,4,3,5,1,0], 10)))
#---------------------------------------------------------
# [[0. 0. 1. 0. 0. 0. 0. 0. 0. 0.]
#  [0. 0. 0. 0. 1. 0. 0. 0. 0. 0.]
#  [0. 0. 0. 1. 0. 0. 0. 0. 0. 0.]
#  [0. 0. 0. 0. 0. 1. 0. 0. 0. 0.]
#  [0. 1. 0. 0. 0. 0. 0. 0. 0. 0.]
#  [1. 0. 0. 0. 0. 0. 0. 0. 0. 0.]]
#---------------------------------------------------------
```


## 2. PyTorch
1. Tensor
    1. structure: tensor = 信息区(size, stride, flag) + storage
        + different tensors might share stroage
        + real data depends on both of storage(), data_ptr() and stride()
        ```
        a = torch.arange(0, 6)

        a
        #--------------------------------------------------------
        # tensor([0, 1, 2, 3, 4, 5])
        #--------------------------------------------------------

        a.storage()
        #--------------------------------------------------------
        #  0
        #  1
        #  2
        #  3
        #  4
        #  5
        # [torch.LongStorage of size 6]
        #--------------------------------------------------------

        b = a.view(2, 3)

        id(a) == id(b)
        #--------------------------------------------------------
        # False
        #--------------------------------------------------------

        id(a.storage()) == id(b.storage())
        #--------------------------------------------------------
        # True
        #--------------------------------------------------------

        a[1] = 100

        a
        #--------------------------------------------------------
        # tensor([  0, 100,   2,   3,   4,   5])
        #--------------------------------------------------------

        b
        #--------------------------------------------------------
        # tensor([[  0, 100,   2],
        #         [  3,   4,   5]])
        #--------------------------------------------------------

        c = a[2:]

        a.data_ptr(), c.data_ptr(), c.data_ptr() - a.data_ptr() # a和c的data_ptr只差2个元素
        #--------------------------------------------------------
        # (94065919799920, 94065919799936, 16)
        #--------------------------------------------------------

        a.dtype
        #--------------------------------------------------------
        # torch.int64
        #--------------------------------------------------------
        
        a.storage_offset(), b.storage_offset(), c.storage_offset()
        #--------------------------------------------------------
        # (0, 0, 2)
        #--------------------------------------------------------


        
        #--------------------------------------------------------
        # True
        #--------------------------------------------------------

        a.stride(), b.stride(), c.stride() # stride表示这个维度上要跳过多少个元素
        #--------------------------------------------------------
        # ((1,), (3, 1), (1,))
        #--------------------------------------------------------

        d = b[::2, ::2] # 隔2个取元素
        d
        #--------------------------------------------------------
        # tensor([[0, 2]])
        #--------------------------------------------------------

        d.stride()
        #--------------------------------------------------------
        # (6, 2)
        #--------------------------------------------------------
        
        id(a.storage()) == id(b.storage()) == id(c.storage()) == id(d.storage())
        #--------------------------------------------------------
        # True
        #--------------------------------------------------------

        d.is_contiguous()
        #--------------------------------------------------------
        # False
        #--------------------------------------------------------


        ```
    2. create tensor
        1. raw
            + torch.Tensor: allocate without initialization
            ```
            torch.Tensor(2, 3) 
            #--------------------------------------------
            # tensor([[9.3319e+05, 3.0651e-41, 1.4013e-45],
            #         [0.0000e+00, 0.0000e+00, 0.0000e+00]])
            #--------------------------------------------

            torch.Tensor(2, 3).dtype
            #--------------------------------------------
            # torch.float32
            #--------------------------------------------
            ```
            + torch.rand: create an array filled with random numbers generated by uniform distribution on the interval \[0, 1)
            ```
            >>> torch.rand(4)
            tensor([ 0.5204,  0.2503,  0.3525,  0.5673])
            >>> torch.rand(2, 3)
            tensor([[ 0.8237,  0.5781,  0.6879],
                    [ 0.3816,  0.7249,  0.0998]])

            ```
            + torch.ones
        2. from numpy
        ```
        np_data = np.arange(6).reshape([2,3])
        torch_data = torch.from_numpy(np_data)
        #--------------------------------------------
        #  0  1  2
        #  3  4  5
        # [torch.LongTensor of size 2x3]
        #--------------------------------------------
        ```
        3. from python list: 默认int => torch.int64, float => torch.float32
        ```
        l = [[1, 2], [3, 4]]
        tensor = torch.FloatTensor(l)
        ```
    3. Convertion: Tensor & numpy data & cuda & Varaible
        1. tensor & variable: they have almost the same interface
            + tensor => variable
            ```
            variable = Variable(tensor, requires_grad=True) 
            var2 = variable.detach() # 使用detach可以从这个变量处截断梯度
            var2.requries_grad => False
            ```
            + variable => tensor
            ```
            variable.data # variable => tensor
            ```
        2. tensor & float, int
        ```
        float(loss.cuda().data.cpu().numpy())
        #---------------------------
        # 0.5294359922409058
        #---------------------------
        ```
    4. method
        1. size()
        2. normal(): 根据means和std数组来正态分布地生成对应数据
        3. dtype: (成员): 查看类型
        4. view()
        5. squeeze, unsqueeze, expand(复制维度为1的值使得满足对应shape)
        ```
        # 广播法则: 2个不同shape的tensor相加的解决方式:
        # 1. 维度数量保持一致: 在最前面添加1 ---- unsqueeze
        # 2. 每个维度的数保持一致: 变成最大的那个 ---- expand
        In [69]: a = torch.ones(3, 2)

        In [70]: b = torch.zeros(2, 3, 1)

        In [71]: a + b
        Out[71]: 
        tensor([[[1., 1.],
                 [1., 1.],
                 [1., 1.]],

                [[1., 1.],
                 [1., 1.],
                 [1., 1.]]])

        In [72]: a.unsqueeze(0).expand(2, 3, 2) + b.expand(2, 3, 2)
        Out[72]: 
        tensor([[[1., 1.],
                 [1., 1.],
                 [1., 1.]],

                [[1., 1.],
                 [1., 1.],
                 [1., 1.]]])

        ```
        6. 逐元素操作:
            + abs, sqrt, div, exp, fmod, log, pow
            + cos, sin, asin, atan2, cosh
            + ceil, round, floor, trunc
            + clamp(input, min, max) # 超过min和max的部分截断
            + sigmoid, tanh
        7. 归并操作: 输出形状 < 输入形状, 可以全体,也可以某一个维度(dim=> 如果输入形状是(x, y, z),如果dim=0,输出(1, y, z)或者(y,z),如果dim=1, 则输出(x, 1, z)或者(x, z),是否有1取决于keep_dim是否为True)
            + mean, sum, median, mode(众数)
            + norm, dist
            + std, var
            + cumsum, cumprod
        8. 比较
            + gt, lt, ge, le, eq, ne
            + topk
            + sort
            + max, min: torch.max(tensor, dim) / torch.max(tensor1, tensor2) 两个tensor相比较大的元素
        9. 线性代数
            + trace
            + diag
            + triu, tril
            + mm, bmm
            + addmm, adddbmm, addmv
            + t(转置)
            + dot, cross
            + inverse
            + svd

2. function
    + add / add_(带下划线表示会修改本身,后面同理), mm, rand, unsqueeze(1D->2D), max(`torch.max(F.softmax(out), dim=1)`), cat(`y = torch.cat((y0, y1), 0).type(torch.LongTensor)`), manual_seed(`torch.manual_seed(1)`)
    1. nearly the same as numpy, such as `torch.add`, `torch.abs`, sin, mean and so on.
    2. `torch.mm`: matmul
    3. WARNING: In numpy, `data.dot(data2)` is matrix multiply, while in torch, `tensor.dot(tensor2)` is a dot production
    ```
    l = [[1, 2], [3, 4]]
    tensor = torch.FloatTensor(l)
    
    torch.add(tensor, tensor)
    #------------------------------------------------
    #  2  4
    #  6  8
    # [torch.FloatTensor of size 2x2]
    #------------------------------------------------

    tensor.dot(tensor)
    #------------------------------------------------
    # 30.0
    #------------------------------------------------

    torch.mm(tensor, tensor)
    #------------------------------------------------
    #   7  10
    #  15  22
    # [torch.FloatTensor of size 2x2]
    #------------------------------------------------
    
    tensor*tensor
    #------------------------------------------------
    #   1   4
    #   9  16
    # [torch.FloatTensor of size 2x2]
    #------------------------------------------------

    ```
3. variable, backward, grad, data
+ variable
    1. variable可以反向传播, tensor不能反向传播
    2. method:
        + requries_grad: Variable() default to False while middle variable depends on the calculation
        + is_leaf
        + backward(): 第一参数只有在对象不是scalar的时候才使用.比如`a = [1, 2, 3]`,那么`a.backward([5, 2, 6])`实际上调用的是`c = torch.sum([1, 2, 3] * [5, 2, 6]); c.backward()`
        ```
        a = Variable(torch.ones([2, 3]), requires_grad=True); b = Variable(torch.zeros([2, 3]), requires_grad=True); c = 10 * a + 2 * b

        aa = Variable(torch.ones([2, 3]), requires_grad=True); bb = Variable(torch.zeros([2, 3]), requires_grad=True); cc = 10 * aa + 2 * bb

        matrix = torch.randn(2, 3)

        matrix
        #-----------------------------------------------
        # tensor([[ 0.4612, -0.1106,  0.2141],
        #         [-0.5328, -1.3619, -0.8939]])
        #-----------------------------------------------

        c.backward(matrix); a.grad, b.grad
        #-----------------------------------------------
        # (tensor([[  4.6121,  -1.1059,   2.1413],
        #          [ -5.3279, -13.6189,  -8.9389]]), tensor([[ 0.9224, -0.2212,  0.4283],
        #         [-1.0656, -2.7238, -1.7878]]))
        #-----------------------------------------------

        torch.sum(matrix * cc).backward(); a.grad, b.grad
        #-----------------------------------------------
        # (tensor([[  4.6121,  -1.1059,   2.1413],
        #          [ -5.3279, -13.6189,  -8.9389]]), tensor([[ 0.9224, -0.2212,  0.4283],
        #          [-1.0656, -2.7238, -1.7878]]))
        #-----------------------------------------------

        ```
        
        + grad: backward之后只有leaf的grad保留,其他都是None.可以使用torch.autograd.grad或hook临时获取,也可以用retain_grad保留梯度
        ```
        x = Variable(torch.ones(3), requires_grad=True)
        w = Variable(torch.rand(3), requires_grad=True)
        y = x * w
        z = y.sum()

        z.backward()
        x.grad
        #-----------------------------------------------
        # tensor([0.00251, 0.94014, 0.21194])
        #-----------------------------------------------

        w.grad
        #-----------------------------------------------
        # tensor([1., 1., 1.])
        #-----------------------------------------------

        print(y.grad, z.grad)
        #-----------------------------------------------
        # None None
        #-----------------------------------------------
        
        torch.autograd.grad(z, y)
        #-----------------------------------------------
        # (tensor([1., 1., 1.]),)
        #-----------------------------------------------

        ```
        + grad_fn: z = a + b => grad_fn = <ThAddBackward at 0x7f669e8e31d0>
        ```
        In [46]: c = a + b

        In [47]: d = c * b

        In [48]: d.grad_fn
        Out[48]: <ThMulBackward at 0x7f669e90e240>

        In [49]: d.grad_fn.next_functions # next_functions代表产生我的2个节点的grad_fn.而leaf的grad_fn是None
        Out[49]: 
        ((<ThAddBackward at 0x7f669e90e048>, 0),
         (<AccumulateGrad at 0x7f669eb0fda0>, 0))

        In [50]: d.grad_fn.next_functions[0][0] == c.grad_fn
        Out[50]: True

        ```
+ backward
    1. 只有variable的结果 && 是scalar 才能backward
    2. 多个variable是允许的,本质是对每个单独元素做偏微分
+ grad
    1. variable数据格式
    2. 累加多次backward结果
+ data
    1. variable => tensor
```
from torch.autograd import Variable

# 搭建图纸,对这个variable进行BP
variable = Variable(tensor, requires_grad=True) 
#-------------------------------------------------------------
# Variable containing: # different from that of tensor
#  1  2
#  3  4
# [torch.FloatTensor of size 2x2]
#-------------------------------------------------------------

v_out = torch.mean(variable * variable)
v_out.backward()

variable.grad # variable是2x2矩阵,因为针对是每个元素的偏微分,所以就是(v1^2 + v2^2 + v3^2 +v4^2) / 4对每个v_i进行偏微分,所以是1/2*v_i
#-------------------------------------------------------------
# Variable containing:
#  0.5000  1.0000 
#  1.5000  2.0000
# [torch.FloatTensor of size 2x2]
#-------------------------------------------------------------


variable.data # variable => tensor
#-------------------------------------------------------------
#  1  2
#  3  4
# [torch.FloatTensor of size 2x2]
#-------------------------------------------------------------
```

4. activation function
+ expected a Variable argument ~~tensor~~

1. `variable_relu = F.relu(variable)`
```
model = torch.nn.Sequential(
        torch.nn.Linear(2, 10),
        torch.nn.ReLU(), # 笔记中推荐functional,但是其实Module和functional性能上没有区别,所以无所谓
        torch.nn.Linear(10, 2),
        )
#--------------------------------------
# Sequential (
#   (0): Linear (2 -> 10)
#   (1): ReLU () # different from Class method
#   (2): Linear (10 -> 2)
# )
#--------------------------------------

```
2. `variable_sigmoid = F.sigmoid(variable)`
3. `variable_tanh = F.tanh(variable)`
4. `variable_softplus = F.softplus(variable)`
5. `variable_softmax = F.softmax(variable)`: 默认对行元素进行softmax
```
F.softmax(Variable(torch.FloatTensor([[1, 2], [1, 2]])))
#-------------------------------------------------------------
# Variable containing:
#  0.2689  0.7311
#  0.2689  0.7311
# [torch.FloatTensor of size 2x2]
#-------------------------------------------------------------
```

5. GPU
    1. check if our GPU driver & CUDA is enabled
    ```
    torch.cuda.is_available()
    ```
    2. convert CPU version to GPU version
        1. 数据集的x和y迁移到cuda上
        2. 模型迁移到cuda上
        3. 每隔多少时间进行的y_pred计算也可以迁移到cuda上
        ```
        test_x = torch.unsqueeze(test_dataset.test_data, dim=1).type(torch.FloatTensor)[:2000].cuda()/255.
        test_y = test_dataset.test_labels[:2000].cuda() # x = x.cuda(0)表示迁移到第0块GPU上
        # ...
        model = CNN()
        model = model.cuda()

        # ...
        for step ,(x_batch, y_batch) in enumerate(train_loader):
            x_batch = Variable(x_batch).cuda()
            y_batch = Variable(y_batch).cuda()

        # ...
        if step % 50 == 0:
            test_output = model(test_x)
            pred_y = torch.max(test_output, 1)[1].cuda().data
        ```
    3. create tensor
    ```
    x = torch.cuda.FloatTensor([[1, 2], [3, 4]])
    ```
    4. 判断: 通过`torch.tensor`来从python中获得tensor
    ```
    if args.cuda and torch.cuda.is_available():
        torch.set_default_tensor_type('torch.cuda.FloatTensor')
    else:
        torch.set_default_tensor_type('torch.FloatTensor')
    ```
    5. save & load
    ```
    In [3]: a = a.cuda(0)

    In [4]: torch.save(a, 'a.pth')

    In [5]: b = torch.load('a.pth')

    In [6]: b
    Out[6]: 
    tensor([[-0.0000,  0.0000, -0.0000],
            [ 0.0000,  0.0000,  0.0000]], device='cuda:0')

    In [7]: a
    Out[7]: 
    tensor([[-0.0000,  0.0000, -0.0000],
            [ 0.0000,  0.0000,  0.0000]], device='cuda:0')

    In [8]: c = torch.load('a.pth', map_location=lambda storage, loc: storage)

    In [9]: c
    Out[9]: 
    tensor([[-0.0000,  0.0000, -0.0000],
            [ 0.0000,  0.0000,  0.0000]])

    In [11]: c.device
    Out[11]: device(type='cpu')

    In [12]: d = torch.load('a.pth', map_location={'cuda: 0':'cuda: 1'}) # gpu0 => gpu1

    In [13]: d
    Out[13]: 
    tensor([[-0.0000,  0.0000, -0.0000],
            [ 0.0000,  0.0000,  0.0000]], device='cuda:0') # gpu 1 is unavaliable so d is still on the gpu 0
    ```
    
6. fc
    1. structure
    + weight, bias: 可以自己初始化
        1. get
        ```
        In [106]: fc = torch.nn.Linear(3, 5)

        In [107]: fc.weight.data
        Out[107]: 
        tensor([[-0.20275,  0.45114,  0.37249],
                [-0.00234,  0.47446,  0.12859],
                [-0.33050,  0.33825, -0.05387],
                [ 0.26576, -0.06059, -0.15322],
                [ 0.08199, -0.17242, -0.53074]])

        In [108]: fc.bias.data
        Out[108]: tensor([ 0.03640,  0.56060, -0.40017,  0.05474, -0.03544])

        ```
        2. set (init): 对于(3, 4)的全连接层`fc = torch.nn.Linear(3, 4)`来说,以xavier_normal来举例
            1. method 1
            ```
            torch.manual_seed(1)
            torch.nn.init.xavier_normal(fc.weight)
            #--------------------------------------------------------------
            # Parameter containing:
            # tensor([[ 0.3535,  0.1427,  0.0330],
            #         [ 0.3321, -0.2416, -0.0888],
            #         [-0.8140,  0.2040, -0.5493],
            #         [-0.3010, -0.4769, -0.0311]], requires_grad=True)
            #--------------------------------------------------------------
            ```
            2. method 2
            ```
            torch.manual_seed(1)

            std = math.sqrt(2) / math.sqrt(7.)

            fc.weight.data.normal_(0, std)
            #--------------------------------------------------------------
            # tensor([[ 0.3535,  0.1427,  0.0330],
            #         [ 0.3321, -0.2416, -0.0888],
            #         [-0.8140,  0.2040, -0.5493],
            #         [-0.3010, -0.4769, -0.0311]])
            #--------------------------------------------------------------

            ```
    2. regression
    ```
    import torch
    import numpy as np
    import torch.nn.functional as F
    from torch.autograd import Variable
    import matplotlib.pyplot as plt

    batch_size = 200

    x = torch.unsqueeze(torch.linspace(-5, 5, batch_size), dim=1) # tensor
    y = x.pow(2) + torch.rand(x.size()) # tensor

    x, y = Variable(x), Variable(y) # cast tensor to variable

    class NN(torch.nn.Module):
        def __init__(self, input_size, hidden_size, output_size):
            super(NN, self).__init__()
            self.fc1 = torch.nn.Linear(input_size, hidden_size)
            self.fc2 = torch.nn.Linear(hidden_size, output_size)

        def forward(self, net): # not underlined
            net = F.relu(self.fc1(net))
            net = self.fc2(net)
            return net

    model = NN(1, 10, 1)
    #print(model)
    #----------------------------------------
    # NN (
    #   (fc_1): Linear (1 -> 10)
    #   (fc_2): Linear (10 -> 1)
    # )
    #----------------------------------------

    optimizer = torch.optim.SGD(model.parameters(), lr=0.01)
    loss_fn = torch.nn.MSELoss()

    plt.ion() # 实时画图
    plt.show()

    for epoch in range(1000 + 1):
        y_pred = model(x)
        loss = loss_fn(y_pred, y)

        if epoch % 20 == 0:
            plt.cla() # clear the current axes
            plt.scatter(x.data.numpy(), y.data.numpy())
            plt.plot(x.data.numpy(), y_pred.data.numpy(), 'r-', lw=5)
            plt.text(0.5, 0, 'Loss=%.4f' % loss.data[0], fontdict={"size": 20, "color": "red"})
            plt.pause(0.1) # pause

        optimizer.zero_grad() # clear the current grads
        loss.backward() 
        optimizer.step() # update the grads

    plt.ioff()
    plt.show() # if commented, then the picture box will disappear when the program finishes
    ```
    3. classification
    ```
    import torch
    import numpy as np
    import torch.nn.functional as F
    from torch.autograd import Variable
    import matplotlib.pyplot as plt

    batch_size = 200

    n_data = torch.ones(100, 2)
    x0 = torch.normal(2 * n_data, 1)
    y0 = torch.zeros(100)
    x1 = torch.normal(-2 * n_data, 1)
    y1 = torch.ones(100)
    x = torch.cat((x0, x1), 0).type(torch.FloatTensor) # default type
    y = torch.cat((y0, y1), 0).type(torch.LongTensor) # labels must be int

    x, y = Variable(x), Variable(y) # cast tensor to variable

    #plt.scatter(x.data.numpy()[:, 0], x.data.numpy()[:, 1], c=y.data.numpy())
    #plt.show()

    class NN(torch.nn.Module):
        def __init__(self, input_size, hidden_size, output_size):
            super(NN, self).__init__()
            self.fc1 = torch.nn.Linear(input_size, hidden_size)
            self.fc2 = torch.nn.Linear(hidden_size, output_size)

        def forward(self, net): # not underlined
            net = F.relu(self.fc1(net))
            net = self.fc2(net)
            return net

    model = NN(2, 10, 2)

    optimizer = torch.optim.SGD(model.parameters(), lr=0.003)
    loss_fn = torch.nn.CrossEntropyLoss() # shape of argument? after activation fn

    plt.ion() # 实时画图
    plt.show()

    for epoch in range(1000 + 1):
        out = model(x) # (-infty, infty)
        loss = loss_fn(F.softmax(out), y)
        y_pred = torch.squeeze(torch.max(F.softmax(out), dim=1)[1]) # the output of torch.max is a tuple with (max_values, max_indices)

        if epoch % 20 == 0:
            plt.cla() # clear the current axes
            #plt.scatter(x.data.numpy(), y.data.numpy())
            plt.scatter(x.data.numpy()[:, 0], x.data.numpy()[:, 1], c=y_pred.data.numpy())
            #plt.plot(x.data.numpy(), y_pred.data.numpy(), 'r-', lw=5)
            plt.text(0.5, 0, 'Loss=%.4f' % loss.data[0], fontdict={"size": 20, "color": "red"})
            plt.pause(0.1) # pause

        optimizer.zero_grad() # clear the current grads
        loss.backward() 
        optimizer.step() # update the grads

    plt.ioff()
    plt.show() # if commented, then the picture box will disappear when the program finishes
    ```


7. loss function
    1. torch.nn.MSELoss
        1. `loss = loss_fn(网络的最后层未激活的输出, y)`
    2. torch.nn.CrossEntropyLoss
        1. `loss = loss_fn(F.softmax(网络的最后层未激活的输出), y)`: `[batch_size, num_classes]` & `[batch_size]`
    3. `F.cross_entropy` <=> `F.nll_loss`
    ```
    input = torch.randn(3, 5, requires_grad=True)
    target = torch.LongTensor([1, 0, 4])
    
    F.nll_loss(F.log_softmax(input), target)
    #--------------------------------------------------------------
    # tensor(2.6430, grad_fn=<NllLossBackward>)
    #--------------------------------------------------------------
    
    F.cross_entropy(F.log_softmax(input), target)
    #--------------------------------------------------------------
    # tensor(2.6430, grad_fn=<NllLossBackward>)
    #--------------------------------------------------------------
    
    ```

8. save & load: [reference codes](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/pytorch/save_load.py)
    1. the whole model
    ```
    # model = torch.nn.Sequential(...)
    torch.save(model, "model.pkl")
    
    ####################################################
    
    model = torch.load("model.pkl")
    ```
    2. the params of the model
    ```
    # model = torch.nn.Sequential(...)
    torch.save(model.state_dict(), "model_params.pkl")
    
    ####################################################
    model = torch.nn.Sequential(...)
    model.load_state_dict(torch.load("model_params.pkl"))
    ```
    3. class
    ```
    class Net(torch.nn.Module):
        def __init__(self):
            super(Net, self).__init__()
            # ...
            self.model_name = str(type(self)).split('.')[-1].split('\'')[0]

        def forward(self, net): # not underlined
            # ...

        def load(self, path):
            self.load_state_dict(torch.load(path))

        def save(self, name=None): # default name: Net_0330_13:13:03.pth
            if name is None:
                prefix = 'checkpoints/' + self.model_name + '_'
                name = time.strftime(prefix + '%m%d_%H:%M:%S.pth')
            torch.save(self.state_dict(), name)
            return name

    # ...
    if len(os.listdir('checkpoints')) != 0:
        print('loading')
        model.load(os.path.join('checkpoints', os.listdir('checkpoints')[0]))
    # ...
    if global_step % 1000 == 0:
        print('saving...')
        model.save()
    ```

9. dataset
    1. TensorDataset
    + 不够batch_size就只返回剩余的 
    + num_workers(几个子进程提取): 提高batch_size和num_workers才能更快地提高效率
    ```
    import torch
    import torch.utils.data as Data

    batch_size = 5

    x = torch.linspace(1, 10, 10)
    y = x * 10

    dataset = Data.TensorDataset(x, y)
    dataset = Data.DataLoader(
            dataset = dataset,
            batch_size = batch_size,
            shuffle = True,
            num_workers = 2,
            )

    for epoch in range(3):
        for step, (x_batch, y_batch) in enumerate(dataset):
            # x_batch, y_batch = Varaible(x_batch), Variable(y_batch)
            print("epoch: {0} step:{1}  {2} {3}".format(epoch, step, x_batch, y_batch))


    #---------------------------------------------------------------------
    # epoch: 0 step:0  [5. 9. 4. 3. 1.] [50. 90. 40. 30. 10.]
    # epoch: 0 step:1  [ 7.  8.  6. 10.  2.] [ 70.  80.  60. 100.  20.]
    # epoch: 1 step:0  [3. 4. 6. 1. 9.] [30. 40. 60. 10. 90.]
    # epoch: 1 step:1  [10.  8.  2.  5.  7.] [100.  80.  20.  50.  70.]
    # epoch: 2 step:0  [2. 1. 5. 7. 9.] [20. 10. 50. 70. 90.]
    # epoch: 2 step:1  [10.  8.  4.  6.  3.] [100.  80.  40.  60.  30.]
    #---------------------------------------------------------------------
    ```
    
    2. Dataset Class
    ```
    from torch.utils.data import Dataset, DataLoader
    
    class SVCDataset(Dataset):
        def __init__(self, filenames):
            self.filenames = filenames # U1S1

        def __getitem__(self, index):
            path = self.filenames[index]
            img = np.stack([cv2.imread(os.path.join(images_dir, path + '_' + str(i) + '.png'), cv2.IMREAD_GRAYSCALE) for i in range(7)], 0)
            img = torch.Tensor(img)

            if int((os.path.basename(path)).split('_')[0].split('S')[-1]) <= 20:
                label = [1]
            else:
                label = [0]
            label = torch.Tensor(label)

            return img, label

        def __len__(self):
            return len(self.filenames)


    train_loader = DataLoader(SVCDataset(filenames=filenames),  batch_size=16, shuffle=True) #
    
    for step, (img, label) in enumerate(train_loader): # call 
        if torch.cuda.is_available():
            x_batch = Variable(img).cuda()
            y_batch = Variable(label).cuda()
        else:
            x_batch = Variable(img)
            y_batch = Variable(label)


    ```
    3. ImageFolder: root下有若干个folder，folder名为label，里面的图片就是data
    ```
    # tree tmp/
    #--------------------------------------------------------
    # tmp/
    # ├── cat
    # │   ├── 01.jpg
    # ├── dog
    # │   ├── 01.jpg
    # └── fox
    #     ├── 01.jpg
    #--------------------------------------------------------
    from torchvision.datasets import ImageFolder
    transforms = T.Compose([
        T.Resize(224),
        T.CenterCrop(224),
        T.ToTensor(),
        T.Normalize(mean=[.5, .5, .5], std=[.5, .5, .5]),
        ])

    dataset = ImageFolder('tmp', transform=transforms) # 必须要transforms转换成tensor
    dataloader = Data.DataLoader(
        dataset = dataset,
        batch_size = 4,
        shuffle = True,
        num_workers = 1,
        )
    for img, label in dataloader:
        print(label)
        print(img.shape)
        break
    #--------------------------------------------------
    # tensor([2, 0, 1, 1])
    # torch.Size([4, 3, 224, 224])
    #--------------------------------------------------
    ```
    

10. optimizer
```
opt = torch.optim.SGD(model.parameters(), lr=learning_rate)
opt = torch.optim.SGD(model.parameters(), lr=learning_rate, momentum=0.8)
opt = torch.optim.RMSprop(model.parameters(), lr=learning_rate, alpha=0.9)
opt = torch.optim.Adam(model.parameters(), lr=learning_rate, betas=(0.9, 0.99))
```


11. torchvision.dataset
    1. MNIST
    ```
    dataset_dir = "/home/orris/dataset/torchvision/mnist"

    train_dataset = torchvision.datasets.MNIST(
            root = dataset_dir,
            train = True, # True: training data; False: testing data
            transform = T.Compose([
                       T.ToTensor(),
                       T.Normalize((0.1307,), (0.3081,))
                   ]),
            download = False, # whether download or not
            )



    train_loader = Data.DataLoader(dataset = train_dataset, batch_size = batch_size, shuffle = True, num_workers = 2)

    for (imgs, labels) in train_loader: # train_data is a list with length 2. [image data, image label]
        plt.figure()
        plt.imshow(images[0].numpy().squeeze(), cmap="gray")
        plt.show()

    ```
    2. CIFAR-10
    ```
    dataset_dir = "cifar10/"

    train_dataset = torchvision.datas   ets.CIFAR10(
            root = dataset_dir,
            train = True, # True: training data; False: testing data
            transform = torchvision.transforms.ToTensor(), # ndarray => torch tensor
            download = False, # whether download or not
            )

    train_loader = Data.DataLoader(dataset = train_dataset, batch_size = batch_size, shuffle = True, num_workers = 2)

    for (imgs, labels) in train_loader: # train_data is a list with length 2. [image data, image label]
        print(imgs.shape)
        break
        #---------------------------------------------------
        # torch.Size([32, 3, 32, 32])
        #---------------------------------------------------
    ```

12. CNN: (batch_size, num_channels, height, width)
    1. CNN
    + 长宽不变: (5, 1, 2)
    + 长宽变1/2: (4, 2, 1)
    + 长宽变1/3: (5, 3, 1)
    ```
    class CNN(torch.nn.Module):
        def __init__(self):
            super(CNN, self).__init__()
            self.conv1 = torch.nn.Sequential( # (batch_size, 1, 28, 28)
                    torch.nn.Conv2d(
                        in_channels = 1,
                        out_channels = 16,
                        kernel_size = 5,
                        stride = 1,
                        padding = 2),
                    torch.nn.ReLU(),
                    torch.nn.MaxPool2d(kernel_size = 2), # (batch_size, 16, 14, 14)
                    )
            self.conv2 = torch.nn.Sequential(
                    torch.nn.Conv2d(
                        in_channels = 16,
                        out_channels = 32,
                        kernel_size = 5,
                        stride = 1,
                        padding = 2),
                    torch.nn.ReLU(),
                    torch.nn.MaxPool2d(kernel_size = 2), # (batch_size, 32, 7, 7)
                    )
            self.fc1 = torch.nn.Linear(32 * 7 * 7, 10)

        def forward(self, net): # not underlined
            net = self.conv1(net) # net is a tensor
            net = self.conv2(net)
            net = net.view(net.size(0), -1) # reshape. view is the method of tensor which works like reshape
            net = self.fc1(net)
            return net
    ```
    2. CNN transpose:
    + 长宽变2倍: (4, 2, 1)
    + 长宽变3倍: (5, 3, 1)
    ```
    img.shape
    #--------------------------------------------------
    # torch.Size([4, 16, 224, 224])
    #--------------------------------------------------
    cnn_transpose = torch.nn.ConvTranspose2d(
        in_channels = 16,
        out_channels = 3,
        kernel_size = 4,
        stride = 2,
        padding = 1)
    cnn_transpose(img)
    #--------------------------------------------------
    # torch.Size([4, 3, 448, 448])
    #--------------------------------------------------
    ```

13. RNN: 支持变长的time_steps
    1. LSTM: !!!IMPORTANT: The states must be given, otherwise states will be initialized to zeros, which has a terrible effect if we call each timestep!!!
    + `batch_first`: True: (batch_size, time_steps, input_size); False: (time_steps, batch_size, input_size)
    + `net_out[:, -1, :]`: 选取最后一个时刻的输出 (batch_size, time_steps, input_size)
    + `None`: initial states. !!!IMPORTANT!!!
    + `h_n`和`h_c`: 好像是进程相关的?
    ```
    class LSTM(torch.nn.Module):
        def __init__(self, input_size, hidden_size, num_classes):
            super(LSTM, self).__init__()
            self.lstm = torch.nn.LSTM(input_size=input_size, hidden_size=hidden_size, num_layers=1, batch_first=True)
            self.fc = torch.nn.Linear(hidden_size, num_classes)

        def forward(self, net): # net: (batch_size, 1, 28, 28)
            net_out, states = self.lstm(net, None) # net_out: (batch_size, net.shape[1], hidden_size); states: a tuple of (hidden_state, cell_state)
            net = net_out[:, -1, :]
            net = self.fc1(net)
            return net
    ```
    2. RNN
    ```
    class RNN(torch.nn.Module):
        def __init__(self, input_size, hidden_size, num_classes):
            super(RNN, self).__init__()
            self.rnn1 = torch.nn.RNN(input_size=input_size, hidden_size=hidden_size, num_layers=1, batch_first=True)
            self.fc1 = torch.nn.Linear(hidden_size, num_classes)

        def forward(self, net, h_state): # net: (batch_size, 1, time_steps, 1)
            # 传递time_steps个timestep的时候,每个timestep输出的结果就是这个sin_y对应的cos_y
            net_out, h_state = self.rnn1(net, h_state) # net_out: [batch_size, time_steps, hidden_size]
            outs = []
            for time_step in range(net.size(1)):
                outs.append(self.fc1(net_out[:, time_step, :])) # outs: [time_steps, 1, 1] (list)

            return torch.stack(outs, dim=1), h_state # torch.stack(outs, dims=1): [1, time_steps, 1] (FloatTensor)
            
            
    # ...
    h_state = None
    for ...:
        y_pred, h_state = model(x, h_state)
        h_state = h_state.data # !! important
    ```
    3. utils
    + jieba
    + pack_padded_sequence(输入padded_sentences和lengths) & pad_packed_sequence(取出padded_sentences和lengths): 根据lengths把input的前2个维度给提取成为1个维度,也就是说,如果input.shape是`(2, 3, 10)`,而`lengths=[3, 1]`指明这个batch里第1个item有效的是3个,而第2个item有效的是1个,所以pack后的第1个元素的shape就是`(4, 10)`,其中4就是这个batch里的第1个item的有效个数+第2个item有效个数
    
    ```
    import jieba
    from torch.nn.utils.rnn import pack_padded_sequence, pad_packed_sequence
    
    seg_list = jieba.cut('我是一个好人')
    #-------------------------------------------------
    # '/'.join(seg_list) # '我/是/一个/好人'
    #-------------------------------------------------
    sen1 = [1] * 3
    sen2 = [2] * 4
    sen3 = [3] * 9
    sen4 = [4] * 5
    sentences = [sen1, sen2, sen3, sen4]
    sentences = sorted(sentences, key=lambda x:len(x), reverse=True) # 按len从大到小排序

    lengths = [5 if len(sen) > 5 else len(sen) for sen in sentences] # 以5截断
    #-------------------------------------------------
    # [5, 5, 4, 3]
    #-------------------------------------------------
    
    def pad_sen(sen, length=5, padded_num=0): # 给sentence补0直到len达到length
        origin_len = len(sen)
        padded_sen = sen[:length]
        padded_sen = padded_sen + [padded_num for _ in range(origin_len, length)]
        return padded_sen
        
    pad_sentences = [pad_sen(sen) for sen in sentences]
    #-------------------------------------------------
    # [[3, 3, 3, 3, 3], [4, 4, 4, 4, 4], [2, 2, 2, 2, 0], [1, 1, 1, 0, 0]]
    #-------------------------------------------------
    pad_tensor = t.Tensor(pad_sentences).long()
    pad_variable = t.autograd.Variable(pad_tensor)
    
    # EMBEDDINGS
    embedding = nn.Embedding(5, 2)
    pad_embeddings = embedding(pad_variable)
    pad_embeddings.shape # (5, 4, 2)

    # PACKING
    packed_variable = pack_padded_sequence(pad_embeddings, lengths, batch_first=True)
    #-------------------------------------------------
    # PackedSequence(data=tensor([[-0.5107, -1.0299],
    #         [-1.0802,  1.8094],
    #         [ 1.6394,  0.9828],
    #         [-0.7368,  1.0198],
    #         [-0.5107, -1.0299],
    #         [-1.0802,  1.8094],
    #         [ 1.6394,  0.9828],
    #         [-0.7368,  1.0198],
    #         [-0.5107, -1.0299],
    #         [-1.0802,  1.8094],
    #         [ 1.6394,  0.9828],
    #         [-0.7368,  1.0198],
    #         [-0.5107, -1.0299],
    #         [-1.0802,  1.8094],
    #         [ 1.6394,  0.9828],
    #         [-0.5107, -1.0299],
    #         [-1.0802,  1.8094]], grad_fn=<PackPaddedBackward>), batch_sizes=tensor([4, 4, 4, 3, 2], grad_fn=<PackPaddedBackward>))
    #-------------------------------------------------
    
    
    rnn = t.nn.LSTM(2, 3)
    output, hn = rnn(packed_variable)
    output = pad_packed_sequence(output)
    output[0].shape
    #---------------------------------------------------------------
    # torch.Size([5, 4, 3])
    #---------------------------------------------------------------
    
    output[1]
    #---------------------------------------------------------------
    # tensor([5, 5, 4, 3])
    #---------------------------------------------------------------
    ```
    4. LSTMCell: 输入是(batch_size, input_size), 而输出的就是2个(batch_size, lstm_size),即没有num_timesteps的维度
    ```
    self.rnn_cell = nn.LSTMCell(self.encoder_dim + embedding_size, lstm_size, bias=True)
    
    hidden_state, cell_state = self.rnn_cell(net, (hidden_state, cell_state)) # (batch_size, lstm_size), (batch_size, lstm_size)
    ```


14. PyTorch代码实战
    1. [CNN实现MNIST](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/pytorch/cnn_mnist.py)
    2. [RNN实现MNIST](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/pytorch/rnn_mnist.py)
    3. [save load实现](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/pytorch/save_load.py)
    4. [RNN实现sin到cos的预测](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/pytorch/rnn_sin_cos.py): 当前若干个sin的值,放入到RNN中,RNN的每个timestep都会输出预测的cos值,把这些积累起来就可以了
    5. [GAN实现曲线模拟](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/pytorch/basic_gan.py)
    6. [CNN实现MNIST的GPU版本](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/pytorch/cnn_mnist_gpu.py)
    7. [dropout实现曲线模拟](https://github.com/orris27/orris/blob/master/python/machine-leaning/codes/pytorch/dropout.py)
    
15. dtype & device
```
>>> tensor = torch.randn(2, 2)  # Initially dtype=float32, device=cpu
>>> tensor.to(torch.float64)
tensor([[-0.5044,  0.0005],
        [ 0.3310, -0.0584]], dtype=torch.float64)

>>> cuda0 = torch.device('cuda:0')
>>> tensor.to(cuda0)
tensor([[-0.5044,  0.0005],
        [ 0.3310, -0.0584]], device='cuda:0')

>>> tensor.to(cuda0, dtype=torch.float64)
tensor([[-0.5044,  0.0005],
        [ 0.3310, -0.0584]], dtype=torch.float64, device='cuda:0')

>>> other = torch.randn((), dtype=torch.float64, device=cuda0)
>>> tensor.to(other, non_blocking=True)
tensor([[-0.5044,  0.0005],
        [ 0.3310, -0.0584]], dtype=torch.float64, device='cuda:0')

```

16. tricks
    1. dropout
    + Dropout可以放在linear和relu之间或者relu后面,自己看情况就好了
    + 预测的时候要先调用`eval()`, 然后直接`model(x)`,最后调用`train()`变回原来的样子
    ```
    model_dropped  = torch.nn.Sequential(
        torch.nn.Linear(1, hidden_size1),
        torch.nn.Dropout(0.5),
        torch.nn.ReLU(), 
        )


    y_pred_dropped = model_dropped(x)
    loss_dropped = loss_fn(y_pred_dropped, y)
    opt_dropped.zero_grad() # clear the current grads
    loss_dropped.backward()
    opt_dropped.step() # update the grads

    if epoch % 20 == 0:
        model_dropped.eval()
        y_pred_dropped = model_dropped(x_test)
        # ...
        print('epoch %d: loss=%.4f loss_dropped=%.4f' % (epoch, loss_fn(y_pred, y_test).data.numpy(), loss_fn(y_pred_dropped, y_test).data.numpy()))
        # ...
        model_dropped.train()
    ```
    2. batch normalization: 貌似bn+activation的顺序比较好?毕竟bn本来可以减少sigmoid的梯度消失问题
    + BatchNorm1d (Linear), BatchNorm2d(CNN)
    ```
    self.conv1 = torch.nn.Sequential(
            # input: (batch_size, 7, height=128, width=w)
            torch.nn.Conv2d(
                in_channels = 7,
                out_channels = 32,
                kernel_size = 3,
                padding = 1,
                stride = 1),
            torch.nn.BatchNorm2d(32), # = output number of channels
            torch.nn.LeakyReLU(),
            # output: (batch_size, 32, 128, w)
            torch.nn.MaxPool2d(kernel_size = 2),
            ... )

    ```


17. other:
    1. torch.set_printoptions设置打印tensor的精度和格式
    ```
    In [17]: a = torch.rand(3, 2)
    
    In [18]: a
    Out[18]: 
    tensor([[0.4265, 0.8119],
            [0.9905, 0.9936],
            [0.3345, 0.8607]])

    In [19]: torch.set_printoptions(precision=5)

    In [20]: a
    Out[20]: 
    tensor([[0.42649, 0.81189],
            [0.99050, 0.99361],
            [0.33448, 0.86070]])

    ```
    2. 向量化的计算方式比python的for循环快
    ```
    In [21]: x = torch.ones(100)

    In [22]: y = torch.zeros(100)

    In [23]: def for_add(x, y):
        ...:     result = list()
        ...:     for i, j in zip(x, y):
        ...:         result.append(i + j)
        ...:     return torch.Tensor(result)
        ...: 

    In [24]: %timeit x + y
    1.51 µs ± 9.31 ns per loop (mean ± std. dev. of 7 runs, 1000000 loops each)

    In [25]: %timeit for_add(x, y)
    456 µs ± 3.89 µs per loop (mean ± std. dev. of 7 runs, 1000 loops each)
    ```
    3. nn.Module & nn.functional:
    + Dropout和有学习参数的选择nn.Module => 区分train()和eval()
    + 没有学习参数的选择nn.functional



18. transforms: 必须使用PIL的Image.open格式读取图片
    1. 常用的函数
    + Resize = Scale: 把最短的一个边变成指定的长度
    + CenterCrop: 从图片中间切出指定长度的图片
    + RandomAffine: 可以随机旋转或平移,给定的参数表示的是最大值
    ```
    T.RandomAffine(degrees=0, translate=(0.5, 0), fillcolor=(0, 0, 0)) # 旋转0度,左右随机平移最大50%,默认填充是黑色(根据需要更改,如果是单色图,则不用tuple)
    ```
    + RandomHorizontalFlip, RandomVerticalFlip: 对PIL图像随机水平/垂直翻转
    ```
    img = T.RandomHorizontalFlip()(img)
    ```
    + RandomRotation(45): 随机旋转45度, 会出现黑边(45度的情况下)
    + RandomCrop: 将PIL图像的img随机切个30*30的局部
    ```
    img = T.RandomCrop(30)(img)  # 将PIL图像的img随机切个30*30的局部
    #-------------------------------------------------------------------------------------------
    # <PIL.Image.Image image mode=RGB size=30x30 at 0x7F29FC591EB8>
    #-------------------------------------------------------------------------------------------
    ```
    + ToTensor: Converts a PIL Image or numpy.ndarray (H x W x C) in the range `[0, 255]` to a torch.FloatTensor of shape (C x H x W) in the range `[0.0, 1.0]`.
    + Normalize: Normalize a tensor image with mean and standard deviation. Given mean: ``(M1,...,Mn)`` and std: ``(S1,..,Sn)`` for ``n`` channels, this transform will normalize each channel of the input ``torch.*Tensor`` i.e. ``input[channel] = (input[channel] - mean[channel]) / std[channel]``: .5指定的是每个channel,所以grayscale图使用`T.Normalize(mean=[.5], std=[.5])`
    ```
    img = Image.open('Pictures/wallpapers/1.jpg')
    #-----------------------------------------------------------------------------------------
    # <PIL.JpegImagePlugin.JpegImageFile image mode=RGB size=2181x1240 at 0x7F29FCB45828>
    #-----------------------------------------------------------------------------------------
    img = T.Resize(224)(img)
    #-----------------------------------------------------------------------------------------
    # <PIL.Image.Image image mode=RGB size=393x224 at 0x7F29FCB349E8>
    #-----------------------------------------------------------------------------------------
    
    img = T.CenterCrop(224)(img)
    #-----------------------------------------------------------------------------------------
    # <PIL.Image.Image image mode=RGB size=224x224 at 0x7F29FC9D5320>
    #-----------------------------------------------------------------------------------------
    
    img = T.ToTensor()(img) # (H, W, C) => (C, H, W) # 转换PIL图片到Tensor,并且归一化到[0, 1]
    #-----------------------------------------------------------------------------------------
    # tensor([[[0.1020, 0.1020, 0.1020,  ..., 0.2510, 0.1647, 0.1176],
    #          [0.1020, 0.1020, 0.1020,  ..., 0.2667, 0.2118, 0.1686],
    #          [0.1020, 0.1020, 0.1020,  ..., 0.2471, 0.2549, 0.2353],...)
    #-----------------------------------------------------------------------------------------
    
    img = T.Normalize(mean=[.5, .5, .5], std=[.5, .5, .5])(img) # 归一化到[-1, 1]之间
    #-----------------------------------------------------------------------------------------
    # tensor([[[-0.7961, -0.7961, -0.7961,  ..., -0.4980, -0.6706, -0.7647],
    #          [-0.7961, -0.7961, -0.7961,  ..., -0.4667, -0.5765, -0.6627],
    #          [-0.7961, -0.7961, -0.7961,  ..., -0.5059, -0.4902, -0.5294], ... )
    #-----------------------------------------------------------------------------------------
    ```
    2. 常用组合
    ```
    imsize = 224
    transforms = {
        'train': T.Compose([
            T.RandomResizedCrop(imsize),
            T.RandomHorizontalFlip(),
            T.ToTensor(),
            T.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
        ]),
        'val': T.Compose([
            T.Resize(imsize),
            T.CenterCrop(imsize),
            T.ToTensor(),
            T.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
        ]),
    }

    ```



19. torchvision.utils
    1. save image
    ```
    G.eval()
    D.eval()
    print('saving...')
    fake_image = G(fix_noises)
    scores = D(fake_image).detach()
    scores = scores.data.squeeze()
    indexs = scores.topk(32)[1]
    result = list()
    for index in indexs:
        result.append(fake_image.data[index])

    torchvision.utils.save_image(torch.stack(result), 'figures/result_{}.png'.format(epoch), normalize=True, range=(-1, 1))
    # tv.utils.save_image(torch.randn(16, 3, 300, 400), 'result.png', normalize=True, range=(-1, 1))
    G.train()
    D.train()
    ```


20. initialize weights:
    1. for torch.nn.Module object
    ```
    def init_weights(m):
        class_name = m.__class__.__name__
        if class_name.find('Conv') != -1:
            m.weight.data.normal_(0, 0.02)
        elif class_name.find('Norm') != -1:
            m.weight.data.normal_(1.0, 0.02)

    G.apply(init_weights)
    D.apply(init_weights)
    ```
    2. directly
    ```
    self.embeddings = nn.Embedding(vocab_size, embedding_size)
    self.embeddings.weight.data.uniform_(-0.1, 0.1)
    ```
    3. load weight
    ```
    def load_pretrained_embeddings(self, embeddings):
        self.embeddings.weight = nn.Parameter(embeddings)
    ```
21. clamp weights
```
for param in D.parameters():
    param.data.clamp_(-clamp_num, clamp_num)
```


22. torchvision.models: 
    1. basic usage: The input images have to be loaded in to a range of `[0, 1]` and then normalized using `mean=[0.485, 0.456, 0.406]` and `std=[0.229, 0.224, 0.225]`. See details in [this](https://pytorch.org/docs/master/torchvision/models.html).
        1. 1st method
        ```
        from torchvision.models import resnet50
        
        img = Image.open('surf.jpg')
        transforms = T.Compose([
            T.ToTensor(),
            T.Normalize(mean=[0.485, 0.456, 0.406],std=[0.229, 0.224, 0.225])
            ])
        img = transforms(img)
        from torchvision.models import resnet50
        model = resnet50(pretrained=True)
        del model.fc
        model.fc = lambda x: x # 用直接映射覆盖原来的全连接层
        output = model(img.unsqueeze(0))
        output.shape
        #-----------------------------------------------------
        # torch.Size([16, 2048])
        #-----------------------------------------------------
        ```
        2. 2nd method: [reference](https://github.com/parksunwoo/show_attend_and_tell_pytorch/blob/master/model.py)
        ```
        resnet = models.resnet101(pretrained=True)
        modules = list(resnet.children())[:-1] # -1 or -2 depends on your model
        self.resnet = nn.Sequential(*modules)
        #...
        with torch.no_grad():
            features = self.resnet(imgs)
        # OR
        def fine_tune(self, fine_tune=True):
            for p in self.resnet.parameters():
                p.requires_grad = False
            for c in list(self.resnet.children())[5:]:
                for p in c.parameters():
                    p.requires_grad = fine_tune
        fine_tune()
        ```
    2. children: in_features, out_features
    ```
    from torchvision.models import resnet152
    
    resnet = resnet152(pretrained=True)
    
    list(resnet.children())[-2]
    #----------------------------------------------------------
    # AvgPool2d(kernel_size=7, stride=1, padding=0)
    #----------------------------------------------------------

    list(resnet.children())[-1]
    #----------------------------------------------------------
    # Linear(in_features=2048, out_features=1000, bias=True)
    #----------------------------------------------------------
    ```


23. multinomial: 执行num_samples次采样,其中`weights[i]`表示i被采样的概率是`weights[i]`,如果`replacement=False`(default)就不放回,采样完weights里的所有有权重的下标后就结束,权重为0的采样结果暂时不太清楚
```
weights = torch.Tensor([0, 10, 3, 0])

torch.multinomial(weights, 4)
#---------------------------------------------------------
# tensor([1, 2, 0, 0])
#---------------------------------------------------------

torch.multinomial(weights, 4)
#---------------------------------------------------------
# tensor([1, 2, 0, 0])
#---------------------------------------------------------

torch.multinomial(weights, 4)
#---------------------------------------------------------
# tensor([1, 2, 0, 0])
#---------------------------------------------------------

torch.multinomial(weights, 4)
#---------------------------------------------------------
# tensor([2, 1, 0, 0])
#---------------------------------------------------------

torch.multinomial(weights, 4)
#---------------------------------------------------------
# tensor([1, 2, 0, 0])
#---------------------------------------------------------

weights = torch.Tensor([0, 10, 3, 0])

torch.multinomial(weights, 4, replacement=True)
#---------------------------------------------------------
# tensor([1, 1, 2, 2])
#---------------------------------------------------------

torch.multinomial(weights, 4, replacement=True)
#---------------------------------------------------------
# tensor([1, 1, 1, 1])
#---------------------------------------------------------

torch.multinomial(weights, 4, replacement=True)
#---------------------------------------------------------
# tensor([2, 1, 1, 1])
#---------------------------------------------------------

```

24. device
```
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

model = model.to(device)
```



25. one_hot
```
def one_hot(y, nb_digits): 
    y_onehot = torch.LongTensor(y.size(0), nb_digits) 
    y_onehot.zero_() 
    y_onehot.scatter_(1, y.unsqueeze(1), 1) 
    return y_onehot 

y = torch.LongTensor([2, 3, 0, 10])
one_hot(y, 11)
#-------------------------------------------------------------
# tensor([[0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
#         [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0],
#         [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
#         [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1]])
#-------------------------------------------------------------

```

26. distribution
    1. Categorical Distribution
    ```
    from torch.distribution import Categorical
    m = Categorical(torch.Tensor([[0.5, 0.2, 0.2, 0.1], [0.1, 0.2, 0.3, 0.4]]))
    m.sample()
    #---------------------------
    # tensor([0, 3])
    #---------------------------
    ```

    2. Bernoulli Distribution
    ```
    from torch.distribution import Bernoulli
    m = Bernoulli(torch.tensor([0.3]))
    m.sample()  # 30% chance 1; 70% chance 0
    #---------------------------
    # torch.Tensor([0.])
    #---------------------------
    ```

27. unbind <=> tf.unstack
```
a = torch.arange(4 * 3).view(4, 3)

#--------------------------------------------
# tensor([[ 0,  1,  2],
#         [ 3,  4,  5],
#         [ 6,  7,  8],
#         [ 9, 10, 11]])
#--------------------------------------------

col1, col2, col3 = torch.unbind(a, dim=1)

col1, col2, col3
#--------------------------------------------
# (tensor([0, 3, 6, 9]), tensor([ 1,  4,  7, 10]), tensor([ 2,  5,  8, 11]))
#--------------------------------------------

```

28. split
```
x = torch.arange(121).view(1, 121)
#-----------------------------------------------------------------------------------------
# tensor([[  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,
#           14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
#           28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,
#           42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,
#           56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
#           70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,
#           84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,
#           98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
#          112, 113, 114, 115, 116, 117, 118, 119, 120]])
#-----------------------------------------------------------------------------------------

pi_hat, mu1, mu2, sigma1_hat, sigma2_hat, rho_hat = torch.split(x[:, 1:], 20, 1)

pi_hat
#-----------------------------------------------------------------------------------------
# tensor([[ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
#          19, 20]])
#-----------------------------------------------------------------------------------------

mu1
#-----------------------------------------------------------------------------------------
# tensor([[21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
#          39, 40]])
#-----------------------------------------------------------------------------------------

```


29. dynamic_rnn: [reference codes](https://github.com/orris27/machine-learning-codes/blob/master/pytorch/dynamic_rnn.ipynb)
+ If the inputs are fixed-lengths data, we recommend to use `torch.nn.LSTM` rather than `torch.nn.LSTMCell` and `dynamic_rnn`.
```
def dynamic_rnn(cell, inputs, sequence_length):
    '''
        Inputs:
            cell: torch.nn.LSTMCell instance
            inputs: (batch_size, max_timestep, input_size)
            sequence_length: (batch_size,)
        Outputs:
            outputs: (batch_size, max_timestep, hidden_size)
            final_states(NOT SUPPORT): a tuple of (hidden_state, cell_state) which are the final states of the cell
    '''
    def sort_batch(data, lengths):
        '''
            data: (batch_size, ?)
            lengths: (batch_size,)
        '''
        sorted_indices, sorted_lengths = zip(*sorted(enumerate(lengths), key=lambda x: x[1], reverse=True))
        sorted_indices = list(sorted_indices)
        sorted_data = data[sorted_indices]
        return sorted_data, sorted_lengths, sorted_indices

    dtype = inputs.dtype
    device = inputs.device
        
    sorted_inputs, sorted_lengths, sorted_indices = sort_batch(inputs, sequence_length)
    
    decoder_lengths = [length - 1 for length in sorted_lengths]

    sorted_outputs = torch.zeros((inputs.shape[0], inputs.shape[1], cell.hidden_size), dtype=dtype).to(device)
    outputs = torch.zeros((inputs.shape[0], inputs.shape[1], cell.hidden_size), dtype=dtype).to(device)


    h, c = None, None
    for step in range(sorted_lengths[0]):
        curr_batch_size = sum([l > step for l in sorted_lengths])
        #sorted_inputs = sorted_inputs[:curr_batch_size, step, :] # (curr_batch_size, timesteps, input_size)
        curr_inputs = sorted_inputs[:curr_batch_size, step, :] # (batch_size, input_size)

        if h is None or c is None:
            h, c = cell(curr_inputs, None) # (curr_batch_size, hidden_size)
        else:
            h, c = h[:curr_batch_size], c[:curr_batch_size] # (curr_batch_size, hidden_size)
            h, c = cell(curr_inputs, (h, c)) # (curr_batch_size, hidden_size)

        sorted_outputs[:curr_batch_size, step, :] = h

    # sort back
    outputs[sorted_indices] = sorted_outputs

    return outputs
```


30. torch.no_grad vs detach

See [this](https://discuss.pytorch.org/t/detach-no-grad-and-requires-grad/16915/7). They are similar but `torch.no_grad` uses less memory because it knows from the beginning that the no gradients are needed so it does not need to keep intermediary results.


31. affine_grid & grid_sample: see more details in [Hungyi Li](https://www.bilibili.com/video/av9770302/?p=5) and [blog](https://www.jianshu.com/p/723af68beb2e)
```
img = Image.open('05.jpg')
img = T.ToTensor()(img)

theta = torch.FloatTensor([[1, 0, -0.2], [0, 1, 0.4]]) # Consider the index of new H*W to be (x, y), the original index would be (x - 0.2, y + 0.4)
grid = F.affine_grid(theta.unsqueeze(0), img.unsqueeze(0).size())
output = F.grid_sample(img.unsqueeze(0), grid)

imshow(output[0].numpy().transpose(1, 2, 0)) # the image pans to the right and pans down
```


32. gather
```
t = torch.tensor([[1, 2, 3],[4, 5, 6]])

torch.gather(t, 1, torch.LongTensor([[1, 0, 2], [2, 1, 0]]))
#----------------------------------------------------------------
# tensor([[2, 1, 3], # [1, 0, 2] means choose element from t[0] based on index, which means that [t[0][1], t[0][0], t[0][2]]
#         [6, 5, 4]]) # [t[1][2], t[1][1], t[1][0]]
#----------------------------------------------------------------
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
# b'\x01\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00'
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
    2. type cast
    ```
    a
    #----------------------------------------------
    # array([1, 2, 3])
    #----------------------------------------------

    a.astype('float')
    #----------------------------------------------
    # array([1., 2., 3.])
    #----------------------------------------------
    ```
    3. reshape
    ```
    a
    #----------------------------------------------
    # array([1, 2, 3])
    #----------------------------------------------

    a.reshape([3,1])
    #----------------------------------------------
    # array([[1],
    #        [2],
    #        [3]])
    #----------------------------------------------

    np.reshape(a, [3,1])
    #----------------------------------------------
    # array([[1],
    #        [2],
    #        [3]])
    #----------------------------------------------
    ```
    
    
    4. save & load
    + `save`: save an array into a binary file that ends with ".npy"
    + `load`
    ```
    ls
    #---------------------------------------------------------------------------------------------------
    # a.py  save/  simple-cnn.py  simple-gan.py  test.py  tmp  train_gan.py
    #---------------------------------------------------------------------------------------------------

    a
    #---------------------------------------------------------------------------------------------------
    # array([1., 2., 3.])
    #---------------------------------------------------------------------------------------------------

    np.save("test.jpg",a)

    ls
    #---------------------------------------------------------------------------------------------------
    # a.py  save/  simple-cnn.py  simple-gan.py  test.jpg.npy  test.py  tmp  train_gan.py
    #---------------------------------------------------------------------------------------------------
    
    
    b = np.load("test.jpg.npy")

    b
    #---------------------------------------------------------------------------------------------------
    # array([1., 2., 3.])
    #---------------------------------------------------------------------------------------------------
    ```
7. np.isnan
```
y
#-------------------------------------------------------------------------------
# array([       nan, 0.08079314, 0.14144054, 0.19439186, 0.24229219,
#               nan])
#-------------------------------------------------------------------------------


y[np.isnan(y)]=0

y
#-------------------------------------------------------------------------------
# array([0.        , 0.08079314, 0.14144054, 0.19439186, 0.24229219,
#        0.        ])
#-------------------------------------------------------------------------------

```



8. KL散度2个高斯分布接近下sigma是二次项作用大还是-log作用大
```
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

fig = plt.figure()
ax = Axes3D(fig)
x = np.linspace(0.1, 2, 31)
y = np.linspace(-2, 2, 31)
X, Y = np.meshgrid(x, y)
Z = -np.log(X) + X * X + Y * Y / 2 - 0.5
ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap='rainbow')
plt.show()
```

9. 填充0: [参考网站](https://blog.csdn.net/zenghaitao0128/article/details/78713663)
+ 第二个参数是一个元组,第i个元素表示在目标数组的第i轴前后填充多少零
+ 默认'constant'填充0
```
#在数组A的边缘填充constant_values指定的数值
#（3,2）表示在A的第[0]轴填充（二维数组中，0轴表示行），即在0轴前面填充3个宽度的0，比如数组A中的95,96两个元素前面各填充了3个0；在后面填充2个0，比如数组A中的97,98两个元素后面各填充了2个0
#（2,3）表示在A的第[1]轴填充（二维数组中，1轴表示列），即在1轴前面填充2个宽度的0，后面填充3个宽度的0
np.pad(A,((3,2),(2,3)),'constant',constant_values = (0,0))  #constant_values表示填充值，且(before，after)的填充值等于（0,0）


a = np.arange(16 * 5281).reshape(16,5281,1)                                                         
a.shape                                                                                             
(16, 5281, 1)

b =np.pad(a, ((0, 0), (0, 424), (0, 0)), 'constant')                                                

b.shape                                                                                            
(16, 5705, 1)
```


10. linear algebra
    1. 数组
    ```
    a = np.array([[1, 2], [3, 4]])
    b = np.empty([3,4]) # uninitialized, faster
    c = np.zeros([3, 5])
    d = np.ones([5, 5])
    ```
    2. vector
    ```
    from numpy import linalg as LA
    v = np.arange(6)
    l2 = LA.norm(v) # computes L^2 norm of v
    ```
    3. matrix
        1. 三角矩阵
        ```
        m = np.arange(9).reshape((3, 3))
        lower = np.tril(m)
        upper = np.triu(m)
        ```
        2. 对角矩阵
        ```
        d = np.diag(m) # [0, 4, 8]
        ```
        3. 单位矩阵
        ```
        I = np.identity(3)
        #----------------------------------
        # [[1. 0. 0.]
        #  [0. 1. 0.]
        #  [0. 0. 1.]]
        #----------------------------------
        ```
        4. 转置矩阵
        ```
        m_tran = m.T
        ```
        5. 矩阵求逆
        ```
        m_inv = LA.inv(m)
        ```
        6. 矩阵的迹: 主对角线（从左上方至右下方的对角线）上各个元素的总和
        ```
        m_trace = np.trace(m)
        ```
        7. 矩阵的行列式
        ```
        m_det = LA.det(m) # 计算矩阵行列式的值
        ```
        8. 矩阵的秩
        ```
        m_rank = LA.matrix_rank(m)
        ```
        9. 矩阵的分解
            1. PLU分解: 所有矩阵都可以做PLU分解. `矩阵 = 置换矩阵 * 下三角矩阵 * 上三角矩阵`(`M = PLU`)
            ```
            from scipy.linalg import lu
            P, L, U = lu(m) # np.matmul(np.matmul(P, L), U)的结果等于m
            ```
            2. QR分解: `矩阵 = 正交矩阵 * 上三角矩阵`(`M = QR`)
            ```
            Q, R = LA.qr(m)
            ```
            3. 特征分解
            ```
            values, vectors = LA.eig(m)
            # np.matmul(m , vectors[:, 0])和values[0] * vectors[:, 0]是一样的
            #---------------------------------------------------------------
            # [ 1.33484692e+01 -1.34846923e+00 -2.48477279e-16] # values
            # [[ 0.16476382  0.79969966  0.40824829] # vectors
            #  [ 0.50577448  0.10420579 -0.81649658]
            #  [ 0.84678513 -0.59128809  0.40824829]]
            #---------------------------------------------------------------
            ```
            4. 奇异值分解
            ```
            U, s, V = LA.svd(m)
            ```
11. 统计
    1. mean, var(方差), std(标准方差), cov(协方差矩阵), lstsq(线性最小二乘)
    
    
    
12. build a matrix
```
shape = (400, 400, 7)
matrix = np.zeros(shape)
```

13. max/min value in one axis of a numpy array
```
a = np.arange(6).reshape(2,3)

a
#--------------------------
# array([[0, 1, 2],
#        [3, 4, 5]])
#--------------------------

np.amax(a, axis=0)
#--------------------------
# array([3, 4, 5])
#--------------------------

np.amax(a, axis=1)
#--------------------------
# array([2, 5])
#--------------------------
```

14. set specific element to another value
```
a
#--------------------------
# array([[0, 1, 2],
#        [3, 1, 5]])
#--------------------------

a[a == 1] = 100

a
#--------------------------
# array([[  0, 100,   2],
#        [  3, 100,   5]])
#--------------------------
```

15. set dtype
```
a
#-------------------------------------------
# array([[0, 1, 2],
#        [3, 4, 5]])
#-------------------------------------------
a. dtype
#-------------------------------------------
# dtype('int64')
#-------------------------------------------


b = np.array(a, np.uint8)
#-------------------------------------------
# array([[0, 1, 2],
#        [3, 4, 5]], dtype=uint8)
#-------------------------------------------
```

16. difference compared with list
    1. multiply 
    ```
    a = [1, 2]
    a * 2
    #---------------------------------
    # [1, 2, 1, 2]
    #---------------------------------
    
    b = np.array([1, 2])
    b * 2
    #---------------------------------
    # array([2, 4])
    #---------------------------------
    ```


17. meshgrid
```
nx, ny = (3, 2)
x = np.linspace(0, 1, nx) # 第一维度的可能坐标值
y = np.linspace(0, 1, ny) # 第二维度的可能坐标值
xv, yv = np.meshgrid(x, y)
xv # xv代表第一维度的坐标可能值,而yv代表第二维度的坐标可能值
#---------------------------------
# array([[ 0. ,  0.5,  1. ],
#        [ 0. ,  0.5,  1. ]])
#---------------------------------
yv
#---------------------------------
# array([[ 0.,  0.,  0.],
#        [ 1.,  1.,  1.]])
#---------------------------------

```

18. flip
```
img = np.random.randn(5, 4, 3)
himg = np.flip(img, axis = 1) # horizontal flip
vimg = np.flip(img, axis = 0) # vertical flip
```

20. histogram: 计算a的元素每个区间内的数量.比如下例`bins=[0, 1, 2, 3, 4]`的情况,就是计算`[0, 1)`,`[1, 2)`,`[2, 3)`, `[3, 4)`区间内a的元素各占多少个.如果bins是int类型,比如2的话,就是说化成2类,并且上下界是a的max和min. hist就是我们的结果,而bin_edges就是bins或者计算出来的分界点
```
hist, bin_edges = np.histogram([1, 1, 2, 2, 2, 2, 3], bins = range(5))

print(hist, bin_edges)
#-------------------------------
# [0 2 4 1] [0 1 2 3 4]
#-------------------------------


np.histogram([1, 1, 2, 2, 2, 2, 3, 10], bins =2)
#-------------------------------
# (array([7, 1]), array([ 1. ,  5.5, 10. ]))
#-------------------------------

```
21. sort
```
a
#-------------------------------
# array([[11,  4,  9,  8],
#        [ 7,  6,  5,  4],
#        [ 3,  2,  1,  0]])
#-------------------------------

np.array(sorted(a, key=lambda x: x[0]))
#-------------------------------
# array([[ 3,  2,  1,  0],
#        [ 7,  6,  5,  4],
#        [11,  4,  9,  8]])
#-------------------------------

np.array(sorted(a, key=lambda x: x[1]))
#-------------------------------
# array([[ 3,  2,  1,  0],
#        [11,  4,  9,  8],
#        [ 7,  6,  5,  4]])
#-------------------------------

```

## 4. plt
+ `import matplotlib.pyplot as plt`
+ Add `%matplotlib inline` in jupyter notebook and use plt the same way
1. 画画
    1. 文字
    ```
    plt.figure() # start
    plt.text(0.3,0.4,"hello")
    #plt.axis('off')
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
        1. colormap
        ```
        plt.figure()
        plt.imshow(image) # [height,width] || [height,width,channels] || etc
        plt.show()
        ```
        2. grayscale
        ```
        plt.figure()
        plt.imshow(image, cmap='gray')
        plt.show()
        ```
    5. 有标记
    ```
    plt.plot(steps, y_np, 'r-', label='target (cos)')
    plt.plot(steps, x_np, 'b-', label='input (sin)')
    plt.legend(loc='best')
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

5. IPython: matplotlib.get_backend() -> 'module://ipykernel.pylab.backend_inline' if jupyter notebook else Qt5Agg (`python test.py` or `ipython`)
```
is_ipython = 'inline' in matplotlib.get_backend()
if is_ipython:
    from IPython import display
```

6. subplots
```
x = np.linspace(0, 2*np.pi, 400)
y = np.sin(x**2)

f, (ax1, ax2) = plt.subplots(1, 2, sharey=True) # share x / y, etc
ax1.plot(x, y)
ax1.set_title('Sharing Y axis')
ax2.scatter(x, y)
plt.show()
```

## 5. PIL
1. 基本操作
```
from PIL import Image

img = Image.open('1.jpg') # 无论RGB还是grayscale都会在np.asarray后自动确定,所以不用添加其他参数
img.tobytes()
img.size
img.height
img.width
len(img.getbands())
```
2. 转换为numpy
```
img = Image.open('1.jpg')
array = np.asarray(img, np.uint8)
array.shape
#----------------------------------------
# (1240, 2181, 3)
#----------------------------------------
```
3. 灰度化处理
```
image = Image.open(os.path.join(cwd,image_path)).convert('L') # L = R * 299/1000 + G * 587/1000 + B * 114/1000
image_array = np.asarray(image, dtype=np.uint8)
```

## 6. Collections
1. 统计词频(Counter或者nltk.FreqDist): Counter是dict的一个子类
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
        #----------------------------------------------------
        # ['a', 'p', 'p', 'l', 'e']
        #----------------------------------------------------

        c = Counter(a) # <=> c = Counter(); c.update(a)
        #----------------------------------------------------
        # Counter({'a': 1, 'e': 1, 'l': 1, 'p': 2})
        #----------------------------------------------------

        c = Counter(a).most_common(2)
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



3. namedtuple: 类似于dict,只是不可变把
```
class Block(collections.namedtuple('Block', ['scope', 'unit_fn', 'args'])):
    pass

a =  Block('scope_name', 'unit_fn_name', [1, 2])

a
#----------------------------------------------------------
# Block(scope='scope_name', unit_fn='unit_fn_name', args=[1, 2])
#----------------------------------------------------------

a.scope
#----------------------------------------------------------
# 'scope_name'
#----------------------------------------------------------

a.unit_fn
#----------------------------------------------------------
# 'unit_fn_name'
#----------------------------------------------------------

a.args
#----------------------------------------------------------
# [1, 2]
#----------------------------------------------------------

a.args[0]
#----------------------------------------------------------
# 1
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
+ text: 所有文字拼接而成的: text = ' '.join(documents)
+ document: 一篇文章,一句话等等
+ documents: 所有文章或所有话组成的列表
+ word: 原始的单词
+ token: 被lower,去除stopwords等操作后的单词
+ tokens: 一组token的列表,可能表示这个document,也可能表示整个text
+ indices: 一组index的列表,可能表示这个document,也可能表示整个text: `[2132, 12, 3, 923, ...]`
+ tokens_list: a list of tokens. 一组tokens的列表,这个tokens表示一个document: `tokens_list = [tokenize(document) for document in documents]`
+ token_set: a set of token. 整个text里所有token的集合
+ vocabulary: 整个text里所有token以及对应的index的一个python词典


1. 思路
    1. Get the dict: {word: index}
    2. Get the dict: {index: word}
    3. Translate the word into the index
2. 普通的文本文件 => 干净的一行一句话的文本文件
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
3. 一行一句话的文本文件 => 一行一个高频词的文本文件(后续将行号作为该单词的编号)
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
4. 一行一个高频词的文本文件 => 一行一句int话的文本文件
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
5. 一行一句int话的文本文件 => `[num_batches, batch_size, num_steps]`的int列表
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
6. dict: {word: index} => dict: {index: word}
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

    vectorizer = DictVectorizer()
    l = [{"gender":"boy", "age": 17}, {"gender":"girl","age":18}, {"gender":"girl","age":20}]

    vectorizer.fit_transform(l).toarray()
    #----------------------------------------------------------------
    # array([[17.,  1.,  0.],
    #        [18.,  0.,  1.],
    #        [20.,  0.,  1.]])
    #----------------------------------------------------------------

    vectorizer.get_feature_names()
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
3. Text Features Extraction
    1. TF Features
    ```
    from sklearn.feature_extraction.text import CountVectorizer
    
    train_data = ("The sky is blue.", "The sun is bright.")
    test_data = ("The sun in the sky is bright.", "We can see the shining sun, the bright sun.")

    vectorizer = CountVectorizer(stop_words=stopwords.words('english'))

    vectorizer.fit_transform(train_data)
    print(vectorizer.vocabulary_)
    #-----------------------------------------------------------------------------------
    # {'sky': 2, 'blue': 0, 'sun': 3, 'bright': 1}
    #-----------------------------------------------------------------------------------
    
    smatrix = vectorizer.transform(test_data)
    tf_features = smatrix.toarray()
    print(tf_features)
    #-----------------------------------------------------------------------------------
    # [[0 1 1 1]
    #  [0 1 0 2]]
    #-----------------------------------------------------------------------------------
    ```
    
    2. tf-idf features
    ```
    from sklearn.feature_extraction.text import TfidfVectorizer

    vectorizer = TfidfVectorizer(stop_words=stopwords.words('english'))

    vectorizer.fit_transform(train_data) # train_data and test data are the same as the above codes

    smtarix = vectorizer.transform(test_data)
    test_features = smtarix.toarray()
    #----------------------------------------------------------------------
    # [[0.         0.57735027 0.57735027 0.57735027]
    #  [0.         0.4472136  0.         0.89442719]]
    #----------------------------------------------------------------------
    ```


4. Decision Tree: 完整代码参看30-5的DecisionTree代码
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


5. KNN
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

6. SVM
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
    
7. train_test_split
```
from sklearn.cross_validation import train_test_split

x = np.arange(128)
y = np.array([1] * 64 + [0] * 64)
x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.33)
```

8. accuracy_score
```
y_predicted = nb.predict(x_test)
score = accuracy_score(y_predicted, y_test) # y_predicted.shape & y_test.shape: (num_samples, )
print(score) # 0.8332835
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

2. env
    1. action_space
        1. generate action randomly
        ```
        env.action_space
        #--------------------------------------------------------
        # Discrete(2)
        #--------------------------------------------------------

        env.action_space.sample()
        #--------------------------------------------------------
        # 0
        #--------------------------------------------------------
        
        env.action_space.sample()
        #--------------------------------------------------------
        # 1
        #--------------------------------------------------------
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


## 12. pickle
1. basic use: `out.pkl: 8086 relocatable (Microsoft)`
```
import pickle

path = "./out.pkl"

class Apple:
    def __init__(self, num):
        self.num = num
    def __str__(self):
        return str(self.num)

var = Apple(123)

with open(path, "wb") as f:
    pickle.dump(var, f)
    
print("restoring...")

with open(path, "rb") as f:
    print(pickle.load(f))
```


2. multiple variables
```
a, b = 1, 2
with open('ab.pkl', 'wb') as f:
    pickle.dump(a, f)
    pickle.dump(b, f)

c, d = 0, 0
with open('ab.pkl', 'rb') as f:
    c = pickle.load(f)
    d = pickle.load(f)
c, d
#------------------
# (1, 2)
#------------------
```


## 13. IPython
### display
1. display an image
```
from IPython.display import display, Image
display(Image(filename="/home/user/orris/image-caption/try/Image-Caption-based-on-SeqGAN/images/surf.jpg"))
```

2. clear_output: Clear the output of the current cell receiving output.
```
display.clear_output(wait=True)
```

3. display SVG
```
from IPython.core.display import SVG
SVG(filename='logo.svg')
```



## 14. Bazel
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


## 15. gensim
1. word2vec
    1. save the model
    ```
    from gensim.test.utils import common_texts, get_tmpfile
    from gensim.models import Word2Vec

    path = get_tmpfile("word2vec.model")

    # common_texts: [['human', 'interface', 'computer'], ['survey', 'user', 'computer', 'system', 'response', 'time'], .. ]
    # size: size of the vector
    # window: The maximum distance between the current and predicted word within a sentence.(default to be 5)
    # min_count: ignore all words with total frequency lower than this (if min_count=4, then a word that appears less than 4 times will be ignored)
    # workers: number of threads to train the model
    model = Word2Vec(common_texts, size=100, window=5, min_count=1, workers=4)
    model.save("word2vec.model")
    ```
    2. convert a word to a vector
        1. single word
        ```
        vector = model.wv['computer']
        ```
        2. total 
        ```
        embedding = model.wv.syn0
        #--------------------------------------------------------------------
        # array([[-0.12377599,  0.0872228 ,  0.07527732,  0.07240492],
        #        [ 0.00060917,  0.10808477,  0.08883764, -0.10234629],
        #        [-0.03281352,  0.01941418,  0.10518485,  0.06932261],
        #        [ 0.03303403, -0.09389477, -0.12104194,  0.05431319]],
        #       dtype=float32)
        #--------------------------------------------------------------------

        dictionary = model.wv.index2word
        #--------------------------------------------------------------------
        # ['system', 'user', 'trees', 'graph']
        #--------------------------------------------------------------------
        
        dictionary.index('trees')
        #--------------------------------------------------------------------
        # 2
        #--------------------------------------------------------------------


        ```
    3. load
    ```
    model = Word2Vec.load("word2vec.model")
    ```
    4. continue to train
    ```
    model.train([["hello", "world"]], total_examples=1, epochs=1)
    #--------------------------------------------------------------------
    # (0, 2)
    #--------------------------------------------------------------------
    ```


2. bow
```
from gensim import corpora
documents = ["I love the red apple".lower().split(), 'He wears a red hat'.lower().split()]
d = corpora.Dictionary(documents) # 将每个单词对应到每个index了

[d[i] for i in range(len(d))] # d[i] <=> d.id2token[i], 同理还有d.token2id['apple']
#---------------------------------------------------------------------------
# ['apple', 'i', 'love', 'red', 'the', 'a', 'hat', 'he', 'wears']
#---------------------------------------------------------------------------


d.doc2bow(['i', 'love', 'the', 'red', 'apple'])
#---------------------------------------------------------------------------
# [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)] # (token_id, token_count), 这里的token_count指的是doc2bow参数里的token的出现次数,而token_id来自d的语料库
#---------------------------------------------------------------------------
d.doc2bow(['red', 'red'])
#---------------------------------------------------------------------------
# [(3, 2)]
#---------------------------------------------------------------------------

d.doc2bow(['red', 'red', 'red'])
#---------------------------------------------------------------------------
# [(3, 3)]
#---------------------------------------------------------------------------
```

3. tfidf model
+ 计算公式参考[this](https://radimrehurek.com/gensim/models/tfidfmodel.html),需要注意的是idf是log2并且分母没有+1, 以及最后有normalization的过程. 所以如果某个word在所有文档中出现,那么idf=0,就不会在`tfidf_model[[(token_id, token_count)]]`中输出
```
from gensim import models

tokens_list = [['i', 'love', 'the', 'red', 'apple', 'juice'], \
             ['he', 'wears', 'a', 'red', 'hat'], \
             ['she', 'enjoys', 'playing', 'games']]
d = corpora.Dictionary(tokens_list)
bow_tokens_list = [d.doc2bow(tokens) for tokens in tokens_list] # convert document to BoW format
tfidf_model = models.TfidfModel(bow_tokens_list) # train model

#tfidf_model[d.doc2bow(tokens_list[0])] # 输入参数是document的BoW format
tfidf_model[bow_tokens_list[0]] # 输入参数是document的BoW format
#---------------------------------------------------------------------------
# [(0, 0.44124367556640004), # 输出形状: (token_id, tfidf_features), 其中tfidf_features的计算公式参考上面
#  (1, 0.44124367556640004),
#  (2, 0.44124367556640004),
#  (3, 0.44124367556640004),
#  (4, 0.16284991207632712),
#  (5, 0.44124367556640004)]
#---------------------------------------------------------------------------

```
    
    
## 16. copy
```
b = copy.deepcopy(a) # deep copy
b = copy.copy(a) # shallow copy
```

## 17. opencv
1. imshow
```
height = 600
width = 800

image = np.arange(height * width).reshape((height, width))
#-----------------------------------------------------------------------
# array([[     0,      1,      2, ...,    797,    798,    799],
#        [   800,    801,    802, ...,   1597,   1598,   1599],
#        [  1600,   1601,   1602, ...,   2397,   2398,   2399],
#        ...,
#        [477600, 477601, 477602, ..., 478397, 478398, 478399],
#        [478400, 478401, 478402, ..., 479197, 479198, 479199],
#        [479200, 479201, 479202, ..., 479997, 479998, 479999]])
#-----------------------------------------------------------------------

#########################################################################
# Normalized to gray image (0~255, np.uint8)
#########################################################################

image = image / np.max(image) * 255
image = np.array(image, np.uint8)

image
#-----------------------------------------------------------------------
# array([[  0,   0,   0, ...,   0,   0,   0],
#        [  0,   0,   0, ...,   0,   0,   0],
#        [  0,   0,   0, ...,   1,   1,   1],
#        ...,
#        [253, 253, 253, ..., 254, 254, 254],
#        [254, 254, 254, ..., 254, 254, 254],
#        [254, 254, 254, ..., 254, 254, 255]], dtype=uint8)
#-----------------------------------------------------------------------

cv2.imshow('winname', image)
cv2.waitKey(0)
```


2. imwrite
```
height = 300
weight = 400
n_channels = 3
image = np.zeros((height, weight, n_channels), np.uint8)

image[:, :, 1] = 255

cv2.imshow('winname', image)
cv2.imwrite("1.png", image)
cv2.waitKey(0)
```

3. binarization
```
def binarization(img):
    bin_img = img.astype(np.uint8)
    cv2.threshold(bin_img, 50, 1, cv2.THRESH_BINARY_INV, bin_img) # pixel = 0 if value > 50 else 1
    return bin_img
```


## 18. spider
1. cookies
    1. get cookies from website
    ```
    # -*- coding: utf-8 -*-
    from selenium import webdriver
    from pickle import dump, load
    import pickle
    from os.path import exists
    from scrapy.selector import Selector
    import time
    import settings

    options = webdriver.ChromeOptions()
    options.add_argument("--incognito")
    driver_path = "/home/orris/dataset/2.44/chromedriver"
    browser = webdriver.Chrome(executable_path=driver_path, chrome_options=options)
    browser.get("http://www.acfun.cn/login")
    t_selector = Selector(text=browser.page_source)

    input("扫码完成后回车继续")
    cookies = browser.get_cookies()
    browser.close()
    pickle.dump(cookies, open("data/cookies/acfun_cookies.pkl", "wb"))

    ```
    2. load cookies
    ```
    cookies = pickle.load(open("data/cookies/acfun_cookies.pkl", "rb"))
    cookies = {cookie['name']: cookie['value'] for cookie in cookies}
    
    cookies = str(cookies).strip('{|}').replace(',',';').replace("'","").replace(": ","=")

    headers={
        "User-Agent":"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
        "Cookie": cookies,
    }
    request = Request(url, headers=headers)
    response = urlopen(request)
    html = response.read().decode("utf-8")
    ```
2. selenium
+ 通过[这个网站](https://chromedriver.storage.googleapis.com/2.44/notes.txt)可以查看2.44支持的chrome版本,现在是v69-71,如果`google-chrome --version`是v73就不行了,所以要重新下载新的
    1. CentOS7
    ```
    wget https://dl.google.com/linux/direct/google-chrome-stable_current_x86_64.rpm
    sudo yum localinstall google-chrome-stable_current_x86_64.rpm
    google-chrome --version
    #--------------------------------------------------------------------
    # Google Chrome 73.0.3683.75
    #--------------------------------------------------------------------
    
    /dataset/chromedriver/2.46/chromedriver --version
    #--------------------------------------------------------------------
    # ChromeDriver 2.46.628388 (4a34a70827ac54148e092aafb70504c4ea7ae926)
    #--------------------------------------------------------------------
    
    options = webdriver.ChromeOptions()
    options.add_argument("--incognito")
    options.add_argument('--headless')
    options.add_argument('--no-sandbox')
    options.add_argument('--disable-dev-shm-usage')
    driver_path = settings.DRIVER_PATH
    browser = webdriver.Chrome(executable_path=driver_path, chrome_options=options)
    browser.get(url)
    # ...
    ```
    2. save screenshot
        1. normal
        ```
        browser.save_screenshot("screenshot.png")
        ```
        2. full page
        ```
        import os
        import time

        from PIL import Image

        def fullpage_screenshot(driver, file):

            print("Starting chrome full page screenshot workaround ...")

            total_width = driver.execute_script("return document.body.offsetWidth")
            total_height = driver.execute_script("return document.body.parentNode.scrollHeight")
            viewport_width = driver.execute_script("return document.body.clientWidth")
            viewport_height = driver.execute_script("return window.innerHeight")
            print("Total: ({0}, {1}), Viewport: ({2},{3})".format(total_width, total_height,viewport_width,viewport_height))
            rectangles = []

            i = 0
            while i < total_height:
                ii = 0
                top_height = i + viewport_height

                if top_height > total_height:
                    top_height = total_height

                while ii < total_width:
                    top_width = ii + viewport_width

                    if top_width > total_width:
                        top_width = total_width

                    print("Appending rectangle ({0},{1},{2},{3})".format(ii, i, top_width, top_height))
                    rectangles.append((ii, i, top_width,top_height))

                    ii = ii + viewport_width

                i = i + viewport_height

            stitched_image = Image.new('RGB', (total_width, total_height))
            previous = None
            part = 0

            for rectangle in rectangles:
                if not previous is None:
                    driver.execute_script("window.scrollTo({0}, {1})".format(rectangle[0], rectangle[1]))
                    print("Scrolled To ({0},{1})".format(rectangle[0], rectangle[1]))
                    time.sleep(0.2)

                file_name = "part_{0}.png".format(part)
                print("Capturing {0} ...".format(file_name))

                driver.get_screenshot_as_file(file_name)
                screenshot = Image.open(file_name)

                if rectangle[1] + viewport_height > total_height:
                    offset = (rectangle[0], total_height - viewport_height)
                else:
                    offset = (rectangle[0], rectangle[1])

                print("Adding to stitched image with offset ({0}, {1})".format(offset[0],offset[1]))
                stitched_image.paste(screenshot, offset)

                del screenshot
                os.remove(file_name)
                part = part + 1
                previous = rectangle

            stitched_image.save(file)
            print("Finishing chrome full page screenshot workaround...")
            return True
        
        fullpage_screenshot(browser, "screenshot.png")

        ```
    3. add cookie: request the page before adding the cookies so that the driver knows where the cookies belong to
    ```
    browser = webdriver.Chrome(executable_path=driver_path, chrome_options=options)
    browser.get(url)

    cookies = pickle.load(open(settings.COOKIES_FILENAME, "rb")) # cookies file from browser.get_cookies()
    for cookie in cookies:
        browser.add_cookie(cookie)
    ```
    4. find element
        1. xpath
        ```
        browser.find_element_by_xpath('//figure/a[@href="/v/ac5004999"]/following-sibling::figcaption/p/span[@id="ticketBtn"]').click()
        ```
        2. name, id, etc: [document](https://selenium-python.readthedocs.io/locating-elements.html)
        ```
        browser.find_element_by_css_selector(".Login-qrcode button").click()
        ```


## 19. os
1. `expanduser`: expand `~`
```
os.listdir('~')
#--------------------------------------------
# ---------------------------------------------------------------------------
# FileNotFoundError                         Traceback (most recent call last)
# <ipython-input-4-6390b0a9c8b9> in <module>()
# ----> 1 os.listdir('~')
# 
# FileNotFoundError: [Errno 2] No such file or directory: '~'
#--------------------------------------------

os.path.expanduser('~')
#--------------------------------------------
# '/home/orris'
#--------------------------------------------

os.listdir(os.path.expanduser('~'))
#--------------------------------------------
# ['vmware',
#  '.atom',
#  '.shutter',
#  'Videos',
#  ...]
#--------------------------------------------
```

2. `os.system`: execute a command in a subshell
```
In [11]: os.system('ls')
gpl-3.0.txt  lswc  README_and_install.sh
Out[11]: 0

In [12]: os.system('ls hello')
ls: cannot access 'hello': No such file or directory
Out[12]: 512
```

3. makedirs
+ exist_ok: False => raise OSError if the directory has already existed
```
os.makedirs('test')

os.makedirs('test')
#-------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# FileExistsError                           Traceback (most recent call last)
# <ipython-input-21-a250051b13d5> in <module>()
# ----> 1 os.makedirs('test')
# 
# ~/anaconda3/lib/python3.6/os.py in makedirs(name, mode, exist_ok)
#     218             return
#     219     try:
# --> 220         mkdir(name, mode)
#     221     except OSError:
#     222         # Cannot rely on checking for EEXIST, since the operating system
# 
# FileExistsError: [Errno 17] File exists: 'test'
#-------------------------------------------------------------------------

os.makedirs('test',exist_ok=True) # OK
```


4. splitext: split the extension from a pathname
```
os.path.splitext('a.txt')
#-----------------------------
# ('a', '.txt')
#-----------------------------

os.path.splitext('a.txt.bak')
#-----------------------------
# ('a.txt', '.bak')
#-----------------------------

os.path.splitext('/a/b/a.txt.bak')
#-----------------------------
# ('/a/b/a.txt', '.bak')
#-----------------------------

```


## 20. fire
在命令行中指定调用的函数以及参数

程序
```
import fire

def add(x, y):
    return x + y

def mul(**kwargs):
    a = kwargs['a']
    b = kwargs['b']
    return a * b

if __name__ == '__main__':
    fire.Fire()

```
调用
```
python test.py add --x=3 --y=5
#--------------------------------------------
# 8
#--------------------------------------------
python test.py add 3 4
#--------------------------------------------
# 7
#--------------------------------------------
python test.py mul --a=2 --b=6 # "python test.py mul 2 6" reports error
#--------------------------------------------
# 12
#--------------------------------------------
```


## 21. tqdm
```
for i in tqdm.tqdm(range(10000)):
    math.factorial(10000)

#------------------------------------------------------------------------------------------
# 100%|█████████████████████████████████████████████████████████| 10000/10000 [00:28<00:00, 355.24it/s]
#------------------------------------------------------------------------------------------
```
show variable value
```
pbar = tqdm(["a", "b", "c", "d"])
for char in pbar:
    pbar.set_description("Processing %s" % char)
#------------------------------------------------------------------------------------------
# Processing d: 100%|███████████████████████████████████████████████████████████████████████| 4/4 [00:00<00:00, 7209.80it/s]
#------------------------------------------------------------------------------------------
```

## 22. nltk
具体参考[this](https://github.com/orris27/nlp_simple_implementation)
1. tokenize
```
text = 'The Georgetown experiment in 1954 involved fully automatic translation of more than sixty Russian sentences into English. The authors claimed that within three or five years, machine translation would be a solved problem.[2] However, real progress was much slower, and after the ALPAC report in 1966, which found that ten-year-long research had failed to fulfill the expectations, funding for machine translation was dramatically reduced. Little further research in machine translation was conducted until the late 1980s, when the first statistical machine translation systems were developed.'

def tokenize(sentence):
    # remove the punctuation
    import string
    remove_punctuation_map = dict((ord(char),None) for char in string.punctuation)
    sentence_no_punctuation = sentence.translate(remove_punctuation_map)
        
    # lower
    sentence_no_punctuation = sentence_no_punctuation.lower()
        
    # word_tokenize
    from nltk import word_tokenize
    words = word_tokenize(sentence_no_punctuation)
        
    # remove stopwords
    from nltk.corpus import stopwords
    filtered_words = [word for word in words if word not in stopwords.words('english')]
        
    # stemming
    from nltk.stem import SnowballStemmer
    snowball_stemmer = SnowballStemmer("english")
    words_stemed = [snowball_stemmer.stem(word) for word in filtered_words]
    
    return words_stemed
tokens = tokenize(text)
print(tokens[:5])
#------------------------------------------------------
# ['georgetown', 'experi', '1954', 'involv', 'fulli']
#------------------------------------------------------
```

2. ngrams
+ Stop-word removal will affect n-grams
```
text = 'The Georgetown experiment in 1954 involved fully automatic ...'
tokens = tokenize(text) # CUSTOMIZED FUNCTION
phrases = ngrams(tokens, 2) # bigram
print(list(pharses))
#------------------------------------------------------------------------------------------------
# [('georgetown', 'experi'), ('experi', '1954'), ('1954', 'involv'), ('involv', 'fulli'), ... ]
#------------------------------------------------------------------------------------------------
```

3. calculates the frequency of a token in tokens
```
from nltk import FreqDist
fdist = FreqDist(tokens) # "tokens" is a CUSTOMIZED VARIABLE
print(dict(fdist.most_common(10)))
#------------------------------------------------------------------------------------------------
# {'translat': 5, 'machin': 4, 'research': 2, 'georgetown': 1, 'experi': 1, '1954': 1, 'involv': 1, 'fulli': 1, 'automat': 1, 'sixti': 1}
#------------------------------------------------------------------------------------------------
```


4. tf(Term Frequency) & idf(Inverse Document Frequency)
```
def tf(word, document):
    word = tokenize(word)[0]
    words = tokenize(document)
    return (sum(1 for word1 in words if word1 == word)) / len(words)

def idf(word, documents):
    tokens_list = [tokenize(document) for document in documents]
    token = tokenize(word)[0]
    import math
    try:
        return math.log(len(tokens_list) / (1 + sum(1 for tokens in tokens_list if token in tokens)))
    except ValueError:
        return 0

def tf_idf(word, document, documents):
    return tf(word, document) * idf(word, documents)
```

5. pos_tag: [task list](https://pythonprogramming.net/natural-language-toolkit-nltk-part-speech-tagging/)
```
from nltk import pos_tag
from nltk.tokenize import word_tokenize

s = "The quick brown fox juped over the dog"
tokens = word_tokenize(s)
pos_tag(tokens)
#-------------------------------------------------------
# [('The', 'DT'),
#  ('quick', 'JJ'),
#  ('brown', 'NN'),
#  ('fox', 'NN'),
#  ('juped', 'VBD'),
#  ('over', 'IN'),
#  ('the', 'DT'),
#  ('dog', 'NN')]
#-------------------------------------------------------
```

6. naive bayes classifier
```
from nltk.classify import NaiveBayesClassifier

# data
s1 = 'This is a good book'
s2 = 'This is a awesome book'
s3 = 'This is a bad book'
s4 = 'This is a terrible book'

# extract features
def preprocess(s):
    return {word: True for word in s.lower().split()}

training_data = [[preprocess(s1),'pos'],
                 [preprocess(s2),'pos'],
                 [preprocess(s3),'neg'],
                 [preprocess(s4),'neg'],]
# train
model = NaiveBayesClassifier.train(training_data)

# predict
print(model.classify(preprocess('He is a good man')))
```


7. Stemming
    1. Snowball
    2. Lemmatization
    ```
    from nltk.stem import WordNetLemmatizer
    wordnet_lemmatizer = WordNetLemmatizer()
    wordnet_lemmatizer.lemmatize('houses')
    #--------------------------------------------------------
    # 'house'
    #--------------------------------------------------------
    wordnet_lemmatizer.lemmatize('went') # default noun
    #--------------------------------------------------------
    # 'went'
    #--------------------------------------------------------
    wordnet_lemmatizer.lemmatize('went',pos='v')
    #--------------------------------------------------------
    # 'go'
    #--------------------------------------------------------
    ```

8. bleu
```
from nltk.translate.bleu_score import sentence_bleu
reference = [['this', 'is', 'a', 'test'], ['this', 'is' 'test']]
candidate = ['this', 'is', 'a', 'test']
score = sentence_bleu(reference, candidate)
print(score)
#-----------------------------------------------------------------
# 1.0
#-----------------------------------------------------------------
```

## 23. wordcloud
1. WordCloud
```
from wordcloud import WordCloud

cloud = WordCloud(max_words=20)
cloud.generate_from_text(text)
## imshow(cloud)
```


## 24. pprint
```
labels = [('Sports',), ('Politics',), ('Sports',), ('Politics', 'Sports'), ()]

from pprint import pprint
print(labels)
#-----------------------------------------------------------------------------------------------
# [['Politics', 'Sports'], ['Sports', 'Politics'], ['Politics', 'Sports'], ['Sports', 'Sports'], ['Politics', 'Politics']]
#-----------------------------------------------------------------------------------------------

pprint(labels) # jupyter notebook
#-----------------------------------------------------------------------------------------------
# [['Politics', 'Sports'],
#  ['Sports', 'Politics'],
#  ['Politics', 'Sports'],
#  ['Sports', 'Sports'],
#  ['Politics', 'Politics']]
#-----------------------------------------------------------------------------------------------
```


## 25. jieba
### 1. 分词
精确模式(默认)
```

```
### 2. 词性标注
词性参考北大词性标注集
```
import jieba.posseg as psg

s = '中文分词是文本处理的不可或缺的一步！'
s = psg.cut(s)
' '.join(['{}/{}'.format(w, t) for w, t in s])
#--------------------------------------------------------------------
# '中文/nz 分词/n 是/v 文本处理/n 的/uj 不可或缺/l 的/uj 一步/m ！/x'
#--------------------------------------------------------------------

```
### 3. Keyword Extraction
TextRank
```
from jieba import analyse

text = '6月19日,《2012年度“中国爱心城市”公益活动新闻....'
textrank = analyse.textrank
keywords = textrank(text, num_keywords, allowPOS=('ns', 'n'))
```



## 26. dateutil
```
from dateutil.parser import parse

parse('4月5日', fuzzy=True)
#-------------------------------------------
# datetime.datetime(2019, 4, 5, 0, 0)
#-------------------------------------------

parse('4月5号', fuzzy=True)
#-------------------------------------------
# datetime.datetime(2019, 4, 5, 0, 0)
#-------------------------------------------

parse('2020年4月5号', fuzzy=True)
#-------------------------------------------
# datetime.datetime(2019, 4, 5, 0, 0)
#-------------------------------------------

parse('2029/04/05', fuzzy=True)
#-------------------------------------------
# datetime.datetime(2029, 4, 5, 0, 0)
#-------------------------------------------
```


## 27. urllib
Download files
```
def download(url, filename=None):
    import os
    from urllib.request import urlretrieve
    if filename is None:
        filename = url.split('/')[-1]
    if not os.path.exists(filename):
        print('Downloading {} from {}...'.format(filename, url))
        urlretrieve(url, filename=filename)
download('https://pytorch.org/tutorials/_static/img/neural-style/picasso.jpg')
```


## 28. time
计时
```
t1 = time.time();time.sleep(5); t2 = time.time(); t2 - t1
#-------------------------------------------------------------
# 5.005254745483398
#-------------------------------------------------------------
```


## 29. heapq
priority queue
```
import heapq

heap = list()
heapq.heappush(heap, (96, 'alice'))
heapq.heappush(heap, (60, 'lucy'))
heapq.heappush(heap, (70, 'bob'))
heapq.heappush(heap, (85, 'tom'))

heap
#------------------------------------------------------------------
# [(60, 'lucy'), (85, 'tom'), (70, 'bob'), (96, 'alice')]
#------------------------------------------------------------------

heap[0] # data with minimum score
#------------------------------------------------------------------
# (60, 'lucy')
#------------------------------------------------------------------

heapq.heappop(heap)
#------------------------------------------------------------------
# (60, 'lucy')
#------------------------------------------------------------------

heap
#------------------------------------------------------------------
# [(70, 'bob'), (85, 'tom'), (96, 'alice')]
#------------------------------------------------------------------

heapq.heappushpop(heap, (60, 'john')) # push first, and then pop
#------------------------------------------------------------------
# (60, 'john')
#------------------------------------------------------------------

heap
#------------------------------------------------------------------
# [(70, 'bob'), (85, 'tom'), (96, 'alice')]
#------------------------------------------------------------------

```



## 30. contextlib
timer
```
from contextlib import contextmanager
import time

@contextmanager
def timer(name):
    start = time.time()
    yield
    end = time.time()
    print('{} COST:{}'.format(name, end - start))


with timer('Timer Monte Carlo Iter'):
    mc.monte_carlo_opt(agent, env)

```




## 31. colab
### mount the drive to colab
```
from google.colab import drive
drive.mount('/content/gdrive')

import os
os.chdir("gdrive/My Drive")

!ls
#---------------------------------------
# 'Colab Notebooks'   test.ipynb
#---------------------------------------
```

## 32. argparse
+ bool: If the command arguments have `--load=False`, the `args.load` is still True. The only way to turn it off is not to include `--load` in the command parameters
```
def main(args):
    vocab = build_vocab(caption_path=args.caption_path, min_count=args.min_count)
    # ...

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--caption_path', type=str, default='data/annotations/captions_train2014.json', help='path for train annotation file')
    parser.add_argument('--min_count', type=int, default=4, help='minimum word count min_count')
    args = parser.parse_args()
    main(args)
```

## 33. multiprocessing
1. Pool
```
from multiprocessing import Pool
import time

def f(x):
    while True:
        print(x * x)
        time.sleep(1)

if __name__ == '__main__':
    pool = Pool(processes=4)
    pool.apply_async(f,(1,))
    pool.apply_async(f,(2,))
    pool.apply_async(f,(3,))

    pool.close()
    pool.join()
```


## 34. svgwrite
example
```
dwg = svgwrite.Drawing('test.svg', profile='tiny')
dwg.add(dwg.line((0, 0), (10, 0), stroke=svgwrite.rgb(10, 10, 16, '%')))
dwg.add(dwg.text('Test', insert=(2, 10), fill='red'))
dwg.save()
```

### construct an object
+ filename
+ size=('100%', '100%'): Note that the default background color is transparent
+ extra: additional SVG attributs as keyword-arguments, such as fill(color), profile, etc.
```
dwg = svgwrite.Drawing('test.svg', profile='tiny')
```
### add patterns
#### 1. line
+ `'RGB'`: returns a rgb-string format: ``'rgb(r, g, b)'``
+ `'%'`: returns percent-values as rgb-string format: ``'rgb(r%, g%, b%)'``
```
dwg.add(dwg.line((0, 0), (10, 0), stroke=svgwrite.rgb(10, 10, 16, '%')))
```
#### 2. text
```
dwg.add(dwg.text('Test', insert=(0, 0.2), fill='red'))
```
#### 3. rect
+ insert: (x, y)
+ size: (width, height)
```
dwg.add(dwg.rect(insert=(0, 0), size=(100, 100), fill='blue'))
```

#### 4. path
```
dwg.add(dwg.path('M 1,2 L 100, 0 L 0,100 z').stroke('black', 1).fill('rgb(200, 12, 234)'))
```
1. commands: [refer](https://www.w3.org/TR/SVG11/paths.html#PathData)
+ moveto: M(absolute), m(relative)
+ lineto: L(absolute), l(relative), H, h(horizontal), V, v(vertical)
+ closepath: Z / z => draw a line from current point to the initial point to form a closed path

2. stroke:
+ color: the color of the line/stroke
+ width: width of the pen

3. fill:
+ color: the color that is surrounded by the lines



### extra
1. fill
+ `fill='white'`
+ `fill='rgb(255, 255, 0)'`


### Drawing
#### 1. save
Write the XML string to filename
```
dwg.save() # the filename is given when the object 'dwg' is constructed
```


#### 2. write
```
with open('test.svg', 'w', encoding='utf-8') as f:
    dwg.write(f)
```

#### 3. tostring
```
from cairosvg import svg2png
svg2png(bytestring=dwg.tostring(), write_to='output.png')
```


## 35. cairosvg
convert svg xml to png
### 1. to png file
```
from cairosvg import svg2png

svg_code = """
    <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="10"/>
        <line x1="12" y1="8" x2="12" y2="12"/>
        <line x1="12" y1="16" x2="12" y2="16"/>
    </svg>
"""

svg2png(bytestring=svg_code,write_to='output.png')
```
### 2. to byte string
```
img = svg2png(bytestring=svg_code)

with open('output1.png', 'wb') as f:
    f.write(img)
```



## 36. zipfile
```
def unzip(path):
    from zipfile import ZipFile
    with ZipFile(path, 'r') as z:
        z.extractall()
```

## 37. glob
```

import glob

all_filenames = glob.glob('../data/names/*.txt') # return a list of filenames that match the pattern
print(all_filenames) # ['../data/names/Arabic.txt', ...]
```


## 38. python
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
    3. shuffle two lists
    + `random_state`: seed
    ```
    from sklearn.utils import shuffle

    shuffle([3,4,5],[30,40,50])
    #--------------------------------------------
    # [[4, 3, 5], [40, 30, 50]]
    #--------------------------------------------

    shuffle([3,4,5],[30,40,50])
    #--------------------------------------------
    # [[5, 3, 4], [50, 30, 40]]
    #--------------------------------------------

    shuffle([3,4,5],[30,40,50], random_state=1)
    #--------------------------------------------
    # [[3, 5, 4], [30, 50, 40]]
    #--------------------------------------------

    shuffle([3,4,5],[30,40,50], random_state=1)
    #--------------------------------------------
    # [[3, 5, 4], [30, 50, 40]]
    #--------------------------------------------

    shuffle([3,4,5],[30,40,50], random_state=1)
    #--------------------------------------------
    # [[3, 5, 4], [30, 50, 40]]
    #--------------------------------------------


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
    5. str & list:
    ```
    list('abcd')
    #--------------------------------
    # ['a', 'b', 'c', 'd']
    #--------------------------------
    ```

    6. slice:
    ```
    l =  [2, 1, 3, 5]

    l[:-1]
    #-------------------------------
    # [2, 1, 3]
    #-------------------------------
    ```
    
    7. remove
    ```
    l = [3, 5, 6, 3]
    l.remove(3)
    #-------------------------------
    # [5, 6, 3]
    #-------------------------------
    ```

    8. pop
    ```
    l = [3, 5, 4]
    l.pop()
    #-------------------------------
    # 4
    #-------------------------------
    l
    #-------------------------------
    # [3, 5]
    #-------------------------------
    ```

24. Python里面的三目运算符`?:`
```
action = 1 if np.random.random() < probability1 else 0
```

25. 计时
```
time.time() #<=> clock()
```


26. set
    1. cast a list into a set( a set is different from a dict)
    ```
    a = set([1,2,3])
    #-----------------------
    # {1, 2, 3}
    #-----------------------
    ```
    2. Basic use
    ```
    1 in a
    #-----------------------
    # True
    #-----------------------

    1 not in a
    #-----------------------
    # False
    #-----------------------

    a.add(4)
    #-----------------------
    # {1, 2, 3, 4}
    #-----------------------
    
    a.remove(3)
    #-----------------------
    # {1, 2, 4}
    #-----------------------

    a.clear()
    #-----------------------
    # set()
    #-----------------------
    
    a = set([1,2,3])
    a.pop()
    #-----------------------
    # 1
    #-----------------------
    
    a.pop()
    #-----------------------
    # 2
    #-----------------------
    
    a.pop()
    #-----------------------
    # 3
    #-----------------------
    ```
    3. sorted: discard the replicated elm
    ```
    a = [4,3,4,5,2, 1]
    
    sorted(a)
    #--------------------------------------------------------
    # [1, 2, 3, 4, 4, 5]
    #--------------------------------------------------------
    
    sorted(set(a))
    #--------------------------------------------------------
    # [1, 2, 3, 4, 5]
    #--------------------------------------------------------
    ```
    4. update
    ```
    s = set()
    s |= {1, 2}
    s |= {5, 2, 6}
    #-----------------------------------------
    # {1, 2, 5, 6}
    #-----------------------------------------
    ```


27. str.zfill: pad a string with zeros on the left, to fill a field of the specific width
```
"23".zfill(5)
#------------------------------------------
# '00023'
#------------------------------------------

"123456".zfill(5)
#------------------------------------------
# '123456'
#------------------------------------------

"ab".zfill(5)
#------------------------------------------
# '000ab'
#------------------------------------------
```


28. 智能创建文件夹
```
dir = xxx
if not os.path.exists(dir):
    os.makedirs(dir)
```


29. ipynb
1. 使用
```
sudo pip install ipython --upgrade
sudo pip install jupyter
jupyter notebook --ip=10.214.144.222
```
2. 转化ipynb文件为py文件
```
jupyter nbconvert --to script xx.ipynb
```

30. 命令行参数
+ 详细参考[here](http://www.runoob.com/python/python-command-line-arguments.html)
```
'''
    只获取--ckpt的内容,如果没有参数就报错
'''
import sys
import getopt

def parse_args(argv):
    try:
        opts, args = getopt.getopt(argv, '', ['ckpt='])
    except getopt.GetoptError:
        print('python training.py --ckpt=xxx/')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '--ckpt':
            return arg
        else:
            print('python training.py --ckpt=xxx/')
            return None
    return None



if __name__ == '__main__':
    output_dir = parse_args(sys.argv[1:])
    if output_dir is None:
        print('python training.py --ckpt=xxx/')
        sys.exit()

```
31. 创建目录
```
if not os.path.exists(output_dir):
    os.mkdir(output_dir)

```

32. string:
    1. to string
    ```
    class Point(object):
        def __init__(self, x=-1, y=-1):
            super(Point, self).__init__()
            self.x = x
            self.y = y

        def __repr__(self): # not __str__
            return '({}, {})'.format(self.x, self.y)
    ```
    2. multiple
    ```
    '23'*3
    #---------------------------------------------------------
    # '232323'
    #---------------------------------------------------------
    ```
    3. split
    ```
    s = '23//45/6'
    s.split('/')
    #---------------------------------------------------------
    # ['23', '', '45', '6']
    #---------------------------------------------------------
    ```

33. `print`:
```
print(value, ..., sep=' ', end='\n', file=sys.stdout, flush=False)

print('123\n')
#--------------------------------------------
# 123
# 
# 
#--------------------------------------------
```


34. magic commands: `%hist?`可以查看`%hist`的用法
    1. %timeit: 检测某个命令执行时间
    ```
    import torch
    a = torch.Tensor(3, 4)
    %timeit a.sum()
    #----------------------------------------------------------------------------------
    # 1.87 µs ± 30.9 ns per loop (mean ± std. dev. of 7 runs, 1000000 loops each)
    #----------------------------------------------------------------------------------
    ```
    2. %hist: 输入历史
    3. %xdel与del的不同之处在于前者会删除其在ipython上的所有应用.如
    ```
    In [1]: import torch

    In [2]: a = torch.Tensor(5, 5)

    In [3]: a
    Out[3]: 
    tensor([[0.0000, 0.0000, 0.0000, 0.0000, 0.0000],
            [0.0000, 0.0000, 0.0000, 0.0000, 0.0000],
            [0.0000, 0.0000, 0.0000, 0.0000, 0.0000],
            [0.0000, 0.0000, 0.0000, 0.0000, 0.0000],
            [0.0000, 0.0000, 0.0000, 0.0000, 0.0000]])

    In [4]: del a # 没有彻底释放空间,因为还被Out [3]所引用

    In [5]: c = torch.Tensor(1000, 1000)

    In [6]: c
    Out[6]: 
    tensor([[0., 0., 0.,  ..., 0., 0., 0.],
            [0., 0., 0.,  ..., 0., 0., 0.],
            [0., 0., 0.,  ..., 0., 0., 0.],
            ...,
            [0., 0., 0.,  ..., 0., 0., 0.],
            [0., 0., 0.,  ..., 0., 0., 0.],
            [0., 0., 0.,  ..., 0., 0., 0.]])

    In [7]: Out [3]
    Out[7]: 
    tensor([[0.0000, 0.0000, 0.0000, 0.0000, 0.0000],
            [0.0000, 0.0000, 0.0000, 0.0000, 0.0000],
            [0.0000, 0.0000, 0.0000, 0.0000, 0.0000],
            [0.0000, 0.0000, 0.0000, 0.0000, 0.0000],
            [0.0000, 0.0000, 0.0000, 0.0000, 0.0000]])

    In [8]: %xdel c

    In [9]: Out [6]
    ------------------------------------------------------------
    KeyError                   Traceback (most recent call last)
    <ipython-input-9-e5e038e1222a> in <module>()
    ----> 1 Out [6]

    KeyError: 6

    ```
    4. %quickref, %who, %debug, %magic,%env, %xdel
    
    
35. `zip(*params)`
```
name_scores = [('alice', 100), ('bob', 80), ('john', 73)]

names, scores = zip(*name_scores) # a list of tuples => 2 lists

names   
#------------------------------------------------
# ('alice', 'bob', 'john')
#------------------------------------------------

scores                                                       
#------------------------------------------------
# (100, 80, 73)
#------------------------------------------------
```


36. print in one line dynamically
```
for i in range(5):
    print('\rHello %d'%(i), end='', flush=True)
print()
#------------------------------------------------------------
# Hello 4
#------------------------------------------------------------
```

37. eval:
```
eval('[1, 2]')
#-------------------------------
# [1, 2]
#-------------------------------

eval('{3, 4}')
#-------------------------------
# {3, 4}
#-------------------------------

eval('{1: 10, 4: 40}')
#-------------------------------
# {1: 10, 4: 40}
#-------------------------------

```
