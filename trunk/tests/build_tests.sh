g++ -g -Wall -Wextra \
-o test \
`pkg-config --libs --cflags gthread-2.0 glibmm-2.4` \
-ldl \
`pkg-config --libs cppunit` \
identity_vertex_test.cc \
interleaver_test.cc \
deinterleaver_test.cc \
interleaver_deinterleaver_test.cc \
threader_test.cc \
test_runner.cc
