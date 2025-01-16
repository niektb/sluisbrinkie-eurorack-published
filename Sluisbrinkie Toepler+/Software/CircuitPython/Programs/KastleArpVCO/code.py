# CV1 selects played note. Plays short note when a new pitch is given
# CV2 selects the decay of the note. CCW no new notes can be played when there is still a note playing. CW new notes interrupt the previous one.
# CV3 selects the chord from which new notes play. When transitioning between chords, a new note can play (depending on CV2 setting)
# Left LED indicates selected root note
# Right LED indicates selected mode (major, minor, etc.)
# short button press switches between root notes. Long button press switches between musical modes.
import time
import board
import audiomixer, audiopwmio, synthio
import neopixel, rainbowio
import ulab.numpy as np
import adafruit_wave
from analogio import AnalogIn
import digitalio
from adafruit_debouncer import Debouncer

cv1 = AnalogIn(board.CV1)
cv2 = AnalogIn(board.CV2)
cv3 = AnalogIn(board.CV3)
out2 = digitalio.DigitalInOut(board.OUT2)
out2.direction = digitalio.Direction.OUTPUT

button_pin = digitalio.DigitalInOut(board.SW1)
button_pin.direction = digitalio.Direction.INPUT
sw = Debouncer(button_pin)

led = neopixel.NeoPixel(board.NEOPIXEL, 2, brightness=0.7)

waiting_for_release = False

num_octaves = 5
cv_max = 50000
midi_lowest_note = 36
selected_root_note = 1
selected_mode = 1
selected_chord = 1
selected_note = -1;

MODES = {
    1: [0, 2, 4, 5, 7, 9, 11],  # Ionian (Major scale)
    2: [0, 2, 3, 5, 7, 9, 10],  # Dorian
    3: [0, 1, 3, 5, 7, 8, 10],  # Phrygian
    4: [0, 2, 4, 6, 7, 9, 11],  # Lydian
    5: [0, 2, 4, 5, 7, 9, 10],  # Mixolydian
    6: [0, 2, 3, 5, 7, 8, 10],  # Aeolian (Natural minor)
    7: [0, 1, 3, 5, 6, 8, 10],  # Locrian
}

MODE_COLOR = {
    1: (255, 0, 0),
    2: (255, 128, 0),
    3: (255, 255, 0),
    4: (128, 255, 0),
    5: (0, 255, 0),
    6: (0, 255, 128),
    7: (0, 255, 255),
}

ROOT_COLOR = {
    0: (255, 0, 0),
    1: (255, 128, 0), 
    2: (255, 255, 0),
    3: (128, 255, 0),
    4: (0, 255, 0),
    5: (0, 255, 128),
    6: (0, 255, 255),
    7: (0, 128, 255),
    8: (0, 0, 255),
    9: (128, 0, 255),
    10: (255, 0, 255),
    11: (255, 0, 128),
}

wavetable_fname = "wav/PLAITS02.WAV"  # from http://waveeditonline.com/index-17.html
wavetable_sample_size = 256  # number of samples per wave in wavetable (256 is standard)
sample_rate = 44100

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

def change_mode():
    global selected_mode, CHORDS
    selected_mode = (selected_mode % len(MODES)) +1
    CHORDS = get_mode_chords(selected_mode)
    print(f"Mode changed to mode {selected_mode}")

def clamp(num, min_value, max_value):
    return max(min(num, max_value), min_value)

def get_chord_from_cv(cv_value, hysteresis = 500):
    global selected_chord
    
    step = cv_max // (len(CHORDS))
    current_chord = cv_value // step
    current_chord = clamp(current_chord, 1, 7)
    
    if selected_chord != -1 and abs(cv_value - (selected_chord * step)) < hysteresis:
        return selected_chord
    
    return current_chord

# simple range mapper, like Arduino map()
def map_range(s, a1, a2, b1, b2): return  b1 + ((s - a1) * (b2 - b1) / (a2 - a1))

# mix between values a and b, works with numpy arrays too,  t ranges 0-1
def lerp(a, b, t):
    return (1-t)*a + t*b

# this function will give a number between 0 and 15 (I think), need to extract the midi note from this
def get_note_from_cv(cv_value, hysteresis = 500):
    global selected_note
    
    step = cv_max // ((num_octaves * 3)) - 1 # num_octaves * notes in chord
    current_note_index = cv_value // step
    current_note_index = clamp(current_note_index, 0, 14)
    
    if selected_note != -1 and abs(cv_value - (selected_note * step)) < hysteresis:
        return selected_note

    return current_note_index

def get_mode_chords(mode):
    """Generate chord definitions for a given mode."""
    if mode not in MODES:
        raise ValueError("Invalid mode number. Choose between 1 and 7.")    
    
    intervals = MODES[mode]
    chords = {}
    
    for degree in range(7):
        root = intervals[degree]
        third = intervals[(degree + 2) % 7]
        fifth = intervals[(degree + 4) % 7]
        chords[degree + 1] = [root, third, fifth]
    
    return chords
        
def generate_scale_from_chord(chord):
    """Generates a scale of MIDI notes based on the selected chord and mode."""
    if chord not in CHORDS:
        raise ValueError("Invalid chord number. Choose between 1 and 7.")    
    
    intervals = CHORDS[chord]
    scale = []
    
    for octave in range(num_octaves):
        for interval in intervals:
            scale.append(midi_lowest_note + selected_root_note + interval + (octave * 12))
    
    scale.sort()
    
    return scale  

def update_rgb_leds():
    led[0] = ROOT_COLOR[selected_root_note]
    led[1] = MODE_COLOR[selected_mode]

wavetable1 = Wavetable(wavetable_fname, wave_len=wavetable_sample_size)

audio = audiopwmio.PWMAudioOut(board.OUT1)
amp_env = synthio.Envelope(attack_time=0.001, decay_time=0.75, sustain_level = 0.0, release_time = 0.2)
synth = synthio.Synthesizer(channel_count=1, sample_rate=sample_rate, envelope = amp_env)
audio.play(synth)

CHORDS = get_mode_chords(selected_mode)
scale = generate_scale_from_chord(selected_chord)

update_rgb_leds()

while True:
    sw.update()
    
    if sw.value == False and waiting_for_release and sw.current_duration >= 1.0:
        # print("Button held")
        waiting_for_release = False
        change_mode()
        scale = generate_scale_from_chord(selected_chord)
        update_rgb_leds()
    elif sw.rose and waiting_for_release:
        # print("Button release")
        waiting_for_release = False
        selected_root_note = (selected_root_note + 1) % 12;
        scale = generate_scale_from_chord(selected_chord)
        update_rgb_leds()
    elif sw.fell:
        # print("Button press")
        waiting_for_release = True
    else:
        pass
        
    cv2_value = cv2.value
    current_chord_index = get_chord_from_cv(cv2_value)   
    
    if current_chord_index != selected_chord:
        selected_chord = current_chord_index
        scale = generate_scale_from_chord(selected_chord)
        print(selected_chord)

    cv1_value = cv1.value
    current_note_index = get_note_from_cv(cv1_value)
    
    if current_note_index != selected_note:
        note = synthio.Note(synthio.midi_to_hz(scale[current_note_index]), amplitude = 0.8, waveform=wavetable1.waveform)
        synth.release_all_then_press(note)      
        selected_note = current_note_index
