from setuptools import setup
import frankwolfe as fw
import gym
import numpy as np
import sys
import ray

setup(name='traffic_env',
    version='0.1',
    install_requires=['gym']
)