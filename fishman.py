#!/usr/bin/python
#https://discourse.ardour.org/t/fishman-tripleplay-and-midi-velocity-data/86880/8
from mididings import *

config(
backend=‘jack’,
client_name=‘TP Boost’,
)

run( Velocity(gamma=2.0))
