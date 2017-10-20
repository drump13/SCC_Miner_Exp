/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	main function for RootedTreeMiner

	Description: 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Rmisc.h"
#include "RFreeTree.h"
#include "RBFCFTree.h"
#include "RDFCFTree.h"
#include "RFrequentTreeFamily.h"
#include "RFrequentTreeList.h"

using namespace std;

short MIN_EDGE = ENDTAG;
short MAX_EDGE = 0;
short MIN_VERTEX = ENDTAG;
short MAX_VERTEX = 0;

int main(int argc, char* argv[])
{
	if ( argc != 4 )
	{	
		cout << "Usage: RootedTreeMiner support input_file output_file" << endl;
		exit (1);
	}
	
	long support;
	istringstream iss(argv[1]);
    iss >> support;
    if(!iss)
    {
        cout << "invalid argument, not an integer value" << endl;
        exit (1);
    }

	time_t start_time;
	time_t stop_time;
	long temp;


	/******************************************************************
	step1: read in the database
	******************************************************************/
	string inputFile = argv[2];
	string outputFile = argv[3];

	ofstream dataFile(outputFile.c_str());
	if(!dataFile)
	{
		cerr << "cannot open OUTPUT file!" << endl;
		exit(1);
	}

	ifstream inFile(inputFile.c_str());
	if(!inFile)
	{
		cerr << "cannot open INPUT file!" << endl;
		exit(1);
	}


	dataFile << "#Output Statistics for " << argv[2] << endl;
        dataFile << "#with support " << argv[1] << endl << endl;	
	
	vector<ptrRFreeTree> database;

	//read in the database
	ptrRFreeTree pft;
	char c;
	long counter = 1;
	database.push_back(new RFreeTree()); //a dummy tree
	while ( !inFile.eof() )
	{
		c = inFile.get();
		if ( inFile.eof() ) break;
		else inFile.putback(c);
		pft = new RFreeTree();
		inFile >> *pft;
		pft->tid = counter++; //force the tid list to be
		database.push_back(pft);
		c = inFile.get(); //to eat the last "end of line"
	}
	inFile.close();

	/******************************************************************
	step2: explore frequent free trees, either using pure extension, or
	use the hybrid method (extension + join)
	******************************************************************/
	vector<long> frequency(100);

	//pure extension, based on DFCF
	start_time = time(0);

	RFrequentTreeList freq3;
	freq3.populate3(database);
	freq3.finalize3(support);
	freq3.extensionExploreList3(database, dataFile, frequency, support);
	
	stop_time = time(0);

	dataFile << "pure extension for DFCF: " << endl;
	long totalSum = 0;
	for ( int i = 2; i < 100; i++ )
	{
		if ( frequency[i] > 0 )
		{
			dataFile << "frequent " << i << " trees: " << frequency[i] << endl;
			totalSum += frequency[i];
		}
		else break;
	}
	dataFile << endl;
	dataFile << "total frequent trees: " << totalSum << endl;
	dataFile << "time spent for pure extension: " << difftime(stop_time, start_time) << endl;
	dataFile << "time for each each pattern: " << difftime(stop_time, start_time)/totalSum << endl;
	dataFile << endl;

	for ( short j = 0; j < 100; j++ ) frequency[j] = 0;
	totalSum = 0;

	//pure extension, based on BFCF
	start_time = time(0);

	RFrequentTreeList freq2;
	freq2.populate2(database);
	freq2.finalize2(support);
	freq2.extensionExploreList2(database, dataFile, frequency, support);
	
	stop_time = time(0);

	dataFile << "pure extension for BFCF: " << endl;
	totalSum = 0;
	for ( int i = 2; i < 100; i++ )
	{
		if ( frequency[i] > 0 )
		{
			dataFile << "frequent " << i << " trees: " << frequency[i] << endl;
			totalSum += frequency[i];
		}
		else break;
	}
	dataFile << endl;
	dataFile << "total frequent trees: " << totalSum << endl;
	dataFile << "time spent for pure extension: " << difftime(stop_time, start_time) << endl;
	dataFile << "time for each each pattern: " << difftime(stop_time, start_time)/totalSum << endl;
	dataFile << endl;

	for ( short j = 0; j < 100; j++ ) frequency[j] = 0;
	totalSum = 0;

	//hybrid, based on BFCF
	start_time = time(0);

	RFrequentTreeList freq1;
	freq1.populate1(database);
	freq1.finalize1(support);
	freq1.hybridExploreList(database, dataFile, frequency, support);

	stop_time = time(0);


	dataFile << "hybrid: " << endl;
	totalSum = 0;
	for ( int i = 2; i < 100; i++ )
	{
		if ( frequency[i] > 0 )
		{
			dataFile << "frequent " << i << " trees: " << frequency[i] << endl;
			totalSum += frequency[i];
		}
		else break;
	}
	dataFile << endl;
	dataFile << "total frequent trees: " << totalSum << endl;
	dataFile << "time spent for hybrid method: " << difftime(stop_time, start_time) << endl;
	dataFile << "time for each each pattern: " << difftime(stop_time, start_time)/totalSum << endl;
	dataFile << endl;

	/******************************************************************
	step3: remove the database from the memory and output the results
	******************************************************************/
	for ( long s = 0; s < database.size(); s++ )
	{
		delete database[s];
	}

	dataFile.close();

	return 0;
}
