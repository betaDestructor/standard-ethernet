#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "FrameEncode.h"

using namespace std;
const string SOURCE_ADDR = "4A:30:10:21:10:1A";

void printBinary (string binary) {
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
			dest_address = "FF:FF:FF:FF:FF:FF";
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

    // while (1)
    {
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Take an input arr2ing from user.
        // 80 is maximum length
        // fgets(arr2, 80, stdin);
		char *binaryFrameCharArray = (char *) calloc (binaryFrame.length() + 1, sizeof(char));
		strcpy(binaryFrameCharArray, binaryFrame.c_str());
        // Write the input arr2ing on FIFO
        // and close it
        write(fd, binaryFrameCharArray, binaryFrame.length() + 1);
		free(binaryFrameCharArray);
        close(fd);

        // Open FIFO for Read only
        // fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        // read(fd, binaryFrame, sizeof(arr1)); // Read into a char array

        // Print the read message
        // printf("User2: %s\n", binaryFrame);
        // close(fd);
    }

    delete data;
	return 0;
}