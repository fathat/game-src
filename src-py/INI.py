import string
import textutil
import re


# def parse_equals_statement(  statement ):
# 	"""Parses a string in the form of 'a=somevalue', and returns (a, somevalue) """
# 	statement = statement.strip( string.whitespace )
# 	t = statement.split('=')
# 	return t[0].strip( string.whitespace ), encode_string_to_value( t[1] )
	
def parse_ini_line( line ):
	try:
		m = re.search( r"(?P<key>\w+)\s*=\s*(?P<value>.*)", line )
		key = m.group("key")
		val = m.group("value")
		#if value is surrounded by quotes, remove the quotes
		n = re.search( "\"(?P<inner_string>.*)\"", val )
		if n:
			val = n.group("inner_string" ) 
		#return the key and the value
		return key, textutil.encode_string_to_value( val ) 
	except:
		return None

class INIFile(object):
	values = {}
	
	def __init__(self, filename ):
		current_section = ""
		section_regexp = re.compile(r"\[(.*)\]")  #regexp for matching against []
		f = open( filename )
		for line in f:
			m = section_regexp.search(line)
			if m != None: #must be a group name
				current_section = m.group(1)
			else: #must be a key/value
				eq = parse_ini_line(line)
				if eq:
					values[ (current_section, eq[0]) ] = eq[1]
	
	def getValue( self, section, key ):
		rval = None
		try:
			rval = self.values[(section,key)]
		except:
			rval = None
		return rval

