#include "CAST256.h"
#include <cstring>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
using namespace std; 
namespace fs = filesystem;
int main(){
	string key = "qwertyuiopasdfghjklzxcvbnmqwerty";
	string iv = "qwertyuiopasdfge";
	vector<string> files;
	vector<string> dirs;  
	auto path1 = fs::path("./");
	if(!fs::exists(path1)){
		throw runtime_error("Error: File/directory not found");
	} 
	if(fs::is_directory(path1)){    
		for (auto & p : fs::recursive_directory_iterator(path1)){
			if(!p.is_directory()){
				files.push_back(p.path().c_str());
			} else {
				dirs.push_back(p.path().c_str());
			}
		}
	} else {
		files.push_back(path1.c_str());
	}
	
	ofstream asd("./info.txt");
    for (auto i: dirs)    
		asd << i << endl; 
	asd <<  "\n";
	for (auto i: files)    
		asd << i << endl;
	asd.close(); 
	dirs.clear();
	files.clear();

	ifstream asd1("./info.txt");
	char temp[256];
	bool counter = false; 
	while(asd1.getline(temp, 255)){
		if(!counter && (int)temp[0] != 0){
			dirs.push_back(temp);
		} else {
			files.push_back(temp);
		}
		if((int)temp[0] == 0){
			if(counter == true){
				break; 
			}
			counter = true; 
		}
	}
	for (auto i: dirs)    
		cout  << i << endl; 
	cout <<  "\n";
	for (auto i: files)    
		cout << i << endl;
	cout <<  "\n";
	CAST256 test = CAST256(key, iv);
	cout << test.encryptFile("./asd.txt", "./Encrypted.txt") << "\n";
	cout << test.decryptFile("./Encrypted.txt", "./Decrypt.txt") << "\n";
	return 0;
}