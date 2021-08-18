/*
*---------------------------------------------------------------------------------------------------------------
*	Author	: P Punyacharan, 18-August-2021
*	Email	: punyacharan@gmail.com
*	Licence	: GPL-2.0 License
*	Purpose	: Driver file for the client end of Ethernet protocol.
*---------------------------------------------------------------------------------------------------------------
*/

#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "FrameEncode.h"

using namespace std;

// Hardcoded source address.
const string SOURCE_ADDR = "4A:30:10:21:10:1A";

// Prints the input binary string in chunks of 4 bits so that it is readable.
void printBinary (string binary) {

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

int main() {

	cout << "Enter the type of destination address:" << endl;
	cout << "1. Unicast / Multicast" << endl << "2. Broadcast" << endl << "Enter choice: ";

	int choice;
	string dest_address;
	cin >> choice;

	switch (choice) {
		case 1: cout << "You have chosen Unicast / Multicast. Enter the address of the target: ";
			cin >> dest_address;
			break;
		case 2: cout << "You have chosen broadcast." << endl;
			dest_address = "FF:FF:FF:FF:FF:FF"; // Default address for broadcast.
			break;
		default: cout << "INVALID CHOICE. Exiting" << endl; exit(1);
	}

	// string source = getsourceaddress();
	// cout << "Home address: " << source << endl;
	// for (size_t i = 0 ; i < 6 ; i++)
    //     cout << hex << data->destination_address.__in6_u.__u6_addr16[i] << ":";

	cout << "Enter the message (payload): ";
	string message;
	cin.ignore();
	getline(cin, message);

    Frame *data = new Frame(SOURCE_ADDR, dest_address, message);

	cout << "Preamble: ";
	string preamble = data->getBinaryPreamble();
	printBinary(preamble);

	cout << "SFD: ";
	string sfd = data->getBinarysfd();
	printBinary(sfd);

	cout << "Destination address: ";
	string dest = data->getBinaryDestination_address();
	printBinary(dest);

	cout << "Source address: ";
	string src = data->getBinarySource_address();
	printBinary(src);

	cout << "Type: ";
	string type = data->getBinaryType();
	printBinary(type);

	cout << "Payload: ";
	string payload = data->getBinaryPayload();
	printBinary(payload);

	cout << "CRC: ";
	string crc = data->getBinaryCRC();
	printBinary(crc);

	string binaryFrame = preamble + sfd + dest + src + type + payload + crc;

	cout << "Final binary frame:";
	printBinary(binaryFrame);

	int fd;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    // Open FIFO for write only
    fd = open(myfifo, O_WRONLY);

	char *binaryFrameCharArray = (char *) calloc (binaryFrame.length() + 1, sizeof(char));
	strcpy(binaryFrameCharArray, binaryFrame.c_str());
    
	// Write the input binaryFrameCharArray on FIFO and close it
    write(fd, binaryFrameCharArray, binaryFrame.length() + 1);
	free(binaryFrameCharArray);
    close(fd);

    delete data;
	return 0;
}
