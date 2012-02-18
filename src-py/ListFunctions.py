
def reinterpretFlatList( lst, tuple_size ):
	"""Reinterprets a flat list into a list of tuples. So, for instance,
	you might reinterpret a list like this: [1,2,3,4,5,6,7,8,9] into this:
	[(1,2,3),(4,5,6),(7,8,9)] with a given tuple_size of 3"""
	newlist = []
	r = [i for i in range(len(lst)) if i%tuple_size == 0 ] 
	for i in r:
		t = []
		for j in range(tuple_size):
			t.append( lst[i+j] )
		newlist.append( tuple(t) )
	return newlist

def flattenListOfLists( list ):
	newlist = []
	for l in list: newlist.extend(l)
	return newlist
	
if __name__ == '__main__':
	print flattenListOfLists( [[1,2], [3], [4,5,6]] )
