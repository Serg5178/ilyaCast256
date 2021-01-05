#include "CAST256.h"
using namespace std;
static const uint32_t mod32 = 0xffffffffUL;
uint32_t CAST256::F1(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri){
    uint32_t temp = ROL((Kmi + Data) & mod32, Kri, 32);
    uint8_t Ia = temp >> 24, Ib = (temp >> 16) & 255, Ic = (temp >> 8) & 255, Id = temp & 255;
    return ((CAST_S1[Ia] ^ CAST_S2[Ib]) - CAST_S3[Ic] + CAST_S4[Id]) & mod32;
}

uint32_t CAST256::F2(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri){
    uint32_t temp = ROL(Kmi ^ Data, Kri, 32);
    uint8_t Ia = temp >> 24, Ib = (temp >> 16) & 255, Ic = (temp >> 8) & 255, Id = temp & 255;
    return ((CAST_S1[Ia] - CAST_S2[Ib] + CAST_S3[Ic]) & mod32) ^ CAST_S4[Id];
}

uint32_t CAST256::F3(const uint32_t Data, const uint32_t Kmi, const uint8_t Kri){
    uint32_t temp = ROL((Kmi - Data) & mod32, Kri, 32);
    uint8_t Ia = temp >> 24, Ib = (temp >> 16) & 255, Ic = (temp >> 8) & 255, Id = temp & 255;
    return ((((CAST_S1[Ia] + CAST_S2[Ib]) & mod32) ^ CAST_S3[Ic]) - CAST_S4[Id]) & mod32;
}

uint64_t CAST256::toint(const string & s, const int & base){
    uint64_t value = 0;
    switch (base){
        case 2:
            for(const unsigned char & c : s){
                value = (value << 1) + (static_cast <uint8_t> (c) - '\x30');
            }
            break;
        case 8:
            stringstream(s) >> oct >> value;
            break;
        case 10:
            stringstream(s) >> dec >> value;
            break;
        case 16:
            stringstream(s) >> hex >> value;    // Thanks to Oli Charlesworth @ stackoverflow
            break;
        case 256:
            for(const unsigned char & c : s){
                value = (value << 8) + static_cast <uint8_t> (c);
            }
            break;
        default:
            stringstream s; s << dec << base;
            throw runtime_error("Error: toint() undefined for base: " + s.str());
            break;
    };
    return value;
}

string CAST256::unhexlify(const string & in){
    // Reverse hexlify
    if (in.size() & 1){
        throw runtime_error("Error: input string of odd length.");
    }
    string out(in.size() >> 1, 0);
    for(unsigned int x = 0; x < in.size(); x += 2){
        if (('0' <= in[x]) && (in[x] <= '9')){
            out[x >> 1] = static_cast <uint8_t> ((in[x] - '0') << 4);
        }
        else if(('a' <= in[x]) && (in[x] <= 'f')){
            out[x >> 1] = static_cast <uint8_t> ((in[x] - 'a' + 10) << 4);
        }
        else if(('A' <= in[x]) && (in[x] <= 'F')){
            out[x >> 1] = static_cast <uint8_t> ((in[x] - 'A' + 10) << 4);
        }
        else{
            throw runtime_error("Error: Invalid character found: " + string(1, in[x]));
        }
        if (('0' <= in[x + 1]) && (in[x + 1] <= '9')){
            out[x >> 1] |= static_cast <uint8_t> (in[x + 1] - '0');
        }
        else if(('a' <= in[x + 1]) && (in[x + 1] <= 'f')){
            out[x >> 1] |= static_cast <uint8_t> (in[x + 1] - 'a' + 10);
        }
        else if(('A' <= in[x + 1]) && (in[x + 1] <= 'F')){
            out[x >> 1] |= static_cast <uint8_t> (in[x + 1] - 'A' + 10);
        }
        else{
            throw runtime_error("Error: Invalid character found: " + string(1, in[x + 1]));
        }
    }
    return out;
}

template <typename T> 
string CAST256::makehex(T value, unsigned int size = 2 * sizeof(T), bool caps = false){
    if (!size){
        stringstream out;
        out << hex << value;
        return out.str();
    }

    string out(size, '0');
    while (value && size){
        if (caps){
            out[--size] = "0123456789ABCDEF"[value & 15];
        }
        else{
            out[--size] = "0123456789abcdef"[value & 15];
        }
        value >>= 4;
    }
    return out;
}

void CAST256::W(const uint8_t i){
    g ^= F1(h, Tm[0][i], Tr[0][i]);
    f ^= F2(g, Tm[1][i], Tr[1][i]);
    e ^= F3(f, Tm[2][i], Tr[2][i]);
    d ^= F1(e, Tm[3][i], Tr[3][i]);
    c ^= F2(d, Tm[4][i], Tr[4][i]);
    b ^= F3(c, Tm[5][i], Tr[5][i]);
    a ^= F1(b, Tm[6][i], Tr[6][i]);
    h ^= F2(a, Tm[7][i], Tr[7][i]);
}

vector <uint8_t> CAST256::kr(){
    vector <uint8_t> out;
    out.push_back(a & 31);
    out.push_back(c & 31);
    out.push_back(e & 31);
    out.push_back(g & 31);
    return out;
}

vector <uint32_t> CAST256::km(){
    vector <uint32_t> out;
    out.push_back(h);
    out.push_back(f);
    out.push_back(d);
    out.push_back(b);
    return out;
}

void CAST256::Q(const uint8_t & i){
    C ^= F1(D, Km[i][0], Kr[i][0]);
    B ^= F2(C, Km[i][1], Kr[i][1]);
    A ^= F3(B, Km[i][2], Kr[i][2]);
    D ^= F1(A, Km[i][3], Kr[i][3]);
}

void CAST256::QBAR(const uint8_t & i){
    D ^= F1(A, Km[i][3], Kr[i][3]);
    A ^= F3(B, Km[i][2], Kr[i][2]);
    B ^= F2(C, Km[i][1], Kr[i][1]);
    C ^= F1(D, Km[i][0], Kr[i][0]);
}

string CAST256::run(const string & DATA){
    if (!keyset){
        throw runtime_error("Error: Key has not been set.");
    }

    if (DATA.size() != 16){
        throw runtime_error("Error: Data must be 128 bits in length.");
    }

    A = toint(DATA.substr(0, 4), 256);
    B = toint(DATA.substr(4, 4), 256);
    C = toint(DATA.substr(8, 4), 256);
    D = toint(DATA.substr(12, 4), 256);
    for(uint8_t i = 0; i < 6; i++){
        Q(i);
    }
    for(uint8_t i = 6; i < 12; i++){
        QBAR(i);
    }
    return unhexlify(makehex(A, 8) + makehex(B, 8) + makehex(C, 8) + makehex(D, 8));
}

CAST256::CAST256()
    :
      A(0), B(0), C(0), D(0), a(0), b(0), c(0), d(0), e(0), f(0), g (0), h(0),
      Kr(0), Tr(0),
      Km(0), Tm(0)
{}

CAST256::CAST256(const string & KEY, const string & IV)
    : CAST256()
{
    setkey(KEY);
    setIV(IV);
}

template <typename T>
T CAST256::ROR(T x, const uint64_t & n, const uint64_t & bits){
    static_assert(is_integral <T>::value, "Error: Value being rotated should be integral.");
    return (x >> n) | ((x & ((1ULL << n) - 1)) << (bits - n));
}
template <typename T>
T CAST256::ROL(const T & x, const uint64_t & n, const uint64_t & bits){
    static_assert(is_integral <T>::value, "Error: Value being rotated should be integral.");
    return ROR(x, bits - n, bits);
}

void CAST256::setkey(string KEY){
    if (keyset){
        throw runtime_error("Error: Key has already been set.");
    }

    if ((KEY.size() != 16) && (KEY.size() != 20) && (KEY.size() != 24) && (KEY.size() != 28) && (KEY.size() != 32)){
        throw runtime_error("Error: Key must be 128, 160, 192, 224, or 256 bits in length.");
    }

    KEY += string(32 - KEY.size(), 0);
    a = toint(KEY.substr(0, 4), 256);
    b = toint(KEY.substr(4, 4), 256);
    c = toint(KEY.substr(8, 4), 256);
    d = toint(KEY.substr(12, 4), 256);
    e = toint(KEY.substr(16, 4), 256);
    f = toint(KEY.substr(20, 4), 256);
    g = toint(KEY.substr(24, 4), 256);
    h = toint(KEY.substr(28, 4), 256);

    uint32_t Cm = 0x5A827999, Mm = 0x6ED9EBA1, Cr = 19, Mr = 17;

    vector <uint8_t> range24_8(24, 0);
    vector <uint32_t> range24_32(24, 0);

    Tm.push_back(range24_32); Tr.push_back(range24_8);
    for(uint8_t x = 0; x < 8; x++){
        Tm.push_back(range24_32);
        Tr.push_back(range24_8);
    }
    for(uint8_t i = 0; i < 24; i++){
        for(uint8_t j = 0; j < 8; j++){
            Tm[j][i] = Cm;
            Cm = (Cm + Mm) & mod32;
            Tr[j][i] = Cr;
            Cr = (Cr + Mr) & 31;
        }
    }
    for(uint8_t i = 0; i < 12; i++){
        W(i << 1);
        W((i << 1) + 1);
        Kr.push_back(kr());
        Km.push_back(km());
    }

    keyset = true;
}

void CAST256::setIV(string IV){
    if (ivset){
        throw runtime_error("Error: IV has already been set.");
    }
    if(IV.length() != 16){
        throw runtime_error("Error: IV must be 128 bits in length.");
    }
    iv = IV; 
    ivset = true; 
}

string CAST256::encrypt(const string & DATA){
    return run(DATA);
}

long fileSize(char* file) {
    ifstream stream(file, ios_base::binary);
    long cur_pos, length;
    cur_pos = stream.tellg();
    stream.seekg(0,ios_base::end);
    length = stream.tellg();;
    stream.seekg(0,ios_base::beg);
    stream.close();
    return length;
}

int CAST256::encryptFile(char* input, char* output){
    ifstream inputFile(input, ios::binary);
    if(!inputFile.is_open()){
        throw runtime_error("Error: Not such file in this directory");
    }
    ofstream outputFile(output, ios::binary);
    long size = fileSize(input);
    char textBuff[17] = {0};
    string tempIV = iv;  
    for(size; size > 0; size-=16){
        memset(textBuff, 0x20, 16);
        inputFile.read(textBuff, 16);
        for(int i = 0; i < 16; i++){
            textBuff[i] ^= tempIV[i];
        }
        string buff(textBuff, 16);
        tempIV = encrypt(buff); 
        outputFile << tempIV;
    }
    inputFile.close();
    outputFile.close();
    return 0; 
}

int CAST256::decryptFile(char* input, char* output){
    ifstream inputFile(input, ios::binary);
    if(!inputFile.is_open()){
        throw runtime_error("Error: Not such file in this directory");
    }
    ofstream outputFile(output, ios::binary);
    long size = fileSize(input);
    char textBuff[17] = {0};  
    string tempIV = iv;
    for(size; size > 0; size-=16){
        memset(textBuff, 0, 16);
        inputFile.read(textBuff, 16);
        string buff(textBuff, 16);
        string temp = buff; 
        buff = decrypt(buff);
        for(int i = 0; i < 16; i++){
            buff[i] = tempIV[i] ^ buff[i];
        }
        if(size == 16){ 
            for(int i = buff.length() - 1; i > 0; i--){
                if((int)buff[i] == ' '){
                    buff.pop_back();
                } else {
                    break;
                }
            }
            outputFile << buff;
            break;
        }
        tempIV = temp; 
        outputFile << buff;
    }
    inputFile.close();
    outputFile.close();
    return 0; 
}


string CAST256::decrypt(const string & DATA){
    reverse(Kr.begin(), Kr.end());
    reverse(Km.begin(), Km.end());
    string out = run(DATA);
    reverse(Kr.begin(), Kr.end());
    reverse(Km.begin(), Km.end());
    return out;
}

unsigned int CAST256::blocksize() const {
    return 128;
}
