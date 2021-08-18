#ifndef _FRAME_DECODE_H_
#define _FRAME_DECODE_H_

#include <bits/stdc++.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <arpa/inet.h>

#define PREAMBLE_LEN 7*8
#define SFD_LEN 1*8
#define DEST_ADDR_LEN 6*8
#define SRC_ADDR_LEN 6*8
#define TYPE_LEN 2*8
#define PAYLOAD_MIN_LEN 46*8
#define PAYLOAD_MAX_LEN 1500*8
#define CRC_LEN 4*8
const unsigned char CRC7_POLY = 0x91;

class FrameDecode {
    private:
        std::string binaryFrame;
        struct in6_addr destination_address;
        struct in6_addr source_address;
        unsigned int type;
        std::string payload;
    public:
        bool isValidPreamble (std::string inputPreamble);
        bool isValidSFD (std::string inputSFD);
        void printBinary (std::string binary);
        void removePadding (std::string &payload);
        void sanitiseAndPopulate();
        void populateAddress(const std::string &address, bool isSource);
        void populateType(std::string type);
        bool checkErrors(std::string crcBits);
        void printFrame(bool printBinFrame);
        FrameDecode (std::string binaryFrame) {
            this->binaryFrame = binaryFrame;
            this->sanitiseAndPopulate();
        }
};

#endif