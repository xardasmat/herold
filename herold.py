
import srt
import wave
from pydub import AudioSegment
from piper import PiperVoice
from absl import app
from absl import flags
import io

FLAGS = flags.FLAGS

flags.DEFINE_string('piper_model', 'piper.onnx', 'path to piper tts model')
flags.DEFINE_string('audio', 'input.mp3', 'path to original audio track')
flags.DEFINE_string('output', 'output.mp3', 'path to output audio track')
flags.DEFINE_string('sub', 'sub.srt', 'path to original subtitles file')

seconds_to_millis = 1000

def get_sub():
    with open(FLAGS.sub, "r", encoding="utf-8") as file:
        text = file.read()
        return srt.parse(text)
    
def synth(voice, text):
    in_mem_file = io.BytesIO()
    wave_file = wave.open(in_mem_file, 'wb')
    voice.synthesize_wav(text, wave_file)
    return AudioSegment.from_file(in_mem_file, "wav")

def main(argv):
    full_audio = AudioSegment.from_file(FLAGS.audio)
    voice = PiperVoice.load(FLAGS.piper_model)

    lektor_pos = 0
        
    for sub in get_sub():
        print(str(sub.start)+"->"+str(sub.end)+" : "+sub.content)
        lektor = synth(voice, sub.content)
        lektor_pos = max(sub.start.total_seconds(), lektor_pos)
        full_audio = full_audio.overlay(lektor, position=int(lektor_pos*seconds_to_millis), gain_during_overlay=-8)
        lektor_pos += lektor.duration_seconds

    full_audio.export(FLAGS.output)


if __name__ == '__main__':
  app.run(main)