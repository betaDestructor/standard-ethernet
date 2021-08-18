/*
*---------------------------------------------------------------------------------------------------------------
*	Author	: P Punyacharan, 18-August-2021
*	Email	: punyacharan@gmail.com
*	Licence	: GPL-2.0 License
*	Purpose	: Header file containing declarations of the classes and methods to assist to decode the ethernet frame.
*---------------------------------------------------------------------------------------------------------------
*/

#include "FrameDecode.h"

using std::cout;
using std::string;
using std::endl;
using std::hex;
using std::uppercase;

/*
Ethernet frame header:
+----------+-----+-----------+----------+------+------------------+-----+
| Preamble | SFD | Dest Addr | Src Addr | Type | Data and Padding | CRC |
+----------+-----+-----------+----------+------+------------------+-----+
*/

// Checks whether the input preamble is valid or not. Preamble should contain 56 bits of alternate 0s and 1s.
bool FrameDecode::isValidPreamble(string inputPreamble) {

    // 24019198012642645 is the integer equivalent of 56 bits of alternate 0s and 1s starting 0.
    // This conditional checks for quality between input preamble and the expected preamble.
    // XOR is used for compariosion for better efficiency.

    if (stol(inputPreamble, 0, 2) ^ 24019198012642645)
        return false;
    return true;
}

// Checks if start frame delimiter is valid or not. SFD should contain 10101011.
bool FrameDecode::isValidSFD(string inputSFD) {

    // This conditional checks for quality between input SFD and the expected SFD.
    // XOR is used for compariosion for better efficiency.

    if (stoi(inputSFD, 0, 2) ^ 0b10101011)
        return false;
    return true;
}

// Prints the input binary string in chunks of 4 bits so that it is readable.
void FrameDecode::printBinary (string binary) {

    // Inorder to print input binary in chunks of 4 bits, the length of the input binary
    // should be an integral multiple of 4. The conditional pads couple of 0s to make the length
    // an integral multiple of 4 if not so already.
	int padding = binary.length() % 4;
	if (padding != 0) {
		string padbits = "";
		while (padding--)
			padbits += '0';
		binary = padbits + binary;
	}
	cout << endl;

    // Prints the binary in chunks of 4.
	for (size_t i = 0 ; i < binary.length() ; i+=4) {
		for (size_t j = i ; j < i + 4 ; j++)
			cout << binary[j];
		cout << " ";
	}
	cout << endl;
}

// Removes the leading bytes of 0s.
void FrameDecode::removePadding(string &payload) {
    while (payload.substr(0, 8) == "00000000")
        payload.erase(0, 8);
}

// Parses the input binaryFrame, decodes it and populates the corresponding data members of the class.
void FrameDecode::sanitiseAndPopulate() {

    string binaryFrame = this->binaryFrame;
    if (!this->isValidPreamble(binaryFrame.substr(0, PREAMBLE_LEN))) {
        cout << "[ERROR] Invalid Preamble" << endl;
        delete this;
        exit(EXIT_FAILURE);
    }
    cout << "[INFO] Preamble Validated" << endl;
    binaryFrame.erase(0, PREAMBLE_LEN);

    if (!this->isValidSFD(binaryFrame.substr(0, SFD_LEN))) {
        cout << "[ERROR] Invalid SFD" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "[INFO] SFD Validated" << endl;
    binaryFrame.erase(0, SFD_LEN);

    this->populateAddress(binaryFrame.substr(0, DEST_ADDR_LEN), false);
    binaryFrame.erase(0, DEST_ADDR_LEN);

    this->populateAddress(binaryFrame.substr(0, SRC_ADDR_LEN), true);
    binaryFrame.erase(0, SRC_ADDR_LEN);

    this->populateType(binaryFrame.substr(0, TYPE_LEN));
    binaryFrame.erase(0, TYPE_LEN);

    string payload = binaryFrame.substr(0, binaryFrame.length() - CRC_LEN);
    binaryFrame.erase(0, binaryFrame.length() - CRC_LEN);

    this->removePadding(payload);
    this->payload = "";
    for (size_t i = 0 ; i < payload.length() ; i+=8)
        this->payload += (char) stoi(payload.substr(i, 8), 0, 2);

    this->removePadding(binaryFrame);
    if(!this->checkErrors(binaryFrame)) {
        delete this;
        cout << "[ERROR] Payload corrupt";
        exit(EXIT_FAILURE);
    }
}

// Reads the address binary string, populates the corresponding struct in6_addr.
void FrameDecode::populateAddress(const string &address, bool isSource) {

    // struct in6_addr sed to store the IPv6 address. Read ipv6(7) manual article for more details.
    struct in6_addr temp_address;

    // Set 16 Byte address member to all 0s.
    memset(&temp_address.__in6_u.__u6_addr16, 0, sizeof(temp_address.__in6_u.__u6_addr16));

    // Determine the beggining of the address in the member array of struct (__u6_addr16) as the size of the
    // member array is greater than the original address.
    size_t j = sizeof(temp_address.__in6_u.__u6_addr16) / (sizeof(unsigned char) * 2) - DEST_ADDR_LEN / 8;

    // Extract address one byte at a type and populate temp_address.
    for (size_t i = 0 ; i < address.length() ; i += 8) {
        unsigned char value = stoi(address.substr(i, i + 8), 0, 2);
        temp_address.__in6_u.__u6_addr16[j++] = value;
    }

    // Assign temp_address to corresponding data members depending on the 2nd parameter of the current function.
    if (isSource) this->source_address = temp_address;
    else this->destination_address = temp_address;
}

// Populates the type data member.
void FrameDecode::populateType(string type) {
    this->type = stoi(type, 0, 2);
}

// Extracts CRC bits from the binaryFrame, calculates the CRC of the payload and compares.
bool FrameDecode::checkErrors(string crcBits) {
    unsigned char i, j;
    int crc = 0;
    // Calculate CRC for the current payload.
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
    // Compare the calculated CRC and the received CRC to validate the received payload.
	if (crc == stoi(crcBits, 0, 2))
        return true;
    return false;
}

// Print the data members of the class with corresponding labels. If the parameter is true, it also prints the binary frame.
void FrameDecode::printFrame(bool printBinFrame) {

    cout << "[INFO] Destination Address in hexadecimal values";
    size_t struct_address_len = sizeof((this->destination_address).__in6_u.__u6_addr16) / (sizeof(unsigned char) * 2);
    size_t j = struct_address_len - DEST_ADDR_LEN / 8;
    for (; j < struct_address_len ; j++)
        cout << ":" << uppercase << hex << this->destination_address.__in6_u.__u6_addr16[j];
    cout << endl;

    cout << "[INFO] Source Address in hexadecimal values";
    j = sizeof((this->source_address).__in6_u.__u6_addr16) / (sizeof(unsigned char) * 2) - SRC_ADDR_LEN / 8;
    for (; j < struct_address_len ; j++)
        cout << ":" << uppercase << hex << this->source_address.__in6_u.__u6_addr16[j];
    cout << endl;

    cout << "[INFO] Type [0x" << uppercase << hex << this->type << "]" << endl;

    cout << "[INFO] Payload: " << this->payload << endl;

    if (printBinFrame) {
        cout << "[INFO] Binary Frame:" << endl;
        cout << this->binaryFrame;
    }
    cout << endl;
}
