/*
*---------------------------------------------------------------------------------------------------------------
*	Author	: P Punyacharan, 18-August-2021
*	Email	: punyacharan@gmail.com
*	Licence	: GPL-2.0 License
*	Purpose	: To read the incoming binary coded frame and decode it into various elements of the ethernet frame
*---------------------------------------------------------------------------------------------------------------
*/
#include "FrameDecode.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_FRAME_SIZE 1519
#define MIN_FRAME_SIZE 512
using namespace std;

int main() {

    int fd1;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(myfifo, 0666);

    char *binaryPayloadCharArray = (char *) calloc (MAX_FRAME_SIZE, sizeof(char));

    // Open FIFO file in read only and read.
    fd1 = open(myfifo,O_RDONLY);

    // Read the contents of the FIFO file and save it in a character array.
    read(fd1, binaryPayloadCharArray, MAX_FRAME_SIZE);

    // Create a string object containing the received binary coded frame.
    string binaryPayload (binaryPayloadCharArray);
    free(binaryPayloadCharArray);

    if (binaryPayload.length() < MIN_FRAME_SIZE) {
        cout << "[ERROR] DATA LOSS ALERT!" << endl;
        exit(EXIT_FAILURE);
    }
    // Create a FrameDecode object with the received frame.
    FrameDecode *frame = new FrameDecode(binaryPayload);

    // Print the contents of the frame in human readable form.
    frame->printFrame(true);

    // Garbage collection.
    delete frame;

    return 0;

}
