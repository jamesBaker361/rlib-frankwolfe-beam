import frankwolfe as fw
import unittest
import gc
import pickle
from random import randint, choice,sample

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

pairs=fw.importODPairsFrom(demand)
graph=fw.Graph(edges,0,100)

def make_orderer():
    order = {}

    def ordered(f):
        order[f.__name__] = len(order)
        return f

    def compare(a, b):
        return [1, -1][order[a] < order[b]]

    return ordered, compare

ordered, compare = make_orderer()
unittest.defaultTestLoader.sortTestMethodsUsing = compare

class FWTestCase(unittest.TestCase):
    def setUp(self):
        pairs=fw.importODPairsFrom(demand)
        graph=fw.Graph(edges,0,100)
        self.before_assignment=fw.FrankWolfeAssignment(graph,pairs,False,False)
        '''self.before_cod=fw.ClusteredOriginDestination(1,2,3,4,5,6)
        self.after_cod=pickle.loads(pickle.dumps(self.before_cod))
        self.before_fwa_stats=fw.FrankWolfeAssignmentStats()
        self.after_fwa_stats=pickle.loads(pickle.dumps(self.before_fwa_stats))
        self.before_all_stats=fw.AllOrNothingAssignmentStats(0)
        self.after_all_stats=pickle.loads(pickle.dumps(self.before_all_stats))
        self.before_graph=fw.Graph(edges,0,100)
        self.after_graph=pickle.loads(pickle.dumps(self.before_graph))'''

        
    @ordered
    def test_cod(self):
        self.before_cod=fw.ClusteredOriginDestination(1,2,3,4,5,6)
        self.after_cod=pickle.loads(pickle.dumps(self.before_cod))
        self.assertEqual(self.before_cod.destination,self.after_cod.destination,"ClusteredOD destination mismatch!")
        self.assertEqual(self.before_cod.origin,self.after_cod.origin,"ClusteredOD origin mismatch!")
        self.assertEqual(self.before_cod.rebalancer,self.after_cod.rebalancer,"ClusteredOD rebalancer mismatch!")
        self.assertEqual(self.before_cod.edge1,self.after_cod.edge1,"ClusteredOD edge1 mismatch!")
        self.assertEqual(self.before_cod.edge2,self.after_cod.edge2,"ClusteredOD edge2 mismatch!")
        self.assertEqual(self.before_cod.volume,self.after_cod.volume,"ClusteredOD volume mismatch!")

    @ordered
    def test_fwa_stats(self):
        pass

    @ordered
    def test_all_stats(self):
        pass

    @ordered
    def test_graph(self):
        for ce in [x for x in range(0,125,25)]:
            self.before_graph=fw.Graph(edges,ce,100-ce)
            self.after_graph=pickle.loads(pickle.dumps(self.before_graph))
            self.assertEqual(self.before_graph.constParameter,self.after_graph.constParameter)
            self.assertEqual(self.before_graph.ceParameter,self.after_graph.ceParameter)
            self.assertEqual(self.before_graph.edges,self.after_graph.edges)
        

    @ordered
    def test_fw_before(self):
        pairs=fw.importODPairsFrom(demand)
        graph=fw.Graph(edges,0,100)
        self.before_assignment=fw.FrankWolfeAssignment(graph,pairs,False,False)
        self.before_assignment.runPython(10)
        self.before_assignment.runPython(10)
    
    @ordered
    def test_fw_after(self):
        self.after_assignment=pickle.loads(pickle.dumps(self.before_assignment))
        #self.after_assignment.runPython(10)
        #self.after_assignment.runPython(10)

    @ordered
    def test_atp(self):
        self.before_assignment_wrapper=fw.AssignTrafficPython()
        self.before_assignment_wrapper.flow(demand,edges,10)
        self.before_assignment_wrapper.flow(demand,edges,10)
        self.after_assignment_wrapper=pickle.loads(pickle.dumps(self.before_assignment_wrapper))
        self.after_assignment_wrapper.flow(demand,edges,10)
        self.after_assignment_wrapper.flow(demand,edges,10)

cod=fw.ClusteredOriginDestination(1,2,3,4,5,6)


#ass.runPython(10)
#ass.runPython(11)


unittest.main()

'''
print(len(demand["destination"]))

pairs=fw.importODPairsFrom(demand)

print("pairs",pairs.__repr__)

graph=fw.Graph(edges,0,100)
ass=fw.FrankWolfeAssignment(graph,pairs,True,False)
f=ass.runPython(10)

pickle.dumps(ass)
pickle.dumps(graph)
pickle.dumps(pairs)

print("python over")
'''