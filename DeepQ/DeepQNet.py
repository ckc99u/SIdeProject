import tensorflow as tf
import numpy as np
import random
from collections import deque
from CVMapping import CVMapping

GAMMA = 0.9
EPSILON = 1.0
INC = 0.002
class DeepQNetwork():
    def __init__(self, env):
        self.img2Bin = CVMapping()
        self.epsilon_max = EPSILON
        

        self.epsilon = 0 if INC is not None else self.epsilon_max
        self.step = 0
        self.buffer = deque()
        self.action_len = env.action_space.n
        
        self.batch = 32

        self.session = tf.InteractiveSession()
        self.create()
        self.observe = 0
        
        ##write result to tensorboard
        self.merge = tf.summary.merge_all()
        self.summary_writer = tf.summary.FileWriter('./logs', self.session.graph)
        self.session.run(tf.initialize_all_variables())
    
    def replay_control(self, cur_state, next_state, action_id, reward, done):
        actions = np.zeros(self.action_len)

        actions[action_id] = 1
        self.buffer.append([cur_state, actions, reward, next_state, done])

        self.observe += 1

        if(self.observe % 1000 and self.observe <= 500 == 0):
            print(self.observe)
        if(len(self.buffer) > 5000):
            self.buffer.popleft()
        ##batch size
        if(len(self.buffer) > self.batch and self.observe > 500 ):
            self.train()

    def policy(self, cur_state):
        if(self.epsilon > 0.1 and self.observe > 500):
            self.epsilon -= (0.00001)

        actions = np.zeros(self.action_len)
        idx = None

        if np.random.uniform() < self.epsilon :
            return  random.randrange(self.action_len)
        else:
            predict = self.QValue.eval(feed_dict = {self.layer0: [cur_state]})[0]
            return np.argmax(predict)
        #actions[idx] = 1
        #return actions
        
        

    
    def create(self):
        self.layer0 = tf.placeholder(tf.float32,[None, 80, 80, 4], name='input')
        self.actions = tf.placeholder(tf.float32, [None, self.action_len])
        self.qs = tf.placeholder(tf.float32, [None])

        weight1 = tf.truncated_normal([8, 8, 4, 32], stddev=0.01)
        weight1 = tf.Variable(weight1)
        bias1 = tf.constant(0.01, shape=[32])
        bias1 = tf.Variable(bias1)
        cov1 = tf.nn.relu(tf.nn.conv2d(self.layer0, weight1, strides=[1, 4, 4, 1], padding='SAME') + bias1)
        conv1 = tf.nn.max_pool(cov1, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')
        
        weight2 = tf.truncated_normal([4, 4, 32, 64], stddev=0.01)
        weight2 = tf.Variable(weight2)
        bias2 = tf.constant(0.01, shape=[64])
        bias2 = tf.Variable(bias2)
        conv2 = tf.nn.relu(tf.nn.conv2d(conv1, weight2, strides=[1, 2, 2, 1], padding='SAME')+bias2)

        weight3 = tf.truncated_normal([3, 3, 64, 64], stddev=0.01)
        weight3 = tf.Variable(weight3)
        conv3 = tf.nn.relu(tf.nn.conv2d(conv2, weight3, strides=[1, 1, 1, 1], padding='SAME')+bias2)

        ##flat layer
        flat = tf.reshape(conv3, [-1,1600])
        ##fc layer
        weight4 = tf.truncated_normal([1600,512], stddev=0.01)
        weight4 = tf.Variable(weight4)
        bias4 = tf.constant(0.01, shape=[512])
        bias4 = tf.Variable(bias4)
        fc1 = tf.nn.relu(tf.matmul(flat, weight4) + bias4)

        weight5 = tf.truncated_normal([512, self.action_len], stddev=0.01)
        weight5 = tf.Variable(weight5)
        bias5 = tf.constant(0.01, shape=[self.action_len])
        bias5 = tf.Variable(bias5)
        self.QValue = (tf.matmul(fc1, weight5) + bias5)
        act = tf.reduce_sum(tf.multiply(self.QValue, self.actions), reduction_indices=1) 

        self.cost = tf.reduce_mean(tf.square(self.qs - act))
        ##adam
        ##self.optimizer = tf.train.AdamOptimizer(0.001).minimize(self.cost)
        self.optimizer = tf.train.RMSPropOptimizer(0.00025,0.99,0.0,1e-6).minimize(self.cost)




    def train(self):
        self.step += 1
        tmp = random.sample(self.buffer, self.batch)
        #[cur_state, actions, reward, next_state, done]
        cur_states = [i[0] for i in tmp]
        acts = [i[1] for i in tmp]
        rewards = [i[2] for i in tmp]
        nextstates = [i[3] for i in tmp]
        dones = [i[4] for i in tmp]

        values = self.QValue.eval(feed_dict = {self.layer0: nextstates})
        q = []
        for i in range(self.batch):
            if dones[i]:
                q.append(rewards[i])
            else:
                q.append(rewards[i] + GAMMA *np.max(values[i]))
        
        self.optimizer.run(feed_dict={
            self.layer0: cur_states,
            self.actions: acts,
            self.qs: q
        })

