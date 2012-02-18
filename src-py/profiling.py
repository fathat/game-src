import pstats
p = pstats.Stats('gameprof')
p.sort_stats('time')
p.print_stats()
