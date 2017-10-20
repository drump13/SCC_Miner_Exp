#include<iostream>
#include<random>
#include"../trees/tree.cpp"
using namespace std;

EnumerationTree*  make_artificial_tree(int n,int b);

int main(){
  cout << "hello generator " << endl;
  make_artificial_tree(10,10);
  return 0;
}


/*
@brief ノード数と平均次数からその条件を満たした木を人工的に作り返す
@param n その木のノード数
@param b その木の平均次数
@detail
次数の分布は[1,2b-1]の一様分布を用いる
 */
EnumerationTree* make_artificial_tree(int n ,int b){
  random_device rnd;
  uniform_int_distribution<> randomIntDistribution(1,2*b-1);
  for(int i = 0 ; i < n ; i ++){
    cout << randomIntDistribution(rnd) << endl;
  }
  return NULL;
}
