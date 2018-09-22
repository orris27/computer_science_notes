'''
    int_text => [num_batches, batch_size, num_steps]
'''
import tensorflow as tf
import numpy as np

def make_batches(data_file, batch_size, num_steps):
    # put together a large sentence
    with open(data_file,'r') as f:
        sentence = ' '.join([line.strip() for line in f.readlines()])
    # split into [int, ]
    data = np.array(sentence.strip().split())
    # calculate number of batches (len(data) - 1, because we need an extra elm)
    num_batches = (len(data) - 1 ) // (batch_size * num_steps)

    ###############################################################################
    # features
    ###############################################################################
    # capture the first (num_batches * batch_size * num_steps) elms
    features = data[:(num_batches * batch_size * num_steps)]
    # split into num_batches batches => (num_batches, batch_size * num_steps)
    features = np.split(features, num_batches)
    # reshape => [num_batches, batch_size, num_steps]
    features = np.reshape(features, [num_batches, batch_size, num_steps])
     
     
    ###############################################################################
    # label
    ###############################################################################
    # capture the first elm to (num_batches * batch_size * num_steps + 1) elms
    labels = data[1:(num_batches * batch_size * num_steps + 1)]
    # split into num_batches batches => (num_batches, batch_size * num_steps)
    labels = np.split(labels, num_batches)
    # reshape => [num_batches, batch_size, num_steps]
    labels = np.reshape(labels, [num_batches, batch_size, num_steps])

    # return zip (features,label)
    return list(zip(features,labels))
