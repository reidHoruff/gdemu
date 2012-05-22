env = Environment()

#add support for GTK
env.ParseConfig('pkg-config --cflags --libs gtk+-2.0 gmodule-2.0')

env.Program('dcpu', 'src/main.c')

#compile java tool
Java('tools', './')
