'''
    Usage: Download text8.zip(http://mattmahoney.net/dc/text8.zip) and `python skip-gram.py`
    Training Dataset: 
        features: [batch_size] (indices)
        labels: [batch_size, 1] (indices)
    Graph: skip-gram.png
'''
import numpy as np
import tensorflow as tf
import zipfile
import collections
import math
import matplotlib.pyplot as plt



###############################################################################
# Constants
###############################################################################

filename = "text8.zip"

#vocab_size = 5000
vocab_size = 50000
skip_window = 1
num_skips = 2

#batch_size = 8
batch_size = 128

#embedding_size = 1000
embedding_size = 128

num_sampled = 64
learning_rate = 0.01
#num_epochs = 10001
num_epochs = 100001
log_every = 100

valid_size = 20
valid_window = 100
top_k = 10


picname = "my-pic.png"

###############################################################################
# Build dataset: generate_batch
###############################################################################


# read the text8.zip into a list of words

with zipfile.ZipFile(filename) as f:
    words = (tf.compat.as_str(f.read(f.namelist()[0]))).split()

    
# transfer the list of words into a list of indices
def words_to_indices(words):
    # count words and filter the most common words
    count = collections.Counter(words).most_common(vocab_size - 1)
    # add <unk>
    freq = [['<unk>', -1]]
    freq.extend(count)
    
    dictionary = dict()
    # calc the dictionary
    for i in range(vocab_size):
        dictionary[freq[i][0]] = i

    # calc the reverse_dictionary
    reverse_dictionary = dict(zip(dictionary.values(), dictionary.keys()))

    # define a indices
    indices = []
    # create indices
    for word in words:
        if word in dictionary:
            indices.append(dictionary[word])
        else:
            indices.append(0)
    
    return indices, dictionary, reverse_dictionary
    
indices, dictionary, reverse_dictionary = words_to_indices(words)

del words # save memory

data_index = 0

# define generate_batch function
def generate_batch(indices, batch_size):

    global data_index
    len_indices = len(indices)

    X = np.ndarray(shape=(batch_size,), dtype=np.int32)
    y = np.ndarray(shape=(batch_size, 1), dtype=np.int32)

    dq_len = skip_window * 2 + 1
    # create a deque with length of "skip_window * 2 + 1"
    dq = collections.deque(maxlen=dq_len)
    # init dq
    for _ in range(dq_len):
        dq.append(indices[data_index])
        data_index = (data_index + 1) % len_indices

    sub = 0
    for _ in range(batch_size // num_skips):

        # obtain the feature: dq[skip_window]
        # randomly choose num_skips elms from 0 to skip_window*2+1(not included) except skip_window
        # 1. create a list <choices>:[0, skip_window*2+1)
        choices = [i for i in range(skip_window * 2 + 1)]
        # 2. remove skip_window
        choices.pop(skip_window)
        # 3. while len(choices) != num_skips:
        while len(choices) != num_skips:
            # 1. randomly determine a elm: [0, len(choices))
            choice = np.random.randint(0, len(choices))
            # 2. remove it
            choices.pop(choice)

        # obtain the label
        for choice in choices:
            X[sub] = dq[skip_window]
            y[sub][0] = dq[choice]
            sub += 1

        # move the dq
        dq.append(indices[data_index])
        data_index = (data_index + 1) % len_indices

    return X, y

###############################################################################
# Construct a tensor graph: train + valid
###############################################################################




graph = tf.Graph()
with graph.as_default():
    with tf.name_scope('placeholder'):
        # define features placeholder
        features = tf.placeholder(tf.int32,[batch_size], name='features') 
        # define labels placeholder
        labels = tf.placeholder(tf.int32,[batch_size,1], name='labels')


    # define embedding matrix
    embedding = tf.Variable(tf.random_uniform([vocab_size, embedding_size], -1.0, 1.0))
    # embedding: [batch_size] => [batch_size, embedding_size]
    embedded_chars = tf.nn.embedding_lookup(embedding, features)

    with tf.name_scope("nce"):
        # define NCE loss
        nce_weights = tf.Variable(tf.truncated_normal([vocab_size, embedding_size], stddev=1.0 / math.sqrt(embedding_size)))
        nce_biases = tf.Variable(tf.zeros([vocab_size]))

        loss = tf.reduce_mean(tf.nn.nce_loss(weights = nce_weights, # [num_classes, embedding_size]
                                             biases = nce_biases, # [num_classes] 
                                             inputs = embedded_chars, # [batch_size, embedding_size]
                                             labels = labels, # [batch_size, num_true]; num_true means represents the number of positive samples
                                             num_sampled = num_sampled, # the number of negative samples
                                             num_classes = vocab_size)) # num_classes

    # train
    #train = tf.train.AdamOptimizer(learning_rate).minimize(loss)
    train = tf.train.GradientDescentOptimizer(1.0).minimize(loss)

    # randomly generate dataset designed to valid: [valid_size]
    valid_features = np.random.choice(valid_window, size=(valid_size,), replace=False)

    # define unitized embedding matrix
    def unitize_2d(vectors):
        return vectors / tf.sqrt(tf.reduce_sum(tf.square(vectors), axis=1, keepdims=True))

    unitized_embedding = unitize_2d(embedding)

    # embedding: [valid_size] => [valid_size, embedding_size]
    valid_embedded_chars = tf.nn.embedding_lookup(unitized_embedding, valid_features)

    # NN using unitized embedding matrix: [valid_size, embedding_size] => ([embedding_size, vocab_size]) => [valid_size, vocab_size]
    final_embedding = tf.matmul(valid_embedded_chars, unitized_embedding, transpose_b=True)


###############################################################################
# Train + valid(print)
###############################################################################



# check for accuracy
# words: [valid_size, vocab_size]
# labels: [valid_size]. contains the indices of these words
def check(words, labels):
    # sort the dim1 reversely
    res = []
    # pick out the first <top_k> for each word
    for word in words:
        res.append(((-word).argsort())[:top_k])
    
    # print dictionary[word] & dictionary[top_k_word]
    for i, label in enumerate(labels):
        print("Nearest to", reverse_dictionary[label],end=': ')
        for j in range(top_k):
            print(reverse_dictionary[res[i][j]], end=' ')
        print()


gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.7)
config=tf.ConfigProto(gpu_options=gpu_options)
# with tf.Session() as sess:
with tf.Session(config=config, graph=graph) as sess:
    # init global variables
    tf.global_variables_initializer().run()


    # define a loss to recv
    total_loss = 0.

    num_batches = len(indices) // batch_size

    #print("Start {0} epochs' training...".format(num_epochs * num_batches))


    # for i in range(num_epochs * num_batches):
    #for step in range(num_epochs * num_batches):
    for step in range(num_epochs):
        # get X,y from generate_batch
        X, y = generate_batch(indices, batch_size)
        # train and get NCE loss
        _, loss1 = sess.run([train, loss], feed_dict = {features:X, labels:y})

        total_loss += loss1

        # if step % LOG_EVERY == 0:
        if step % log_every == 0:
            # print NCE loss
            print("step", step, ": loss =" ,total_loss / (step + 1))

    # get final embedding matrix
    final_embedding1 = sess.run(final_embedding)
    
    # check for accuracy
    check(final_embedding1, valid_features)

    # TSNE
    from sklearn.manifold import TSNE
    tsne = TSNE(perplexity=30, n_components=2, init='pca', n_iter=5000)

    unitized_embedding1 = sess.run(unitized_embedding)

    plot_only = 100
    low_dim_embs = tsne.fit_transform(unitized_embedding1[:plot_only])

    # get indices, translate
    annotations = []
    for i in range(plot_only):
        annotations.append(reverse_dictionary[i])
    
    # draw a graph that describes the embedding matrix
    def plot_with_labels(low_dim_embs, annotations):
        plt.figure(figsize=(18,18))
        for i, annotation in enumerate(annotations):
            x, y = low_dim_embs[i, :]
            plt.scatter(x, y)
            plt.annotate(annotation,
                         xy = (x, y),
                         xytext = (5, 2),
                         textcoords = 'offset points',
                         ha = 'right',
                         va = 'bottom')

        plt.savefig(picname)

    plot_with_labels(low_dim_embs,annotations)

