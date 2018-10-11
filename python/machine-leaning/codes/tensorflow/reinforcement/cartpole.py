'''
    Usage: python cartpole.py
'''
import tensorflow as tf
import numpy as np
import gym




################################################################
# Constants
################################################################

# define num_episodes
#num_episodes = 300
num_episodes = 10000 
# define hidden_size
hidden_size = 50
# define env_dims
env_dims = 4
# define action_dims
action_dims = 1
# define gamma
gamma = 0.99
# define learning rate
#learning_rate = 1.0
learning_rate = 0.1

# define batch_size
batch_size = 25

################################################################
# Graph
################################################################
with tf.name_scope("placeholder"):
    # define <input_e> as env state: [env_dims]
    input_e = tf.placeholder(tf.float32,[None, env_dims], name='input_e') 


with tf.name_scope("nn1"):
    # define weights: [env_dims, hidden_size]
    #W1 = tf.Variable(tf.random_normal([env_dims,hidden_size],stddev = 1/tf.sqrt(float(hidden_size))))
    W1 = tf.get_variable("W1", shape=[env_dims, hidden_size], initializer=tf.contrib.layers.xavier_initializer())
    # activator(tf.matmul() + biases)
    a1 = tf.nn.relu(tf.matmul(input_e, W1))

with tf.name_scope("nn2"):
    # define weights: [hidden_size, action_dims]
    #W2 = tf.Variable(tf.random_normal([hidden_size,action_dims],stddev = 1/tf.sqrt(float(action_dims))))
    W2 = tf.get_variable("W2", shape=[hidden_size, action_dims], initializer=tf.contrib.layers.xavier_initializer())
    # define biases: [action_dims]
    #b2 = tf.Variable(tf.zeros([action_dims])+0.1)
    # activator(tf.matmul() + biases) => probability
    probability = tf.nn.sigmoid(tf.matmul(a1, W2))

with tf.name_scope("placeholder"):
    # define <advantages> to recv discounted rewards: [None]
    #advantages = tf.placeholder(tf.float32,[None], name='advantages') 
    advantages = tf.placeholder(tf.float32,[None, 1], name='advantages') 
    # define <false_labels> as labels: [None]
    #false_labels = tf.placeholder(tf.float32,[None], name='false_labels') 
    false_labels = tf.placeholder(tf.float32,[None, 1], name='false_labels') 

    # define <grad1>: [None] 
    #grad1 = tf.placeholder(tf.float32,[None], name='grad1') 
    grad1 = tf.placeholder(tf.float32, name='grad1') 
    # define <grad2>: [None]
    #grad2 = tf.placeholder(tf.float32,[None], name='grad2') 
    grad2 = tf.placeholder(tf.float32, name='grad2') 


with tf.name_scope("loss"):
    # calc the log((labels - probability)2): loss
    loglik = tf.log((false_labels - probability) ** 2)
    #loglik = tf.log(false_labels * (false_labels - probability) + (1 - false_labels) * (false_labels + probability))
    # calc the loss: loss * advantages
    # loss = loss * advantages
    loss = -tf.reduce_mean(loglik * advantages)

with tf.name_scope("gradients"):
    # get trainable variables
    params = tf.trainable_variables()
    # calc the gradients from loss and trainable variables
    gradients = tf.gradients(loss, params)
    #################################################################################
    # gradients: [W1_grad, W2_grad]
    # W1_grad.shape <= (4,50)
    # W2_grad.shape <= (50,1)
    # In gradients, it was viewed as [tf.Tensor(...), tf.Tensor(...)], but we should know that the tf.Tensor(...).eval() might be an array
    #################################################################################

    # define optimizer
    #opt = tf.train.GradientDescentOptimizer(learning_rate)
    opt = tf.train.AdamOptimizer(learning_rate)

    batch_grad = [grad1, grad2]
    # update gradients with grad1 and grad2
    train = opt.apply_gradients(zip(batch_grad, params))

################################################################
# Train
################################################################



env = gym.make("CartPole-v0")

# define a function to calc discounted future reward from a list
def discount(rewards):
    # init an array with the same shape as rewards
    discounted_rewards = np.zeros_like(rewards)
    # init <discounted_value>: 0
    discounted_value = 0
    

    # for ri in reversed(range(len(rewards)):
    for ri in reversed(range(len(rewards))):
        # discounted_value = discounted_value * gamma + rewards[ri]
        discounted_value = discounted_value * gamma + rewards[ri]
        # assign discounted_value to discounted_rewards[ri]
        discounted_rewards[ri] = discounted_value
    # return discounted_rewards
    return discounted_rewards

gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.7)
config = tf.ConfigProto(gpu_options=gpu_options)



# start session
with tf.Session(config=config) as sess:
    sess.run(tf.global_variables_initializer())

    # init <grads> with gradients from tensorflow: [batch_size]
   

    #for i in range(gradients):
    #   grads[i] = gradients[i] * 0

    # init avg_reward: 0
    avg_reward = 0

    # init curr_episode
    curr_episode = 0

    # reset total_reward: 0
    total_reward = 0

    # reset game & get the current observation
    observation = env.reset()



    #grads_refer = sess.run(gradients, feed_dict={})
    #grads = [np.zeros(shape=(env_dims, hidden_size),dtype=np.float32), np.zeros(shape=(hidden_size, action_dims),dtype=np.float32)]
    grads_buf = sess.run(params)
    for index, grad in enumerate(grads_buf):
        grads_buf[index] = grad * 0


    # reset xs, ys, drs
    xs = []
    ys = []
    drs = []

    # while curr_episode < num_episodes:
    while curr_episode < num_episodes:


        observation = np.reshape(observation, (1, env_dims))
        # get the probability with the observation
        probability1 = sess.run(probability, feed_dict = {input_e: observation})

        # calc the action from the probability
        action = 1 if np.random.random() < probability1 else 0


        ###############################################################
        # append the observation before env.step!!
        ###############################################################
        xs.append(observation)


        # get new observation, reward, done from the execution
        observation, reward, done, _  = env.step(action)
        

        # update total_reward
        total_reward += reward
        # append the observation to xs
        #xs.append(observation)
        # append (1 - probability) to ys
        #ys.append(1 - probability1)
        ys.append(1 - action)
        # append the reward to drs
        drs.append(reward)
        
        # if done:
        if done:

            # update curr_episode
            curr_episode += 1

            # stack xs => vxs
            vxs = np.vstack(xs)
            # stack ys => vys
            vys = np.vstack(ys)
            # stack drs => vdrs
            vdrs = np.vstack(drs)

            # calc the discounted reward from drs
            vdiscounted_rewards = discount(vdrs)
            #vdiscounted_rewards = np.vstack(discounted_rewards)

            # reset xs, ys, drs
            xs = []
            ys = []
            drs = []

            def standardize(l):
                l -= np.mean(l)
                l /= np.std(l)
                return l
            
            vdiscounted_rewards = standardize(vdiscounted_rewards)

            # calc curr_gradient: {input_e: xs, false_labels: vys, advantages: vdrs}
            curr_gradients = sess.run(gradients, feed_dict = {input_e: vxs, false_labels: vys, advantages: vdiscounted_rewards})

            #if curr_episode % batch_size == 0:
            #   print(curr_gradients)



            # reset env
            #env.reset()
            observation = env.reset()

        
            # for i, curr_grads in enumerate(curr_gradients):
            for i, curr_grads in enumerate(curr_gradients):
                # update grads[0]: grads[0][i] = curr_grads[0]
                #grads[i] += curr_grads[i]
                # update grads[1]: grads[1][i] = curr_grads[1]
                #grads[1][i] = curr_grads[1]
                grads_buf[i] += curr_grads
    

            # if curr_episode % batch_size == 0:
            if curr_episode % batch_size == 0:

                # update gradients: {grad1: grads[0], grad2: grads[1]}
                sess.run(train, feed_dict = {grad1: grads_buf[0], grad2: grads_buf[1]})
                # reset grads
                #grads = []
                #for i in range(gradients):
                #   grads[i] = gradients[i] * 0
                #grads = [np.zeros(shape=(env_dims, hidden_size),dtype=np.float32), np.zeros(shape=(hidden_size, action_dims),dtype=np.float32)]
                for index, grad in enumerate(grads_buf):
                    grads_buf[index] = grad * 0
                

                # calc average reward: total_reward / batch_size
                avg_reward = total_reward / batch_size
                # print average reward
                print("Episode {0}: avg_reward={1}\ttotal_reward={2}".format(curr_episode, avg_reward, total_reward))

                # reset total_reward: 0
                total_reward = 0
