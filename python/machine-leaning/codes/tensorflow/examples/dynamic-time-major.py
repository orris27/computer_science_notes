'''
    本身不能运行,只说明time_major=True的时候需要同时改变输入和输出
'''
# reshape
inputs=tf.reshape(features,[-1,train_times,n_inputs])
##############################################################################
# inputs需要改变
##############################################################################
inputs = tf.transpose(inputs,[1,0,2])  ##

lstm_cell = tf.contrib.rnn.BasicLSTMCell(lstm_size,state_is_tuple=True)
lstm_cell = tf.nn.rnn_cell.DropoutWrapper(lstm_cell, output_keep_prob=0.5)
outputs,final_state=tf.nn.dynamic_rnn(lstm_cell,inputs,dtype=tf.float32,time_major=True)

##############################################################################
# outputs需要改变,后面使用final_state[1]的地方,用output就可以了
##############################################################################
output = tf.squeeze(outputs[-1,:,:])
