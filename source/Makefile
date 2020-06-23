CXX= g++
CXXFLAGS=-g -std=c++11 -Werror -D_GLIBCXX_DEBUG -pthread
# The object files for the program.
SSL = -lcrypto -lssl

ClientNodeOFILES = ClientNode/node.o ClientNode/fileDescriptor.o ClientNode/client.o ClientNode/clientNode.o
ClientNodeHFILES = ClientNode/node.h ClientNode/fileDescriptor.h ClientNode/client.h ClientNode/clientNode.h config.h
ClientProgOFILES = ClientProg/hashObject.o ClientProg/fileDescriptor.o ClientProg/fileObject.o
ClientProgHFILES = ClientProg/hashObject.h ClientProg/fileDescriptor.h ClientProg/fileObject.h config.h
DataNodeOFILES = DataNode/node.o DataNode/dataNode.o
DataNodeHFILES = DataNode/node.h DataNode/dataNode.h config.h
MasterNodeHFILES = MasterNode/secureMap.h MasterNode/secureMap-inl.h MasterNode/inode.h MasterNode/dirInode.h MasterNode/dirInode-inl.h MasterNode/fileInode.h MasterNode/fileInode-inl.h MasterNode/fileDirectory.h MasterNode/fileDirectory-inl.h MasterNode/node.h MasterNode/node-inl.h MasterNode/client.h MasterNode/client-inl.h MasterNode/masterNode.h MasterNode/masterNode-inl.h config.h

all: ClientNode/main DataNode/main MasterNode/main Experiments/exp1-1 Experiments/exp1-2 Experiments/exp1-3 Experiments/exp1-4 Experiments/exp2-1 Experiments/exp2-2 Experiments/exp2-3 Experiments/exp2-4 Experiments/exp3-1 Experiments/exp3-2 Experiments/exp3-3 Experiments/exp3-4

ClientNode/main: $(ClientNodeHFILES) $(ClientNodeOFILES) ClientNode/main.o
			$(CXX) $(CXXFLAGS) -o $@ ClientNode/main.o $(ClientNodeOFILES)

ClientNode/%.o: ClientNode/%.cpp $(ClientNodeHFILES)
				$(CXX) $(CXXFLAGS) -c -o $@ $<

ClientProg/%.o: ClientProg/%.cpp $(ClientProgHFILES)
				$(CXX) $(CXXFLAGS) -c -o $@ $<

DataNode/main: $(DataNodeHFILES) $(DataNodeOFILES) DataNode/main.o
			$(CXX) $(CXXFLAGS) -o $@ DataNode/main.o $(DataNodeOFILES)

DataNode/%.o: DataNode/%.cpp $(DataNodeHFILES)
				$(CXX) $(CXXFLAGS) -c -o $@ $<

MasterNode/main: $(MasterNodeHFILES) MasterNode/main.o
			$(CXX) $(CXXFLAGS) -o $@ MasterNode/main.o

MasterNode/%.o: MasterNode/%.cpp $(MasterNodeHFILES)
				$(CXX) $(CXXFLAGS) -c -o $@ $<

Experiments/exp1-1: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp1-1.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp1-1.o $(ClientProgOFILES) $(SSL)

Experiments/exp1-2: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp1-2.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp1-2.o $(ClientProgOFILES) $(SSL)

Experiments/exp1-3: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp1-3.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp1-3.o $(ClientProgOFILES) $(SSL)

Experiments/exp1-4: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp1-4.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp1-4.o $(ClientProgOFILES) $(SSL)

Experiments/exp2-1: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp2-1.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp2-1.o $(ClientProgOFILES) $(SSL)

Experiments/exp2-2: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp2-2.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp2-2.o $(ClientProgOFILES) $(SSL)

Experiments/exp2-3: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp2-3.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp2-3.o $(ClientProgOFILES) $(SSL)

Experiments/exp2-4: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp2-4.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp2-4.o $(ClientProgOFILES) $(SSL)

Experiments/exp3-1: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp3-1.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp3-1.o $(ClientProgOFILES) $(SSL)

Experiments/exp3-2: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp3-2.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp3-2.o $(ClientProgOFILES) $(SSL)

Experiments/exp3-3: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp3-3.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp3-3.o $(ClientProgOFILES) $(SSL)

Experiments/exp3-4: $(ClientProgHFILES)  $(ClientProgOFILES) Experiments/exp3-4.o
		$(CXX) $(CXXFLAGS) -o $@ Experiments/exp3-4.o $(ClientProgOFILES) $(SSL)

Experiments/%.o: Experiments/%.cpp $(ClientProgHFILES)
				$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean all
clean:
		rm -f ClientNode/*.o
		rm -f ClientNode/main
		rm -f ClientProg/*.o
		rm -f DataNode/*.o
		rm -f DataNode/main
		rm -f MasterNode/*.o
		rm -f MasterNode/main
		rm -f Experiments/*.o
		rm -f Experiments/exp1-1
		rm -f Experiments/exp1-2
		rm -f Experiments/exp1-3
		rm -f Experiments/exp1-4
		rm -f Experiments/exp2-1
		rm -f Experiments/exp2-2
		rm -f Experiments/exp2-3
		rm -f Experiments/exp2-4
		rm -f Experiments/exp3-1
		rm -f Experiments/exp3-2
		rm -f Experiments/exp3-3
		rm -f Experiments/exp3-4
