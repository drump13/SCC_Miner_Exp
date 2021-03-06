/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	Class: RDFCFTree

	Description: Used to store one tree in DFCF
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Rmisc.h"
#include "RFreeTree.h"
#include "RDFCFTree.h"

using namespace std;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: Assuming that the number of vertices v and the tid t are given
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RDFCFTree::RDFCFTree(short v, long t) :
  RFreeTree(v, t), level(v+1,0), automorphism(v+1,0)
{
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: Assuming that only tid t is given
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RDFCFTree::RDFCFTree(long t) :
  RFreeTree(t)
{
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: Assuming that nothing is given
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RDFCFTree::RDFCFTree() : 
  RFreeTree()
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: copy constructor
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RDFCFTree::RDFCFTree(const RDFCFTree& rhs)
  : RFreeTree(rhs)
{
	level = rhs.level;
	automorphism = rhs.automorphism;
	canonicalString = rhs.canonicalString;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: copy from the parent in the enumeration tree, plus
	a new leg
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RDFCFTree::RDFCFTree(const RDFCFTree& parent, 
				   short newEdgeLabel, short newVertexLabel, short position)
{
	/******************************************************************
	idea: copy the tree structure of the parent, plus one new leg
	Note: use deep copy and preserve the original order of link list
	at the end, recompute the DFCS and automorphisms
	******************************************************************/
	vCount = parent.vCount + 1;
	tid = parent.tid;
	adj.resize(vCount + 1);
	TnodeLink t1, t2, t3;
	for ( short i = 1; i <= vCount - 1; i++ ) //copy the parent part here
	{
		t1 = parent.adj[i];
		if ( t1 == 0 ) //unlike to happen for a tree
		{
			adj[i] = 0;	
			continue;
		}
		else
		{
			t2 = new Tnode(*t1);
			adj[i] = t2;
			while ( t1->next != 0 )
			{
				t1 = t1->next;
				t3 = new Tnode(*t1);
				t2->next = t3;
				t2 = t3;
			}
		}
	}
	vertexLabel = parent.vertexLabel;
	vertexLabel.push_back(newVertexLabel);
	degree = parent.degree;
	degree.push_back(0);
	level = parent.level;
	level.push_back(level[position]+1);
	insertEdge(Edge(position,vCount,newEdgeLabel));
	automorphism.resize(vCount+1);

	computeDFCS();
	computeAutomorphism();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	constructor

	Decription: construct from a canonical string
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RDFCFTree::RDFCFTree(const vector<short>& cString) 
{
	tid = 0; //there is no tid information in this case
	short currentIndex;
	
	vCount = cString[0];
	adj.resize(vCount+1, (TnodeLink) 0);
	vertexLabel.resize(vCount+1);
	degree.resize(vCount+1,0);
	
	level.resize(vCount+1);
	automorphism.resize(vCount+1);

	vertexLabel[1] = cString[2]; // cString[1] = 0 all the time

	if ( vCount == 1 ) 
	{
		level[1] = 1;
		automorphism[1] = 1;
		return;
	}

	currentIndex = 2;
	stack<short> tempK;
	tempK.push(1);

	short idx = 3;
	while ( cString[idx] != ENDOFTREETAG )
	{
		if ( cString[idx] == ENDTAG ) //backtrack
		{
			tempK.pop();
			idx++;
			continue;
		}
		else
		{
			vertexLabel[currentIndex] = cString[idx+1];
			insertEdge(Edge(currentIndex,tempK.top(),cString[idx]));
			tempK.push(currentIndex);
			currentIndex++;
			idx+=2;
		}
	}

	computeDFCS();
	computeAutomorphism();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	destructor

	Decription: defaultly call the destructor of RFreeTree
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RDFCFTree::~RDFCFTree()
{
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	operator=()

	Decription: assignment copy, same as copy constructor
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
RDFCFTree& RDFCFTree::operator=(const RDFCFTree& rhs)
{
	RFreeTree::operator =((RFreeTree)rhs); //is this the correct way?
	level = rhs.level;
	automorphism = rhs.automorphism;
	canonicalString = rhs.canonicalString;	
	return *this;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	computeDFCS()

	Decription: compute the canonicalString of the DFCF tree, assuming
	that the tree has been normalized.
	Note: vector level is a byproduct of this step
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void RDFCFTree::computeDFCS()
{
	short i;
	canonicalString.clear();
	stack<short> tempK;
	vector<bool> isVisited(vCount + 1, 0);
	vector<short> edgeToParent(vCount + 1, 0);
	TnodeLink t;
	short tempCount = vCount;

	canonicalString.push_back(vCount);

	tempK.push(1);
	isVisited[1] = true;
	level[1] = 1;

	while ( ! tempK.empty() && tempCount > 0 )
	{
		i = tempK.top();
		tempK.pop();
		if ( i == -1 ) //a backtrack
		{
			canonicalString.push_back(ENDTAG);
			continue;
		}

		tempCount--;

		//step 1: output the current node
		canonicalString.push_back(edgeToParent[i]);
		canonicalString.push_back(vertexLabel[i]);
		
		//step 2: push into stack all decendents of i
		t = adj[i];
		while ( t != 0 )
		{
			if ( t->v > i && !isVisited[t->v]) //decendents
			{
				tempK.push(-1);
				tempK.push(t->v);
				isVisited[t->v] = true;
				edgeToParent[t->v] = t->eLabel;
				level[t->v] = level[i] + 1;
			}
			t = t->next;
		}
	}

	canonicalString.push_back(ENDOFTREETAG);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	computeSubDFCS()

	Decription: compute the canonicalString of a subtree of the DFCF tree, 
	assuming that the tree has been normalized.
	Note: the subDFCS does not include the information about # of vertices
	Note: assuming the parameter "root" of the function can never be the
	root of the whole tree
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void RDFCFTree::computeSubDFCS(short root, vector<short>& cString)
{
	short i;
	stack<short> tempK;
	vector<bool> isVisited(vCount + 1, 0);
	vector<short> edgeToParent(vCount + 1, 0);
	TnodeLink t;
	short tempCount = 0;

	queue<short> tempQ;
	tempQ.push(root);
	while ( !tempQ.empty() )
	{
		i = tempQ.front();
		tempQ.pop();
		tempCount++;
		t = adj[i];
		while ( t->next != 0 )
		{
			tempQ.push(t->v);
			t = t->next;
		}
	}

	tempK.push(root);
	isVisited[root] = true;

	while ( ! tempK.empty() && tempCount > 0 )
	{
		i = tempK.top();
		tempK.pop();
		if ( i == -1 ) //a backtrack
		{
			cString.push_back(ENDTAG);
			continue;
		}

		tempCount--;

		//step 1: output the current node
		cString.push_back(edgeToParent[i]);
		cString.push_back(vertexLabel[i]);
		
		//step 2: push into stack all decendents of i
		t = adj[i];
		while ( t != 0 )
		{
			if ( t->v > i && !isVisited[t->v]) //decendents
			{
				tempK.push(-1);
				tempK.push(t->v);
				isVisited[t->v] = true;
				edgeToParent[t->v] = t->eLabel;
				level[t->v] = level[i] + 1;
			}
			t = t->next;
		}
	}

	cString.push_back(ENDOFTREETAG);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	computeAutomorphism()

	Decription: compute the automorphisms of the DFCF tree, assuming
	that the tree has been normalized, and the computeDFCF() has be called
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void RDFCFTree::computeAutomorphism()
{
	vector<short> rank(vCount+1);
	vector<short> order(vCount+1);
	vector<short> parent(vCount+1);
	vector< vector<short> > codeString(vCount+1);

	TnodeLink t;
	short head, tail, i, j, k;
	
	vector<short> countLevel(vCount+1, 0);
	vector<short> mapping(vCount+1,0);

	for ( i = 1; i <= vCount; i++ ) countLevel[level[i]]++;

	short temp = countLevel[1];
	countLevel[1] = 1;
	short total = temp;
	for ( i = 2; i <= vCount; i++ )
	{
		temp = countLevel[i];
		countLevel[i] = total + 1; 
		total += temp;
	}

	for ( i = 1; i <= vCount; i++ )
		mapping[countLevel[level[i]]++] = i;

	vector<short> inverseMapping(vCount + 1, 0);
	for ( i = 1; i <= vCount; i++ )
		inverseMapping[mapping[i]] = i;

	for ( i = 0; i <= vCount; i++ ) order[i] = i; //initialize vector order

	//stage 1: upward stage
	tail = vCount;
	while ( level[mapping[tail]] != 1 ) //not the root
	{
		head = tail;
		while ( level[mapping[head-1]] == level[mapping[head]] ) head--;
		
		//stage1_1: construct codeString for current level
		for ( i = head; i <= tail; i++ )
		{
			codeString[i].push_back(0); //temporary edgeToParent label
			codeString[i].push_back(vertexLabel[mapping[i]]);
			t = adj[mapping[i]];
			while ( t->next != 0 )
			{
				codeString[i].push_back(rank[inverseMapping[t->v]]);
				t = t->next;
			}
			codeString[i].push_back(ENDTAG);
			parent[i] = inverseMapping[t->v];
			codeString[i][0] = t->eLabel; //put in correct edgeToParent label
		}

		//stage1_2: sort all vertices in current level to get order
		for ( i = head; i < tail; i++ )
		{
			for ( j = tail; j > head; j-- )
			{
				if ( codeString[order[j - 1]] > codeString[order[j]] )
				{
					k = order[j-1];
					order[j-1] = order[j];
					order[j] = k;
				}
			}
		}

		//stage1_3: scan current level to get rank
		k = 1; //current rank
		rank[order[head]] = k;
		for ( i = head + 1; i <= tail; i++ )
		{
			if ( codeString[order[i]] > codeString[order[i-1]] ) k++;
			rank[order[i]] = k;
		}
		tail = head - 1;
	}

	//stage 2: downward stage, compute automorphisms
	automorphism[mapping[1]] = 1;
	head = 2;
	while ( head <= vCount )
	{
		tail = head;
		while ( (tail <= vCount) && (level[mapping[tail]] == level[mapping[head]]) ) tail++;
		tail--;
		//stage2_1: bubblesort again, refine the order, 
		//          taking automorphisms of parents into consideration
		for ( i = head; i < tail; i++ )
		{
			for ( j = tail; j > head; j-- )
			{
				if ( (rank[order[j - 1]] == rank[order[j]]) &&
					 (automorphism[mapping[parent[order[j-1]]]] 
						> automorphism[mapping[parent[order[j]]]]) )
				{
					k = order[j-1];
					order[j-1] = order[j];
					order[j] = k;
				}
			}
		}

		//stage2_2: compute automorphims of current level 
		k = mapping[order[head]]; //current automorphism
		automorphism[mapping[order[head]]] = k;
		for ( i = head + 1; i <= tail; i++ )
		{
			if ( !((rank[order[i - 1]] == rank[order[i]]) &&
					 (automorphism[mapping[parent[order[i-1]]]] 
					 == automorphism[mapping[parent[order[i]]]])) )
				k = mapping[order[i]];
			automorphism[mapping[order[i]]] = k;
		}

		head = tail + 1;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	possibleLegs()

	Decription: for a location i, compute the range of the possible legs
	Note: (0,0) means no restriction, (MAX_EDGE+1,MAX_VERTEX+1) means impossible
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void RDFCFTree::possibleLegs(const short i, short& minEdge, short& minVertex )
{
	if ( automorphism[i] != i ) //no leg possible
	{
		minEdge = MAX_EDGE + 1;
		minVertex = MAX_VERTEX + 1;
		return;
	}

	//find how many backtrack are needed to go from the right most node to i
	short distanceToRightmost = 0;
	short j = i;
	if ( vCount != 1 ) //be careful! For a single-vertex tree, adj[j] = empty
	{
		while ( level[j] == 1 || degree[j] > 1 )
		{
			distanceToRightmost++;
			j = adj[j]->v;
		}
	}

	minEdge = MIN_EDGE;
	minVertex = MIN_VERTEX;
	//first, check the position itself
	if ( (level[i] != 1 && degree[i] != 1) ||
			(level[i] == 1 && degree[i] != 0) ) 
	//degree[i] is 1 and i is not the root, it's a leaf, no restriction at this level
	{
		minEdge = adj[i]->eLabel; //the last leg
		minVertex = vertexLabel[adj[i]->v]; //the last leg
	}
	
	//second, check all above ancestors
	j = i;
	short p, q;
	bool possible = true;
	while ( possible && level[j] != 1 )
	{
		//find the parent of j
		TnodeLink t = adj[j];
		while ( t->next != 0 ) t = t->next;
		p = t->v; //p is the parent of j;
		if ( level[p] != 1 ) //if p is not the root
		{
			if ( degree[p] <= 2 ) //if j has no sibling, nothing to do
			{
				j = p;
				continue;
			}
		}

		//find the sibling to the left of j
		t = adj[p];
		while ( t->v != j ) t = t->next; //find the correct sibling
		
		if ( (level[p] == 1 && t->next == 0) || 
				(level[p] != 1 && t->next->next == 0) ) //if j is the left most sibling
		{
			j = p;
			continue;
		}

		//now t points to j, t->next points to the left sibling of j
		if ( t->eLabel != t->next->eLabel || 
				vertexLabel[t->next->v] != vertexLabel[j] )
		{
			j = p;
			continue;
		}

		//find the restriction on the new leg by current sibling
		vector<short> cString1;
		vector<short> cString2;
		computeSubDFCS(t->next->v, cString1);
		computeSubDFCS(j, cString2);
		
		q = distanceToRightmost;
		if ( q > 0 )
		{
			cString2[cString2.size() - 1] = ENDTAG;
			q--;
			while ( q > 0 )
			{
				cString2.push_back(ENDTAG);
				q--;
			}
			cString2.push_back(ENDOFTREETAG);
		}

		//cout << "j is: " << j << endl;
		//cout << "string1: " << endl;
		//for ( short m = 0; m < cString1.size(); m++ )
		//	cout << cString1[m] << ' ';
		//cout << endl;

		//cout << "string2: " << endl;
		//for ( short m = 0; m < cString2.size(); m++ )
		//	cout << cString2[m] << ' ';
		//cout << endl;

		//cout << "before! edge: " << minEdge << ' ' << " vertex: " << minVertex << endl;

		short k;
		for ( k = 0; k < cString2.size() - 1; k++ )
		{
			if ( cString1[k] > cString2[k] )
			{
				possible = false;
				break;
			}
			else if ( cString1[k] < cString2[k] )
			{
				break;
			}
		}

		if ( k == cString2.size() - 1 ) //a tie
		{
			if ( cString1[k] == ENDTAG || cString1[k] == ENDOFTREETAG )
			{
				possible = false;
				break;
			}
			else if ( cString1[k] > minEdge )
			{
				minEdge = cString1[k];
				minVertex = cString1[k+1];
			}
			else if ( cString1[k] == minEdge && cString1[k+1] > minVertex )
				minVertex = cString1[k+1];
		}

		j = p;
	}//end of while(possible...)

	if ( !possible )
	{
		minEdge = MAX_EDGE + 1;
		minVertex = MAX_VERTEX + 1;
	}
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	extensionExplore()

	Decription: find all possible children in the enumeration tree that
	are frequent. Using extension operation only.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void RDFCFTree::extensionExplore( vector<long>&frequency,
						ostream & outFile,
						const vector<ptrRFreeTree>& database,
					    const vector<Occurrence> & occList,
					   	const vector< vector<short> > & frequent2Tree,
						const vector<long> & headIndex,
						const long & threshold )
{
	/******************************************************************
	step0: output this tree
	******************************************************************/	
	frequency[vCount]++;
	//if ( vCount == 10 ) 
	//{
	//	cout << "canonical string length is: " << canonicalString.size() << endl;
	//	for ( short i = 0; i < canonicalString.size(); i++ )
	//	{
	//		cout << canonicalString[i] << ' ';
	//	}
	//	cout << endl;
	//}
	//cout << *this;

	/******************************************************************
	step1: find the locations where a new leg can grow
	******************************************************************/	
	short j = 1;
	vector<short> positionToExplore;
	while ( level[j] == 1 || degree[j] > 1 )
	{
		positionToExplore.push_back(j);
		j = adj[j]->v;
	}

	positionToExplore.push_back(j);

	/******************************************************************
	step2: main loop, for each position, explore
	******************************************************************/	
	for ( j = 0; j < positionToExplore.size(); j++ )
	{
		short i = positionToExplore[j];
		//step2_1: get the range of valid legs
		short minEdge;
		short minVertex;
		possibleLegs(i, minEdge, minVertex);

		if ( minEdge > MAX_EDGE ) continue; //continue the for loop

		if ( headIndex[vertexLabel[i] - MIN_VERTEX] == 0 ) continue;

		TnodeLink t;

		//step2_2: get the possible frequent legs
		vector<bool> isFrequent( (MAX_EDGE - MIN_EDGE + 1) 
								*(MAX_VERTEX - MIN_VERTEX + 1), false);
		for (short j = headIndex[vertexLabel[i] - MIN_VERTEX]; 
			  (j < frequent2Tree.size() && frequent2Tree[j][0] == vertexLabel[i]); j++ )
			isFrequent[( frequent2Tree[j][1] - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 )
				+ ( frequent2Tree[j][2] - MIN_VERTEX )] = true;


		//step2_3: explore each potential leg
		set<short> neighbors;
		set<short>::iterator pos;
		Occurrence tempOcc;
		vector<SupportNode> potential((MAX_EDGE - MIN_EDGE + 1) 
									  *(MAX_VERTEX - MIN_VERTEX + 1));

		for ( long s = 0; s < occList.size(); s++ )
		{
			neighbors.clear();
			t = adj[i];
			while ( t != 0 ) //insert index of all neighbors of the position i
			{
				neighbors.insert(occList[s].nodeIndex[t->v - 1]);//inconsistency on index
				t = t->next;
			}
			t = database[occList[s].tid]->adj[occList[s].nodeIndex[i-1]];
			while ( t != 0 )
			{
				if ( occList[s].nodeIndex[i-1] < t->v )
				{
					pos = neighbors.find( t->v );
					if ( pos == neighbors.end() ) //if the vertex hasn't been used
					{
						short tempE = t->eLabel;
						short tempV = database[occList[s].tid]->vertexLabel[t->v];
						short location = ( tempE - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 ) 
							+ ( tempV - MIN_VERTEX ); 
						if ( ((tempE > minEdge) || (tempE == minEdge && tempV >= minVertex)) &&
							isFrequent[location] ) //if the leg is potentially frequent
						{
							tempOcc = occList[s];
							tempOcc.nodeIndex.push_back(t->v);
							potential[location].occList.push_back(tempOcc);
							if ( tempOcc.tid != potential[location].lastTid )
							{
								potential[location].lastTid = tempOcc.tid;
								potential[location].support++;
							}
						}
					}
				}
				t = t->next;
			}//end of while ( t != 0 )
		}//end of for ( s = 0; ...)

		for ( long s = 0; s < potential.size(); s++ )
		{
			if ( potential[s].support >= threshold )
			{
				short tempE = MIN_EDGE + (short)(floor(s/(MAX_VERTEX - MIN_VERTEX + 1)));
				short tempV = MIN_VERTEX + (s % (MAX_VERTEX - MIN_VERTEX + 1));
				RDFCFTree *pbfcf = new RDFCFTree(*this,tempE,tempV,i);
				pbfcf->extensionExplore(frequency, outFile, database,potential[s].occList,
							   frequent2Tree,headIndex,threshold);
				delete pbfcf;
			}
		}

		////test
		//cout << "leg position is: " << i << " vertex label is: " << vertexLabel[i] << endl;
		//cout << "min edge and min vertex are: " << minEdge << ' ' << minVertex << endl;
		//for ( j = 0; j < isFrequent.size(); j++ )
		//	cout << isFrequent[j] << ' ';
		//cout << endl;
		//cout << endl;

	}//end of for(short i = ...)
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	closedExtensionExplore()

	Decription: find all possible children in the enumeration tree that
	are frequent. Using extension operation only. Using closed-pruning
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void RDFCFTree::closedExtensionExplore( vector<long>&frequency,
						ostream & outFile,
						const vector<ptrRFreeTree>& database,
					    const vector<Occurrence> & occList,
					   	const vector< vector<short> > & frequent2Tree,
						const vector<long> & headIndex,
						const long & threshold,
					    vector<long> & checked, 
						vector<long> & closed, 
						vector<long> & maximal)
{
	/******************************************************************
	step0: output this tree
	******************************************************************/	
	checked[vCount]++;
	//if ( vCount == 10 ) 
	//{
	//	cout << "canonical string length is: " << canonicalString.size() << endl;
	//	for ( short i = 0; i < canonicalString.size(); i++ )
	//	{
	//		cout << canonicalString[i] << ' ';
	//	}
	//	cout << endl;
	//}

	//cout << *this;
	//cout << "support is: " << occList.size() << endl << endl;

	TnodeLink t;

	/******************************************************************
	step1: using occurrence-match pruning
	******************************************************************/	
	/******************************************************************
	step1.1: initialize the parent from the first occurrence
	******************************************************************/	
	short parentVertex, parentEdge;
	bool sameParent = true;

	if ( occList[0].nodeIndex[0] == 1 ) //if the first occurrence's root
										//is the root of the transaction
		sameParent = false;
	else
	{
		t = database[occList[0].tid]->adj[occList[0].nodeIndex[0]];
		while ( t->next != 0 ) t = t->next;
		parentEdge = t->eLabel;
		parentVertex = database[occList[0].tid]->vertexLabel[t->v];
	}

	/******************************************************************
	step1.2: use other occurrences to compute the intersections of parents
	******************************************************************/
	for ( long s = 1; s < occList.size(); s++ )
	{
		short tempEdge, tempVertex;
		if ( occList[s].nodeIndex[0] == 1 ) //if the occurrence's root
											//is the root of the transaction
			sameParent = false;
		else
		{
			t = database[occList[s].tid]->adj[occList[s].nodeIndex[0]];
			while ( t->next != 0 ) t = t->next;
			tempEdge = t->eLabel;
			tempVertex = database[occList[s].tid]->vertexLabel[t->v];

			if ( tempEdge != parentEdge || tempVertex != parentVertex )
				sameParent = false;
		}

		if ( sameParent == false ) break;
	}

	//parent-pruning
	if ( sameParent == true ) return;

	/******************************************************************
	step1.3: find the locations where a new leg can grow
	******************************************************************/	
	vector<short> positionToExplore;
	if ( vCount != 1 ) //be careful! For a single-vertex tree, adj[j] = empty
	{
		short j = 1;
		while ( level[j] == 1 || degree[j] > 1 )
		{
			positionToExplore.push_back(j);
			j = adj[j]->v;
		}
		positionToExplore.push_back(j);
	}
	else
		positionToExplore.push_back(1);

	/******************************************************************
	step1.4: compute the range of labels for each vertex
	******************************************************************/	
	vector<short> vertexRange(vCount + 1, MAX_VERTEX + 1);
	vector<short> edgeRange(vCount + 1, MAX_EDGE + 1);
	for ( short j = 0; j < positionToExplore.size(); j++ )
	{
		short i = positionToExplore[j];
		possibleLegs(i, edgeRange[i], vertexRange[i]);
	}

	/******************************************************************
	step1.5: initialize the list of legs from the first occurrence
	******************************************************************/	
	vector<short> legTriple(3); //vertex index, leg edge label, leg vertex label
	vector<vector<short> > commonLegs;
	set<short> neighbors; // 
	set<short>::iterator pos;

	for ( short i = 1; i <= vCount; i++ )
	{
		neighbors.clear();
		t = adj[i];
		while ( t != 0 ) //insert index of all neighbors of the position i
		{
			neighbors.insert(occList[0].nodeIndex[t->v - 1]);//inconsistency on index
			t = t->next;
		}
			
		t = database[occList[0].tid]->adj[occList[0].nodeIndex[i-1]];
		while ( t != 0 )
		{
			if ( occList[0].nodeIndex[i-1] < t->v )
			{
				pos = neighbors.find( t->v );
				if ( pos == neighbors.end() ) //if the vertex hasn't been used
				{
					legTriple[0] = i;
					legTriple[1] = t->eLabel;
					legTriple[2] = database[occList[0].tid]->vertexLabel[t->v];
					commonLegs.push_back(legTriple);
				}
			}
			t = t->next;
		}//end of while ( t != 0 )
	}

	/******************************************************************
	step1.6: use other occurrences to compute the intersections of legs
	******************************************************************/
	for ( long s = 1; s < occList.size(); s++ )
	{
		vector<bool> isFetched(vCount + 1, false);
		vector<vector<short> > tupleLegs(0);
		vector<short> legEVPair(2);
		vector<vector<short> >::iterator pos1;

		//debug
		//cout << "a new round!" << endl;
		//cout << "the size of commonLegs is: " << commonLegs.size() << endl;

		for ( pos1 = commonLegs.begin(); pos1 != commonLegs.end(); )
		{
			vector<short> thisTriple = *pos1; //get the next commonLeg
			
			//debug
			//cout << commonLegs.size() << endl;
			//cout << thisTriple[0] << ' ' <<  thisTriple[1] << ' ' <<  thisTriple[2] << endl; 

			short i = thisTriple[0]; //the index of vertex
			//assuming the indices in the commonLegs are non-decreasing

			if ( !isFetched[i] ) //fetch all neighbors of the vertex in the
								//corresponding database transaction
			{
				neighbors.clear();
				tupleLegs.resize(0);
				t = adj[i];
				while ( t != 0 ) //insert index of all neighbors of the position i
				{
					neighbors.insert(occList[s].nodeIndex[t->v - 1]);//inconsistency on index
					t = t->next;
				}
				t = database[occList[s].tid]->adj[occList[s].nodeIndex[i-1]];
				while ( t != 0 )
				{
					if ( occList[s].nodeIndex[i-1] < t->v )
					{
						pos = neighbors.find( t->v );
						if ( pos == neighbors.end() ) //if the vertex hasn't been used
						{
							legEVPair[0] = t->eLabel;
							legEVPair[1] = database[occList[s].tid]->vertexLabel[t->v];
							tupleLegs.push_back(legEVPair);
						}
					}
					t = t->next;
				}
				isFetched[i] = true;
			}

			bool isFound = false;
			for ( short k = 0; k < tupleLegs.size(); k++ )
			{
				if ( thisTriple[1] == tupleLegs[k][0] && thisTriple[2] == tupleLegs[k][1] )
				{
					isFound = true;
					break;
				}
			}

			if ( !isFound )
			{
				pos1 = commonLegs.erase(pos1);
			}
			else
			{
				++pos1;
			}
		}

		if ( commonLegs.size() == 0 ) break;
	}

	if ( commonLegs.size() != 0 )
	{
		set<short> positionSet;
		for ( short i = 0; i < positionToExplore.size(); i++ )
			positionSet.insert(positionToExplore[i]);
		
		for ( short i = 0; i < commonLegs.size(); i++ )
		{
			pos = positionSet.find(commonLegs[i][0]);
			if ( pos == positionSet.end() ) //not on the rightmost path
			{
				return;
			}
			else
			{
				short j = commonLegs[i][0];
				if ( (commonLegs[i][1] < edgeRange[j]) ||
					((commonLegs[i][1] == edgeRange[j]) && (commonLegs[i][2] < vertexRange[j])) )
					return;
			}
		}
	}


	bool isClosed = true;
	bool isMaximal = true;
	/******************************************************************
	step2: check if this tree is closed
	******************************************************************/	
	while ( true )
	{
		/******************************************************************
		step2.1: if from the previous step, there are common legs, then not closed
		******************************************************************/	
		if ( commonLegs.size() != 0 )
		{
			isClosed = false;
			isMaximal = false;
			break;
		}

		/******************************************************************
		step2.2: get the list of parents of the first tid
		******************************************************************/	
		vector< vector<short> > candidateParent;
		vector<short> parentPair(2,0); //parentEdge, then parentVertex
		sameParent = true;

		long m = 0;
		long n = 0;
		long tempTid = occList[0].tid;
		while ( m < occList.size() && occList[m].tid == tempTid )
		{
			if ( occList[m].nodeIndex[0] != 1 )	
			//if the first occurrence's root
			//is not the root of the transaction
			{
				t = database[occList[m].tid]->adj[occList[m].nodeIndex[0]];
				while ( t->next != 0 ) t = t->next;
				parentPair[0] = t->eLabel;
				parentPair[1] = database[occList[m].tid]->vertexLabel[t->v];
				candidateParent.push_back(parentPair);
			}
			m++;
		}
		//now candidateParent holds all possible parents

		/******************************************************************
		step2.3: use other transactions to compute the intersections of parents
		******************************************************************/
		if ( candidateParent.size() == 0 )
		{
			sameParent = false;
		}
		else
		{
			while ( m < occList.size() && candidateParent.size() != 0 )
			{
				n = m;
				short tempEdge, tempVertex;
				while ( n < occList.size() && occList[n].tid == occList[m].tid )
					n++;
				n--;

				vector < vector<short> >::iterator pos1;

				for ( pos1 = candidateParent.begin(); pos1 != candidateParent.end(); )
				{
					bool tempFlag = false;
					for ( long s = m; s <= n; s++ )
					{
						if ( occList[s].nodeIndex[0] != 1 )
						{
							t = database[occList[s].tid]->adj[occList[s].nodeIndex[0]];
							while ( t->next != 0 ) t = t->next;
							tempEdge = t->eLabel;
							tempVertex = database[occList[s].tid]->vertexLabel[t->v];

							if ( tempEdge == (*pos1)[0] && tempVertex == (*pos1)[1] )
							{
								tempFlag = true;
								break; //break the for loop: for ( s = m; ... )
							}
						}
					}

					if ( tempFlag == true ) ++pos1;
					else pos1 = candidateParent.erase(pos1);
				}

				m = n+1;
			}//end of while ( m < ... )
		}

		//parent-closed-checking
		if ( candidateParent.size() == 0 )
			sameParent = false;

		if ( sameParent == true )
		{
			isClosed = false;
			isMaximal = false;
			break;
		}

		/******************************************************************
		step2.4: get the list of legs of the first tid
		******************************************************************/	
		commonLegs.clear();

		m = 0;
		n = 0;
		tempTid = occList[0].tid;
		while ( n < occList.size() && occList[n].tid == tempTid )
			n++;
		n--;
		for ( short i = 1; i <= vCount; i++ )
		{
			for ( long s = m; s <= n; s++ )
			{
				neighbors.clear();
				t = adj[i];
				while ( t != 0 ) //insert index of all neighbors of the position i
				{
					neighbors.insert(occList[s].nodeIndex[t->v - 1]);//inconsistency on index
					t = t->next;
				}
			
				t = database[occList[s].tid]->adj[occList[s].nodeIndex[i-1]];
				while ( t != 0 )
				{
					if ( occList[s].nodeIndex[i-1] < t->v )
					{
						pos = neighbors.find( t->v );
						if ( pos == neighbors.end() ) //if the vertex hasn't been used
						{
							legTriple[0] = i;
							legTriple[1] = t->eLabel;
							legTriple[2] = database[occList[s].tid]->vertexLabel[t->v];
							commonLegs.push_back(legTriple);
						}
					}
					t = t->next;
				}//end of while ( t != 0 )
			}//end of for ( long s = m; ... )
		}
		//now commonLegs stores all possible new legs


		/******************************************************************
		step2.5: using other transactions to prune commonLegs
		******************************************************************/	
		m = n+1; //next tid
		while ( m < occList.size() && commonLegs.size() != 0 )
		{		
			n = m+1;
			while ( n < occList.size() && occList[n].tid == occList[m].tid )
				n++;
			n--; //now from m to n are the occurrences sharing the same tid

			vector<bool> isFetched(vCount + 1, false);
			vector<vector<short> > tupleLegs(0);
			vector<short> legEVPair(2);
			vector<vector<short> >::iterator pos1;

			for ( pos1 = commonLegs.begin(); pos1 != commonLegs.end(); )
			{
				vector<short> thisTriple = *pos1; //get the next commonLeg
			
				short i = thisTriple[0]; //the index of vertex
				//assuming the indices in the commonLegs are non-decreasing

				if ( !isFetched[i] ) //fetch all neighbors of the vertex in the
									//corresponding database transaction
				{
					tupleLegs.resize(0);
					for ( long s = m; s <= n; s++ )
					{
						neighbors.clear();
						t = adj[i];
						while ( t != 0 ) //insert index of all neighbors of the position i
						{
							neighbors.insert(occList[s].nodeIndex[t->v - 1]);//inconsistency on index
							t = t->next;
						}
						t = database[occList[s].tid]->adj[occList[s].nodeIndex[i-1]];
						while ( t != 0 )
						{
							if ( occList[s].nodeIndex[i-1] < t->v )
							{
								pos = neighbors.find( t->v );
								if ( pos == neighbors.end() ) //if the vertex hasn't been used
								{
									legEVPair[0] = t->eLabel;
									legEVPair[1] = database[occList[s].tid]->vertexLabel[t->v];
									tupleLegs.push_back(legEVPair);
								}
							}
							t = t->next;
						}
					}//end of for ( long s = m; ... )
					isFetched[i] = true;
				}

				bool isFound = false;
				for ( short k = 0; k < tupleLegs.size(); k++ )
				{
					if ( thisTriple[1] == tupleLegs[k][0] && thisTriple[2] == tupleLegs[k][1] )
					{
						isFound = true;
						break;
					}
				}

				if ( !isFound )
				{
					pos1 = commonLegs.erase(pos1);
				}
				else
				{
					++pos1;
				}
			}

			if ( commonLegs.size() == 0 ) break;

			m = n+1;
		}//end of while ( m < ... )

		if ( commonLegs.size() != 0 )
		{
			isClosed = false;
			isMaximal = false;
			break;
		}
		break;
	}//end of while at the very beginning of step2

	if ( isClosed == true ) closed[vCount]++;


	/******************************************************************
	step3: main loop, for each position, explore
	******************************************************************/	
	for ( short j = 0; j < positionToExplore.size(); j++ )
	{
		short i = positionToExplore[j];
		//step3_1: get the range of valid legs
		short minEdge = edgeRange[i];
		short minVertex = vertexRange[i];

		//if there is no possible leg at this position
		if ( minEdge > MAX_EDGE ) continue; //continue the for loop

		//if there is no frequent 2-tree starting from this vertex label
		if ( headIndex[vertexLabel[i] - MIN_VERTEX] == 0 ) continue;

		//step3_2: get the possible frequent legs
		vector<bool> isFrequent( (MAX_EDGE - MIN_EDGE + 1) 
								*(MAX_VERTEX - MIN_VERTEX + 1), false);
		for (short j = headIndex[vertexLabel[i] - MIN_VERTEX]; 
			  (j < frequent2Tree.size() && frequent2Tree[j][0] == vertexLabel[i]); j++ )
			isFrequent[( frequent2Tree[j][1] - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 )
				+ ( frequent2Tree[j][2] - MIN_VERTEX )] = true;


		//step2_3: explore each potential leg
		Occurrence tempOcc;
		vector<SupportNode> potential((MAX_EDGE - MIN_EDGE + 1) 
									  *(MAX_VERTEX - MIN_VERTEX + 1));

		for ( long s = 0; s < occList.size(); s++ )
		{
			neighbors.clear();
			t = adj[i];
			while ( t != 0 ) //insert index of all neighbors of the position i
			{
				neighbors.insert(occList[s].nodeIndex[t->v - 1]);//inconsistency on index
				t = t->next;
			}
			t = database[occList[s].tid]->adj[occList[s].nodeIndex[i-1]];
			while ( t != 0 )
			{
				if ( occList[s].nodeIndex[i-1] < t->v )
				{
					pos = neighbors.find( t->v );
					if ( pos == neighbors.end() ) //if the vertex hasn't been used
					{
						short tempE = t->eLabel;
						short tempV = database[occList[s].tid]->vertexLabel[t->v];
						short location = ( tempE - MIN_EDGE ) * ( MAX_VERTEX - MIN_VERTEX + 1 ) 
							+ ( tempV - MIN_VERTEX ); 
						if ( ((tempE > minEdge) || (tempE == minEdge && tempV >= minVertex)) &&
							isFrequent[location] ) //if the leg is potentially frequent
						{
							tempOcc = occList[s];
							tempOcc.nodeIndex.push_back(t->v);
							potential[location].occList.push_back(tempOcc);
							if ( tempOcc.tid != potential[location].lastTid )
							{
								potential[location].lastTid = tempOcc.tid;
								potential[location].support++;
							}
						}
					}
				}
				t = t->next;
			}//end of while ( t != 0 )
		}//end of for ( s = 0; ...)

		for ( long s = 0; s < potential.size(); s++ )
		{
			if ( potential[s].support >= threshold )
			{
				isMaximal = false; //this tree cannot be maximal 
				short tempE = MIN_EDGE + (short)(floor(s/(MAX_VERTEX - MIN_VERTEX + 1)));
				short tempV = MIN_VERTEX + (s % (MAX_VERTEX - MIN_VERTEX + 1));
				RDFCFTree *pbfcf = new RDFCFTree(*this,tempE,tempV,i);
				pbfcf->closedExtensionExplore(frequency, outFile, database,potential[s].occList,
							   frequent2Tree,headIndex,threshold, checked, closed, maximal);
				delete pbfcf;
			}
		}

		////test
		//cout << "leg position is: " << i << " vertex label is: " << vertexLabel[i] << endl;
		//cout << "min edge and min vertex are: " << minEdge << ' ' << minVertex << endl;
		//for ( j = 0; j < isFrequent.size(); j++ )
		//	cout << isFrequent[j] << ' ';
		//cout << endl;
		//cout << endl;

	}//end of for(short j = ...)

	/******************************************************************
	step4: check if this tree is maximal
	******************************************************************/	
	/******************************************************************
	step4.1: if determined from the previous step not maximal
	******************************************************************/	
	if ( isClosed == false || isMaximal == false) return;

	/******************************************************************
	step4.2: check the frequent parents
	******************************************************************/	
	vector<long> tempVector(MAX_VERTEX-MIN_VERTEX+1,0);
	vector < vector <long> > countingMatrix(MAX_EDGE-MIN_EDGE+1,tempVector);

	long m = 0;
	long n = 0;
	while ( m < occList.size() )
	{
		n = m+1;
		while ( n < occList.size() && occList[n].tid == occList[m].tid ) 
			n++;
		n--;		
		set<pair<short,short> > parentEVPairs;
		short tempEdge, tempVertex;
		for ( long s = m; s <= n; s++ )
		{
			if ( occList[s].nodeIndex[0] != 1 )	
			//if the first occurrence's root
			//is not the root of the transaction
			{
				t = database[occList[s].tid]->adj[occList[s].nodeIndex[0]];
				while ( t->next != 0 ) t = t->next;
				tempEdge = t->eLabel;
				tempVertex = database[occList[s].tid]->vertexLabel[t->v];
				parentEVPairs.insert(make_pair(tempEdge,tempVertex));
			}
		}

		set<pair<short,short> >::iterator pos2;
		for ( pos2 = parentEVPairs.begin(); pos2 != parentEVPairs.end(); ++pos2 )
			countingMatrix[pos2->first - MIN_EDGE][pos2->second - MIN_VERTEX]++;
		m = n+1;
	}//end of while ( m < ... )

	bool tempFlag = false;
	for ( short i = 0; i < MAX_EDGE-MIN_EDGE+1; i++ )
	{
		if ( tempFlag == false )
		{
			for ( short j = 0; j < MAX_VERTEX - MIN_VERTEX+1; j++ )
			{
				if ( countingMatrix[i][j] >= threshold )
				{
					tempFlag = true;
					break;
				}

			}
		}
		else
			break;
	}

	if ( tempFlag == true ) //not maximal
	{
		isMaximal = false;
		return;
	}

	/******************************************************************
	step4.3: check the frequent new legs, at any place
	******************************************************************/	
	for ( short i = 1; i <= vCount; i++ )
	{
		vector<long> tempVector2(MAX_VERTEX-MIN_VERTEX+1,0);
		vector < vector <long> > countingMatrix2(MAX_EDGE-MIN_EDGE+1,tempVector2);

		long m = 0;
		long n = 0;
		while ( m < occList.size() )
		{
			n = m+1;
			while ( n < occList.size() && occList[n].tid == occList[m].tid ) 
				n++;
			n--;		
			set<pair<short,short> > legEVPairs;
			short tempEdge2, tempVertex2;


			for ( long s = m; s <= n; s++ )
			{
				neighbors.clear();
				t = adj[i];
				while ( t != 0 ) //insert index of all neighbors of the position i
				{
					neighbors.insert(occList[s].nodeIndex[t->v - 1]);//inconsistency on index
					t = t->next;
				}
				t = database[occList[s].tid]->adj[occList[s].nodeIndex[i-1]];
				while ( t != 0 )
				{
					if ( occList[s].nodeIndex[i-1] < t->v )
					{
						pos = neighbors.find( t->v );
						if ( pos == neighbors.end() ) //if the vertex hasn't been used
						{
							tempEdge2 = t->eLabel;
							tempVertex2 = database[occList[s].tid]->vertexLabel[t->v];
							legEVPairs.insert(make_pair(tempEdge2,tempVertex2));
						}
					}
					t = t->next;
				}
			}//end of for ( long s = m; ... )

			set<pair<short,short> >::iterator pos2;
			for ( pos2 = legEVPairs.begin(); pos2 != legEVPairs.end(); ++pos2 )
				countingMatrix2[pos2->first - MIN_EDGE][pos2->second - MIN_VERTEX]++;
			m = n+1;
		}//end of while ( m < ... )

		bool tempFlag2 = false;
		for ( short k = 0; k < MAX_EDGE-MIN_EDGE+1; k++ )
		{
			if ( tempFlag2 == false )
			{
				for ( short j = 0; j < MAX_VERTEX - MIN_VERTEX+1; j++ )
				{
					if ( countingMatrix2[k][j] >= threshold )
					{
						tempFlag2 = true;
						break;
					}

				}
			}
			else
				break;
		}

		if ( tempFlag2 == true ) //not maximal
		{
			isMaximal = false;
			return;
		}
	}//end of for ( short i ... )

	if ( isMaximal == true ) maximal[vCount]++;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	operator<()

	Decription: compare two trees in DFCF
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool RDFCFTree::operator<(const RDFCFTree& rhs) const
{
	return canonicalString < rhs.canonicalString;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	operator>>()

	Decription: read in DFCF trees
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
istream& operator>>(istream& in, RDFCFTree& rhs)
{
  long t;	//tid
  short v;	//number of vertices
  short p1, p2;
  short vLabel, eLabel;

  in >> t;
  in >> v;

  rhs.adj.resize(v+1);
  rhs.vertexLabel.resize(v+1);
  rhs.degree.resize(v+1);

  rhs.level.resize(v+1);
  rhs.automorphism.resize(v+1);

  for (short i = 0; i <= v; i++)
  {
	rhs.adj[i] = 0;
	rhs.degree[i] = 0;
  }

  rhs.tid = t;
  rhs.vCount = v;
  rhs.vertexLabel[0] = 0;	//null
  for (short i = 1; i <= v; i++)
  {
	in >> vLabel;
	rhs.vertexLabel[i] = vLabel;
  }
	  
  for (short i = 1; i < v; i++) //the number of edges is v-1
  {
	in >> eLabel;
	in >> p1;
	in >> p2;
	rhs.insertEdge(Edge(p1,p2,eLabel));
  }
return in;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	operator<<()

	Decription: write out DFCF trees
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
ostream& operator<<(ostream& out, const RDFCFTree& rhs)
{
  out << "Tid: " << rhs.tid << endl;
  out << "Vertices Number: " << rhs.vCount << endl;
  for ( short i = 1; i <= rhs.vCount; i++ )
    {
		out << rhs.vertexLabel[i] << ' ' 
		<< rhs.degree[i] << ':';
		TnodeLink n = rhs.adj[i];
		while ( n != 0 ) 
		{
			out << "--" << n->v << '_' << n->eLabel << ' ';
			n = n->next;
		}
		out << endl;
    }
  out << "automorphism: ";
  for ( short i = 1; i <= rhs.vCount; i++ ) out << rhs.automorphism[i] << ' ';
  out << endl;
  out << "canonical string: ";
  for ( short i = 0; i < rhs.canonicalString.size(); i++ ) out << rhs.canonicalString[i] << ' ';
  out << endl;
  return out;
}  


