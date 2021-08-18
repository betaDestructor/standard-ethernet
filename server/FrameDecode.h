/*
*---------------------------------------------------------------------------------------------------------------
*	Author	: P Punyacharan, 18-August-2021
*	Email	: punyacharan@gmail.com
*	Licence	: GPL-2.0 License
*	Purpose	: Header file containing declarations of the classes and methods to assist to decode the ethernet frame.
*---------------------------------------------------------------------------------------------------------------
*/

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

/*
Ethernet frame header:
+----------+-----+-----------+----------+------+------------------+-----+
| Preamble | SFD | Dest Addr | Src Addr | Type | Data and Padding | CRC |
+----------+-----+-----------+----------+------+------------------+-----+
*/

    private:
        std::string binaryFrame;
        // struct in6_addr sed to store the IPv6 address. Read ipv6(7) manual article for more details.
        struct in6_addr destination_address;
        struct in6_addr source_address;
        unsigned int type;
        std::string payload;
    public:
        // Checks whether the input preamble is valid or not. Preamble should contain 56 bits of alternate 0s and 1s.
        bool isValidPreamble (std::string inputPreamble);

        // Checks if start frame delimiter is valid or not. SFD should contain 10101011.
        bool isValidSFD (std::string inputSFD);

        // Prints the input binary string in chunks of 4 bits so that it is readable.
        void printBinary (std::string binary);

        // Removes the leading bytes of 0s.
        void removePadding (std::string &payload);

        // Parses the input binaryFrame, decodes it and populates the corresponding data members of the class.
        void sanitiseAndPopulate();

        // Reads the address binary string, populates the corresponding struct in6_addr.
        void populateAddress(const std::string &address, bool isSource);

        // Populates the type data member.
        void populateType(std::string type);

        // Extracts CRC bits from the binaryFrame, calculates the CRC of the payload and compares.
        bool checkErrors(std::string crcBits);

        // Print the data members of the class with corresponding labels. If the parameter is true, it
        // also prints the binary frame.
        void printFrame(bool printBinFrame);

        // Constructor
        FrameDecode (std::string binaryFrame) {
            this->binaryFrame = binaryFrame;
            this->sanitiseAndPopulate();
        }
};

#endif
