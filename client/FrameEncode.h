#ifndef _FRAME_ENCODE_H_
#define _FRAME_ENCODE_H_

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <arpa/inet.h>
#include <cstring>

#define PREAMBLE_LEN 7*8
#define SFD_LEN 1*8
#define DEST_ADDR_LEN 6*8
#define SRC_ADDR_LEN 6*8
#define TYPE_LEN 2*8
#define PAYLOAD_MIN_LEN 46*8
#define PAYLOAD_MAX_LEN 1500*8
#define CRC_LEN 4*8

using std::string;

class Frame {
    private:
        unsigned long int preamble = 0b01010101010101010101010101010101010101010101010101010101;
        u_int8_t start_frame_delimiter = 0b010101011;
        struct in6_addr destination_address;
        struct in6_addr source_address;
        unsigned int type = 0x86DD; // IPv6
        string payload;
        u_int8_t crc;
    
    public:
        void checkHostName (int hostname);
        void checkHostEntry (struct hostent * hostentry);
        string getsourceaddress();
        size_t findNextDelimiter (string &addr, size_t i);
        void populate_source_addr (string addr);
        void populate_dest_addr (string addr);
        void populateCRC();

		// Getters
		string getBinaryPreamble();
		string getBinarysfd();
		string getBinaryDestination_address();
        string getBinarySource_address();
        string getBinaryType();
        string getBinaryPayload();
        string getBinaryCRC();

		// Constructor
        Frame(string source, string dest, string payload) {
            memset(&(this->destination_address).__in6_u.__u6_addr16, 0, sizeof((this->destination_address).__in6_u.__u6_addr16));
            this->payload = payload;
            this->populate_dest_addr(dest);
            this->populate_source_addr(source);
            this->populateCRC();
        }

		// Destructor
		~Frame();
};

#endif
