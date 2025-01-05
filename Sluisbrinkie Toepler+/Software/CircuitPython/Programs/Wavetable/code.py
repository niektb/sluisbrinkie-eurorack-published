import time
import busio
import board
import audiomixer
import audiopwmio
import synthio
from analogio import AnalogIn
import digitalio
import audiobusio
import neopixel, rainbowio
import ulab.numpy as np
import adafruit_wave

import usb_midi
import adafruit_midi
from adafruit_midi.note_on import NoteOn
from adafruit_midi.note_off import NoteOff
from adafruit_midi.control_change import ControlChange

auto_play = False  # set to true to have it play its own little song
auto_play_notes = [36, 38, 40, 41, 43, 45, 46, 48, 50, 52]
auto_play_speed = 0.9  # time in seconds between notes

midi_channel = 1

wavetable_fname = "wav/PLAITS02.WAV"  # from http://waveeditonline.com/index-17.html
wavetable_sample_size = 256  # number of samples per wave in wavetable (256 is standard)
sample_rate = 44100

# simple range mapper, like Arduino map()
def map_range(s, a1, a2, b1, b2): return  b1 + ((s - a1) * (b2 - b1) / (a2 - a1))

# mix between values a and b, works with numpy arrays too,  t ranges 0-1
def lerp(a, b, t):
    return (1-t)*a + t*b

cv1 = AnalogIn(board.CV1)
cv2 = AnalogIn(board.CV2)
cv3 = AnalogIn(board.CV3)

out2 = digitalio.DigitalInOut(board.OUT2)
out2.direction = digitalio.Direction.OUTPUT

audio = audiopwmio.PWMAudioOut(board.OUT1)
mixer = audiomixer.Mixer(buffer_size=4096, voice_count=1, sample_rate=sample_rate, channel_count=1,
                         bits_per_sample=16, samples_signed=True)
audio.play(mixer)  # attach mixer to audio playback
synth = synthio.Synthesizer(sample_rate=sample_rate)
mixer.voice[0].play(synth)  # attach synth to mixer
mixer.voice[0].level = 1

led = neopixel.NeoPixel(board.NEOPIXEL, 2, brightness=0.2)
uart = busio.UART(rx=board.RX, baudrate=31250, timeout=0.001)
midi_usb  = adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=midi_channel-1)
midi_uart = adafruit_midi.MIDI(midi_in=uart, in_channel=midi_channel-1)

class Wavetable:
    """ A 'waveform' for synthio.Note that uses a wavetable w/ a scannable wave position."""
    def __init__(self, filepath, wave_len=256):
        self.w = adafruit_wave.open(filepath)
        self.wave_len = wave_len  # how many samples in each wave
        if self.w.getsampwidth() != 2 or self.w.getnchannels() != 1:
            raise ValueError("unsupported WAV format")
        self.waveform = np.zeros(wave_len, dtype=np.int16)  # empty buffer we'll copy into
        self.num_waves = self.w.getnframes() // self.wave_len
        self.set_wave_pos(0)

    def set_wave_pos(self, pos):
        """Pick where in wavetable to be, morphing between waves"""
        pos = min(max(pos, 0), self.num_waves-1)  # constrain
        samp_pos = int(pos) * self.wave_len  # get sample position
        self.w.setpos(samp_pos)
        waveA = np.frombuffer(self.w.readframes(self.wave_len), dtype=np.int16)
        self.w.setpos(samp_pos + self.wave_len)  # one wave up
        waveB = np.frombuffer(self.w.readframes(self.wave_len), dtype=np.int16)
        pos_frac = pos - int(pos)  # fractional position between wave A & B
        self.waveform[:] = lerp(waveA, waveB, pos_frac)  # mix waveforms A & B

wavetable1 = Wavetable(wavetable_fname, wave_len=wavetable_sample_size)


lpf = synth.low_pass_filter(4000, 1)  # cut some of the annoying harmonics
notes_pressed = {}  # keys = midi note num, value = synthio.Note

def note_on(notenum, vel):
    amp_level = map_range(vel, 0,127, 0,1)
    amp_env = synthio.Envelope(attack_time=0.01, decay_time=0.05,
                           release_time=0.01,
                           attack_level=amp_level, sustain_level=amp_level*0.8)
    
    # release old note at this notenum if present
    if oldnote := notes_pressed.pop(notenum, None):
        synth.release(oldnote)

    f = synthio.midi_to_hz(notenum)

    vibrato_lfo = synthio.LFO(rate=1, scale=0.01)
    note = synthio.Note(frequency=f, waveform=wavetable1.waveform,
                         envelope=amp_env, filter=lpf)
    synth.press(note)
    notes_pressed[notenum] = note

# midi note off
def note_off(notenum,vel):
    if note := notes_pressed.pop(notenum, None):
        synth.release(note)
        
last_synth_update_time = 0
def update_synth():
    # pylint: disable=global-statement
    global last_synth_update_time
    # only update 100 times a sec to lighten the load
    if time.monotonic() - last_synth_update_time > 1:
        last_update_time = time.monotonic()
        num_waves = wavetable1.num_waves-1
        new_wave_pos = float(map_range(cv1.value, 0, 54685, 0, num_waves))
        #print(new_wave_pos)
        wavetable1.set_wave_pos( new_wave_pos )

last_auto_play_time = 0
auto_play_pos = -1

def update_auto_play():
    # pylint: disable=global-statement
    global last_auto_play_time, auto_play_pos
    if auto_play and time.monotonic() - last_auto_play_time > auto_play_speed:
        last_auto_play_time = time.monotonic()
        note_off( auto_play_notes[ auto_play_pos ], 127 )
        auto_play_pos = (auto_play_pos + 3) % len(auto_play_notes)
        note_on( auto_play_notes[ auto_play_pos ], 127 )

print("wavetable midisynth i2s. auto_play:",auto_play)

while True:
    update_synth()
    update_auto_play()

    msg = midi_uart.receive() or midi_usb.receive()

    if isinstance(msg, NoteOn) and msg.velocity != 0:
        note_on(msg.note, msg.velocity)

    elif isinstance(msg,NoteOff) or isinstance(msg,NoteOn) and msg.velocity==0:
        note_off(msg.note, msg.velocity)

    #chord = map_range(cv1.value, 0, 54685, 0, 7)
    #inversion = map_range(cv2.value, 0, 54685, 0, 15)
    # root = map_range(cv2.value, 0, 54685, 0, 60)
    
    # TODO: Use button to cycle through wavetables
