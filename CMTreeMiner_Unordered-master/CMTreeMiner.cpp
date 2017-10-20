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

bool ALL = false;

int main(int argc, char* argv[])
{
	if ( argc != 4 )
	{	
		cout << "Usage: CMUnorderedTreeMiner support input_file output_file" << endl;
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
	step2.1: explore frequent free trees, using closed-pruning
	******************************************************************/
	vector<long> frequency(1000,0);
	vector<long> checked(1000,0);
	vector<long> closed(1000,0);
	vector<long> maximal(1000,0);
	long totalSum = 0;
	long temp = 0;

	//using closed-pruning
	start_time = time(0);

	RFrequentTreeList freq4;
	freq4.populate1(database);
	freq4.finalize1(support);
	freq4.populate3(database);
	freq4.finalize3(support);
	freq4.extensionExploreList4(database, dataFile, frequency, support, checked, closed, maximal);
	
	stop_time = time(0);

	dataFile << "using closed-pruning: " << endl;
	dataFile << "****************checked:****************" << endl;
	totalSum = 0;
	for ( int i = 1; i < 1000; i++ )
	{
		if ( checked[i] > 0 )
		{
			dataFile << "checked " << i << " trees: " << checked[i] << endl;
			totalSum += checked[i];
		}
	}
	dataFile << endl;
	dataFile << "total checked trees: " << totalSum << endl;

	dataFile << endl;
	dataFile << "****************closed:****************" << endl;
	totalSum = 0;
	for ( int i = 1; i < 1000; i++ )
	{
		if ( closed[i] > 0 )
		{
			dataFile << "closed " << i << " trees: " << closed[i] << endl;
			totalSum += closed[i];
		}
	}
	dataFile << endl;
	dataFile << "total closed trees: " << totalSum << endl;

	dataFile << endl;
	dataFile << "****************maximal:****************" << endl;
	totalSum = 0;
	for ( int i = 1; i < 1000; i++ )
	{
		if ( maximal[i] > 0 )
		{
			dataFile << "maximal " << i << " trees: " << maximal[i] << endl;
			totalSum += maximal[i];
		}
	}
	dataFile << endl;
	dataFile << "total maximal trees: " << totalSum << endl;


	dataFile << endl;
	dataFile << "time spent for closed-pruning extension: " << difftime(stop_time, start_time) << endl;
	dataFile << endl;

	/******************************************************************
	step2.2: explore frequent free trees, using pure extension
	******************************************************************/
	if ( ALL )
	{
		for ( short j = 0; j < 1000; j++ ) frequency[j] = 0;

		totalSum = 0;

		//pure extension, based on DFCF
		start_time = time(0);

		RFrequentTreeList freq3;
		freq3.populate3(database);
		freq3.finalize3(support);
		freq3.extensionExploreList3(database, dataFile, frequency, support);
		
		stop_time = time(0);

		dataFile << "pure extension for DFCF: " << endl;

		dataFile << "frequent " << '1' << " trees: " << checked[1] << endl;
		
		for ( int i = 2; i < 1000; i++ )
		{
			if ( frequency[i] > 0 )
			{
				dataFile << "frequent " << i << " trees: " << frequency[i] << endl;
				totalSum += frequency[i];
			}
			else break;
		}

		totalSum += checked[1];

		dataFile << endl;
		dataFile << "total frequent trees: " << totalSum << endl;
		dataFile << "time spent for pure extension: " << difftime(stop_time, start_time) << endl;
		dataFile << "time for each each pattern: " << difftime(stop_time, start_time)/totalSum << endl;
		dataFile << endl;
	}
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

/******************************************************************
Notes:
comparing the RootedTreeMiner, the following are changed
1.	didn't use BFCF at all, although the codes are still there
2.	in RFrequentTreeList, added extensionExploreList4(), to implement
	closed-pruning extension
3.	it seems that the vString(7) should be vString(6)
4.	instead of starting from 2-trees, start from single vertex (1-trees)
5.	changed possibleLeg() to consider single-vertex tree
6.	
******************************************************************/
