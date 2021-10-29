from __future__ import division
import math
import pygame
import pygame.midi
from pygame.locals import *

SCREEN_SIZE = 640, 480
FONT_COLOR = 255, 255, 255
BG_COLOR = 0, 0, 50
FPS = 60

GM = '''1 Acostic Grand Piano
2 Bright Acostic Piano
3 Electric Grand Piano
4 Honky-Tonk Piano
5 Electric Piano 1
6 Electric Piano 2
7 Harpsicord
8 Clavi
9 Celesta
10 Glockenspiel
11 Music Box
12 Vibraphone
13 Marimba
14 Xylophone
15 Tubular Bells
16 Dulcimer
17 Drawber Organ
18 Percussive Organ
19 Rock Organ
20 Church Organ
21 Reed Organ
22 Accordion
23 Harmonica
24 Tango Accordion
25 Acostic Guitar (nylon)
26 Acostic Guitar (steel)
27 Electric Guitar (jazz)
28 Electric Guitar (clean)
29 Electric Guitar (muted)
30 Overdriven Guitar
31 Distortion Guitar
32 Guitar Harmonics
33 Acosic Bass
34 Electric Bass (finger)
35 Electric Bass (pick)
36 Fretless Bass
37 Slap Bass 1
38 Slap Bass 2
39 Synth Bass 1
40 Synth Bass 2
41 Violin
42 Viola
43 Cello
44 Contrabass
45 Tremoro Strings
46 Pizzicato Strings
47 Orchestral Harp
48 Timpani
49 String Ensamble
50 String Ensamble 2
51 Synth Strings 1
52 Synth Strings 2
53 Choir Aahs
54 Voice Oohs
55 Synth Voice
56 Orchestra Hit
57 Trumpet
58 Trombone
59 Tuba
60 Muted Trumpet
61 French Horn
62 Brass Section
63 Synth Brass 1
64 Synth Brass 2
65 Soprano Sax
66 Alto Sax
67 Tenor Sax
68 Baritone Sax
69 Oboe
70 English Horn
71 Bassoon
72 Clarinet
73 Piccolo
74 Flute
75 Recorder
76 Pan Flute
77 Bottle Blow
78 Shakuhachi
79 Whistle
80 Ocarina
81 Lead 1 (square)
82 Lead 2 (sawtooth)
83 Lead 3 (caliope)
84 Lead 4 (chiff)
85 Lead 5 (charang)
86 Lead 6 (voice)
87 Lead 7 (fifth)
88 Lead 8 (bass+lead)
89 Pad 1 (new age)
90 Pad 2 (warm)
91 Pad 3 (polysynth)
92 Pad 4 (choir)
93 Pad 5 (bowed)
94 Pad 6 (metalic)
95 Pad 7 (halo)
96 Pad 8 (sweep)
97 FX (rain)
98 FX (soundtrack)
99 FX (crystal)
100 FX (atmosphere)
101 FX (brightness)
102 FX (goblins)
103 FX (echoes)
104 FX (sci-fi)
105 Sitar
106 Banjo
107 Shamisen
108 Koto
109 Kalimba
110 Bagpipe
111 Fiddle
112 Shanai
113 Tinkle Bell
114 Agogo
115 Steel Drums
116 Woodblock
117 Taiko Drum
118 Melodic Tom
119 Synth Drum
120 Reverse Cymbal
121 Guitar Fret Noise
122 Breath Noise
123 Seashore
124 Bird Tweet
125 Telephone Ring
126 Helicopter
127 Applause
128 Gun Shot'''

GM = dict([(int(i.split()[0]) - 1, i)
           for i in GM.split('\n')])

KEY = 'C C# D D# E F F# G G# A A# B'.split()

def joystick(joy):
    axes = [joy.get_axis(i) for i in range(joy.get_numaxes())]
    buttons = [joy.get_button(i) for i in range(joy.get_numbuttons())]
    hats = [joy.get_hat(i) for i in range(joy.get_numhats())]
    return axes, buttons, hats

def put_text(surface, text, pos, fontsize=72, color=FONT_COLOR, fontname=None):
    font = pygame.font.Font(fontname, fontsize)
    textsurface = font.render(text, True, color)
    surface.blit(textsurface, pos)
    x, y = pos
    return x + textsurface.get_width(), y + textsurface.get_height()

def set_expression(midiout, expression=0.0, ch=0):
    modulation = min(127, int(abs(expression) * 128))
    midiout.write_short(0xb0 + ch, 0x01, modulation)
    exp = min(127, int((expression + 1) * 128 / 2))
    midiout.write_short(0xb0 + ch, 0x0b, exp)
    return exp, modulation

def get_note(x, y):
    return int((math.atan2(y, x) / math.pi + 1) * 6 + 0.5)

def main():
    screen = pygame.display.set_mode(SCREEN_SIZE)
    pygame.display.set_caption('hoge')
    background = pygame.Surface(screen.get_size()).convert()
    clock = pygame.time.Clock()
    joycount = pygame.joystick.get_count()
    if joycount:
        joy = pygame.joystick.Joystick(0)
        joy.init()
        print '%s, axex: %d, buttons: %d, hats: %d' % (
            joy.get_name(), joy.get_numaxes(),
            joy.get_numbuttons(), joy.get_numhats())
    else:
        print 'Joystick not found.'
        return
    pygame.midi.init()
    midi_id = pygame.midi.get_default_output_id()
    midiout = pygame.midi.Output(midi_id)
    print pygame.midi.get_device_info(midi_id)
    clock.tick()
    old_note = None
    note = old_note
    instrument = 0
    midiout.set_instrument(instrument)
    expression = 0.0
    exp, modulation = set_expression(midiout, expression)
    hat_y = 0
    old_n = 0
    base = 0
    while True:
        background.fill(BG_COLOR)
        axes, buttons, hats = joystick(joy)
        hy = -hats[0][0] * 8 + hats[0][1]
        if hat_y != hy:
            instrument -= hy
            if instrument < 0:
                instrument = 127
            elif instrument > 127:
                instrument = 0
            hat_y = hy
            midiout.set_instrument(instrument)
        y = 20
        x, y = put_text(background, GM[instrument], (20, y))
        a = -axes[2]
        if a != expression:
            expression = a
            exp, modulation = set_expression(midiout, expression)
        x, y = put_text(background, 'exp:%3d, mod:%3d' % (exp, modulation),
                        (20, y))
        if max(abs(axes[0]), abs(axes[1])) < 0.3:
            base = n = 0
        else:
            n = get_note(axes[0], axes[1])
            if n >= 12:
                n = 0
            if n < old_n - 6:
                base += 12
            elif n > old_n + 6:
                base -= 12
            old_n = n
        o, k = divmod(n + base + 60, 12)
        x, y = put_text(background, '%s%d' % (KEY[k], o - 2), (20, y), 72 * 6)        
        if buttons[0] and note is None:
            note = base + 60 + n
            old_note = note
            midiout.note_on(note, 100)
        elif not buttons[0] and note is not None:
            midiout.note_off(old_note)
            old_note = note = None
        screen.blit(background, (0, 0))
        pygame.display.flip()
        for event in pygame.event.get():
            if event.type == QUIT:
                return
            elif event.type == KEYDOWN and event.key == K_ESCAPE:
                return
        if buttons[6]:  # Back
            return
        clock.tick(FPS)
        pygame.display.set_caption('%.2f fps' % clock.get_fps())

try:
    pygame.init()
    main()
finally:
    pygame.quit()
