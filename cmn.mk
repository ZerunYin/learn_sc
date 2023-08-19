SC_HOME := $(HOME)/learn_sc/systemc/2.3.4
SC_INC  := $(SC_HOME)/include
SC_LIB  := $(SC_HOME)/lib-linux64


CXX_SC := g++ -std=c++11 -Wall -g -I$(SC_INC) -L$(SC_LIB) -Wl,-rpath=$(SC_LIB) -lpthread -lsystemc 
