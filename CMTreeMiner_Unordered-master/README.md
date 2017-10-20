CMTreeMiner Unordered: an algorithm for mining closed and maximal frequent rooted unordered trees

Citation information:

[1]   Yun Chi, Yirong Yang, Yi Xia, and Richard R. Muntz. Cmtreeminer: Mining both closed and maximal frequent subtrees. In PAKDD'04: Proceedings of the 8th Pacific-Asia in Knowledge Discovery and Data Mining, pages 63-73, Sydney, Australia, 2004. Springer.

[2]	 Yun Chi, Yi Xia, Yirong Yang, and Richard R. Muntz. Mining closed and maximal frequent subtrees from databases of labeled rooted trees. IEEE Trans. Knowl. Data Eng., 17(2):190-202, 2005.

1) The input should be text file in the following format for each transaction:

tid_1

number_of_nodes

node1_label

...

noden_label

edge1_label connect_from_which_node connect_to_which_node

...

edge(n-1)_label connect_from_which_node connect_to_node

2) All transactions are put one after one in the text file. A sample database "example_rooted.txt" is given. Notice that the order of edges are given should be in such a way that the tree is always connected as edges are added one by one.

3) Usage: CMUnorderedTreeMiner support input_file output_file
where support is an integer, i.e., minimal number of occurrence

4) Output file contains the running time. It also contains the number of trees that have been checked by the algorithm, the number of closed trees, and the number of maximal trees, for each tree size.
