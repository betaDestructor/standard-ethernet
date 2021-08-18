/*
*---------------------------------------------------------------------------------------------------------------
*	Author	: P Punyacharan, 18-August-2021
*	Email	: punyacharan@gmail.com
*	Licence	: GPL-2.0 License
*	Purpose	: Implementation file of FrameEncode.h header file.
*---------------------------------------------------------------------------------------------------------------
*/

#include <bits/stdc++.h>
#include "FrameEncode.h"

using namespace std;

const unsigned char CRC7_POLY = 0x91;
const int MAX_PAYLOAD_LENGTH = 1500;

// Checks if the fetching operation of hostname was successful or not.
void Frame::checkHostName (int hostname) {
	if (hostname == -1) {
		cout << "gethostname error";
		exit(EXIT_FAILURE);
	}
}

void Frame::checkHostEntry (struct hostent * hostentry) {
	if (hostentry == NULL) {
		cout << "gethostname error";
		exit(EXIT_FAILURE);
	}
}

// Returns the string containing the source address in IPv4.
string Frame::getsourceaddress() {
	struct hostent *host_entry;
	char hostbuffer[256];
	int hostname = gethostname(hostbuffer, sizeof(hostbuffer));
	checkHostName(hostname);
	host_entry = gethostbyname(hostbuffer);
	checkHostEntry(host_entry);
	char *IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
	string addr;
	for (size_t i = 0 ; i < strlen(IPbuffer) ; i++)
		addr += IPbuffer[i];
	return addr;
	// cout << "Host name: " << hostbuffer << endl;
	// cout << "IP: " << IPbuffer;
}

// Returns the index of the next ':' present in the first parameter starting from second parameter as index.
size_t Frame::findNextDelimiter (string &addr, size_t i) {
    while (i < addr.length() && addr.at(i++) != ':');
    return i - 1;
}

// Populates the structure member array of source_address which is the data member of this class.
// Reads string containing hex values separated with ':' delimiter
void Frame::populate_source_addr (string addr) {
	for (size_t i = 0, j = 0 ; i < addr.length() ;)
    {
        size_t k = findNextDelimiter(addr, i);
        (this->source_address).__in6_u.__u6_addr16[j++] = stoi(addr.substr(i, i-k), 0, 16);
        i = k + 1;
    }
}

// Populates the structure member array of dest_address which is the data member of this class.
// Reads string containing hex values separated with ':' delimiter
void Frame::populate_dest_addr (string addr) {
	for (size_t i = 0, j = 0 ; i < addr.length() ;)
    {
        size_t k = findNextDelimiter(addr, i);
        (this->destination_address).__in6_u.__u6_addr16[j++] = stoi(addr.substr(i, i-k), 0, 16);
        i = k + 1;
    }
}

// Calculates the CRC of the payload and populates it to the corresponding member data of this class.
void Frame::populateCRC()
{
	unsigned char i, j, crc = 0;
	for (i = 0; i < (this->payload).length(); i++)
	{
		crc ^= (this->payload)[i];
		for (j = 0; j < 8; j++)
		{
			if (crc & 1)
				crc ^= CRC7_POLY;
			crc >>= 1;
		}
	}
	(this->crc) = crc;
}


// ---------------GETTERS-----------------
string Frame::getBinaryPreamble() {
	return bitset<PREAMBLE_LEN>(this->preamble).to_string();
}

string Frame::getBinarysfd() {
	return bitset<SFD_LEN>(this->start_frame_delimiter).to_string();
}

string Frame::getBinaryDestination_address() {
	string binary = "";
	unsigned short int bitCount = 0;
	for (auto ip : (this->destination_address).__in6_u.__u6_addr16) {
		bitset<8> bs4 (ip);
		binary += bs4.to_string();
		bitCount += 8;
		if (bitCount >= DEST_ADDR_LEN)
			break;
	}
	return binary;
}

string Frame::getBinarySource_address() {
	string binary = "";
	unsigned short int bitCount = 0;
	for (auto ip : (this->source_address).__in6_u.__u6_addr16) {
		bitset<8> bs4 (ip);
		binary += bs4.to_string();
		bitCount += 8;
		if (bitCount >= SRC_ADDR_LEN)
			break;
	}
	return binary;
}

string Frame::getBinaryType() {
	return bitset<TYPE_LEN>(this->type).to_string();
}

string Frame::getBinaryPayload() {
	string binary = "";
	for (size_t i = 0 ; i < (this->payload).length() ; i++) {
		bitset<8> bs4 ((this->payload)[i]);
		binary += bs4.to_string();
	}
	if (binary.length() < PAYLOAD_MIN_LEN) {
		unsigned int padding_len = (PAYLOAD_MIN_LEN - binary.length()) / 8;
		string padbits = "";
		while(padding_len--)
			padbits += "00000000";
		binary = padbits + binary;
	}
	return binary;
}

string Frame::getBinaryCRC() {
	return bitset<CRC_LEN>(this->crc).to_string();
}

Frame::~Frame() {}
