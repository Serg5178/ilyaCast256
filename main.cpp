#include "CAST256.h"
#include <cstring>
int main(){
	std::string key = "qwertyuiopasdfghjklzxcvbnmqwerty";
	std::string iv = "qwertyuiopasdfge";
	CAST256 test = CAST256(key, iv);
	//std::cout << test.decrypt(test.encrypt("Hello, world!!!L")) << "\n";
	std::cout << test.encryptFile("./asd.txt", "./Encrypted.txt") << "\n";
	std::cout << test.decryptFile("./Encrypted.txt", "./Decrypt.txt") << "\n";
	return 0;
}
