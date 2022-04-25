# Frank-Wolfe for Traffic Assignment Python Wrapper and Reinforcement Learning Framework

This repo contains the python wrapper for the C++ code that performs frank-Wolfe for Traffic Assignment
As well as a gym environment package called "envs" for training RL agents
The Dockerfile also pulls from this repo (https://github.com/jamesBaker361/surrogate.git) that
contains the code for running Rlib experiments in the rlib folder
The Dockerfile also pulls the data from https://bucketbeam.s3-website-us-west-1.amazonaws.com/scratch.zip
which takes a while, so removing these lines in the Dockerfile

```
RUN wget https://bucketbeam.s3-website-us-west-1.amazonaws.com/scratch.zip
RUN unzip scratch.zip
```

will make things faster