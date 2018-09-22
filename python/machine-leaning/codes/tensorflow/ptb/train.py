'''
    Train a model that have high fitting degree with PTB dataset
'''
import tensorflow as tf
import numpy as np
import sys
import data_loader



tf.flags.DEFINE_integer("train_batch_size", 20, "batch size of training")
tf.flags.DEFINE_integer("train_num_steps", 35, "num of layers of training")
tf.flags.DEFINE_string("train_data", "ptb.train", "filename of training data")

tf.flags.DEFINE_integer("num_layers", 2, "num of layers")
tf.flags.DEFINE_integer("lstm_size", 30, "hidden size of lstm")
tf.flags.DEFINE_float("lstm_keep_prob", 0.9, "keep prob of training")
tf.flags.DEFINE_float("embedding_keep_prob", 0.9, "keep prob of embedding")
tf.flags.DEFINE_integer("vocab_size", 10000, "the number of different words")
tf.flags.DEFINE_boolean("share_emb_and_softmax", False, "share the weights of embedding layer and softmax layer")
tf.flags.DEFINE_integer("max_grad_norm", 5, "control grad's value")
tf.flags.DEFINE_float("learning_rate", 1.0, "learning rate")
tf.flags.DEFINE_integer("eval_batch_size", 1, "batch size of training")
tf.flags.DEFINE_integer("eval_num_steps", 1, "num of layers of training")

tf.flags.DEFINE_string("eval_data", "ptb.eval", "filename of eval data")
tf.flags.DEFINE_string("test_data", "ptb.test", "filename of test data")
tf.flags.DEFINE_integer("num_epochs", 5, "num of epochs")


FLAGS = tf.flags.FLAGS
FLAGS(sys.argv) # 启用flags


class PTBModel(object):
    def __init__(self,num_steps, lstm_size, vocab_size, num_layers, share_emb_and_softmax, is_training, learning_rate, max_grad_norm,batch_size):
        with tf.name_scope('placeholder'):
            # features placeholder:[batch_size, num_steps]
            self.features = tf.placeholder(tf.int32,[None,num_steps], name='features') 
            # labels placeholder:[batch_size, num_steps]
            self.labels = tf.placeholder(tf.int32,[None,num_steps], name='labels')

            self.keep_prob = tf.placeholder(tf.float32, name='keep_prob')

        self.num_steps = num_steps
        self.lstm_size = lstm_size
        self.vocab_size = vocab_size
        self.num_layers = num_layers
        self.share_emb_and_softmax = share_emb_and_softmax
        self.is_training = is_training
        self.learning_rate = learning_rate
        self.max_grad_norm = max_grad_norm
        
        
        ###############################################################################
        # embedding
        ###############################################################################
        # assign lstm_size to embedding_size
        embedding_size = lstm_size

        # define an embedding matrix: [vocab_size, embedding_size(lstm_size)]
        W_embedding = tf.get_variable(name="W_embedding", shape=[vocab_size,embedding_size])

        # embedding: => [batch_size, num_steps, lstm_size]
        embedded_chars = tf.nn.embedding_lookup(W_embedding, self.features)

        # dropout
        # new
        if is_training:
            embedded_chars = tf.nn.dropout(embedded_chars,FLAGS.embedding_keep_prob)
        #
        #
        ###############################################################################
        # lstm
        ###############################################################################
        # define a multiple LSTM cell
        def create_lstm_cell(lstm_size, output_keep_prob):
            lstm_cell=tf.contrib.rnn.BasicLSTMCell(lstm_size,state_is_tuple=True)
            lstm_cell = tf.nn.rnn_cell.DropoutWrapper(lstm_cell, output_keep_prob=output_keep_prob)
            return lstm_cell

        lstm_cell = tf.nn.rnn_cell.MultiRNNCell([create_lstm_cell(lstm_size, self.keep_prob) for _ in range(num_layers)])

        # dynamic_run: outputs=[batch_size, num_steps, lstm_size]
        outputs,final_state = tf.nn.dynamic_rnn(lstm_cell,embedded_chars,dtype=tf.float32)


        ###############################################################################
        # softmax => [batch_size, num_steps, vocab_size]
        ###############################################################################
        # if share weights with embedding layer:
        if share_emb_and_softmax:
            # #W_softmax <= get_variable('embedding'): [vocab_size, embedding_size]
            # directly transpose W_embedding
            W_softmax = tf.transpose(W_embedding)
            # transpose W_softmax: => [lstm_size, vocab_size]
        else:
            W_softmax = tf.get_variable(name='W_softmax',shape=[lstm_size, vocab_size])
        # define a bias: [vocab_size]
        b_softmax = tf.get_variable(name="b_softmax", shape=[vocab_size])


        # get outputs: reshape output into [batch_size * num_steps, lstm_size]
        outputs = tf.reshape(outputs, [-1, lstm_size])
        #outputs = tf.reshape(tf.concat(outputs,1),[-1,lstm_size])


        # outputs * W + b: => [batch_size, num_steps, vocab_size]
        y_predicted = tf.matmul(outputs,W_softmax) + b_softmax
       
        # get labels: reshape self.labels into [batch_size * num_steps]
        # calc loss
        self.loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = tf.reshape(self.labels, [-1]),logits = y_predicted))

        #self.loss = tf.reduce_sum(tf.nn.sparse_softmax_cross_entropy_with_logits(labels = tf.reshape(self.labels, [-1]),logits = y_predicted)) / batch_size
        

        # if not is_training:
        if not is_training:
            # return 
            return 

        # calc gradients
        params = tf.trainable_variables()
        opt = tf.train.GradientDescentOptimizer(learning_rate)
        gradients = tf.gradients(self.loss * num_steps, params)
        # clip by grad norm
        clipped_gradients, norm = tf.clip_by_global_norm(gradients,max_grad_norm)
        # apply gradients
        self.train = opt.apply_gradients(zip(clipped_gradients, params))

def run_epoch(sess, model, batches, keep_prob, train_op, is_training):
    '''
        train, evaluate and test the model
    '''

    # define total_loss: 0.0
    total_loss = 0.0
    # define step: 0
    step = 0
    iters = 0
    # for X, y in batches:
    for X, y in batches:
        # update step
        step += 1
        # run train_op and loss_op
        if is_training:
            _, loss= sess.run([model.train, model.loss],feed_dict={model.features:X,model.labels:y,model.keep_prob:keep_prob})
        else:
            loss = sess.run([model.loss],feed_dict={model.features:X,model.labels:y,model.keep_prob:keep_prob})
        iters += model.num_steps

        # add loss_op to total_loss
        total_loss += loss
        # divide total_loss by step to get loss_per_word
        loss_per_word = total_loss / step
        # calc perplexity: np.exp(loss_per_word)
        perplexity = np.exp(loss_per_word)
        # print perplexity
        if step % 100 == 0:
            #print('step {0}:{1}'.format(step,perplexity))
            print('step {0}:{1}'.format(step,np.exp(total_loss / step)))
    # return perplexity
    return perplexity


def main():
    # get train_batches
    train_batches = data_loader.make_batches(FLAGS.train_data,FLAGS.train_batch_size,FLAGS.train_num_steps)
    # get eval_batches
    eval_batches = data_loader.make_batches(FLAGS.eval_data,FLAGS.eval_batch_size,FLAGS.eval_num_steps)
    # get test_batches
    test_batches = data_loader.make_batches(FLAGS.test_data,FLAGS.eval_batch_size,FLAGS.eval_num_steps)
    # define an initializer
    initializer=tf.random_uniform_initializer(-0.05, 0.05)

    # with tf.variable_scope('model',reuse=False,initializer)
    with tf.variable_scope('model',reuse=False,initializer=initializer):
        # construct a train model object
        train_model = PTBModel(FLAGS.train_num_steps, FLAGS.lstm_size, FLAGS.vocab_size, FLAGS.num_layers, FLAGS.share_emb_and_softmax, True, FLAGS.learning_rate, FLAGS.max_grad_norm,FLAGS.train_batch_size)
    # with tf.variable_scope('model',reuse=True,initializer)
    with tf.variable_scope('model',reuse=True,initializer=initializer):
        # construct a eval model object: eval model also used as test model
        eval_model = PTBModel(FLAGS.eval_num_steps, FLAGS.lstm_size, FLAGS.vocab_size, FLAGS.num_layers, FLAGS.share_emb_and_softmax, False, FLAGS.learning_rate, FLAGS.max_grad_norm,FLAGS.eval_batch_size)

    # start session
    gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.7)
    config = tf.ConfigProto(gpu_options=gpu_options)
    with tf.Session(config=config) as sess:
        sess.run(tf.global_variables_initializer())
        # for i in range(num_epochs):
        for epoch in range(FLAGS.num_epochs):
            # run train epoch
            train_perplexity = run_epoch(sess,train_model,train_batches, FLAGS.lstm_keep_prob, train_model.train, True)
            # print train perplexity
            print('epoch {0}: perplexity={1}'.format(epoch,train_perplexity))
            # run eval epoch
            eval_perplexity = run_epoch(sess,eval_model,eval_batches, 1, tf.no_op(), False)
            # print eval perplexity
            print('epoch {0}: perplexity={1}'.format(epoch,eval_perplexity))
        # run test epoch
        # print test perplexity

if __name__ == '__main__':
    main()
