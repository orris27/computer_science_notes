import numpy as np
import tensorflow as tf
from scipy.stats import norm
import matplotlib.pyplot as plt

seed = 42
np.random.seed(seed)
tf.set_random_seed(seed)


# Data的类
class Data(object):
    class train(object):
        '''
            返回训练集的相关数据
        '''
        # 定义正态分布的均值
        mu = 4
        # 定义正态分布的方差
        sigma = 0.5
        
        @classmethod
        def next_batch(self,batch_size):
            '''
                返回self.batch_size个实例数的一批数据,用来给判别器做预处理的
                参数
                1. batch_size:给出的X_train和y_train结构是[batch_size,1]
            '''
            # 生成X_train:获取区间为[-5,5)的平均分布下随机产生的形状为[batch_size]的值
            X_train = (np.random.random(batch_size) - 0.5) * 10.0
            # 生成y_train:正态分布的概率密度函数(X_train)
            y_train = norm.pdf(X_train, loc=self.mu, scale=self.sigma)
            # 返回X_train和y_train
            return X_train,y_train

        @classmethod
        def noise(self,batch_size):
            '''
                返回生成器使用的假钞材料
            '''
             
            # 定义随机生成的数据的范围data_range
            data_range = 8
            
            # 返回随便生成的形状为[batch_size,1]的数据
            return np.linspace(-data_range, data_range, batch_size) + np.random.random(batch_size) * 0.01

        @classmethod
        def real(self,batch_size):
            '''
                返回判别器正式训练的真钞[batch_szie,1]
            '''
            # 获得正态分布下随机生成的形状为[batch_size,1]的数据
            samples = np.random.normal(self.mu, self.sigma, batch_size)
            # 排序
            samples.sort()
            # 返回数据
            return samples


# GAN的类
class GAN(object):
    def __init__(self,batch_size, learning_rate,num_pretrain_steps, num_steps):
        '''
            1. 定义GAN的一系列参数
            2. 建立GAN的预训练判别器,生成器和判别器的模型
        '''
        # 定义batch_size
        self.batch_size = batch_size

        # 定义中间层的节点个数
        self.mlp_hidden_size = 4
        
        # 定义学习率
        self.learning_rate = learning_rate 

        # 定义预训练的次数
        self.num_pretrain_steps = num_pretrain_steps

        # 定义预训练的次数
        self.num_steps = num_steps 

        # 创建3个基本模型
        self.create_model()


    def scope_assign(self,src,dest,sess):
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




    def optimizer(self, loss, var_list, initial_learning_rate):
        '''
            var_list:要训练的张量集合
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

    def generator(self,inputs,dim):
        '''
            定义生成器的模型
        '''
        # 第0层全连接神经网络:节点数=dim,激活函数=tf.nn.softplus,scope=g0
        a0 = self.nn(inputs,dim,tf.nn.softplus,'g0') 
        # 第1层全连接神经网络:节点数=1,激活函数=None,scope=g1
        a1 = self.nn(a0,1,None,'d1') 
        # 返回生成器制造出来的假钞
        return a1


    def create_model(self):
        '''
            1. 定义预训练判别器的张量
            2. 定义生成器的张量
            3. 定义判别器的张量
        '''
        # 定义预训练的变量域:scope=d_pre
        with tf.variable_scope("d_pre", reuse=tf.AUTO_REUSE):
            # 定义预训练的真钞的placeholder
            self.pre_features = tf.placeholder(tf.float32, shape=(self.batch_size, 1))
            # 定义预训练的正确给分的placeholder
            self.pre_labels = tf.placeholder(tf.float32, shape=(self.batch_size, 1))
            # 定义预训练的预测给分的计算张量
            pre_y_predicted = self.discriminator(self.pre_features,self.mlp_hidden_size)

            # 定义预训练的代价函数
            self.pre_loss = tf.reduce_mean(tf.square(pre_y_predicted - self.pre_labels))
            # 定义预训练的训练张量
            self.pre_train  = self.optimizer(self.pre_loss, None, self.learning_rate)

        # 获得预训练判别器的张量集合
        self.d_pre_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='d_pre')

        # 定义生成器的张量:scope=gen
        with tf.variable_scope("gen", reuse=tf.AUTO_REUSE):
            # 定义生成器的假钞材料的placeholder
            self.z = tf.placeholder(tf.float32, shape=(self.batch_size, 1))
            # 定义生成器制造出来的假钞计算张量
            self.g = self.generator(self.z,self.mlp_hidden_size)
            
        # 定义判别器的张量:scope=disc
        with tf.variable_scope("disc", reuse=tf.AUTO_REUSE):
            # 定义判别器的真钞的placeholder
            self.x = tf.placeholder(tf.float32, shape=(self.batch_size, 1))
            # 定义判别器给真钞的给分计算张量
            self.d_x = self.discriminator(self.x,self.mlp_hidden_size)
            # 定义判别器给假钞的给分计算张量
            self.d_g = self.discriminator(self.g,self.mlp_hidden_size)

        # 定义生成器的代价函数
        self.g_loss = tf.reduce_mean(-tf.log(self.d_g))
        # 获得生成器要训练的张量集合
        self.g_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='gen')
        # 定义生成器的训练张量
        self.g_train  = self.optimizer(self.g_loss, self.g_params, self.learning_rate)
        
        # 定义判别器的代价函数
        self.d_loss = tf.reduce_mean(-tf.log(self.d_x) - tf.log(1 - self.d_g))
        # 获得判别器要训练的张量集合
        self.d_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='disc')
        # 定义判别器的训练张量
        self.d_train  = self.optimizer(self.d_loss, self.d_params, self.learning_rate)

    def train(self,pre_next_batch,real,noise):
        '''
            1. 预训练判别器
            2. 复制预训练的判别器到判别器
            3. 交替训练判别器和生成器,并周期地输出代价函数的值
            参数
            num_pretrain_steps:预训练判别器的训练次数
            num_steps:交替训练判别器和生成器的训练次数
            pre_next_batch:用来生成预训练判别器的特征值和标签
                "X_train,y_train = pre_next_batch(self.batch_size)"
            real:获得判别器训练用的真钞
                "x = real(self.batch_size)"
            noise:获得生成器的假钞材料
                "z = noise(self.batch_size)"
        '''
        # 开启session
        with tf.Session() as sess:
            # 初始化变量 
            sess.run(tf.global_variables_initializer())

            # 预训练判别器num_pretrain_steps次
            for step in range(self.num_pretrain_steps):
                # 获得训练集用的真钞
                # 获得训练集用的真钞给分
                X_train,y_train = pre_next_batch(self.batch_size)
                # 训练判别器
                sess.run(self.pre_train,feed_dict = {
                    self.pre_features:np.reshape(X_train,(self.batch_size,1)),
                    self.pre_labels:np.reshape(y_train,(self.batch_size,1))})
                

            
            # 复制预训练的判别器的参数到正式的判别器的参数中
            self.scope_assign('d_pre','disc',sess)
            
            # 交替训练判别器和生成器num_steps次
            for step in range(self.num_steps):
                # (先训练判别器)
                # 获得判别器训练用的真钞
                x = real(self.batch_size)
                # 获得生成器的假钞材料
                z = noise(self.batch_size)
                # 训练判别器,并获得判别器的代价函数的值
                d_loss, _ = sess.run([self.d_loss,self.d_train],feed_dict = {
                    self.x:np.reshape(x,(self.batch_size,1)),
                    self.z:np.reshape(z,(self.batch_size,1))})
                 
                # (接着训练生成器)
                # 训练生成器,直接用之前的生成器的假钞材料,并获得生成器的代价函数的值
                g_loss, _ = sess.run([self.g_loss,self.g_train],feed_dict = {
                    self.z:np.reshape(z,(self.batch_size,1))})

                # 打印
                if step % 10 == 0:
                    print("epoch {0}: {1}:{2}".format(step,d_loss,g_loss))
		
                self.data_range = 8
		# 画图
                if step % 100 == 0:
                    self._plot_distributions(sess, real)  	


    def _samples(self, session, real, num_points=10000, num_bins=100):
        xs = np.linspace(-self.data_range, self.data_range, num_points)
        bins = np.linspace(-self.data_range, self.data_range, num_bins)

        # data distribution
        d = real(num_points)
        # 画出准确值的结果
        pd, _ = np.histogram(d, bins=bins, density=True)

        # generated samples
        zs = np.linspace(-self.data_range, self.data_range, num_points)
        g = np.zeros((num_points, 1))
        # 按batch_size的大小一批一批计算结果
        for i in range(num_points // self.batch_size):
            g[self.batch_size * i:self.batch_size * (i + 1)] = session.run(self.g, {
                self.z: np.reshape(
                    zs[self.batch_size * i:self.batch_size * (i + 1)],
                    (self.batch_size, 1)
                )
            })
        # 计算得到生成器的y值
        pg, _ = np.histogram(g, bins=bins, density=True)

        return pd, pg

    def _plot_distributions(self, session, real):
        pd, pg = self._samples(session, real)
        p_x = np.linspace(-self.data_range, self.data_range, len(pd))
        f, ax = plt.subplots(1)
        ax.set_ylim(0, 1)
        # 画图:点=(p_x的元素,pd的元素)
        plt.plot(p_x, pd, label='real data')
        # 画图:点=(p_x的元素,pg的元素)
        plt.plot(p_x, pg, label='generated data')
        plt.title('1D Generative Adversarial Network')
        plt.xlabel('Data values')
        plt.ylabel('Probability density')
        plt.legend()
        plt.show()





# main函数
if __name__ == '__main__': 
    # 确定真钞的形状:[batch_size,1]
    # 确定假钞材料的形状:[batch_size,1]
    # 确定给分的形状:[batch_size,1]

    # 获得预训练判别器的真钞:均匀分布下随机生成[batch_size,1]
    data = Data()
    # 定义GAN的模型
    gan = GAN(batch_size=12,learning_rate=0.03, num_pretrain_steps=1000,num_steps=1200)
    # 训练判别器
    gan.train(data.train.next_batch,data.train.real,data.train.noise)
