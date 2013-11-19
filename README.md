Remodel
=======
Remodel is a replacement program for 'make'. 'make' takes a series of dependencies and builds targets by executing commands. Its use of UNIX timestamps as a happens-before relation means that it can fail to build properly. This is especially true during parallel builds. 

Remodel, will also take a series of dependencies and targets, etc. (grammar below), but unlike make, it will use MD5 hashes to detect new content and provide, together with the dependency chain, a happens-before relation (md5diff(oldA,newA) => everything that depends on A must be rebuilt). It executes all independent (non-conflicting) commands in parallel, using threads or processes. 

Remodel uses a different grammar than make. Dependencies can appear in any order. If you execute remodel with no arguments, it should start with the pseudo-target DEFAULT. Otherwise, the root is the argument to remodel, as in remodel foo.o. 

program ::= production*
production ::= target '<-' dependency (':' '"' command '"")
dependency ::= filename (',' filename)*
target ::= filename (',' filename)*


Here's an example that builds the program baz from two source files, foo.cpp and bar.cpp. 

DEFAULT <- baz
baz <- foo.o, bar.o: "g++ foo.o bar.o -o baz"
foo.o <- foo.cpp : "g++ -c foo.cpp -o foo.o"
bar.o <- bar.cpp: "g++ -c bar.cpp -o bar.o"

