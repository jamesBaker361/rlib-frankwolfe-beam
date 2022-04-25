import gym
import envs

import frankwolfe as fw
import unittest
import gc
import pickle
from random import randint, choice,sample

from envs.traffic_env_dir.traffic_env import TrafficEnv

import tensorflow as tf

edges={}
demand={}
nodes=10
commuters=set()
paths=set()
for z in range(5):
    x=randint(0,nodes)
    y=randint(0,nodes)
    while x==y or (x,y) in commuters:
        y=randint(0,nodes)
        x=randint(0,nodes)
    commuters.add((x,y))
    stops=randint(1,3)
    intermediate=[z for z in range(nodes) if z!=x and z!=y]
    path=(x,* sample(intermediate,stops),y)
    while path in paths:
        path=(x,* sample(intermediate,stops),y)
    paths.add(path)
    for p in range(len(path)-1):
        paths.add((path[p],path[p+1]))



for key in ["edge_tail", "edge_head", "length", "capacity", "speed"]:
    edges[key]=[]

for key in ["origin","destination","volume"]:
    demand[key]=[]

for (t,h) in commuters:
    demand["origin"].append(t)
    demand["destination"].append(h)
    demand["volume"].append(randint(20,100))

for p in sorted(paths):
    if len(p)>2:
        continue
    (t,h)=p
    edges["edge_tail"].append(t)
    edges["edge_head"].append(h)
    edges["length"].append(randint(5,10))
    edges["capacity"].append(randint(20,100))
    edges["speed"].append(randint(25,70))

#print(example.flow(demand,edges))
#assign=fw.getFWAssignment(demand,edges)
#assign=fw.getFWAssignment(demand,edges)

'''atp=fw.AssignTrafficPython()
state=atp.flow(demand,edges,10)
print(state)
exit()'''

config={
			"demand": demand,
			"perturbed" :edges,
			"fake_flow" : [randint(20,50) for i in range(len(edges["speed"]))],
			"real_flow": [randint(20,50) for i in range(len(edges["speed"]))],
            "horizon":10
		}

env=gym.make('TrafficEnv-v0',config=config)
import ray
from ray.rllib import agents
ray.init() # Skip or set to ignore if already called
agent_config = {'gamma': 0.9,
          'lr': 1e-2,
          'vf_clip_param': 1000,
          'num_workers': 4,
          'train_batch_size': 1000,
          'model': {
              'fcnet_hiddens': [128, 128]
          },
          "env_config":config}
trainer = agents.ppo.PPOTrainer(env=TrafficEnv, config=agent_config)
max_training_episodes = 1000
while True:
    results = trainer.train()
    # Enter whatever stopping criterion you like
    print('Mean Rewards:\t{:.1f}'.format(results['episode_reward_mean']))
    if results['episodes_total'] >= max_training_episodes:
        break
for k in results.keys():
    print(k)