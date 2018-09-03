################################################################
# 1. 直接在scope的with语句里面开启重用
# 2. 使用scope.reuse_variables()开启,不过必须保证第一次创建的时候不能开启重用.(取消#代码的注释)
################################################################


import tensorflow as tf

#i = 0 
# 定义一个函数,获得"foo/v"变量
def foo():
    #global i
    with tf.variable_scope("foo", reuse=tf.AUTO_REUSE):
    #with tf.variable_scope("foo") as scope:
        norm = tf.random_normal_initializer(stddev=1.0)
        #if i == 1:
            #scope.reuse_variables()
        v = tf.get_variable("v", [1] ,initializer=norm)
        #i = 1
    return v

# 定义v1
v1 = foo()  # Creates v.
# 定义v2
v2 = foo()  # Gets the same, existing v.

# 开启session
with tf.Session() as sess:
    # 初始化所有变量
    sess.run(tf.global_variables_initializer())
    # 打印v1的值
    print("v1:",sess.run(v1))
    # 打印v2的值
    print("v2:",sess.run(v2))
