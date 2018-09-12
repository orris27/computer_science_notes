    
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
        # W3 = tf.Variable(tf.random_normal([输入值中每行有多少个,节点个数/输出值中每行有多少个],stddev = 0.1))
        W3 = tf.Variable(tf.random_normal([7*7*64,1024],stddev = 1/tf.sqrt(1024.)))
        # b
        b3 = tf.Variable(tf.zeros([1024])+0.1)
        # activate
        a3 = tf.nn.sigmoid(tf.matmul(a2_pool_tran,W3)+b3)
        # dropout
        a3_dropout = tf.nn.dropout(a3,keep_prob)
        ```
        2. 方法2
        ```
        def nn(self, inputs, output_dim, activator=None, scope=None):
            '''
                定义神经网络的一层
            '''
            # 定义权重的初始化器
            norm = tf.random_normal_initializer(stddev=1.0)
            # 定义偏差的初始化
            const = tf.constant_initializer(0.0)

            # 打开变量域,或者使用None
            with tf.variable_scope(scope, reuse=tf.AUTO_REUSE):
                # 定义权重
                W = tf.get_variable("W",[inputs.get_shape()[1],output_dim],initializer=norm)
                # 定义偏差
                b = tf.get_variable("b",[output_dim],initializer=const)
                # 激活
                if activator is None:
                    return tf.matmul(inputs,W)+b
                a = activator(tf.matmul(inputs,W)+b)
                # dropout
                # 返回输出值
                return a
            
        # 使用方法如下所示
        def discriminator(self,inputs,dim):
            '''
                定义判别器的模型
            '''
            # 第0层全连接神经网络:节点数=dim,激活函数=tf.tanh,scope=d0
            a0 = self.nn(inputs,dim*2,tf.tanh,'d0')
            # 第1层全连接神经网络:节点数=dim,激活函数=tf.tanh,scope=d1
            a1 = self.nn(a0,dim*2,tf.tanh,'d1')
            # 第2层全连接神经网络:节点数=dim,激活函数=tf.tanh,scope=d2
            a2 = self.nn(a1,dim*2,tf.tanh,'d2')
            # 第3层全连接神经网络:节点数=dim,激活函数=tf.tanh,scope=d3
            #a3 = self.nn(a2,dim*2,tf.tanh,'d3') 
            # 返回判别器的预测给分:节点数=1,激活函数=tf.sigmoid,形状=[batch_size,1]
            y_predicted = self.nn(a2,1,tf.nn.sigmoid,'d3')
            return y_predicted
        ```
        
    4. [神经网络和卷积神经网络的实现](https://github.com/orris27/orris/tree/master/python/machine-leaning/codes/tensorflow/cnn)
3. 定义代价函数,并定义训练tensor
    1. 方法1
        + labels:正确的y
        + logits:预测的y
        + 优化器:AdamOptimizer
    ```
    cross_entropy = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = labels,logits = y_predicted))
    train = tf.train.AdamOptimizer(learning_rate).minimize(cross_entropy)
    ```
    2. 方法2:自带学习率衰减
        + var_list:类似于`self.d_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='disc')`
            + tf.GraphKeys.TRAINABLE_VARIABLES是字符串类型,为`trainable_variables`
            + tf.get_collection返回的是变量的列表
            + GraphKeys
                + `GLOBAL_VARIABLES`: the default collection of Variable objects, shared across distributed environment (model variables are subset of these). See tf.global_variables for more details. Commonly, all TRAINABLE_VARIABLES variables will be in MODEL_VARIABLES, and all MODEL_VARIABLES variables will be in GLOBAL_VARIABLES.
                + `LOCAL_VARIABLES`: the subset of Variable objects that are local to each machine. Usually used for temporarily variables, like counters. Note: use tf.contrib.framework.local_variable to add to this collection.
                + `MODEL_VARIABLES`: the subset of Variable objects that are used in the model for inference (feed forward). Note: use tf.contrib.framework.model_variable to add to this collection.
                + `TRAINABLE_VARIABLES`: the subset of Variable objects that will be trained by an optimizer. See tf.trainable_variables for more details.
                + `SUMMARIES`: the summary Tensor objects that have been created in the graph. See tf.summary.merge_all for more details.
                + `QUEUE_RUNNERS`: the QueueRunner objects that are used to produce input for a computation. See tf.train.start_queue_runners for more details.
                + `MOVING_AVERAGE_VARIABLES`: the subset of Variable objects that will also keep moving averages. See tf.moving_average_variables for more details.
                + `REGULARIZATION_LOSSES`: regularization losses collected during graph construction.
        + self.d_loss:代价函数
        + self.learning_rate:0.03等
    ```
    def optimizer(self, loss, var_list, initial_learning_rate):
        '''
            var_list:要训练的张量集合.
        '''
        decay = 0.95
        num_decay_steps = 150
        batch = tf.Variable(0)
        learning_rate = tf.train.exponential_decay(
            initial_learning_rate,
            batch,
            num_decay_steps,
            decay,
            staircase=True
        )
        optimizer = tf.train.GradientDescentOptimizer(learning_rate).minimize(
            loss,
            global_step=batch,
            var_list=var_list
        )
        return optimizer
    
    self.d_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='disc')
    self.d_train  = self.optimizer(self.d_loss, self.d_params, self.learning_rate)
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
    1. batch_size:第一个维度,表示一个特征值/标签的实例个数=单次训练的实例个数
    2. 维度:1开始的下标
    3. 训练次数
        1. `num_pretrain_steps`
        2. `num_steps`


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
    1. 保存会话里的所有变量
        + `+=`赋值的变量不会被保存
    ```
    W1 = tf.Variable(tf.truncated_normal([1],stddev = 0.1))
    
    saver=tf.train.Saver()
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())
        print(sess.run(W1))
        saver.save(sess,"save/1.ckpt")
    ```
    2. 还原整个会话里的变量
    ```
    W1 = tf.Variable(tf.truncated_normal([1],stddev = 0.1))

    saver=tf.train.Saver()
    with tf.Session(config=config) as sess:
        saver.restore(sess,"save/1.ckpt")
        print(sess.run(W1))
    ```
    3. 保存结果
    ```
    save/
    ├── 1.ckpt.data-00000-of-00001
    ├── 1.ckpt.index
    ├── 1.ckpt.meta
    └── checkpoint

    ```
15. 生成全是0的变量
    1. 定义变量
    ```
    W=tf.Variable(tf.zeros(3),name='weights',dtype=tf.float32)
    ```
    2. 定义张量
    ```
    tf.zeros([3, 4], tf.int32) # 默认类型是tf.float32
    tf.zeros(3) #< =>tf.zeros([3])
    ```
    
    
    
16. GPU
    1. tf.ConfigProto的参数
        1. log_device_placement=True : 是否打印设备分配日志
        2. allow_soft_placement=True ： 如果你指定的设备不存在，允许TF自动分配设备
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
        gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
        config=tf.ConfigProto(gpu_options=gpu_options)
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
    1. 使用rnn:`[-1,784]`=>`[-1,28,28]`=>rnn=>`[-1,lstm_size]`
        + 后续可以考虑全连接层.比如W为`[lstm_size,10]`,b为`[10]`,然后用矩阵乘法来解决.`y_predicted=tf.nn.softmax(tf.matmul(final_state[1],weights)+bias)`
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
    # map
    ouputs,final_state=tf.nn.dynamic_rnn(lstm_cell,inputs,dtype=tf.float32)

    ```
    2. BasicLSTMCell.[原理图](https://github.com/orris27/orris/blob/master/python/machine-leaning/images/BasicLSTMCell.png)
        1. state_is_tuple
            + 如果为True,返回的state是一个tuple:(c=array([[]]), h=array([[]]):其中c代表Ct的最后时间的输出，h代表Ht最后时间的输出，h是等于最后一个时间的output的.即state=(c, h)
            + 如果是False，那么state是一个由c和h拼接起来的张量，`state=tf.concat(1,[c,h])`。在运行时，则返回2值，一个是h，还有一个state
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
            + 通过inputs中的max_time将网络按时间展开
            1. 参数
                1. cell:上面的返回值
                2. inputs:`[batch_size,max_time,size]`
                3. sequence_length:是一个list，如果你要输入三句话，且三句话的长度分别是5,10,25,那么sequence_length=[5,10,25]
            2. 返回:（outputs, states）.一般我们选择states[1]就行了
                1. output:输出的是最上面一层的输出
                    1. time_major=False:`[batch_size, max_time, num_units]`
                    2. time_major=True:`[max_time,batch_size,num_units]`
                2. states:保存的是最后一个时间输出的states
                    1. `[batch_size, 2*len(cells)]或[batch_size,s]`
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
```
writer=tf.summary.FileWriter(log_dir,sess.graph)
writer.close()

####################################################################################
# 命令行
####################################################################################
tensorboard --logdir=.
```


24. 训练,自己处理梯度:[第三方文档](https://blog.csdn.net/u012436149/article/details/53006953)
    1. 计算梯度
    2. 处理梯度
    3. 应用梯度
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
26. 变量名字
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
    2. name_scope:只不影响get_vafiable
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
    tf.random_normal(shape, mean=0.0, stddev=1.0, dtype=tf.float32, seed=None, name=None)

    tf.truncated_normal(shape, mean=0.0, stddev=1.0, dtype=tf.float32, seed=None, name=None)

    tf.random_uniform(shape, minval=0, maxval=None, dtype=tf.float32, seed=None, name=None)

    tf.random_shuffle(value, seed=None, name=None)

    tf.random_crop(value, size, seed=None, name=None)

    tf.multinomial(logits, num_samples, seed=None, name=None)

    tf.random_gamma(shape, alpha, beta=None, dtype=tf.float32, seed=None, name=None)

    tf.set_random_seed(seed)
    ```
    2. 常量tensor
    ```
    tf.zeros(shape, dtype=tf.float32, name=None)

    tf.zeros_like(tensor, dtype=None, name=None)

    tf.ones(shape, dtype=tf.float32, name=None)

    tf.ones_like(tensor, dtype=None, name=None)

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
30. [构建TF代码](https://blog.csdn.net/u012436149/article/details/53843158)

31. 计算梯度
    + 如果函数是`y = tf.clip_by_value(x, 0, 5)`,那么`[0,5]`内返回1,而其他范围内返回0
    1. 如果xs是列表:计算ys对每个xs里的x的偏导并返回.结果的shape=b的shape
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

32. 设置随机数的种子
```
tf.set_random_seed(1)
#x = tf.random_normal(shape=[1,3,3,1])
```

33. checkpoint
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
    2. 检查
    ```
    from tensorflow.python.tools.inspect_checkpoint import print_tensors_in_checkpoint_file
    #print_tensors_in_checkpoint_file("test-ckpt/model-2", None, True) # model-2是上次使用save时的参数"test-ckpt/model-2.ckpt"
    print_tensors_in_checkpoint_file("test-ckpt/", None, True) # 这样就是恢复test-ckpt里默认的checkpoint
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
```
global_step = tf.Variable(0, trainable=False)

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
        1. `[b'hello', 1]`
        2. `[b'world', 2]`
        3. `[b'orris', 3]`
        4. `[b'hello', 1]`
    ```
    # enqueue_many 的写法，两个元素放在两个列表里。
    en_m = queue.enqueue_many([['hello', 'world', 'orris'], [1, 2, 3]])
    # enqueue 的写法
    en = queue.enqueue(['hello', 1])
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

40. 获得全局变量
```
print(sess.run(tf.global_variables()))
#print(sess.run(tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES)))
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

    tf.flags.DEFINE_float("dev_sample_percentage", .1, "Percentage of the training data to use for validation")
    tf.flags.DEFINE_integer("embedding_dim", 128, "Dimensionality of character embedding (default: 128)")
    tf.flags.DEFINE_string("filter_sizes", "3,4,5", "Comma-separated filter sizes (default: '3,4,5')")
    tf.flags.DEFINE_boolean("log_device_placement", False, "Log placement of ops on devices")

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
46. 转换一句话的列表为数字的特征值,并且自动填充
```
import numpy as np
from tensorflow.contrib import learn


x_text = ['I have a ball', 'He loves animals']

max_document_length = max([len(x.split(" ")) for x in x_text])
vocab_processor = learn.preprocessing.VocabularyProcessor(max_document_length)
x = np.array(list(vocab_processor.fit_transform(x_text)))

print(x)

#---------------------------------------------------------------------------------
# [[1 2 3 4]
#  [5 6 7 0]]
#---------------------------------------------------------------------------------
```
## 2. Python
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
```
np.array(np.random.choice(2, size=(5,)))
```

4. 组装字符串的列表.将字符串的内容分隔后,转换成整数,然后组成列表
```
list(map(int, '1,2,3'.split(',')))
```
5. 处理txt文件,获得字符串列表与标签,并且批量输出
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
    
X, y = load_data_and_labels('./data/rt-polaritydata/rt-polarity.pos','./data/rt-polaritydata/rt-polarity.neg')


def next_batch(data, batch_size, num_epochs,shuffle=True):
    '''
        获得数据,一批一批的
        1. data: 列表
        2. batch_size: 一批数据的大小
        3. num_epochs: 总共迭代整个数据的次数
        4. shuffle:是否洗牌
    '''
    
    # 转换data为numpy array
    data = np.array(data)
    # 获得1个epoch有多少个batch_size
    num_batches = int((len(data) - 1)/batch_size) + 1
    # for(i:0=>epoch的次数)
    for i in range(num_epochs):
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

``` 

6. 洗牌
    + data:列表
    + `l[[3,5]]`会取出第3个和第5个元素(0为下标)并组成列表.
        + 下面的内容会返回`[4, 5]`
        ```
        data = np.array([1,2,3,4,5,6,7,8])
        print(data[[3,4]]) #[4, 5]
        ```
```
import numpy as np

shuffle_indices = np.random.permutation(np.arange(len(data))) # 会将[0, len(data))的整数洗牌.比如说shuffle_indices会变成[2 4 6 7 1 0 3 5]
shuffled_data = data[shuffle_indices]
```
