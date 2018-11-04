'''
    Usage: python dcgan-mnist.py
    编写代码时的收获: 1. 计算梯度的的时候,使用各自的GradientTape
                      2. 应用梯度的时候,使用各自的Optimizer
                      3. 生成图像检查的时候,注意Discriminator传入的real_images和fake_images形状是一样的,根据这一点,我们可以将fake_images还原成可以看的图片
                      4. Evaluate和Train的时候,Dropout和BatchNormal表现不一样,必须通过training控制
                      5. eager mode还是什么原因,总之MNIST输入必须使用tf.keras.datasets.mnist.load_data()这个API,得到的结果是ndarray,形状为[60000, 28, 28] 和 [60000,]
                      6. 真实图片可以考虑手动切换到[-1, 1]的区域之间,之后再传入网络
                      7. Discriminator中如果报错最后全连接层时说shape不对,是因为real_images和fake_images的shape不对,前者已经定义好Fully_connected层的结构了,所以如果fake_images不正确就会出现问题
'''
import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data
import numpy as np
import matplotlib.pyplot as plt
import sys

config = tf.ConfigProto()
config.gpu_options.allow_growth = True
tf.enable_eager_execution(config=config)
# eager mode

############################################################
# Constants
############################################################
# noise_size
noise_size = 100
# num_epochs
num_epochs = 300
# batch_size
batch_size = 256



############################################################
# Models
############################################################
# define generator: [batch_size, noise_size] => [batch_size, width, height, 1]
class Generator(tf.keras.Model):
    def __init__(self):
        super(Generator, self).__init__()
        # fc1
        #self.dense1 = tf.keras.layers.Dense(width * height * 64, activation=None)
        self.dense1 = tf.keras.layers.Dense(7* 7* 64, use_bias=False)
        # bn1
        self.bn1 = tf.keras.layers.BatchNormalization()
        # conv1
        #self.conv1 = tf.keras.layers.Conv2D(filters=64, kernel_size=[5, 5], padding='same', activation=None)
        self.conv1 = tf.keras.layers.Conv2DTranspose(filters=64, kernel_size=[5, 5], strides=[1, 1], padding='same', use_bias=False)
        # bn2
        self.bn2 = tf.keras.layers.BatchNormalization()
        # conv2
        #self.conv2 = tf.keras.layers.Conv2D(filters=32, kernel_size=[5, 5], padding='same', activation=None)
        self.conv2 = tf.keras.layers.Conv2DTranspose(filters=32, kernel_size=[5, 5], strides=[2, 2], padding='same', use_bias=False)
        # bn3
        self.bn3 = tf.keras.layers.BatchNormalization()
        # conv3
        #self.conv3 = tf.keras.layers.Conv2D(filters=1, kernel_size=[5, 5], padding='same', activation=None)
        self.conv3 = tf.keras.layers.Conv2DTranspose(filters=1, kernel_size=[5, 5], strides=[2, 2], padding='same', use_bias=False)

    def call(self, inputs, training=True):
        # fc: [batch_size, noise_size] => [batch_size, 7 * 7 * 64]
        net = self.dense1(inputs) 
        # BN
        net = self.bn1(net, training=training)
        # relu
        net = tf.nn.relu(net)
        # reshape: [batch_size, 7 * 7 * 64] => [batch_size, 7, 7, 64]
        net = tf.reshape(net, [batch_size, 7, 7, 64]) 
        # Conv2DTranspose: [batch_size, 7, 7, 64] => [batch_size, 7, 7, 64]
        net = self.conv1(net)
        # BN
        net = self.bn2(net, training=training)
        # relu
        net = tf.nn.relu(net)
        # Conv2DTranspose: [batch_size, 7, 7, 64] => [batch_size, 7, 7, 32]
        net = self.conv2(net)
        # BN
        net = self.bn3(net, training=training)
        # relu
        net = tf.nn.relu(net)
        # Conv2DTranspose: [batch_size, 7, 7, 32] => [batch_size, 7, 7, 1]
        net = self.conv3(net)
        # tanh
        net = tf.tanh(net)
        return net



# define discriminator: [batch_size, n, n, 1] => [batch_size, 1]
class Discriminator(tf.keras.Model):
    def __init__(self):
        super(Discriminator, self).__init__()
        # conv1
        #self.conv1 = tf.keras.layers.Conv2D(filters=64, kernel_size=[5, 5], padding='same', activation=None)
        self.conv1 = tf.keras.layers.Conv2D(filters=64, kernel_size=[5, 5], strides=[2, 2], padding='same')
        # dropout: 0.3
        self.dropout = tf.keras.layers.Dropout(0.3)
        # conv2
        #self.conv2 = tf.keras.layers.Conv2D(filters=128, kernel_size=[5, 5], padding='same', activation=None)
        self.conv2 = tf.keras.layers.Conv2D(filters=128, kernel_size=[5, 5], strides=[2, 2], padding='same')
        # flatten
        self.flatten1 = tf.keras.layers.Flatten()
        # fc1
        self.fc1 = tf.keras.layers.Dense(1)

    def call(self, inputs, training=True):
        # Conv2D: [batch_size, n, n, 1] => [batch_size, n, n, 64]
        net = self.conv1(inputs)
        # leaky relu
        net = tf.nn.leaky_relu(net)
        # dropout:
        net = self.dropout(net, training=training)
        # Conv2D: [batch_size, n, n, 64] => [batch_size, n, n, 128]
        net = self.conv2(net)
        # leaky relu
        net = tf.nn.leaky_relu(net)
        # dropout
        net = self.dropout(net, training=training)
        # flatten
        net = self.flatten1(net)
        # FC: [batch_size, n, n, 128] => [batch_size, 1]
        net = self.fc1(net)
        return net


# define discriminator loss: real_pred, fake_pred
def get_disc_loss(real_pred, fake_pred):
    # calc cross entropy of ones and real_pred
    real_loss = tf.losses.sigmoid_cross_entropy(multi_class_labels = tf.ones_like(real_pred), logits=real_pred)
    # calc cross entropy of zeros and fake_pred
    fake_loss = tf.losses.sigmoid_cross_entropy(multi_class_labels = tf.zeros_like(fake_pred), logits=fake_pred)
    # sum up
    return real_loss + fake_loss

# define generator loss: fake_pred
def get_gen_loss(fake_pred):
    # calc cross entropy of ones and fake_pred
    fake_loss = tf.losses.sigmoid_cross_entropy(multi_class_labels = tf.ones_like(fake_pred), logits=fake_pred)
    # return
    return fake_loss



(train_images, train_labels), (_, _) = tf.keras.datasets.mnist.load_data()

train_images = train_images.reshape(train_images.shape[0], 28, 28, 1).astype('float32')
# We are normalizing the images to the range of [-1, 1]
train_images = (train_images - 127.5) / 127.5

buffer_size = 60000
train_dataset = tf.data.Dataset.from_tensor_slices(train_images).shuffle(buffer_size).batch(batch_size)


generator = Generator()
discriminator = Discriminator()

# new
generator.call = tf.contrib.eager.defun(generator.call)
discriminator.call = tf.contrib.eager.defun(discriminator.call)

noise = np.random.uniform([batch_size, noise_size])
# define a function that generates & saves a picture 
def generate_save_image(noise, epoch):
    # get noise
    # generate
    fake_images = generator(noise, training=False)

    # plt.figure
    #plt.figure()
    plt.figure(figsize=[4, 4])
    # plt.imshow
    #plt.imshow(fake_images[0].numpy())
    image = np.squeeze(fake_images[0].numpy())
    image = image * 127.5 + 127.5
    plt.imshow(image, cmap='gray')
    # plt.savefig
    plt.savefig("myimages/pic" + str(epoch) + ".png")

#! Use different optimizer
#opt = tf.train.AdamOptimizer()
gen_opt = tf.train.AdamOptimizer(1e-4)
disc_opt = tf.train.AdamOptimizer(1e-4)




############################################################
# Train
############################################################
for epoch in range(num_epochs):
    # get images, labels
    for images in train_dataset:
        # get noise 
        noise = tf.random_uniform([batch_size, noise_size])
        # tape gradients
        with tf.GradientTape() as disc_tape, tf.GradientTape() as gen_tape:
            # generate a sample from noise
            #fake_images = generator(noise)
            fake_images = generator(noise, training=True)
            # discriminate images
            #real_pred = discriminator(images)
            real_pred = discriminator(images, training=True)
            # discriminate fake_images
            #fake_pred = discriminator(fake_images)
            fake_pred = discriminator(fake_images, training=True)

            disc_loss = get_disc_loss(real_pred, fake_pred)
            gen_loss = get_gen_loss(fake_pred)



        # calc gradients for discriminator
        disc_gradients = disc_tape.gradient(disc_loss, discriminator.variables)
        # calc gradients for generator
        gen_gradients = gen_tape.gradient(gen_loss, generator.variables)


        # apply gradients to discriminator
        disc_opt.apply_gradients(zip(disc_gradients, discriminator.variables))
        # apply gradients to generator
        gen_opt.apply_gradients(zip(gen_gradients, generator.variables))
        #? How to train the model in the eager?

        # generates & saves a picture
        #generate_save_image(tf.random_normal([1, noise_size]), epoch)
        generate_save_image(noise, epoch)


        print("epoch={2}    gen_loss={0}    disc_loss={1}".format(gen_loss, disc_loss, epoch))
        sys.stdout.flush()




