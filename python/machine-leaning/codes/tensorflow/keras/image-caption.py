'''
    配置环境: 1. 当前目录下放"train2014"(COCO中的train2014.zip解压出来)
             2. 当前目录下放"captions_train2014.json"
             3. 当前目录下放"surf.jpg"(比如可以使用官网的冲浪图片https://github.com/tensorflow/tensorflow/blob/r1.11/tensorflow/contrib/eager/python/examples/generative_examples/image_captioning_with_attention.ipynb)
             
    Usage: 配置环境 & 取消110行左右的所有注释,获取npy文件,然后python image-caption.py & 之后可以注释110行的内容,这样就可以省下npy的制作,直接python image-caption.py
'''
import matplotlib.pyplot as plt
import re
from PIL import Image
import pickle
from glob import glob
import numpy as np
import time
import os
import sys
import json
from sklearn.utils import shuffle
from sklearn.model_selection import train_test_split
import tensorflow as tf
#gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.7)
#config = tf.ConfigProto(gpu_options=gpu_options)
#tf.enable_eager_execution(config=config)


config = tf.ConfigProto()
config.gpu_options.allow_growth = True
tf.enable_eager_execution(config=config)

#tf.enable_eager_execution()




##############################################################################
# Constants
##############################################################################
# select the first 30000 samples
num_samples = 30000

# define current dir
curr_dir = os.path.abspath('.')

# define top_k
top_k = 5000

batch_size = 64
buffer_size = 1000
units = 512
features_shape = 2048
attention_features_shape = 64
num_epochs = 20


##############################################################################
# Download images, annotations, inceptionv3 model
##############################################################################


##############################################################################
# get captions and corresponding filenames
##############################################################################
# open the file 
with open("./captions_train2014.json", "r") as f:
    # load the json 
    annotations = json.load(f)

# obtain captions
# obtain filenames
l = [[os.path.join(curr_dir, 'train2014', 'COCO_train2014_' + '%012d.jpg' % (annotation['image_id'])), annotation['caption']] for image, annotation in zip(annotations['images'], annotations['annotations'])]

# add <start> & <end>
for (index, (filename, caption)) in enumerate(l):
    l[index][1] = '<start> ' + caption + ' <end>'


    
# shuffle
l = shuffle(l, random_state=1)


filenames_train = []
captions_train = []
index = 0
for filename_caption in l:
    index += 1
    if index > num_samples:
        break
    filenames_train.append(filename_caption[0])
    captions_train.append(filename_caption[1])

del l

print(len(filenames_train))
print(filenames_train[:10])
print(captions_train[:10])


##############################################################################
# build dataset
##############################################################################
# define image's height and width
image_h, image_w = 299, 299

# define a load function
def load_image(filename):
    image_contents = tf.read_file(filename)
    image = tf.image.decode_jpeg(image_contents,channels=3)
    image = tf.image.resize_images(image,(image_h,image_w))
    image = tf.keras.applications.inception_v3.preprocess_input(image)
    return image, filename

image_model = tf.keras.applications.InceptionV3(include_top=False, weights='imagenet')
new_input = image_model.input
hidden_layer = image_model.layers[-1].output
image_features_extract_model = tf.keras.Model(new_input, hidden_layer)


print(len(filenames_train))
unique_filenames_train = sorted(set(filenames_train))
print(len(unique_filenames_train))        

dataset = tf.data.Dataset.from_tensor_slices(unique_filenames_train).map(load_image).batch(batch_size)


# pc = 0
# for batch_images, batch_filenames in dataset:
#     batch_features = image_features_extract_model(batch_images)
#     # batch_features => shape=(16, 8, 8, 2048), dtype=float32
# 
#     batch_features = tf.reshape(batch_features, (batch_features.shape[0], -1, batch_features.shape[3]))
#     # batch_features => shape=(16, 64, 2048), dtype=float32
# 
#     for feature, filename in zip(batch_features, batch_filenames):
#         # feature => shape=(64, 2048), dtype=float32
#         # filename => tf.Tensor(b'/home/user/orris/image-caption/tutorial/train2014/COCO_train2014_000000000025.jpg', shape=(), dtype=string)
#         filename = filename.numpy().decode('utf-8')
#         #np.save(filename, feature)
#         np.save(filename, feature.numpy())
#         pc += 1
#         if pc % log_every == 0:
#             print("%d has finished..."%(pc))

################################################################################################################
# NLP

def calc_max_length(tensor):
    return max(len(t) for t in tensor)

tokenizer = tf.keras.preprocessing.text.Tokenizer(num_words=top_k, oov_token='<unk>', filters='!"#$%&()*+.,-/:;=?@[\]^_`{|}~ ')
tokenizer.fit_on_texts(captions_train)
indices_train = tokenizer.texts_to_sequences(captions_train)

tokenizer.word_index = {key: value for key, value in tokenizer.word_index.items() if value <= top_k}

#tokenizer.word_index[tokenizer.oov_token] = top_k + 1
tokenizer.word_index[tokenizer.oov_token] = top_k 
tokenizer.word_index['<pad>'] = 0

indices_train = tokenizer.texts_to_sequences(captions_train)

index_word = {value: key for key, value in tokenizer.word_index.items()}

# padding
padded_indices_train = tf.keras.preprocessing.sequence.pad_sequences(indices_train, padding='post')
# we only need filenames_train & padded_indices_train

max_length = calc_max_length(indices_train)


filenames_train, filenames_test, padded_indices_train, padded_indices_test = train_test_split(filenames_train, padded_indices_train, test_size=0.2, random_state=0)


print(len(filenames_train), len(filenames_test), len(padded_indices_train), len(padded_indices_test))


###############################################
# build dataset 
###############################################
vocab_size = len(tokenizer.word_index)

def map_func(filename, padded_indices):
    image_tensor = np.load(filename.decode("utf-8")+".npy")
    return image_tensor, padded_indices

dataset = tf.data.Dataset.from_tensor_slices((filenames_train,padded_indices_train))
dataset = dataset.map(lambda item1, item2: tf.py_func(
            map_func, [item1, item2], [tf.float32, tf.int32]),
            num_parallel_calls=8)

dataset = dataset.shuffle(buffer_size)
dataset = dataset.batch(batch_size)
dataset = dataset.prefetch(1)
# repeat num_epochs


####################################################################################
# build model
####################################################################################

embedding_size = 256
class CNNEncoder(tf.keras.Model):
    def __init__(self, embedding_size):
        super(CNNEncoder, self).__init__()

        self.dense = tf.keras.layers.Dense(embedding_size, activation=tf.nn.relu)

    def call(self, inputs):
        return self.dense(inputs)


gru_size = 512
# define GRU
def gru(units):
    if tf.test.is_gpu_available():
        return tf.keras.layers.CuDNNGRU(units,
                                        return_sequences=True,
                                        return_state=True,
                                        recurrent_initializer='glorot_uniform')
    else:
        return tf.keras.layers.GRU(units,
                                   return_sequences=True,
                                   return_state=True,
                                   recurrent_activation='sigmod',
                                   recurrent_initializer='glorot_uniform')

#wv_size = 256
wv_size = 512

class BahdanauAttention(tf.keras.Model):
    def __init__(self, wv_size):
        super(BahdanauAttention, self).__init__()
        
        self.V = tf.keras.layers.Dense(wv_size, activation=None)
        self.W = tf.keras.layers.Dense(wv_size, activation=None)

        self.U = tf.keras.layers.Dense(1, activation=None)

        

    def call(self, encoder_output, hidden_state):
    
        # encoder_output: [batch_size, 64, embedding_size]
        # hidden_state: [batch_size, gru_size]

        hidden_state_with_time = tf.expand_dims(hidden_state, axis=1)

        weights = tf.nn.softmax(self.U(tf.tanh(self.V(encoder_output) + self.W(hidden_state_with_time))), axis=1)

        context = tf.reduce_sum(weights * encoder_output, axis=1)

        # context: [batch_size, embedding_size]
        return context

# define decoder
class RNNDecoder(tf.keras.Model):
    def __init__(self, embedding_size, wv_size, vocab_size):
        super(RNNDecoder, self).__init__()

        self.embedding = tf.keras.layers.Embedding(vocab_size, embedding_size)
        self.attention = BahdanauAttention(wv_size)
    
        self.gru_size = gru_size
        self.gru = gru(self.gru_size)


        #?the dims of the outputs from these functions
        #!the number of first layer's node is [units:512], which is also the hidden size of GRU
        #self.dense1 = tf.keras.layers.Dense(embedding_size, activation=tf.nn.relu)
        self.dense1 = tf.keras.layers.Dense(self.gru_size, activation=None)
        #!The final fully connected layer's hidden size should be vocab_size, because we will compute loss from it and [batch_size, vocab_size] using sparse_softmax
        #self.dense2 = tf.keras.layers.Dense(embedding_size, activation=tf.nn.relu)
        self.dense2 = tf.keras.layers.Dense(vocab_size, activation=None)
        

    def call(self, decoder_input, encoder_output, hidden_state):
        # decoder_input: [batch_size, 1]
        decoder_input = self.embedding(decoder_input)

        # decoder_input: [batch_size, 1, embedding_size]

        # computes attention => context
        context = self.attention(encoder_output, hidden_state)
        # context: [batch_size, embedding_size]
        
        # concat context & decoder_input
        #decoder_input = tf.concat([decoder_input, tf.expand_dims(context, axis=1)], -1)
        decoder_input = tf.concat([tf.expand_dims(context, axis=1), decoder_input], -1)

        # gru
        #output, hidden_state = self.gru(decoder_input, hidden_state)
        output, hidden_state = self.gru(decoder_input)
        
        # fc1
        output = self.dense1(output)

        output = tf.reshape(output, [-1, output.shape[-1]])

        # fc2
        output = self.dense2(output)

        #?what is the output
        return output, hidden_state

    def reset_state(self, batch_size):
        return tf.zeros([batch_size, self.gru_size])

# def __init__(self, embedding_size):
encoder = CNNEncoder(embedding_size)
#def __init__(self, embedding_size, wv_size, vocab_size):
decoder = RNNDecoder(embedding_size, units, vocab_size)


def calc_loss(logits, labels):
    # get mask
    mask = 1 - np.equal(labels, 0)
    # calc & dot mask
    #loss = tf.reduce_mean(tf.square(logits - labels) * mask)
    # squeeze the logits to [batch_size, vocab_size]
    loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = labels,logits = logits) * mask)
    return loss




def evaluate(filename):

    image, _ = load_image(filename)
    images = np.expand_dims(image, axis=0)

    # extract features from image
    dimages = image_features_extract_model(images)
    #dimages => shape=(1, 8, 8, 2048), dtype=float32

    # CNN encoder => encoder_output
    encoder_output = encoder(dimages)

    # reshape encoder_output to [batch_size, 64, 256]
    encoder_output = tf.reshape(encoder_output, [1, -1, encoder_output.shape[-1]])


    # init decoder_input 
    decoder_input = tf.expand_dims([tokenizer.word_index['<start>']], 0)


    # get initial hidden state: [batch_size, gru_size]
    hidden_state = decoder.reset_state(batch_size=1)

    # result = []
    result = []
    # for curr_timestep in range(max_timesteps):
    for i in range(1, padded_indices.shape[1]):
        # RNN decoder => decoder_output, hidden_state
        # decoder_input: [batch_size, 1]
        # hidden_state: [batch_size, gru_size]
        # encoder_output: [batch_size, 8, 8, 256]
        output, hidden_state = decoder(decoder_input, encoder_output, hidden_state)


        # append decoder_output to result
        # output => [1, 1, vocab_size]
        #result.append((tf.squeeze(tf.argmax(output, axis=-1))).numpy())
        result.append((tf.squeeze(tf.argmax(output, axis=-1))).numpy())
        # output => []

        if tokenizer.index_word[result[-1]] == '<end>':
            break
        
        # assign decoder_output to decoder_input
        decoder_input = tf.expand_dims(tf.argmax(output, axis=-1), axis=1)
        
    # translate result
    #for index in result:
    #    print(tokenizer.index_word[index])
    print(' '.join([tokenizer.index_word[index] for index in result]))




####################################################################################
# train
####################################################################################

#learning_rate = 0.01 
optimizer = tf.train.AdamOptimizer()
for epoch in range(num_epochs):

    total_loss = 0

    #?a new way of iterating datasets?
    for (step, (dimages, padded_indices)) in enumerate(dataset):
        # one batch

        # initial loss
        loss = 0

        # get initial hidden state: [batch_size, gru_size]
        hidden_state = decoder.reset_state(batch_size=padded_indices.shape[0])

        # encode dimages using CNN: [batch_size, 64, embedding_size]
        #encoder = CNNEncoder(embedding_size)
        #encoder_output = encoder(embedding_size)

        # initialize init input as "<start>"s: [batch_size, 1]
        decoder_input = tf.expand_dims([tokenizer.word_index["<start>"]] * batch_size, axis=1)


        with tf.GradientTape() as tape:

            # def call(self, inputs):
            encoder_output = encoder(dimages)

            # iterates all time steps
            #?Why does the official codes use "range(1, xxx.shape[1])"
            #!Because we have at each time step, we need to use the output and the next timestep's target to compute loss, and if we are at the last time step, then we will have no <curr_timestep + 1>'s output
            #![1, padded_indices.shape[1]) instead of [0, padded_indices.shape[1] - 1) depends on your arguments in calc_loss
            for curr_timestep in range(1, padded_indices.shape[1]):
                # get hidden cell state input
                # decoder => output, hidden_state
                #def call(self, decoder_input, hidden_state, encoder_output):
                # decoder_input: [batch_size, 1]
                # hidden_state: [batch_size, gru_size]
                # encoder_output: [batch_size, 64, 256]
                output, hidden_state = decoder(decoder_input, encoder_output, hidden_state)

                # expands hidden state
                #?I think the output of GRU is 3 dims, so is it necessary to exapand the dims of hidden_state?

                # We do not convert output (batch_size, 1, 256) into (batch_size, 1)
                #!We do not need to convert it because we use sparse_softmax_...
                #output = tf.argmax(output, axis=-1, output_type=tf.int32)

                # calc current loss & update loss
                # output => [batch_size, 1, vocab_size]
                loss += calc_loss(logits=output, labels=padded_indices[:, curr_timestep])

                # get decoder input
                # convert output (batch_size, 1, 256) into (batch_size, 1)
                #!During training process, the decoder input should be the padded_indices!!!
                #decoder_input = tf.argmax(output, axis=-1)
                decoder_input = tf.expand_dims(padded_indices[:, curr_timestep], axis=1)

        #?Where to apply masking? At the satuation where we compute loss

        # computes grads
        #?How to get all variables: the class that inherits tf.keras.Model has attribute called variables
        grads = tape.gradient(loss, encoder.variables + decoder.variables)



        #?Can we use tf.apply_gradients?? => NO! We use optimizer.apply_gradients
        #?Do we need to save the return value of apply_gradients in eager mode?
        # apply grads
        train = optimizer.apply_gradients(zip(grads, encoder.variables + decoder.variables))

        #!We should print the average loss based on one timestep, which means we need to divide the <loss> by num_timesteps
#log_every = 100
        log_every = 20

        if step % log_every == 0:
            print("batch {2} step {0}: loss={1}".format(step, loss.numpy() / (int)(padded_indices.shape[1]), epoch))
            evaluate("surf.jpg")
            sys.stdout.flush()


####################################################################################
# Evaluate
####################################################################################
# def load_image(filename):
#     image_contents = tf.read_file(filename)
#     image = tf.image.decode_jpeg(image_contents,channels=3)
#     image = tf.image.resize_images(image,(image_h,image_w))
#     image = tf.keras.applications.inception_v3.preprocess_input(image)
#     return image, filename
