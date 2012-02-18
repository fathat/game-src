#!/usr/bin/env python

from distutils.core import setup
import py2exe
import glob

opts = {
	"py2exe" : {
		"optimize" : 2,
		"dist_dir" : "../exe/",
		"excludes" : ["Settings", "hotshot", "profile"]
	}
}

setup ( name = 'HatGame',
		version = '1.0',
		description = 'HatGame written in python! EXCITING!',
		author = 'Ian Overgard',
		author_email = 'ian.overgard@gmail.com',
		url='http://www.facehatsoftware.com',
		console=["main.py"],
		data_files=["Settings.py"],
		options = opts
	   )
