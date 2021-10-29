#! /usr/bin/env python
#
# Copyright 2002, 2003 Tim Goetze <tim@quitte.de>
#
# Nedko Arnaudov sent a patch with minor fixes in December 2005,
# applied January 2006.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA or point your web browser to http://www.gnu.org.

"""SF2.py -- soundfont 2.01 reader/writer module."""
__version__ = 1.4

import struct
from math import *

SFVERSION  = (2, 1)

SFPHDRSIZE = 38
SFBAGSIZE  =  4
SFMODSIZE  = 10
SFGENSIZE  =  4
SFIHDRSIZE = 22
SFSHDRSIZE = 46

def _signed (word):
	'convert word from uint to signed int'
	if word & 0x8000:
		return word - 0x10000
	return word
	
# unit classes, converting sf2 words to readable and reverse
# conversion code taken from http://smurf.sf.net, by josh green (thanks, josh!)

class unit:
	'plain integer unit'
	label = ''
	format = '%d'
	def __init__ (u, word):
		u.value = word
	def __int__ (u):
		return u.value
	def __repr__ (u):
		return (u.format % u.value) + u.label

class u_signed (unit):
	'same as class unit, only signed instead.'
	def __init__ (u, word):
		u.value = _signed (word)
	def __int__ (u):
		return u.value & 0xFFFF

class u_sample (unit):
	label = ' sample(s)'

class u_32k_samples (unit):
	label = ' 32k samples'

class u_cent_fs (u_signed):
	label = ' cent'
	format = '%+d'

class u_cent (unit):
	label = ' Hz'
	format = '%.2f'
	def __init__ (u, word):
		u.value = 8.176 * pow (2.0, _signed (word) / 1200.0)
	def __int__ (u):
		return int (log (u.value / 8.176) / log (2) * 1200.0)

class u_cB (unit):
	label = ' dB'
	format = '%.2f'
	def __init__ (u, word):
		u.value = _signed (word) / 10.0
	def __int__ (u):
		return int (u.value * 10)
		
class u_cB_fs (u_cB):
	format = '%+.2f'

class u_tenth_percent (unit):
	label = ' %'
	def __init__ (u, word):
		u.value = _signed (word) / 10.0
	def __int__ (u):
		return int (u.value * 10)
			
class u_timecent (unit):
	label = ' s'
	format = '%.3f'
	def __init__ (u, word):
		u.value = pow (2.0, _signed (word) / 1200.0)
	def __int__ (u):
		return int (log (u.value) / log (2) * 1200.0)

class u_tcent_per_key (unit):
	label = ' cent/key'

class u_key_range (unit):
	format = '%d - %d'
	def __init__ (u, word):
		u.value = [word & 0xFF, (word >> 8) & 0xFF]
	def __int__ (u):
		return u.value[0] | (u.value[1] << 8)
	def __repr__ (u):
		return (u.format % (u.value[0], u.value[1])) + u.label

class u_vel_range (unit):
	format = '%d - %d'
	def __init__ (u, word):
		u.value = [word & 0xFF, (word >> 8) & 0xFF]
	def __int__ (u):
		return u.value[0] | (u.value[1] << 8)
	def __repr__ (u):
		return (u.format % (u.value[0], u.value[1])) + u.label

class u_key (unit):
	pass

class u_semitone (u_signed):
	label = ' semitone(s)'

class u_bit_flags (unit):
	label = ''
	modes = ('unlooped', 'looped', 'unlooped', 'looped until release')
	def __repr__ (u):
		if u.value > 3:
			return '%d (undefined)' % u.value
		return u.modes[u.value]

class u_cent_per_key (u_signed):
	label = ' cent/key'

class u_127 (unit):
	pass

# all generators as per sfspec21.pdf, unpack is:
# (generator, label, unit)
# we don't really need 'generator' but it's much nicer to read.

all_gens = (
	( 0, "startAddrsOffset", u_sample),
	( 1, "endAddrsOffset", u_sample),
	( 2, "startloopAddrsOffset", u_sample),
	( 3, "endloopAddrsOffset", u_sample),
	( 4, "startAddrsCoarseOffset", u_32k_samples),
	( 5, "modLfoToPitch", u_cent_fs),
	( 6, "vibLfoToPitch", u_cent_fs),
	( 7, "modEnvToPitch", u_cent_fs),
	( 8, "initialFilterFc", u_cent),
	( 9, "initialFilterQ", u_cB),
	(10, "modLfoToFilterFc", u_cent_fs),
	(11, "modEnvToFilterFc", u_cent_fs),
	(12, "endAddrsCoarseOffset", u_32k_samples),
	(13, "modLfoToVolume", u_cB_fs),
	(14, "unused", unit),
	(15, "chorusEffectsSend", u_tenth_percent),
	(16, "reverbEffectsSend", u_tenth_percent),
	(17, "pan", u_tenth_percent),
	(18, "unused", unit),
	(19, "unused", unit),
	(20, "unused", unit),
	(21, "delayModLFO", u_timecent),
	(22, "freqModLFO", u_cent),
	(23, "delayVibLFO", u_timecent),
	(24, "freqVibLFO", u_cent),
	(25, "delayModEnv", u_timecent),
	(26, "attackModEnv", u_timecent),
	(27, "holdModEnv", u_timecent),
	(28, "decayModEnv", u_timecent),
	(29, "sustainModEnv", u_tenth_percent),
	(30, "releaseModEnv", u_timecent),
	(31, "keynumToModEnvHold", u_tcent_per_key),
	(32, "keynumToModEnvDecay", u_tcent_per_key),
	(33, "delayVolEnv", u_timecent),
	(34, "attackVolEnv", u_timecent),
	(35, "holdVolEnv", u_timecent),
	(36, "decayVolEnv", u_timecent),
	(37, "sustainVolEnv", u_cB),
	(38, "releaseVolEnv", u_timecent),
	(39, "keynumToVolEnvHold", u_tcent_per_key),
	(40, "keynumToVolEnvDecay", u_tcent_per_key),
	(41, "instrument", unit), 
	(42, "reserved", unit),
	(43, "keyRange", u_key_range), 
	(44, "velRange", u_vel_range),
	(45, "startloopAddrsCoarseOffset", u_32k_samples),
	(46, "keynum", u_key_range),
	(47, "velocity", u_vel_range),
	(48, "initialAttenuation", u_cB),
	(49, "reserved", unit),
	(50, "endloopAddrsCoarseOffset", u_32k_samples),
	(51, "coarseTune", u_semitone),
	(52, "fineTune", u_cent_fs),
	(53, "sampleID", unit),
	(54, "sampleModes", u_bit_flags),
	(55, "reserved", unit),
	(56, "scaleTuning", u_cent_per_key),
	(57, "exclusiveClass", u_127),
	(58, "overridingRootKey", u_key),
	(59, "reserved", unit),
	(60, "endOper", unit),
)

# compare function for sorting of generators --
# create sort criteria first.
gen_sort = 61 * [0]
gen_sort [41] = 1 # last
gen_sort [53] = 1 # last
gen_sort [43] = -2 # first
gen_sort [44] = -1 # second

def gen_compare (g1, g2):
	return gen_sort [g1.generator] - gen_sort [g2.generator]

# base that implements some useful routines for SF2 files
# meant to be derived from in combination with __builtin__.file

class SFBinary:
	'read methods for SF2 peculiarities and not-so-peculiarities.'
	def read_four (f, signature):
		'read a four-character code'
		r = f.read (4)
		if r != signature: 
			raise TypeError ("'%s' is not %s." % (repr (r), signature))

	def read_uint (f, n):
		'read an unsigned int'
		s = f.read (n)
		v = 0
		while n:
			n -= 1
			v |= ord (s[n]) << (8 * n)
		return v;	
		
	def read_chdr (f, sign):
		'read a chunk header (four chars + 32 bit size)'
		f.read_four (sign)
		return f.read_uint (4)

	def read_twenty (f):
		'read a SF string (up to twenty chars)'
		s = f.read (20)
		i = s.find ('\0')
		if i >= 0: 
			s = s[:i]
		return s.strip()

	def unpack (f, fmt):
		n = struct.calcsize (fmt)
		return struct.unpack (fmt, f.read (n))

	def unpack_gen (f):
		return struct.unpack ("<HH", f.read (4))

	def pack (f, fmt, *args):
		f.write (struct.pack (fmt, *args))

class SFModulatorWord:
	'readable rendering of a Modulator word (aka "sfModulator enum")'
	sources = {
		0: "no controller",
		2: "note-on velocity",
		3: "note-on key number",
		10: "poly pressure",
		13: "channel pressure",
		14: "pitch wheel",
		16: "pitch wheel sensitivity"}
	types = {
		0: "linear",
		1: "concave",
		2: "convex",
		3: "switch"}

	def __init__ (m, word):
		m.word = word
		m.cc = word & 0x80
		m.index = word & 0x7f
		if m.cc:
			m.source = 'Midi CC %d' % m.index
		else:
			if m.sources.has_key (m.index): m.source = m.sources[m.index]
			else: m.source = 'undefined (%d)' % m.index
			
		if word & 0x100: m.direction = 'sub'
		else: m.direction = 'add'
		
		if word & 0x200: m.polarity = 'bipolar'
		else: m.polarity = 'unipolar'
		
		type = word >> 10
		if m.types.has_key (type): m.type = m.types[type]
		else: m.type = "%d (undefined)" % (type)

	def __repr__ (m):
		return m.source + " " + m.direction + " " + \
				m.polarity + " " + m.type + " (%04x)" % m.word

# dump functions
#
# print the structure of SF2 data in a tree-like fashion.

def _dump_modulators (modulators, s = ""):
	print s, "Modulators:"
	for mod in modulators:
		mod._dump (s + ' ')
		print

def _dump_generators (generators, s = ""):
	print s, "Generators:"
	s += ' '
	for gen in generators:
		print s, gen
		if hasattr (gen, 'sample'):
			gen.sample._dump (s + ' ')
		if hasattr (gen, 'instrument'):
			_dump_instrument (gen.instrument, s + " ")
	
def _dump_bags (bags, s = ""):
	for bag in bags:
		print s, "Bag:"
		if bag.generators:
			_dump_generators (bag.generators, s + " ")
			print
		if bag.modulators:
			_dump_modulators (bag.modulators, s + " ")

def _dump_instrument (inst, s = ""):
	print s, "Instrument:", inst.name
	_dump_bags (inst.bags, s + " ")
	
def _dump_presets (presets):
	for pre in presets:
		print "Preset:", pre.name
		_dump_bags (pre.bags, " ")

def _dump_info (info):
	for key in info:
		print key, repr (info[key])


# soundfont data structures
#
# the classes prefixed with 'SF' provide a constructor that reads from a file;
# those without this prefix are the base classes.

class Preset:
	def __init__ (p, name, num, bank = 0):
		p.name = len (name) > 20 and name[:20] or name
		p.num = num
		p.bank = bank
		p.libr = 0
		p.genre = 0
		p.morph = 0
		p.bags = []

	def pack (p):
		return struct.pack ("<20sHHHIII", \
				p.name, p.num, p.bank, p.bag, p.libr, p.genre, p.morph)

class SFPreset (Preset):
	def __init__ (p, f):
		p.name = f.read_twenty()
		p.num, p.bank, p.bag, p.libr, p.genre, p.morph = f.unpack ("<HHHIII")
		p.bags = []

#

class Bag:
	def __init__ (b):
		b.generators = []
		b.modulators = []

	def pack (b):
		return struct.pack ("<HH", b.gen, b.mod)

class SFBag (Bag):
	def __init__ (b, f):
		b.gen, b.mod = f.unpack ("<HH")
		b.generators = []
		b.modulators = []

#

class Modulator:
	transforms = {
		0: "linear"}

	def __init__ (m, src, dest, amount, amtsrc, trans = 0):
		m.src = src
		m.dest = dest
		m.amount = amount
		m.amtsrc = amtsrc
		m.trans = trans

	def pack (m):
		return struct.pack ("<HHhHH", \
				m.src, m.dest, m.amount, m.amtsrc, m.trans)
		
	def _dump (m, s = ""):
		print s, repr (SFModulatorWord (m.src)) + " -> " + all_gens[m.dest][1]
		print s, "amtsrc:", repr (SFModulatorWord (m.amtsrc))
		if m.transforms.has_key (m.trans): 
			trans = m.transforms[m.trans]
		else: 
			trans = "unknown (%d)" % m.trans
		print s, "amount:", m.amount, "transform:", trans

class SFModulator (Modulator):
	def __init__ (m, f):
		m.src, m.dest, m.amount, m.amtsrc, m.trans = f.unpack ("<HHhHH")
		
#

class Generator:
	def __init__ (g, gen, amt):
		g.generator = gen
		# convert amt from real-world, readable to unsigned short
		u = all_gens[gen][2]
		g.amount = int (u (amt))

	def pack (g):
		return struct.pack ("<HH", g.generator, g.amount)

	def __repr__ (g):
		i,label,u = all_gens [g.generator]
		return "(%2d:%04x) " % (g.generator, g.amount) + \
				label + ' ' + repr (u (g.amount))

class SFGenerator (Generator):
	def __init__ (g, f):
		g.generator, g.amount = struct.unpack ("<HH", f.read (4))

#

class Instrument:
	def __init__ (i, name):
		i.name = len (name) > 20 and name[:20] or name
		i.bags = []
		
	def pack (i):
		return struct.pack ("<20sH", i.name, i.bag)

class SFInstrument (Instrument):
	def __init__ (i, f):
		i.name = f.read_twenty()
		i.bag, = f.unpack ("<H")
		i.bags = []
	
#

class Sample:
	types = {
		1: "mono",
		2: "right",
		4: "left",
		8: "linked"}

	def __init__ (s, name, srate, data, pitch = 60, loop = None):
		s.name = len (name) > 20 and name[:20] or name
		s.srate = srate
		s.pitch = pitch
		s.pitch_adjust = 0
		s.data = data # string of 16bit samples
		s.start = 0
		s.length = len (data) / 2
		s.loop = loop or (s.length - 2, s.length - 1)
		s.type = 1 # mono
		s.link = 0 # unlinked

	def pack (s):
		return struct.pack ("<20sIIIIIBbHH", \
				s.name, s.start, s.start + s.length, \
				s.loop[0] + s.start, s.loop[1] + s.start, \
				s.srate, \
				s.pitch, s.pitch_adjust, s.link, s.type)

	def get_data (s):
		if hasattr (s, 'data'):
			return s.data
		return s.file.read_sample (s.start, s.length)

	def _dump (s, t = ""):
		type = s.types [s.type & 0xF]
		if s.type & 0x8000:
			type = "ROM-" + type
		link = s.link and "-> %d" % s.link or ""
		print t, "'%s' %d samples (%d : %d)" % \
			(s.name, s.length, s.loop[0], s.loop[1])
		print t, "srate %d Hz pitch %d (adjust %d) %s %s" % \
				(s.srate, s.pitch, s.pitch_adjust, type, link)

class SFSample (Sample):
	def __init__ (s, f):
		s.file = f
		s.name = f.read_twenty()
		s.start, end, l_start, l_end, s.srate, \
				s.pitch, s.pitch_adjust, s.link, s.type = f.unpack ("<IIIIIBbHH")
		s.length = end - s.start
		s.loop = (l_start - s.start, l_end - s.start)

	
# the reader --

class SF2Reader (file, SFBinary):
	def __init__ (f, path):
		file.__init__ (f, path, 'r')

		f.info = {}
		f.presets = []
		f.instruments = []
		f.samples = []

		# temporaries.
		f.bags = []
		f.ibags = []

		# here we go.
		size = f.read_chdr ('RIFF')
		f.read_four ('sfbk')
		f.read_info()

		if f.version == None:
			raise "ifil info subchunk is mandatory"
		f.read_samples()
		f.read_hydra()

		# throw away temporaries and unlinked stuff.
		for list in f.presets, f.bags, f.instruments, f.ibags:
			for e in list:
				if hasattr (e, 'bag'): del e.bag
				if hasattr (e, 'gen'): del e.gen
				if hasattr (e, 'mod'): del e.mod

		del f.bags, f.instruments, f.ibags, f.samples

	def read_samples (f):
		size = f.read_chdr ('LIST')
		f.read_four ('sdta')
		sz = f.read_chdr ('smpl')
		size -= 12
		if sz != size: raise "sample chunk size mismatch."
		f.smpl = f.tell()
		f.seek (sz, 1)
	
	def read_sample (f, start, length):
		f.seek (f.smpl + start * 2)
		return f.read (length * 2)

	def read_hydra (f):
		size = f.read_chdr ('LIST')
		f.read_four ('pdta')
		
		f.read_phdr()
		f.read_pbag()
		f.read_pmod()
		f.read_pgen()
		
		f.read_inst()
		f.read_ibag()
		f.read_imod()
		f.read_igen()

		f.read_shdr()

		f.assign_samples()
		f.assign_instruments()

	def read_phdr (f):
		'read preset header'
		size = f.read_chdr ('phdr')
		for n in range (size / SFPHDRSIZE - 1):
			f.presets.append (SFPreset (f))
		SFPreset (f)

	def read_pbag (f):
		'read preset bags'
		size = f.read_chdr ('pbag')
		pre = 0
		for bag in range (size / SFBAGSIZE - 1):
			# the braindead SF spec says that for every preset, we have an index into 
			# the bags list where the preset's bags begin; so we have to look at the
			# next preset to find out when to attach the bags to a different preset.
			# granted, we could do this above, but, wouldn't it be much easier if we 
			# just stored the number of bags per preset in a .sf2?
			#
			# same thing for bags, generators, modulators and instruments. sigh.
			if pre < len (f.presets) - 1 and bag >= f.presets[pre + 1].bag:
				pre += 1
			b = SFBag (f)
			f.bags.append (b)
			f.presets[pre].bags.append (b)
		SFBag (f)
		
	def read_pmod (f):
		'read preset modulators'
		size = f.read_chdr ('pmod')
		bag = 0
		for mod in range (size / SFMODSIZE - 1):
			if bag < len (f.bags) - 1 and mod >= f.bags[bag + 1].mod:
				bag += 1
			f.bags[bag].modulators.append (SFModulator (f))
		SFModulator (f)

	def read_pgen (f):
		'read preset generators'
		size = f.read_chdr ('pgen')
		bag = 0
		for gen in range (size / SFGENSIZE - 1):
			if bag < len (f.bags) - 1 and gen >= f.bags[bag + 1].gen:
				bag += 1
			f.bags[bag].generators.append (SFGenerator (f))
		SFGenerator (f)

	def read_inst (f):
		'read instrument header'
		size = f.read_chdr ('inst')
		for n in range (size / SFIHDRSIZE - 1):
			f.instruments.append (SFInstrument (f)) 
		SFInstrument (f)

	def read_ibag (f):
		'read instrument bags'
		size = f.read_chdr ('ibag')
		inst = 0
		for bag in range (size / SFBAGSIZE - 1):
			if inst < len (f.instruments) - 1 and bag >= f.instruments[inst + 1].bag:
				inst += 1
			b = SFBag (f)
			f.instruments[inst].bags.append (b)
			f.ibags.append (b)
		SFBag (f)

	def read_imod (f):
		'read instrument modulators'
		size = f.read_chdr ('imod')
		bag = 0
		for mod in range (size / SFMODSIZE - 1):
			if bag < len (f.ibags) - 1 and mod >= f.ibags[bag + 1].mod:
				bag += 1
			f.ibags[bag].modulators.append (SFModulator (f))
		SFModulator (f)

	def read_igen (f):
		'read instrument generators'
		size = f.read_chdr ('igen')
		bag = 0
		for gen in range (size / SFGENSIZE - 1):
			if bag < len (f.ibags) - 1 and gen >= f.ibags[bag + 1].gen:
				bag += 1
			f.ibags[bag].generators.append (SFGenerator (f))
		SFGenerator (f)

	def read_shdr (f):
		'read sample headers'
		size = f.read_chdr ('shdr')
		for n in range (size / SFSHDRSIZE - 1):
			f.samples.append (SFSample (f))
		SFSample (f)

	def read_info (f):
		'read the info chunk.'
		size = f.read_chdr ('LIST')
		f.read_four ('INFO')
		size -= 4
		
		while size > 0:
			id = f.read (4)
			sz = f.read_uint (4)
			size -= (8 + sz)

			if id == 'ifil':
				if sz != 4: raise "ifil chunk not 4 bytes."
				f.version = (f.read_uint (2), f.read_uint (2))
			elif id == 'iver':
				if sz != 4: raise "iver chunk not 4 bytes."
				f.info [id] = "%u.%u" % (f.read_uint (2), f.read_uint (2))
			else:
				f.info [id] = f.read (sz).rstrip('\0')

	# cleaning up ...
	def assign_samples (f):
		'find generator 53 and set sample.'
		for bag in f.ibags:
			for gen in bag.generators:
				if gen.generator != 53: # sampleID
					continue
				gen.sample = f.samples[gen.amount]
				break

	def assign_instruments (f):
		'find generator 41 and set instrument.'
		for pre in f.presets:
			for bag in pre.bags:
				for gen in bag.generators:
					if gen.generator != 41: # instrument
						continue
					gen.instrument = f.instruments[gen.amount]
					break

# writer interface --
#

class SFChunk:
	def __init__ (c, f, sign):
		'write signature and zero size, remember where we leave off.'
		c.f = f
		f.write (sign)
		f.write ('\0\0\0\0')
		c.start = f.tell()
	
	def __del__ (c):
		'tell() and update chunk size at where __init__ left off.'
		at = c.f.tell()
		c.f.seek (c.start - 4)
		c.f.pack ('<I', at - c.start)
		c.f.seek (at)

class SFStringChunk (SFChunk):
	'write a string, pad with \0'
	def __init__ (c, f, sign, data):
		SFChunk.__init__ (c, f, sign)
		f.write (data)
		if len (data) & 1:
			f.write ('\0')
		else:
			f.write ('\0\0')

# the writer.
#

class SF2Writer (file, SFBinary):
	def __init__ (f, path, presets, info = None):
		file.__init__ (f, path, 'w')

		info = info or { \
			'isng': 'G-nu 2k2',
			'INAM': '?',
			'ISFT': 'SF2.py ' + repr (__version__),
		}

		f.info = info
		f.presets = presets

		f.bags = []
		f.generators = []
		f.modulators = []
		f.instruments = []
		f.ibags = []
		f.igenerators = []
		f.imodulators = []
		f.samples = []

		f.prep_pbags()

		f.write_all()

	def prep_pbags (f):
		'create lists of bags, generators and modulators and set indices.'
		for pre in f.presets:
			pre.bag = len (f.bags)
			for bag in pre.bags:
				# remember (gen, mod) indices
				f.bags.append ((len (f.generators), len (f.modulators)))
				bag.generators.sort (gen_compare)
				f.generators += bag.generators
				f.modulators += bag.modulators
				
				for gen in bag.generators:
					if gen.generator != 41: # instrument generator
						continue
					inst = gen.instrument
					if inst in f.instruments: # already written
						gen.amount = f.instruments.index (inst)
						continue
					gen.amount = len (f.instruments)
					f.instruments.append (inst)
					# prepare instrument
					f.prep_ibags (inst)

	def prep_ibags (f, inst):
		'create lists of ibags, igenerators and imodulators and set indices.'
		inst.bag = len (f.ibags)
		for bag in inst.bags:
			# remember (gen, mod) indices
			f.ibags.append ((len (f.igenerators), len (f.imodulators)))
			bag.generators.sort (gen_compare)
			f.igenerators += bag.generators
			f.imodulators += bag.modulators
			
			for gen in bag.generators:
				if gen.generator != 53: # sample generator
					continue
				if gen.sample in f.samples:
					gen.amount = f.samples.index (gen.sample)
					continue
				gen.amount = len (f.samples)
				f.samples.append (gen.sample)

	def write_all (f):
		riff_chunk = SFChunk (f, 'RIFF')
		f.write ("sfbk")
		f.write_info()
		f.write_sample_data()
		f.write_hydra()

	def write_hydra (f):
		list_chunk = SFChunk (f, 'LIST')
		f.write ('pdta')

		f.write_phdr()
		f.write_pbag()
		f.write_pmod()
		f.write_pgen()
		
		f.write_inst()
		f.write_ibag()
		f.write_imod()
		f.write_igen()
		
		f.write_shdr()
	
	# presets
	def write_phdr (f):
		chunk = SFChunk (f, 'phdr')
		for pre in f.presets:
			f.write (pre.pack())
		f.pack ("<20sHHHIII", 'EOP', 0, 0, len (f.bags), 0, 0, 0)

	def write_pbag (f):
		chunk = SFChunk (f, 'pbag')
		for bag in f.bags:
			f.pack ("<HH", bag[0], bag[1])
		f.pack ("<HH", len (f.generators), len (f.modulators))

	def write_pmod (f):
		chunk = SFChunk (f, 'pmod')
		for mod in f.modulators:
			f.write (mod.pack())
		f.pack ("<HHhHH", 0, 0, 0, 0, 0)

	def write_pgen (f):
		chunk = SFChunk (f, 'pgen')
		for gen in f.generators:
			f.write (gen.pack())
		f.pack ("<HH", 0, 0)

	# instruments
	def write_inst (f):
		chunk = SFChunk (f, 'inst')
		for inst in f.instruments:
			f.write (inst.pack())
		f.pack ("<20sH", 'EOI', len (f.ibags))

	def write_ibag (f):
		chunk = SFChunk (f, 'ibag')
		for bag in f.ibags:
			f.pack ("<HH", bag[0], bag[1])
		f.pack ("<HH", len (f.igenerators), len (f.imodulators))

	def write_imod (f):
		chunk = SFChunk (f, 'imod')
		for mod in f.imodulators:
			f.write (mod.pack())
		f.pack ("<HHhHH", 0, 0, 0, 0, 0)

	def write_igen (f):
		chunk = SFChunk (f, 'igen')
		for gen in f.igenerators:
			f.write (gen.pack())
		f.pack ("<HH", 0, 0)

	# sample chunk
	def write_shdr (f):
		chunk = SFChunk (f, 'shdr')
		for sample in f.samples:
			f.write (sample.pack())
		f.pack ("<20sIIIIIBbHH", \
				'EOS', 0, 0, 0, 0, 0, 0, 0, 0, 0)

	def write_sample_data (f):
		list_chunk = SFChunk (f, 'LIST')
		f.write ('sdta')
		smpl_chunk = SFChunk (f, 'smpl')
		start = 0
		for sample in f.samples:
			f.write (sample.get_data())
			sample.start = start
			start += sample.length

	def write_info (f):
		chunk = SFChunk (f, 'LIST')
		f.write ("INFO")
		
		c = SFChunk (f, 'ifil')
		f.pack ('<HH', SFVERSION[0], SFVERSION[1])
		del c

		for key in f.info:
			if len (key) != 4: 
				raise repr (key) + " is not a valid info chunk signature."
			SFStringChunk (f, key, f.info[key])

def read_sf2 (path):
	f = SF2Reader (path)
	return f.presets, f.info

def write_sf2 (path, presets, info = None):
	f = SF2Writer (path, presets, info)

if __name__ == '__main__':
	# example showing how to extract 1 preset from a sf2.
	from sys import argv
	default = './example.sf2'
	path = len (argv) > 1 and argv[1] or default
	
	presets, info = read_sf2 (path)

	pre2 = []
	for p in presets:
		if p.name == 'the kit':
			pre2.append (p)

	#_dump_presets (presets)
	info = { \
		'isng': 'G-nu 2k2',
		'INAM': 'the kit',
		'ISFT': 'SF2.py ' + repr (__version__),
	}
														
	write_sf2 ('the-kit.sf2', pre2, info)

