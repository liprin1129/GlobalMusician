from pydub import AudioSegment

sound1 = AudioSegment.from_wav("../DATA/0m-048-C3.wav")
sound2 = AudioSegment.from_wav("../DATA/0m-057-A3.wav")
sound3 = AudioSegment.from_wav("../DATA/0m-060-C4.wav")
#combined_sounds = sound1 + sound2 + sound3 
#combined_sounds.export("joinedFile.wav", format="wav")

tmp_audio = sound1.overlay(sound2)
combined_sound = tmp_audio.overlay(sound3)

combined_sound.export("../DATA/mixed_audio.wav", format="wav")