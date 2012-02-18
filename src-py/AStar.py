import math
import Platform
import mathhat
import Settings

if Settings.OptimizePathing:
	import psyco

#printDebugInfo = False

class Node(object):
	__slots__ = ('x', 'y', 'parent', 'impassable',
				 'f', 'g', 'h', 'adjacent',
				 'open', 'closed',
				 '__weakref__')
	def __init__(self, x, y, impassable):
		self.x = x
		self.y = y
		self.parent = None
		self.impassable = impassable
		self.f = 0
		self.g = 0
		self.h = 0
		self.open = False
		self.closed = False
		self.adjacent = None

	def clear(self):
		self.open = False
		self.closed = False
		#self.f = 0
		#self.g = 0
		#self.h = 0

	def __repr__(self):
		return "<%s, %s>" % (str(self.x), str(self.y))


def _routeValue(fr, to):
	n = fr
	i = 0
	while n != to:
		i += 10
		n = n.parent

	return i

def _routeBack(fr, to):
	"""Returns the route back to a node"""
	n = fr
	path = []
	count = 0
	while n != to and n is not None:
		#print "Adding %s" % (str(n))
		if n in path:
			print path + [n]
			break
		count += 1
		path.append(n)
		n = n.parent
		if count > 200:
			print path
			raise "OH SHIT"

	path.append(to)
	path.reverse()
	return path

def _estimateValue(fr, to):
	a = to.x - fr.x
	b = to.y - fr.y
	return math.pow(a, 2) + math.pow(b, 2)

def _adjacentNodesForNode( nodes, w, h, current_node):
	x = current_node.x
	y = current_node.y
	adj = []
	west = (x-1)+y*w
	east = (x+1)+y*w
	north = x+(y-1)*w
	south = x+(y+1)*w
	nw = (x-1)+(y-1)*w
	ne = (x+1)+(y-1)*w
	sw = (x-1)+(y+1)*w
	se = (x+1)+(y+1)*w
	if x-1 >= 0 and not nodes[west].impassable:
		adj.append(nodes[west])
	if x+1 < w and not nodes[east].impassable:
		adj.append(nodes[east])
	if y-1 >= 0 and not nodes[north].impassable:
		adj.append(nodes[north])
	if y+1 < h and not nodes[south].impassable:
		adj.append(nodes[south])
	if x-1 >= 0 and y-1 >= 0 and not nodes[nw].impassable:
		if not nodes[north].impassable or not nodes[west].impassable:
			adj.append(nodes[nw])
	if x+1 < w and y-1 >= 0 and not nodes[ne].impassable:
		if not nodes[north].impassable or not nodes[east].impassable:
			adj.append(nodes[ne])
	if x-1 >= 0 and y+1 < h and not nodes[sw].impassable:
		if not nodes[south].impassable or not nodes[west].impassable:
			adj.append(nodes[sw])
	if x+1 <  w and y+1 < h and not nodes[se].impassable:
		if not nodes[south].impassable or not nodes[east].impassable:
			adj.append(nodes[se])
	return adj


#def _sortNodesByFCost(x, y):
#	return x.f-y.f

def aStarPath(nodes, width, height, start, end):
	"""start and end MUST be in nodes"""

	#if either the end or start is in impassable area, just
	#force a straight line path
	if end.impassable or start.impassable:
		return [start, end]

	open = [start]
	start.clear()
	touched = []

	while len(open) > 0:
		# Remove lowest f-cost 'node' from open list
		# and mark the current node as closed (so it's
		# not revisited later)
		current_node = open[0]
		#for node in open:
		#	if node.f < current_node.f:
		#		current_node = node
		touched.append(current_node)
		open.remove(current_node)
		current_node.open = False
		current_node.closed = True

		#test for goal
		if current_node == end:
			break
		adjacent = current_node.adjacent#_adjacentNodesForNode( nodes, width, height, current_node)

		#open adjacent nodes (if it makes sense to)
		for node in adjacent:
			if node.closed:
				continue
			if not node.open:
				node.parent = current_node
				node.g = node.parent.g + 10
				dx = end.x - node.x
				dy = end.y - node.y
				node.h = dx*dx + dy*dy
				node.f = node.g + node.h

				#insert in a sorted manner
				i=0
				added = False
				for n in open:
					if n.f > node.f:
						open.insert(i, node)
						added = True
						break
					i+=1
				if not added: open.append( node )
				node.open = True
				touched.append(node)
			else:
				if current_node.g + 10 < node.g:
					node.g = current_node.g + 10
					node.f = node.g + node.h

	#make sure to reset all the nodes for future calculations
	for n in touched:
		n.clear()

	return _routeBack(end, start)

class AStarMap(object):
	def __init__(self, map_mask, width, height):
		"""map_mask is a list of True/False values that
		indicate whether a given grid is passable"""
		self.width  = width
		self.height = height
		self.nodes  = ['HI']*len(map_mask)
		for y in xrange(self.height):
			for x in xrange(self.width):
				index = x+y*self.width
				self.nodes[index] = Node(x, y, not map_mask[index])
		self._computeAdjacency()

	def _computeAdjacency(self):
		for node in self.nodes:
			node.adjacent = _adjacentNodesForNode(self.nodes,
												   self.width,
												   self.height,
												   node)



	def path(self, start_x, start_y, end_x, end_y):
		nodePath = aStarPath(self.nodes,
							 self.width,
							 self.height,
							 self.nodes[start_x+start_y*self.width],
							 self.nodes[end_x+end_y*self.width])
		path = [ (n.x, n.y) for n in nodePath ]
		return path

if Settings.OptimizePathing:
	psyco.bind(AStarMap)
	psyco.bind(aStarPath)

def testC():
	mask = [0,0,0,0,0,0,0,0,
			0,0,0,0,1,0,0,0,
			0,0,0,0,1,0,0,0,
			0,0,0,0,1,0,0,0,
			0,0,0,0,1,0,0,0,
			0,0,0,0,1,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0]
	amap = mathhat.AStarMap( mask, 8, 8 )
	amap.printMask()


def testB():
	mask = [True,True,True,True,True, True,True,True,
			True,True,True,True,False, True,True,True,
			True,True,True,True,False,True,True,True,
			True,True,True,True,False,True,True,True,
			True,True,True,True,False,True,True,True,
			True,True,True,True,False,True,True,True,
			True,True,True,True,True, True,True,True,
			True,True,True,True,True, True,True,True]
	map = AStarMap( mask, 8, 8 )
	#for i in range(200):
	print map.path(2,2, 4,2)

def testA():
	start = Node(2, 2, False)
	end = Node(6, 2, False)

	nodes = [ Node(0, 0, False),
			  Node(1, 0, False),
			  Node(2, 0, False),
			  Node(3, 0, False),
			  Node(4, 0, False),
			  Node(5, 0, False),
			  Node(6, 0, False),
			  Node(7, 0, False),

			  Node(0, 1, False),
			  Node(1, 1, False),
			  Node(2, 1, False),
			  Node(3, 1, False),
			  Node(4, 1, True),
			  Node(5, 1, False),
			  Node(6, 1, False),
			  Node(7, 1, False),

			  Node(0, 2, False),
			  Node(1, 2, False),
			  start,
			  Node(3, 2, False),
			  Node(4, 2, True),
			  Node(5, 2, False),
			  end,
			  Node(7, 2, False),

			  Node(0, 3, False),
			  Node(1, 3, False),
			  Node(2, 3, False),
			  Node(3, 3, False),
			  Node(4, 3, True),
			  Node(5, 3, False),
			  Node(6, 3, False),
			  Node(7, 3, False),

			  Node(0, 4, False),
			  Node(1, 4, False),
			  Node(2, 4, False),
			  Node(3, 4, False),
			  Node(4, 4, False),
			  Node(5, 4, False),
			  Node(6, 4, False),
			  Node(7, 4, False),

			  Node(0, 5, False),
			  Node(1, 5, False),
			  Node(2, 5, False),
			  Node(3, 5, False),
			  Node(4, 5, False),
			  Node(5, 5, False),
			  Node(6, 5, False),
			  Node(7, 5, False),

			  Node(0, 6, False),
			  Node(1, 6, False),
			  Node(2, 6, False),
			  Node(3, 6, False),
			  Node(4, 6, False),
			  Node(5, 6, False),
			  Node(6, 6, False),
			  Node(7, 6, False),

			  Node(0, 7, False),
			  Node(1, 7, False),
			  Node(2, 7, False),
			  Node(3, 7, False),
			  Node(4, 7, False),
			  Node(5, 7, False),
			  Node(6, 7, False),
			  Node(7, 7, False)
			]

	print "Meow?"
	print aStarPath(nodes, 8, 8, start, end)

if __name__ == "__main__":
	#import hotshot, hotshot.stats
	#run_prof = hotshot.Profile('astar.profile')
	#run_prof.run(r'testB()')
	#stats = hotshot.stats.load('astar.profile')
	#stats.sort_stats('time', 'calls') #'cumulative'
	#stats.print_stats(30)
	#import psyco
	#psyco.full()
	start_time = Platform.time_in_seconds()
	testB()
	print "Time:", Platform.time_in_seconds() - start_time
	#testA()

