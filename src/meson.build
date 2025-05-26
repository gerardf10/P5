# Programas del proyecto

sources = [
	'instruments/instrument.cpp',
	'instruments/instrument_dumb.cpp',
	'synth/envelope_adsr.cpp',
	'synth/midi_score.cpp',
	'synth/multinote_instr.cpp',
	'synth/orchest.cpp',
	'synth/synthesizer.cpp',
	'effects/effect.cpp',
	'effects/tremolo.cpp',
	'effects/vibrato.cpp',
	'effects/clipping.cpp',
	'instruments/instrument_sin.cpp',
    'instruments/instrument_sampler.cpp',
    'instruments/instrument_fm.cpp',
	'instruments/instrument_fm_vibrato.cpp',

]

executable(
    'synth',
    sources: sources,
    include_directories: inc,
    link_args: ['-lm', '-lsndfile'],
    link_with: lib_pav,
    install: true,
)
