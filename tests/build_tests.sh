g++ -g -Wall -Wextra \
-o test \
-ldl \
`pkg-config --libs cppunit` \
identity_vertex_test.cc \
interleaver_test.cc \
deinterleaver_test.cc \
test_runner.cc
