#include "FrameDecode.h"

using std::cout;
using std::string;
using std::endl;
using std::hex;
using std::uppercase;

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

void FrameDecode::populateAddress(const string &address, bool isSource) {
    struct in6_addr temp_address;
    memset(&temp_address.__in6_u.__u6_addr16, 0, sizeof(temp_address.__in6_u.__u6_addr16));
    size_t j = sizeof(temp_address.__in6_u.__u6_addr16) / (sizeof(unsigned char) * 2) - DEST_ADDR_LEN / 8;
    for (size_t i = 0 ; i < address.length() ; i += 8) {
        unsigned char value = stoi(address.substr(i, i + 8), 0, 2);
        temp_address.__in6_u.__u6_addr16[j++] = value;
    }
    if (isSource) this->source_address = temp_address;
    else this->destination_address = temp_address;
}

void FrameDecode::populateType(string type) {
    this->type = stoi(type, 0, 2);
}

bool FrameDecode::checkErrors(string crcBits) {
    unsigned char i, j;
    int crc = 0;
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
	if (crc == stoi(crcBits, 0, 2))
        return true;
    return false;
}

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

void FrameDecode::printBinary (string binary) {
	int padding = binary.length() % 4;
	if (padding != 0) {
		string padbits = "";
		while (padding--)
			padbits += '0';
		binary = padbits + binary;
	}
	cout << endl;
	for (size_t i = 0 ; i < binary.length() ; i+=4) {
		for (size_t j = i ; j < i + 4 ; j++)
			cout << binary[j];
		cout << " ";
	}
	cout << endl;
}

bool FrameDecode::isValidSFD(string inputSFD) {
    if (stoi(inputSFD, 0, 2) ^ 0b10101011)
        return false;
    return true;
}

bool FrameDecode::isValidPreamble(string inputPreamble) {
    if (stol(inputPreamble, 0, 2) ^ 24019198012642645)
        return false;
    return true;
}

void FrameDecode::removePadding(string &payload) {
    while (payload.substr(0, 8) == "00000000")
        payload.erase(0, 8);
}