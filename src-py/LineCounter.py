import sys
import os
import os.path

class Counter(object):
	def __init__(self):
		self.lines        = 0
		self.codelines    = 0
		self.blanklines   = 0
		self.commentlines = 0

	def classify(self, line):
		fixed_line = line.strip()
		if fixed_line.startswith('#') or fixed_line.startswith('"""'):
			self.commentlines += 1
		elif fixed_line.isspace() or len(fixed_line) == 0:
			self.blanklines += 1
		else:
			self.codelines += 1
		self.lines += 1

	def __str__(self):
		return """Code: %s\nComments: %s\nBlank: %s\nTotal: %s""" % (
			str(self.codelines), str(self.commentlines), str(self.blanklines),
			str(self.lines) )

def all_python_files():
	return [x for x in os.listdir('.') if x.endswith('.py') ]

def count_lines_in_file( filename, counter):
	f = open(filename)
	for line in f:
		counter.classify( line )
	f.close()

def main():
	main_counter = Counter()
	for file in all_python_files():
		count_lines_in_file( file, main_counter)
	print main_counter


if __name__ == '__main__':
	main()
