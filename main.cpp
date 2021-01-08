#include "CAST256.h"
#include <cstring>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <random>
using namespace std; 
namespace fs = filesystem;


long fileSize(char* file) {
    ifstream stream(file, ios_base::binary);
    long cur_pos, length;
    cur_pos = stream.tellg();
    stream.seekg(0,ios_base::end);
    length = stream.tellg();
    stream.seekg(0,ios_base::beg);
    stream.close();
    return length;
}

int Interface(char *argv[]){
    if (string(argv[1]) == "-e"){
        return 1;
    }
    else if (string(argv[1]) == "-d"){
        return 2;
    }
    else{
        return 0;
    }
}

void generateIV(char* iv){
	for(int i = 0; i < 16; i++){
		iv[i] = rand()%255; 
	}
}
// argv[1] - mod e/d
// argv[2] - input file/dir or file for decrypt 
// argv[3] - key 
// argv[4] - IV, if iv is not found, the program will create iv and write it to IV.txt
// argv[5] - output file 

int main(int argc, char* argv[]){
	srand(time(NULL));
	if(argc < 4 && argc > 7){
		throw runtime_error("Error: wrong arguments\n\t   Example: <programm> -<mod(e/d)> <input file> <key file> <IV file> <output file>");
	}
	int interface = Interface(argv);
	ifstream temp1(argv[3],ios::binary);
	if(!temp1.is_open()){
		throw runtime_error("Error: Key file not found");
	}
	char strkey[17] = {0};
	temp1.read(strkey, 16);
	temp1.close(); 
	temp1.open(argv[4],ios::binary);
	char strIV[17] = {0};
	if(!temp1.is_open()){
		if(interface == 1) {
			cout << "Error: IV file not found, generate IV ? [y/n] ";
			string menu; 
			cin >> menu;
			if(menu == "y"){
				generateIV(strIV);
				ofstream ivset("./IV.txt", ios::binary);
				ivset << string(strIV, 16);
				ivset.close(); 
			} else {
				cout << "Fail\n";
				exit(1);
			}
		} else {
			throw runtime_error("Error: IV file not found");
		}
	} else{
		temp1.read(strIV, 16);
	}
	temp1.close();
	vector<string> files;
	vector<string> dirs;  
	vector<long> size;
	CAST256 cast6 = CAST256(string(strkey, 16), string(strIV, 16));
	if(interface == 1){
		auto path1 = fs::path(argv[2]);
		if(!fs::exists(path1)){
			throw runtime_error("Error: File/directory not found");
		} 
		if(fs::is_directory(path1)){    
			for (auto & p : fs::recursive_directory_iterator(path1)){
				if(!p.is_directory()){
					bool fv = true; 
					for(int i = 1; i < argc; i++){
						if(p.path().generic_string() == string(argv[i]) || p.path().generic_string() == ("./" + string(argv[i]))){
						 	fv = false;
						}
					}		
					if (fv){
						files.push_back(p.path().c_str());
						size.push_back(p.file_size());
					}
				} else {
					dirs.push_back(p.path().c_str());
				}
			}
		} else {
			files.push_back(path1.c_str());
			size.push_back(fileSize((char*)(path1.c_str())));
		}
		
		ofstream output(argv[5]);
		for (auto i: dirs)    
			output << i << endl; 
		output <<  "\n";
		for (auto i: files)    
			output << i << endl;
		output << "\n";
		for (auto i: size)
			output << i << endl;
		output << "\n";
		output.close(); 
		for(int i = 0; i < files.size(); i++)
			cast6.encryptFile((char *)files[i].c_str(), argv[5], size[i]);
	} else if(interface == 2){		
		ifstream input(argv[2]);
		if(!input.is_open()){
			throw runtime_error("Error: File/directory not found");
		}
		char temp[256];
		int counter = 0; 
		long cur_pos = 0;
		while(input.getline(temp, 255)){
			if((int)temp[0] != 0) {
				switch (counter)
				{
				case 0:
					dirs.push_back(temp);
					break;
				case 1: 
					files.push_back(temp);
					break; 
				case 2:
					size.push_back(atoi(temp));
					break;
				default:
					break;
				} 
			} else {
				counter++;
				if(counter == 3){
					cur_pos = input.tellg();
					break;
				} 
			}

		}
		input.close();
		if(size.size() == 0 || files.size() == 0){
			throw runtime_error("Error: wrong file format");
		}
		for(auto i : dirs)
			fs::create_directory(i.c_str());
		for(int i = 0; i < files.size(); i++){
			cast6.decryptFile(argv[2], (char *)files[i].c_str(), cur_pos, size[i]);
			if(size[i]%16 == 0){
				cur_pos += size[i];
			} else {
				cur_pos += size[i] + (16 - size[i]%16);
			}
			
		}
	} 
	cout << "Done\n";
	return 0;
}