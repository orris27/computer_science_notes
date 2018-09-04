'''
    1. get_collection可以获得某个变量域的内所有可训练/...的张量
    2. assign函数可以将已经run出来的值赋值到张量中
'''
import tensorflow as tf


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



def create_v1():
    # 定义1个初始化器
    norm = tf.random_normal_initializer(stddev=1.0)
    # 定义1个变量v1
    v1 = tf.get_variable("v1", [1] ,initializer=norm)
    # 返回v1
    return v1

# 创建1个s1变量域
with tf.variable_scope("s1", reuse=tf.AUTO_REUSE):
    # 定义1个初始化器
    #norm = tf.random_normal_initializer(stddev=1.0)
    # 定义1个变量v1
    #v1 = tf.get_variable("v1", [1] ,initializer=norm)
    v1 = create_v1()

# 获得s1内的所有变量
#s1_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='s1')

# 创建1个s2变量域
with tf.variable_scope("s2", reuse=tf.AUTO_REUSE):
    # 定义1个初始化器
    norm = tf.random_normal_initializer(stddev=1.0)
    # 定义1个变量v2
    v2 = tf.get_variable("v2", [1] ,initializer=norm)
# 获得s2内的所有变量
#s2_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES,scope='s2')


# 开启session
with tf.Session() as sess:
    # 初始化所有变量
    sess.run(tf.global_variables_initializer())
    # 打印v1
    print("v1",sess.run(v1))
    # 打印v2
    print("v2",sess.run(v2))


    # 复制s1内的变量到s2的变量
    scope_assign('s1','s2',sess)


    # 打印v1
    print("v1",sess.run(v1))
    # 打印v2
    print("v2",sess.run(v2))
