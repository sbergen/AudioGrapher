g++ -o test `pkg-config --cflags --libs glibmm-2.4 sndfile samplerate` \
gdither/gdither.cc \
sample_format_converter.cc \
sndfile_writer.cc \
sr_converter.cc \
test.cc