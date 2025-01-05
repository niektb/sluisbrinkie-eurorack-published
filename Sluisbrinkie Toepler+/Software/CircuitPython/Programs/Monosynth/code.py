import time, random
import board, busio
import audiomixer, audiopwmio, synthio
import neopixel, rainbowio
import ulab.numpy as np

import usb_midi
import adafruit_midi
from adafruit_midi.note_on import NoteOn
from adafruit_midi.note_off import NoteOff
from adafruit_midi.control_change import ControlChange

from analogio import AnalogIn
import digitalio

# simple range mapper, like Arduino map()
def map_range(s, a1, a2, b1, b2): return  b1 + ((s - a1) * (b2 - b1) / (a2 - a1))

cv1 = AnalogIn(board.CV1)
cv2 = AnalogIn(board.CV2)
cv3 = AnalogIn(board.CV3)
out2 = digitalio.DigitalInOut(board.OUT2)
out2.direction = digitalio.Direction.OUTPUT

midi_channel = 1       # which midi channel to receive on
oscs_per_note = 3      # how many oscillators for each note
osc_detune = 0.005     # how much to detune oscillators for phatness
filter_freq_lo = 100   # filter lowest freq
filter_freq_hi = 4500  # filter highest freq
filter_res_lo = 0.5    # filter q lowest value
filter_res_hi = 2.0    # filter q highest value

led = neopixel.NeoPixel(board.NEOPIXEL, 2, brightness=0.2)
uart = busio.UART(rx=board.RX, baudrate=31250, timeout=0.001 )
midi_usb  = adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=midi_channel-1)
midi_uart = adafruit_midi.MIDI(midi_in=uart, in_channel=midi_channel-1)

audio = audiopwmio.PWMAudioOut(board.OUT1)
mixer = audiomixer.Mixer(channel_count=1, sample_rate=28000, buffer_size=2048)
synth = synthio.Synthesizer(channel_count=1, sample_rate=28000)

audio.play(mixer)
mixer.voice[0].play(synth)
mixer.voice[0].level = 0.75

wave_saw = np.linspace(28000, -28000, num=512, dtype=np.int16)  # max is +/-32k but gives us headroom

oscs = []   # holds currently sounding oscillators
filter_freq = 2000  # current setting of filter
filter_res = 1.0    # current setting of filter
amp_env_release_time = 0.8  # current release time
note_played = 0  # current note playing



# midi note on
def note_on(notenum, vel):
    amp_level = map_range(vel, 0,127, 0,1)
    amp_env = synthio.Envelope(attack_time=0.01, decay_time=0.05,
                               release_time=0.01,
                               attack_level=amp_level, sustain_level=amp_level*0.8)
    f = synthio.midi_to_hz(notenum)
    oscs.clear()  # chuck out old oscs to make new ones
    for i in range(oscs_per_note):
        fr = f * (1 + (osc_detune*i))
        lpf = synth.low_pass_filter(filter_freq, filter_res)
        # in synthio, 'Note' objects are more like oscillators
        oscs.append( synthio.Note( frequency=fr, filter=lpf, envelope=amp_env,
                                   waveform=wave_saw) )
    synth.press(oscs)  # press the 'note' (collection of oscs acting in concert)

# midi note off
def note_off(notenum,vel):
    synth.release(oscs)
    oscs.clear()

print("monosynth1 ready, listening to incoming USB and Serial MIDI")

while True:

    # to do global filtermod we must iterate over all oscillators in each note
    for osc in oscs:
        osc.filter = synth.low_pass_filter( filter_freq, filter_res )

    msg = midi_uart.receive() or midi_usb.receive()

    if isinstance(msg, NoteOn) and msg.velocity != 0:
        #print("noteOn: ", msg.note, "vel=", msg.velocity)
        led.fill(0xff00ff)
        out2.value = True
        note_off( note_played, 0 )  # this is a monosynth, so if they play legato, noteoff!
        note_on(msg.note, msg.velocity)
        note_played = msg.note

    elif isinstance(msg,NoteOff) or isinstance(msg,NoteOn) and msg.velocity==0:
        #print("noteOff:", msg.note, "vel=", msg.velocity)
        out2.value = False
        if msg.note == note_played:  # only release note that's sounding
            led.fill(0x00000)
            note_off(msg.note, msg.velocity)

    osc_detune  = map_range(cv1.value, 0, 54685, 0, 0.01)
    filter_freq = map_range(cv2.value, 0, 54685, filter_freq_lo, filter_freq_hi)
    filter_res  = map_range(cv2.value, 54685, 0, filter_res_lo, filter_res_hi)

