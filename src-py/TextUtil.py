#Contains utility functions for working with text/strings. 
import string
import re

def remove_quotes( line ):
	val = line
	n = re.search( "\"(?P<inner_string>.*)\"", val )
	if n:
		val = n.group("inner_string" ) 
	return val

def parse_equals_statement(  statement ):
	"""Parses a string in the form of 'a=somevalue', and returns (a, somevalue) """
	statement = statement.strip( string.whitespace )
	t = statement.split('=')
	return t[0].strip( string.whitespace ), encode_string_to_value( t[1] )

def encode_string_to_value( val ):
	"""Takes in a string value, and checks if it's a number. If it is, casts it to an int,
	otherwise it just returns the string (with whitespace stripped)."""
	val = val.strip( string.whitespace )
	try:
		v = int( val )
	except:
		try:
			v = float( val )
		except: 
			v = val
	return v

