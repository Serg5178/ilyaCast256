#ifndef __CAST256__
#define __CAST256__

#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include "CAST_Const.h"

using namespace std;

class CAST256{       
    private:
        bool keyset = false;
        bool ivset = false; 
        string iv = {0}; 
        uint32_t A, B, C, D, a, b, c, d, e, f, g, h;
        vector <vector <uint8_t> > Kr, Tr;
        vector <vector <uint32_t> > Km, Tm;
        vector <uint8_t> kr();
        vector <uint32_t> km();
        template <typename T> T ROR(T x, const uint64_t & n, const uint64_t & bits);
        template <typename T> T ROL(const T & x, const uint64_t & n, const uint64_t & bits);
        string makehex(uint64_t value, bool caps = false);
        uint64_t toint(const string & s, const int & base); 
        uint32_t F1(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri);
        uint32_t F2(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri);
        uint32_t F3(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri);
        void W(const uint8_t i);
        void Q(const uint8_t & i);
        void QBAR(const uint8_t & i);
        long fileSize(char* file);
        string unhexlify(const string & in);
        string run(const string & data);
        string encrypt(const string & DATA);
        string decrypt(const string & DATA);
        unsigned int blocksize() const;
    public:
        CAST256();
        CAST256(const string & KEY, const string & IV);
        void setIV(string IV);
        void setkey(string KEY);
        int encryptFile(char* input, char* output);
        int decryptFile(char* input, char* output);
};

#endif
