#include <iostream>
#include <map>
#include <bitset>
#include <string>
#include <cstring>
#include <queue>
#include <list>
#include <fstream>
#include <iomanip>
using namespace std;

struct tree_node{
    unsigned char name;
    tree_node *left, *right;

    size_t weight;
    string code;

    tree_node(unsigned char name_, tree_node *left_, tree_node *right_,  size_t weight_, string code_){
        name = name_;//字符
        left = left_;
        right = right_;
        weight = weight_;//权重（字符出现频率）
        code = code_;//字符对应的Huffman编码的string形式
    }
};
//统计文件长度，计算各字符出现的频率，返回文件长度，字符到频率的映射存储在char_weight中
size_t char_account(ifstream &input, map<char, size_t> &char_weight);
//跟据字符到频率的映射构建半哈夫曼树，返回树的根节点,此时哈夫曼树未编码
tree_node* built_tree(map<char, size_t> &char_weight);
//销毁树
void destroy_tree(tree_node *);
//根据半哈夫曼树（节点未编码）得到完整的哈夫曼树 （在结点处得到字符到它对应的哈夫曼编码和映射
void encoder(tree_node *root, map<char, string> &dictionary);
//压缩过程，压缩成功返回TRUE
bool compress(const string &source_filename, const string &encoded_filename);
//解压过程，压缩成功返回TRUE
bool uncompress(const string &encoded_filename, const string &source_filename);
//统计文件长度
size_t get_filesize(const string &filename);