'''
    Usage: python model.py
'''
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
