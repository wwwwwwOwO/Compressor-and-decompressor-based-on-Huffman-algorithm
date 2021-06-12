#include "Huffman.hpp"
#define std_extension "huff"    //自定义压缩文件的拓展名

string change_extension(const string& filename, const string& extension){
    size_t p = filename.find_last_of('.');
    string str = filename.substr(0, p) + "." + extension;

    return str;
}

int main(){
    string input_filename, output_filename, ex;
    int select;
    do{
        cout << "************************************************************************************" << endl;
        cout << "*                                1: Compress                                       *"<< endl;
        cout << "*                                2: Uncompress                                     *" << endl;
        cout << "*                                0: Exit                                           *" << endl;
        cout << "************************************************************************************" << endl;
        cout << "   Your choice:" << endl;
        cin >> select;

        switch(select){
            case 1:
                cout << "Please enter the name of the file you want to compress:" << endl;
                cin >> input_filename;

                //压缩文件名和原始文件名相同，拓展名为自定义的拓展名
                output_filename = change_extension(input_filename,std_extension);

                //压缩过程
                //汇报压缩信息
                if(compress(input_filename,output_filename)){
                    cout << "Compress successfully!" << endl;
                    cout << "Size of input file: " << get_filesize(input_filename) << endl;
                    cout << "Size of output file: " << get_filesize(output_filename) << endl;
                    if(get_filesize(input_filename)>0)
                        cout << "Compress ratio: " << ((double)get_filesize(output_filename) / (double)get_filesize(input_filename)) * 100 << "%" << endl;
                }                    
                else
                {
                    cout << "Fail to compress." << endl;
                }
                
                break;
            case 2:
                cout << "Please enter the name of the file you want to uncompress:" << endl;
                cin >> input_filename;

                //拓展名和自定义拓展名不同的文件不能解压
                if(input_filename.substr(input_filename.find_last_of('.')+1) != std_extension){
                    cout << "This kind of file can not be uncompressed!" << endl;
                    break;
                }

                //输入要解压文件的格式
                cout << "Please enter the extension of the output file:" << endl;
                cin >> ex;
                output_filename = change_extension(input_filename,ex);

                //解压过程
                //汇报解压信息
                if(uncompress(input_filename,output_filename)){
                    cout << "Uncompress successfully!" << endl;
                    cout << "Size of input file: " << get_filesize(input_filename) << endl;
                    cout << "Size of output file: " << get_filesize(output_filename) << endl;
                }                    
                else
                {
                    cout << "Fail to uncompress." << endl;
                }
                
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
        input_filename.clear();
        output_filename.clear();

        cout << endl;
    } while (select);

    cout << "Exit successfully!" << endl;
    system("pause");
    return 0;
}