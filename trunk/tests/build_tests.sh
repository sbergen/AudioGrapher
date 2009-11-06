g++ -g -Wall -Wextra \
-o test \
`pkg-config --libs --cflags cppunit gthread-2.0 glibmm-2.4 samplerate` \
-ldl \
\
../sr_converter.cc \
\
identity_vertex_test.cc \
interleaver_test.cc \
deinterleaver_test.cc \
interleaver_deinterleaver_test.cc \
threader_test.cc \
sr_converter_test.cc \
chunker_test.cc \
test_runner.cc
