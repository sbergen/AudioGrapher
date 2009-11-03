g++ -o test \
-ldl \
`pkg-config --libs cppunit` \
identity_vertex_test.cc \
test_runner.cc
