/*******************************
通常の木クラスをAttributeTreeに直す
同じラベルを持つノードが現れた際に最初に現れた子以下を削除する手法と
マージする手法がある．
 *******************************/
#include<iostream>
#include"../trees/tree.h"
#include"../trees/file_io.h"
string CSLOGS_FILE = "cslogs.asc";
string CUT_OUTPUT_FILE = "cut_cslogs.txt";
string MERGE_OUTPUT_FILE = "merged_cslogs.txt";
TreeDB* getCSLogs();
void write_to_file_att_tree(TreeDB* db,string filename);


int main(){
  TreeDB* d = getCSLogs();
  int before = d->get_num_of_nodes();
  cout << "before : " << before << endl;
  d->to_attribute_tree_by_cutting();
  int after = d->get_num_of_nodes();
  cout << "after : " << after << endl;
  write_to_file_att_tree(d,CUT_OUTPUT_FILE);
  return 0;
}

TreeDB* getCSLogs(){
  vector<string> cs_string = read_cslogs();
  TreeDB* cs = new TreeDB();
  for(int i = 0 , n = cs_string.size();i<n;i++){
    cs->add_tree(make_tree(cs_string[i]));
  }
  

  return cs;
}


void write_to_file_att_tree(TreeDB* db,string filename){
  ofstream output_file(filename);
  for(int i = 0,n = db->get_num_of_trees();i < n; i++){
    output_file << db->get_tree(i)->get_tree_string();
    output_file << "\n";
  }
  output_file.close();
}
