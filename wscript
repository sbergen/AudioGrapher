#!/usr/bin/env python
# -*- coding: utf-8 -*-
import autowaf

# Version of this package (even if built as a child)
AUDIOGRAPHER_VERSION = '0.0.0'

# Library version (UNIX style major, minor, micro)
# major increment <=> incompatible changes
# minor increment <=> compatible changes (additions)
# micro increment <=> no interface changes
# Version history:
#   0.0.0 = 0,0,0
AUDIOGRAPHER_LIB_VERSION = '0.0.0'

# Variables for 'waf dist'
APPNAME = 'audiographer'
VERSION = AUDIOGRAPHER_VERSION

# Mandatory variables
srcdir = '.'
blddir = 'build'

def set_options(opt):
	autowaf.set_options(opt)

def configure(conf):
	autowaf.configure(conf)
	
	conf.check_tool('compiler_cxx')
	
	autowaf.check_pkg(conf, 'cppunit', uselib_store='CPPUNIT', atleast_version='1.12.0', mandatory=False)
	autowaf.check_pkg(conf, 'glib-2.0', uselib_store='GLIB', atleast_version='2.2')
	autowaf.check_pkg(conf, 'glibmm-2.4', uselib_store='GLIBMM', atleast_version='2.14.0')
	autowaf.check_pkg(conf, 'gthread-2.0', uselib_store='GTHREAD', atleast_version='2.14.0')
	autowaf.check_pkg(conf, 'samplerate', uselib_store='SAMPLERATE', atleast_version='0.1.7')
	autowaf.check_pkg(conf, 'sndfile', uselib_store='SNDFILE', atleast_version='1.0.18')

	# Boost headers
	autowaf.check_header(conf, 'boost/shared_ptr.hpp')
	autowaf.check_header(conf, 'boost/format.hpp')

def build(bld):

	# Headers
	#bld.install_files('${INCLUDEDIR}/audiographer', 'audiographer/*.h')

	audiographer = bld.new_task_gen('cxx', 'shlib')
	audiographer.source = '''
		src/gdither/gdither.cc
		src/sr_converter.cc
		src/sample_format_converter.cc
		src/sndfile_writer.cc
	'''
	audiographer.name           = 'libaudiographer'
	audiographer.target         = 'audiographer'
	audiographer.export_incdirs = ['.', './src']
	audiographer.includes       = ['.', './src']
	audiographer.uselib         = 'GLIB GLIBMM GTHREAD SAMPLERATE SNDFILE'
	audiographer.vnum           = AUDIOGRAPHER_LIB_VERSION
	audiographer.install_path   = '${LIBDIR}'
	

	if bld.env['HAVE_CPPUNIT']:
		# Unit tests
		obj              = bld.new_task_gen('cxx', 'program')
		obj.source       = '''
			tests/identity_vertex_test.cc
			tests/interleaver_test.cc
			tests/deinterleaver_test.cc
			tests/interleaver_deinterleaver_test.cc
			tests/threader_test.cc
			tests/sr_converter_test.cc
			tests/chunker_test.cc
			tests/sample_format_converter_test.cc
			tests/test_runner.cc
			tests/sndfile_writer_test.cc
		'''
		obj.uselib_local = 'libaudiographer'
		obj.uselib       = 'CPPUNIT GLIBMM'
		obj.target       = 'run-tests'
		obj.install_path = ''

def shutdown():
	autowaf.shutdown()

