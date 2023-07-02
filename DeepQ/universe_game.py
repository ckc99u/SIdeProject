"""
Atari game: Breakout-v4
tensorflow cpu
gym 0.9.5
python3 universe_game.py
"""
import sys
import gym
import universe
import numpy as np
import random
from DeepQNet import DeepQNetwork
import matplotlib.pyplot as plt
np.random.seed(1)
STEP = 500
EPISODE = 10000##


def main():


  env = gym.make('Breakout-v4')
  #env.configure(remotes=1)  # automatically creates a local docker container
  next_state = None; state_n = None; decade = 0
  agent = DeepQNetwork(env)
  #print("test1")
  record = []
  x = []
  for ep in range(EPISODE):
    reward = 0
    state = env.reset()
    state = agent.img2Bin.RGB2Binary(state)
    state_n = np.stack((state, state, state, state), axis = 2)

    for i in range(STEP):
      env.render()
      act_id = agent.policy(state_n)

      next_s, r, done, _ = env.step(act_id)
      next_s = np.reshape(agent.img2Bin.RGB2Binary(next_s), (80,80, 1))

      next_state = np.append(next_s, state_n[:,:,:3], axis =2)
      reward += r
      ##(self, cur_state, next_state, action_id, reward, done)
      agent.replay_control(state_n, next_state, act_id, r, done)

      state_n = next_state
      if done:
        break

    #print('episode: ', ep, ', reward: ',reward)
    decade += reward
    record.append(reward)
    x.append(ep)
    if(ep % 10 == 0):
      print('Decade: ', ep/10, ', rotal r: ', decade)
      print('==============')
      decade = 0
  print(record)
  plt.plot(x,record)
  plt.xlabel("Episode")
  plt.ylabel("Rewards")
  plt.show()
    


if __name__ == '__main__':
  main()

