#include<iostream>
#include<time.h>
//#include<resource.h>
#include "../trees/SCC_Miner.cpp"
#include "../trees/tree.h"
#include "../trees/file_io.h"
using namespace std;

string ATT_TREE_FILE = "cut_cslogs.txt";
string TEST = "test.txt";
int MIN_SUP = 5;
string CONSTRAINED_SUBTREE = "2603 4768";
int main(){
  TreeDB* tdb = get_treedb_from_file(ATT_TREE_FILE);
  //TreeDB* tdb = get_treedb_from_file(TEST);
  EnumerationTree* subtree= (EnumerationTree*) make_tree(CONSTRAINED_SUBTREE);
  
  vector<EnumerationTree*> result_of_scc = SCC_Miner(tdb,subtree,MIN_SUP);
  cout << "SCC_Miner end : " << result_of_scc.size() << endl;
  write_final_result(result_of_scc, "result_of_scc.txt");


  vector<EnumerationTree*> result_of_scc_improved = SCC_Miner_Improved(tdb,subtree,MIN_SUP);
  cout << "SCC_Miner_Improved end : " << result_of_scc_improved.size() << endl;
  write_final_result(result_of_scc_improved, "result_of_scc_improved.txt");
  
  vector<EnumerationTree*> result_of_scc_path = SCC_Path_Miner(tdb,subtree,MIN_SUP);
  cout <<"SCC_Path_Miner end : " << result_of_scc_path.size() << endl;
  write_final_result(result_of_scc_path, "result_of_scc_path.txt");
  
  return 0;

}
