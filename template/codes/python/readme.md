    
## 1. TensorFlow
1. 定义features和labels
```
# [[特征值0,特征值1,特征值2,...,特征值783],
   [第二个实例],
   [第三个实例],]
features=tf.placeholder(tf.float32,[None,784]) 
# [[是分类0的可能性,是分类1的可能性,是分类2的可能性,...,是分类9的可能性],
   [第二个实例],
   [第三个实例]]
labels=tf.placeholder(tf.float32,[None,10])
keep_prob=tf.placeholder(tf.float32)
learning_rate=tf.Variable(1e-3)

```
2. NN
    1. 定义神经网络的一层
    ```
    # Layer1 (conv+pooling)

    # W
    W1=tf.Variable(tf.truncated_normal([5,5,1,32],stddev=0.1))
    # b
    b1=tf.Variable(tf.zeros([32])+0.1)
    # activate
    a1=tf.nn.relu(tf.nn.conv2d(a0,W1,strides=[1,1,1,1],padding='SAME')+b1)
    # pooling
    a1_pool=tf.nn.max_pool(a1,ksize=[1,2,2,1],strides=[1,2,2,1],padding='VALID')

    # a1_pool is [-1,14,14,32]

    ```
