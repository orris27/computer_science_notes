import tensorflow as tf

class CNN(object):
    '''
        Defines CNN model
        [batch_size,time_step,embedding_size,1]=>多个[batch_size,1,1,num_filters]=>[batch_size,1,1,num_filters*n]=>[batch_size,1*1*num_filters*n]
    '''
    def __init__(self,
            filter_sizes,
            num_filters,
            num_classes,
            embedding_size,
            sequence_length,
            batch_size,
            vocab_size,
            ):
        # 获取窗口大小(多选,)的列表
        # 获取窗口大小的宽的列表filter_sizes
        # 定义输出的filter层的个数num_filters
        # 获取分类的个数num_classes
        # 获取单句话的最大单词数sequence_length
        # 获取整个文本的陌生单词数vocab_size
        # 获取batch_size
        # 获取embedding_size
        # 获取features,labels:输入的features是词典id
        self.features = tf.placeholder(tf.int32,[None,sequence_length]) 
        self.labels = tf.placeholder(tf.float32,[None,num_classes])

        # 转换词典id的列表=>词向量
        W = tf.Variable(
            tf.random_uniform([vocab_size, embedding_size], -1.0, 1.0),
            name="W"
            )
        embedded_chars = tf.nn.embedding_lookup(W, self.features)

        # 增加1个维度=>适应CNN
        embedded_chars_expanded = tf.expand_dims(embedded_chars, -1)
        a0 = embedded_chars_expanded


        # 定义保存pooling结果的列表
        outputs = []
        # for i in filter_sizes
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
        y_predicted,l2_loss_delta = self.nn(a1,num_classes,tf.nn.softmax,'fully-connected')
        
        l2_loss = tf.constant(0.0)
        l2_loss +=l2_loss_delta
        # 定义损失函数
        with tf.name_scope('loss'):
            #losses = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels = self.labels,logits = y_predicted))
            #self.loss = losses + l2_loss
            self.loss = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(labels = self.labels,logits = y_predicted))



        # 定义准确度op
        with tf.name_scope('accuracy'):
            self.total = tf.reduce_sum(tf.cast(tf.equal(tf.argmax(y_predicted,axis = 1),tf.argmax(self.labels,axis = 1)),tf.int16),name='accuracy-total')


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
            l2_loss_delta = 0
            l2_loss_delta +=tf.nn.l2_loss(W)
            l2_loss_delta +=tf.nn.l2_loss(b)

            # 激活
            if activator is None:
                return tf.matmul(inputs,W)+b
            a = activator(tf.matmul(inputs,W)+b)
            # dropout
            # 返回输出值
            return a,l2_loss_delta
