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


In order to use the python wrapper, import the AssignTrafficPython class from the frankwolfe library
```
import frankwolfe as fw
atp=fw.AssignTrafficPython()
```

the AssignTrafficPython object has a flow method that takes in the demand, the edge netowrk and the number of iterations as params

The OD-matrix is represented as a dictionary

|origin|destination|volume|
|------|-----------|------|
|20|31|96|
|20|41|120|
|20|57|72|
|20|65|88|

```

demand={
    "origin":[20,20,20,20],
    "destination":[31,41,57,65],
    "volume":[96,120,72,88]
}
```

The road network is also represented as a dictionary 

|edge_tail|edge_head|length|capacity|speed|
|---------|---------|------|--------|-----|
|3|0|172|14783|56|
|2|1|9|14783|56|
|8|1|121|5279|40|
|0|2|51|10559|40|

```
edges={
    "edge_tail":[3,2,8,0],
    "edge_head":[0,1,1,2],
    "length":[172,9,121,51],
    "capacity":[14783,14783,5279,10559],
    "speed":[56,56,40,40]
}
```

The flow function returns a dictionary showing the flow on each edge

|edge_tail|edge_head|flow|
|---------|---------|------|
|3|0|50|
|2|1|60|
|8|1|70|
|0|2|80|

```
iterations= 100

flow=atp.flow(demand,edges,iterations)

'''
flow <- {
    "edge_tail":[3,2,8,0],
    "edge_head":[0,1,1,2],
    "flow":[50,60,70,80]
}
'''

```



