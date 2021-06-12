#include "Huffman.hpp"

//统计文件长度，计算各字符出现的频率，返回文件长度，字符到频率的映射存储在char_weight中
size_t char_account(ifstream &input, map<char, size_t> &char_weight){
    size_t sizes = 0;
    map<char, size_t>::iterator it;

    unsigned char buf = 0;
    input.read((char *)&buf, 1);


    while (!input.eof())
    {
        it = char_weight.find(buf);

        //若字符尚未出现过则插入映射表，否则出现次数+1
        if (it != char_weight.end())
            it->second++;
        else
            char_weight.insert(map<char, size_t>::value_type(buf, 1));

        sizes++;
        input.read((char *)&buf, 1);
        
        }

    return sizes;
}


tree_node *built_tree(map<char, size_t> &char_weight){
    //声明一个链表，下面将树结点按权值升序存储
    list<tree_node*> order;
    list<tree_node*>::iterator it;
    map<char, size_t>::iterator i;
    tree_node *newnode;
    for ( i = char_weight.begin(); i != char_weight.end(); i++){
        newnode = new tree_node(i->first, nullptr, nullptr, i->second, "");

        //插入，保证order中的树结点按权值升序排序
        for (it = order.begin(); it != order.end();it++){
            if (i->second < (*it)->weight)
            {
                order.insert(it, newnode);
                break;
            }
        }
        if (it == order.end())
            order.push_back(newnode);
    }

    //用于森林中最小的两棵树
    tree_node *smallest1, *smallest2;
    //构建哈夫曼树
    while(order.size()>1){
        //将权值最小的两棵树取出作为左、右孩子构建新树， 新树的权值为两个孩子权值之和
        smallest1 = order.front();
        order.pop_front();
        smallest2 = order.front();
        order.pop_front();
        newnode = new tree_node('*', smallest1, smallest2, smallest1->weight + smallest2->weight, "");

        //将新树的根节点插入order，同时保持order是升序的
        for (it = order.begin(); it != order.end();it++){
            if(newnode->weight<(*it)->weight){
                order.insert(it, newnode);
                break;
            }
        }
        if(it==order.end())
        order.push_back(newnode);
    }
    return order.front();
}

void destroy_tree(tree_node *root){
    if(root!=nullptr){
        destroy_tree(root->left);
        destroy_tree(root->right);
        delete root;
    }
    return;
}


void encoder(tree_node *root, map<char, string> &dictionary){
    static bool f = false;
    if(root==nullptr)
        return;
    //该节点是叶节点
    if(root->left==nullptr&&root->right==nullptr){
        dictionary.insert(map<char, string>::value_type(root->name, root->code));
    }
    //节点是内部节点
    else{
        f = true;//进入该循环说明不是只有一个节点huffman树
        root->left->code = root->code + "0";
        root->right->code = root->code + "1";
        encoder(root->left, dictionary);//递归调用
        encoder(root->right, dictionary);
    }
    //文件中只有一种类型的字符串；
    if(!f)
        dictionary.begin()->second = "0";

    return;
}


bool compress(const string &source_filename, const string &encoded_filename){
    map<char, size_t> char_wt;
    map<char, string> encode_dictionary;
    size_t sizes = 0;

    ifstream in(source_filename.c_str(), ios::in | ios::binary);//二进制打开输入文件
    if(!in.is_open()){
        cout << "Fail to open the source file!" << endl;
        return false;
    }
    ofstream out(encoded_filename.c_str(), ios::out | ios::binary);//二进制打开输入文件
    if(!out.is_open()){
        cout << "Fail to open the output file!" << endl;
        return false;
    }

    sizes = char_account(in, char_wt);//统计字符
    out.write((char *)&sizes, 8);//写入原始文件长度

    in.close();
    in.clear();

    tree_node *huffman_tree = built_tree(char_wt);
    encoder(huffman_tree, encode_dictionary);//获得用于压缩的huffman编码

    
    unsigned char dictionary_size = (unsigned char)(encode_dictionary.size());
    out.write((char *)&dictionary_size, 1);//写入解码词典中Huffman编码的个数

    string huffman_code;
    unsigned char buf_index = 0;
    unsigned char buf;
    unsigned char len;
    //table数组 从LSB到MSB依次为1，其他位为0，用于往缓冲区buf逐位写入信息时的位选
    unsigned char table[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

    map<char,string>::iterator it;
    unsigned char i;
    for (it = encode_dictionary.begin(); it != encode_dictionary.end();it++){
        buf = 0;
        huffman_code = it->second;
        len = (unsigned char)(huffman_code.size());

        out.write((char *)&(it->first), 1);// 写入字符的键值
        out.write((char *)&len, 1);//写入编码的长度

        for (i = 0; i < huffman_code.size();i++){
            //将Huffman编码一位一位写入缓冲区buf中
            if(huffman_code[i]=='1')
                buf |= table[buf_index];
            buf_index++;
            //够8位写入压缩文件，当编码长度不少于8位时调用
            if(buf_index==8){
                out.write((char *)&buf, 1);
                buf=0;
                buf_index = 0;
            }

        }
        //最后一个位写完，但是不够一个整八位
        if(buf_index!=0){
            out.write((char *)&buf, 1);
            buf = 0;
            buf_index = 0;
        }
    }

//重新打开输入文件
    in.open(source_filename.c_str(), ios::in | ios::binary);
    char ch;
    buf_index = 0;
    buf = 0;
    in.read((char *)&ch, 1);
    while(!in.eof()){

        it = encode_dictionary.find(ch);//获取字符的huffman编码
        huffman_code = it->second;
        len =(unsigned char) (huffman_code.size());

        //将huffman编码按位一位一位的写入压缩文件
        for (unsigned char i = 0; i < len;i++){
            if(huffman_code[i]=='1')
                buf |= table[buf_index];

            buf_index++;

            //huffman编码凑够八位就按照一个字符写入压缩文件
            if (buf_index == 8)
            {
                out.write((char *)&buf, 1);
                buf = 0;
                buf_index = 0;
            }
        }
        in.read((char *)&ch, 1);
    }
    //最后一个位写完，但是不够一个整八位
    if(buf_index!=0)
        out.write((char*)&buf, 1);


    in.close();
    in.clear();
    out.close();
    out.clear();
    destroy_tree(huffman_tree);

    return true;
}

bool uncompress(const string &encoded_filename, const string &source_filename){

    ifstream in(encoded_filename.c_str(), ios::in | ios::binary);
    if(!in.is_open()){
        cout << "Fail to open the encoded file!" << endl;
        return false;
    }

    ofstream out(source_filename.c_str(), ios::out | ios::binary);
    if(!out.is_open()){
        cout << "Fail to open the output file!" << endl;
        return false;
    }

    size_t  sizes= 0;
    in.read((char *)&sizes, 8);

//首先从压缩文件的其实字符串得到文件的解码词典 之后便可根据解码词典进行解码
    int dictionary_size = 0;
    unsigned char temp;
    in.read((char *)&temp, 1);//解码字典的长度
    //编码词典最多256个字符，由于unsigned char的表示范围为0~255，因此当解码词典个数为256时temp=0
    dictionary_size = temp ? (int)temp : 256;

    map<string, char> decode_dictionary;
    char ch;
    unsigned char buf;
    unsigned char len;
    string huffman_code;
    unsigned char buf_index;
    unsigned char i;
    //table数组 从LSB到MSB依次为1，其他位为0，用于往缓冲区buf逐位写入信息时的位选
    unsigned char table[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    for (int j = 0; j < dictionary_size;j++){
        buf = 0;
        buf_index = 0;
        in.read((char *)&ch, 1); //读入解码字典的字符
        in.read((char *)&len, 1); //读入当前字符编码的编码长度
        in.read((char *)&buf, 1);
        huffman_code = "";

         //根据长度读出后续的编码01串
        
        for ( i = 0; i < len; i++)
        {
            if (buf & table[buf_index])
                huffman_code += "1";
            else
                huffman_code += "0";

            buf_index++;

            if (buf_index == 8)
            {
                buf_index = 0;

                //若i=len-1表示刚好处理完缓冲区的字符，若没处理完则继续读入
                if(i!=len-1)
                in.read((char *)&buf, 1);
            }
        }
        //将获得的字符对应的编码插入解码器
        decode_dictionary.insert(map<string, char>::value_type(huffman_code, ch));
    }


//根据获得的解码器对文件进行解码
    map<string,char>::iterator it;
    buf_index = 0;
    huffman_code = "";
    //二进制读入压缩文件中的一个字符
    in.read((char *)&buf, 1);

    // 若文件没有结束则按照二进制一个字符一个字符的读入所有内容
    while(!in.eof()){
        if (buf & table[buf_index])
            huffman_code += "1";
        else
            huffman_code += "0";

        buf_index++;

        //前缀编码中，根据编码找到对应的字符，将字符写入解压文件
        it = decode_dictionary.find(huffman_code);
        if (it != decode_dictionary.end())
        {

            ch = it->second;
            out.write((char *)&ch, 1);

            --sizes;
            if(sizes==0){
                in.close();
                in.clear();
                out.close();
                out.clear();
                return true;
            }
                
            huffman_code = "";
        }

        //读入的字符处理完毕，继续读入一个字符
        if (buf_index == 8)
        {
            buf_index = 0;
            in.read((char *)&buf, 1);
        }
    }
        
}

//获取文件长度
size_t get_filesize(const string &filename){
    ifstream in(filename.c_str());
    in.seekg(0, ios::end);

    size_t size = in.tellg();

    in.close();
    in.clear();
    return size;
}