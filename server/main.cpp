#include "FrameDecode.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_FRAME_SIZE 1519
using namespace std;

int main() {

    int fd1;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(myfifo, 0666);
    char *binaryPayloadCharArray = (char *) calloc (MAX_FRAME_SIZE, sizeof(char));
    char str1[MAX_FRAME_SIZE], str2[80];
    // while (1)
    {
        // First open in read only and read
        fd1 = open(myfifo,O_RDONLY);
        read(fd1, binaryPayloadCharArray, MAX_FRAME_SIZE);

        // Print the read string and close
        // printf("User1: %s\n", str1);
        // close(fd1);

        // Now open in write mode and write
        // string taken from user.
        // fd1 = open(myfifo,O_WRONLY);
        // fgets(str2, 80, stdin);
        // write(fd1, str2, strlen(str2)+1);
        // close(fd1);
    }

    string binaryPayload (binaryPayloadCharArray);
    // cout << binaryPayload;
    // free(binaryPayloadCharArray);
    // return 0;
    // cin >> binaryPayload;
    FrameDecode *frame = new FrameDecode(binaryPayload);
    frame->printFrame(true);
    delete frame;
    return 0;

}