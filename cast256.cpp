#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <stdint.h>
#include <ctime>
#include <unistd.h>
#include <sstream>
#include <functional>
#include "constants.h"
#include <random>
using namespace std;




void define_block(uint8_t*, uint32_t*, int size);

uint32_t f1(uint32_t, uint32_t, uint32_t);

uint32_t f2(uint32_t, uint32_t, uint32_t);

uint32_t f3(uint32_t, uint32_t, uint32_t);

void decrypt(uint32_t subkeys_Kr_Km[2][4][12],
             uint32_t subkeys_decrypt_Kr_km[2][4][12], uint32_t* text);

void set_subkeys_Tr_Tm(uint32_t [2][8][24]);

void func_W(int, uint32_t subkeys[2][8][24], uint32_t*);

void partition(uint32_t, uint8_t*); // функция разделения инта на 4 байта

void set_subkeys_Kr_Km(int round, uint32_t *key, uint32_t subkeys_Kr_Km[2][4][12]);

void encrypt(uint32_t subkeys[2][8][24], uint32_t *key, 
             uint32_t subkeys_Kr_Km[2][4][12], uint32_t* open_text);

void from_int_to_char(int index, uint8_t*, uint32_t*);

long filesize(char* file);

void setVectorInit(uint32_t*, uint8_t*);

int Interface(char *argv[]);

void from_hex_to_string(uint32_t*, char *argv[]);

void test_control(uint32_t subkeys[2][8][24], uint32_t* key, uint32_t subkeys_Kr_Km[2][4][12]);

// argv 2 = что зашифровать/расшифровать
// argv 3 = пароль (ключ)
// argv 4 = как назвать расшифрованный/зашифрованный файл



int main(int argc, char* argv[]){
    uint32_t start_time = clock();
    uint32_t key[8] = {0}; // изначальный ключ

    
    uint32_t open_text[4] = {0}; // то, что мы считали из файла в виде int
    uint32_t subkeys_Kr_Km[2][4][12] = {0}; // подключи Kr, Km
    uint32_t subkeys_decrypt_Kr_km[2][4][12] = {0};
    uint8_t buffer_text_file[17] = {0}; // текст в виде char
    // subkeys[0][24][8] - маскирующие 0
    // subkeys[1][24][8] - сдвига 1
    // Tr / Kr - ключи сдвига
    // Tm // Km - маскирующие
    uint32_t subkeys[2][8][24] = {0}; // подключи Tr, Tm
    uint8_t buffer_IV[16] = {0}; // хранение гаммы в массиве байт для записи в файл
    uint32_t gamma[4] = {0}; // хранение гаммы для операций xor и шифрования
    
    from_hex_to_string(key, argv);
    uint32_t temp[8] = {0}; // переменная для сохранения первоначального вида ключа
    int size;
    bool flag = 0; // 0 - encrypt; 1 - decrypt
    cout << "*********" <<endl;
    switch (Interface(argv)){
        case 1:
            size = filesize(argv[2]);
            break;
        case 2:
            size = filesize(argv[2]) - 16;
            flag = 1;
            break;
        case 3:
            cout << "TEST" << endl;
            test_control(subkeys, key, subkeys_Kr_Km);
            exit(1);
        case 0:
            cout << "EXIT\n";
            exit(1);

    }


    

    // копирование ключа
    for (int i = 0; i < sizeof(key); ++i){
        temp[i] = key[i];
    }
    ifstream in(argv[2], ios::binary);
    if(!in.is_open()){
        cout << "Not such file in this directory";
        exit(1);
    }
    //     // открываем выходной файл
    ofstream out;          // поток для записи
    out.open(argv[4], ios::binary); // окрываем файл для записи


    set_subkeys_Tr_Tm(subkeys);
    for (int i = 0; i < 12; ++i){
        func_W(2*i, subkeys, key);
        func_W(2*i+1, subkeys, key);
        set_subkeys_Kr_Km(i, key, subkeys_Kr_Km);
    }

    if (flag){
        in.read((char*)buffer_IV, 16);
        define_block(buffer_IV, gamma, sizeof(gamma));
    }
    else{
        setVectorInit(gamma, buffer_IV); // генерация псевдослучайной последовательности
        for (int i = 0; i < 16; ++i){
           out << buffer_IV[i];
        }
    }
    define_block(buffer_IV, gamma, sizeof(gamma));
    for (size; size > 0; size-=16 ){
        for (int i = 0; i < 16; ++i){
            buffer_text_file[i] = 0;
        }

        in.read((char*)buffer_text_file, 16);

        define_block(buffer_text_file, open_text, 16);

        encrypt(subkeys, key, subkeys_Kr_Km, gamma);
        for (int i = 0; i < sizeof(gamma); ++i){
            open_text[i] ^= gamma[i];
        }
        for (int i = 0; i < 16; i+=4){
            from_int_to_char(i, buffer_text_file, open_text);
        }
        if (out.is_open()){
            for (int i = 0; i < 16; ++i){
                out << buffer_text_file[i];
            }
        }
    }
    out.close();
    uint32_t end_time = clock();
    cout << "Working time: " << end_time - start_time << endl;
    cout << "Success" << endl;
}

void from_hex_to_string(uint32_t* key, char *argv[]){
  stringstream ss;
  string parts[8];
  int counter = 0;

  for (int i = 0; i < 8; i++){
    try{
        if (argv[3][8*i] == 0){
            throw 1;
        }
       
        parts[i] = string(argv[3], 8*i, 8);
        counter++;
    }
    catch(int a){
        cout << "Key was extend by zero blocks" << endl;
        counter = i;
        break;
    }
  }
  for (int i = 0; i < counter; ++i)
  {
    ss << hex << parts[i];
    ss >> key[i];
    ss.clear();
  }
  cout << endl;


}

int Interface(char *argv[]){
    if (string(argv[1]) == "-e"){
        return 1;
    }
    else if (string(argv[1]) == "-d"){
        return 2;
    }
    else if (string(argv[1]) == "-t"){
        return 3;
    }
    else{
        return 0;
    }
}


void setVectorInit(uint32_t* vector_init, uint8_t *char_VI){
    // ofstream out;
    random_device rnd;
    uniform_int_distribution<uint32_t> distr;
    for (int i = 0; i < 4; i++){
        vector_init[i] = distr(rnd);
    }
    for (int i = 0; i < 16; i+=4){
        from_int_to_char(i, char_VI, vector_init);
    }
}



void setKey(char* str, uint32_t* key_blocks){
    hash<string> hash_func;
    string array[8];
    array[0] = str;

    for(int i = 1; i < 8; ++i){
        array[i] =str[i] + str[i+1];
    }
    for (int j = 0; j < 8; ++j){
        key_blocks[j] = hash_func(array[j]);
    }
   
}

long filesize(char* file) {
    ifstream stream(file, ios_base::binary);
     long cur_pos, length;
     cur_pos = stream.tellg();
     stream.seekg(0,ios_base::end);
     length = stream.tellg();;
     stream.seekg(0,ios_base::beg);
     return length;
}


void from_int_to_char(int index, uint8_t* buffer_char, uint32_t* buffer_int){

    buffer_char[index]   = (buffer_int[index/4]&0xFF000000)>>24;;
    buffer_char[index+1] = (buffer_int[index/4]&0x00FF0000)>>16;
    buffer_char[index+2] = (buffer_int[index/4]&0x0000FF00)>>8;
    buffer_char[index+3] = (buffer_int[index/4]&0x000000FF);
    

}



void define_block(uint8_t* text, uint32_t* open_text, int size){
    for (int i = 0; i < size; i+=4){
        open_text[i/4] =  (text[i]  <<  24)&0xff000000|
                          (text[i+1]<<  16)&0x00ff0000|
                          (text[i+2]<<   8)&0x0000ff00|
                          (text[i+3]<<   0)&0x000000ff;
    }
}




uint32_t f1(uint32_t data, uint32_t mask_key, uint32_t shift_key){
    data = mask_key+data;
    data = (data>>(32 - shift_key)) | (data << shift_key);
    uint8_t parts[4] = {0};
    partition(data, parts);
    // for(int i = 0; i < 4; i++){
    //     cout << (uint8_t)parts[i];
    // }
    // cout << endl;
    data = ((S[0][parts[0]] ^ S[1][parts[1]])- S[2][parts[2]]) + S[3][parts[3]];
    return data;
}


uint32_t f2(uint32_t data, uint32_t mask_key, uint32_t shift_key){
    data = (mask_key ^ data);
    data = (data>>(32 - shift_key)) | (data << shift_key);
    uint8_t parts[4] = {0};
    partition(data, parts);
    data = ((S[0][parts[0]] - S[1][parts[1]]) + S[2][parts[2]]) ^ S[3][parts[3]];
    return data;
}


uint32_t f3(uint32_t data, uint32_t mask_key, uint32_t shift_key){
    data = mask_key - data;
    data = (data>>(32 - shift_key)) | (data << shift_key);
    uint8_t parts[4];
    partition(data, parts);
    data = ((S[0][parts[0]] + S[1][parts[1]]) ^ S[2][parts[2]]) - S[3][parts[3]];
    return data;
}

void set_subkeys_Tr_Tm(uint32_t subkeys[2][8][24]){
    uint32_t Cm = 0x5a827999;
    uint32_t Mm = 0x6ed9eba1;
    uint32_t Cr = 19;
    uint32_t Mr = 17;
    for (int i = 0; i < 24; ++i){
        for (int j = 0; j < 8; ++j){
            subkeys[0][j][i] = Cm;
            Cm = (Cm + Mm);
            subkeys[1][j][i] = Cr;
            Cr = (Cr + Mr) & 0x1f;

        }
    }
 }

void func_W(int round, uint32_t subkeys[2][8][24], uint32_t *key){
    key[6] = key[6] ^ f1(key[7], subkeys[0][0][round], subkeys[1][0][round]);
    key[5] = key[5] ^ f2(key[6], subkeys[0][1][round], subkeys[1][1][round]);
    key[4] = key[4] ^ f3(key[5], subkeys[0][2][round], subkeys[1][2][round]);
    key[3] = key[3] ^ f1(key[4], subkeys[0][3][round], subkeys[1][3][round]);
    key[2] = key[2] ^ f2(key[3], subkeys[0][4][round], subkeys[1][4][round]);
    key[1] = key[1] ^ f3(key[2], subkeys[0][5][round], subkeys[1][5][round]);
    key[0] = key[0] ^ f1(key[1], subkeys[0][6][round], subkeys[1][6][round]);
    key[7] = key[7] ^ f2(key[0], subkeys[0][7][round], subkeys[1][7][round]);
}


void partition(uint32_t data, uint8_t* parts){
    parts[0] = (data&0xFF000000)>>24;;
    parts[1] = (data&0x00FF0000)>>16;
    parts[2] = (data&0x0000FF00)>>8;
    parts[3] = (data&0x000000FF);
}
void encrypt(uint32_t (*subkeys)[8][24], uint32_t *key,
             uint32_t (*subkeys_Kr_Km)[4][12], uint32_t* CT){

    for (int i = 0; i < 6; ++i){
        CT[2] = CT[2] ^ f1(CT[3], subkeys_Kr_Km[0][0][i], subkeys_Kr_Km[1][0][i]);
        CT[1] = CT[1] ^ f2(CT[2], subkeys_Kr_Km[0][1][i], subkeys_Kr_Km[1][1][i]);
        CT[0] = CT[0] ^ f3(CT[1], subkeys_Kr_Km[0][2][i], subkeys_Kr_Km[1][2][i]);
        CT[3] = CT[3] ^ f1(CT[0], subkeys_Kr_Km[0][3][i], subkeys_Kr_Km[1][3][i]);
    }
    for (int i = 6; i < 12; ++i){
        CT[3] = CT[3] ^ f1(CT[0], subkeys_Kr_Km[0][3][i], subkeys_Kr_Km[1][3][i]);
        CT[0] = CT[0] ^ f3(CT[1], subkeys_Kr_Km[0][2][i], subkeys_Kr_Km[1][2][i]);
        CT[1] = CT[1] ^ f2(CT[2], subkeys_Kr_Km[0][1][i], subkeys_Kr_Km[1][1][i]);
        CT[2] = CT[2] ^ f1(CT[3], subkeys_Kr_Km[0][0][i], subkeys_Kr_Km[1][0][i]);
    }
}
void set_subkeys_Kr_Km(int round, uint32_t *key, uint32_t (*subkeys_Kr_Km)[4][12]){
    for (int i = 0; i < 4; ++i){
        subkeys_Kr_Km[0][i][round] = key[7 -i*2]; 
        subkeys_Kr_Km[1][i][round] = key[i*2] &0x1f; // сдвиговые подключи
    }
}



void decrypt(uint32_t subkeys_Kr_Km[2][4][12], uint32_t subkeys_decrypt_Kr_km[2][4][12],
             uint32_t* text){

    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 12; ++j){
            subkeys_decrypt_Kr_km[0][i][j] = subkeys_Kr_Km[0][i][11-j];
            subkeys_decrypt_Kr_km[1][i][j] = subkeys_Kr_Km[1][i][11-j];
        }
    }
    for (int i = 0; i < 6; ++i){
        text[2] = text[2] ^ f1(text[3], subkeys_decrypt_Kr_km[0][0][i], subkeys_decrypt_Kr_km[1][0][i]);
        text[1] = text[1] ^ f2(text[2], subkeys_decrypt_Kr_km[0][1][i], subkeys_decrypt_Kr_km[1][1][i]);
        text[0] = text[0] ^ f3(text[1], subkeys_decrypt_Kr_km[0][2][i], subkeys_decrypt_Kr_km[1][2][i]);
        text[3] = text[3] ^ f1(text[0], subkeys_decrypt_Kr_km[0][3][i], subkeys_decrypt_Kr_km[1][3][i]);
    }
    for (int i = 6; i < 12; ++i){
        text[3] = text[3] ^ f1(text[0], subkeys_decrypt_Kr_km[0][3][i], subkeys_decrypt_Kr_km[1][3][i]);
        text[0] = text[0] ^ f3(text[1], subkeys_decrypt_Kr_km[0][2][i], subkeys_decrypt_Kr_km[1][2][i]);
        text[1] = text[1] ^ f2(text[2], subkeys_decrypt_Kr_km[0][1][i], subkeys_decrypt_Kr_km[1][1][i]);
        text[2] = text[2] ^ f1(text[3], subkeys_decrypt_Kr_km[0][0][i], subkeys_decrypt_Kr_km[1][0][i]);
    }
}


void test_control(uint32_t subkeys[2][8][24], uint32_t* key, uint32_t subkeys_Kr_Km[2][4][12]){
    set_subkeys_Tr_Tm(subkeys);
    ofstream out;
    out.open("subkeys.txt");
    out << "mask subkeys" << endl;
    for (int i = 0; i < 24; i++){
        for (int j = 0; j < 8; j++){
            out << subkeys[0][j][i] << endl;
        }
    }
    out << "shift subkeys" << endl;
    for (int i = 0; i < 24; i++){
        for (int j = 0; j < 8; j++){
            out << subkeys[1][j][i] << endl;
        }
    }
    for (int i = 0; i < 12; ++i){
        func_W(2*i, subkeys, key);
        func_W(2*i+1, subkeys, key);
        set_subkeys_Kr_Km(i, key, subkeys_Kr_Km);
    }
    cout << endl << "\t\t\tRotation subkeys\n" << endl;
    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 4; j++)
            printf("\t%0x\t", subkeys_Kr_Km[1][j][i]);
        cout << endl;
    }
    cout << endl << "\t\t\tMask subkeys\n" << endl;
    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 4; j++)
            printf("%0x\t ", subkeys_Kr_Km[0][j][i]);
        cout << endl;
    }
}