'''
    Usage: python mnist.py
'''
import keras
from keras.datasets import mnist
from keras.models import Sequential
from keras.layers import Dense, Flatten, Conv2D, MaxPooling2D
from keras import backend as K

########################################################################################
# Create Dataset
########################################################################################
# define image width & image height
img_width, img_height = 28, 28
# get mnist data
(X_train, y_train), (X_test, y_test) = mnist.input_data()
# reshape X
if K.image_data_format() == 'channels_first':
    X_train = X_train.reshape(X_train.shape[0], 1, img_width, img_height)
    X_test = X_test.reshape(X_test.shape[0], 1, img_width, img_height)
    input_shape = (1, img_width, img_height)
else:
    X_train = X_train.reshape(X_train.shape[0], img_width, img_height, 1)
    X_test = X_test.reshape(X_test.shape[0], img_width, img_height, 1)
    input_shape = (img_width, img_height, 1)

# reshape y
y_train = 




########################################################################################
# Build Model
########################################################################################
# init model
# conv1
# pooling1
# conv2
# pooling2
# nn
# nn
# calc train_op


########################################################################################
# train
########################################################################################
# fit



########################################################################################
# Evaluate
########################################################################################
# eval
