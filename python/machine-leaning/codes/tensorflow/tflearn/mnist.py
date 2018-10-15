'''
    Usage: prepare MNIST_data/ & python mnist.py
'''
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


