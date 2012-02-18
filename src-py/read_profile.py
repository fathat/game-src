import profile
import hotshot, hotshot.stats
import sys

if __name__ == '__main__':
	#psyco.full()
	stats = hotshot.stats.load(sys.argv[1])
	stats.sort_stats('time', 'calls') #'cumulative'
	stats.print_stats(30)
	stats.sort_stats('cumulative', 'calls') #'cumulative'
	stats.print_stats(30)
	stats.sort_stats('calls', 'calls') #'cumulative'
	stats.print_stats(30)
