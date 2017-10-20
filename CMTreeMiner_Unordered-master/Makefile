GCC=g++ -O3
OBJS=CMTreeMiner.o RFrequentTreeList.o RFrequentTreeFamily.o RBFCFTree.o RDFCFTree.o RFreeTree.o
CMUnorderedTreeMiner: CMTreeMiner.o RFrequentTreeList.o RFrequentTreeFamily.o RBFCFTree.o RDFCFTree.o RFreeTree.o
	$(GCC) -o CMUnorderedTreeMiner CMTreeMiner.o RFrequentTreeList.o RFrequentTreeFamily.o RBFCFTree.o RDFCFTree.o RFreeTree.o;
	rm -f $(OBJS)
CMTreeMiner.o: CMTreeMiner.cpp RFrequentTreeList.h RFrequentTreeFamily.h RBFCFTree.h RDFCFTree.h RFreeTree.h Rmisc.h
	$(GCC) -c CMTreeMiner.cpp
RFrequentTreeList.o: RFrequentTreeList.cpp RFrequentTreeList.h RFrequentTreeFamily.h RBFCFTree.h RDFCFTree.h RFreeTree.h Rmisc.h
	$(GCC) -c RFrequentTreeList.cpp 
RFrequentTreeFamily.o: RFrequentTreeFamily.cpp RFrequentTreeFamily.h RBFCFTree.h RFreeTree.h Rmisc.h
	$(GCC) -c RFrequentTreeFamily.cpp
RDFCFTree.o: RDFCFTree.cpp RDFCFTree.h RFreeTree.h Rmisc.h
	$(GCC) -c RDFCFTree.cpp
RBFCFTree.o: RBFCFTree.cpp RBFCFTree.h RFreeTree.h Rmisc.h
	$(GCC) -c RBFCFTree.cpp
RFreeTree.o: RFreeTree.cpp RFreeTree.h Rmisc.h
	$(GCC) -c RFreeTree.cpp
