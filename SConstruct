env = Environment()

#add support for GTK
env.ParseConfig('pkg-config --cflags --libs gtk+-2.0 gmodule-2.0')

files = [
	'src/main.c',
	'src/dcpu.c',
	'src/disassembler.c',
	'src/gui.c',
	'src/hardware.c',
	'src/instructions.c',
	'src/keyboard.c',
	'src/lem.c',
	'src/clock.c',
]

env.Program('dcpu', files)

#compile java tool
Java('tools', './')
