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

class CAST256{       
    private:
        bool keyset = false;
        uint32_t A, B, C, D, a, b, c, d, e, f, g, h;
        std::vector <std::vector <uint8_t> > Kr, Tr;
        std::vector <std::vector <uint32_t> > Km, Tm;
        template <typename T> T ROR(T x, const uint64_t & n, const uint64_t & bits);
        template <typename T> T ROL(const T & x, const uint64_t & n, const uint64_t & bits);
        uint64_t toint(const std::string & s, const int & base);
        std::string unhexlify(const std::string & in);
        template <typename T> 
        std::string makehex(T value, unsigned int size = 2 * sizeof(T), bool caps = false);
        uint32_t F1(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri);
        uint32_t F2(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri);
        uint32_t F3(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri);
        void W(const uint8_t i);
        std::vector <uint8_t> kr();
        std::vector <uint32_t> km();
        void Q(const uint8_t & i);
        void QBAR(const uint8_t & i);
        std::string run(const std::string & data);
    public:
        CAST256();
        CAST256(const std::string & KEY);
        void setkey(std::string KEY);
        int encryptFile(char* input, char* output);
        int decryptFile(char* input, char* output);
        std::string encrypt(const std::string & DATA);
        std::string decrypt(const std::string & DATA);
        unsigned int blocksize() const;
};

#endif
