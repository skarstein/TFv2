//#include "lab3.h"

/*
 * u_int16_t checksumSimple(char *data);
 * 
 * Calculates complementing the 16 bit representation of the data. Every 2 bytes is taken
 * as a single u_int16t and added together for the checksum. Overflow is welcome. 
 *
 * In your main function you should seed the random number generator: srand(time(NULL));
 * 
 */

u_int16_t checksumSimple(char *data){
    int i = 0;
    int checksum = 0;
    int length = strlen(data);
    float random = (float) rand();

    if (random / (float)RAND_MAX < CORRUPTION_RATE)
        return (int) random;

    return 17;
}

/*
 * void checkResult(int retval, char *message)
 *
 * Prints the given message to stdout if retval is -1
 *
 */

void checkResult(int retval, char *message){
    if (retval == -1){
        printf("%s: %s.\n",message, strerror(errno));
        exit(-1);
    }
}

/*
 * void printSegment(Segment *s, int receiving);
 *
 * Prints out the contents of the given segment
 *
 */

void printSegment(Segment *s, int receiving){
    if (receiving == 1)
        fprintf(stderr, "Receiving - ");
    else if (receiving == -1)
        fprintf(stderr, "Sending - ");

    fprintf(stderr, "Type: %d\tSequence: %u\tNum Bytes %u\t:Data: %02x\tChecksum %u\n", 
        s->type, s->sequence, s->numBytes, s->data, s->checksum);
}

/*
 * void printAddrInfo(struct sockaddr_in*)
 *
 * Prints out the addrinfo 
 *
 */

void printAddrInfo(struct sockaddr_in* addrInfo){
    assert (addrInfo != NULL);
    fprintf(stderr, "(%u, %u\n)", addrInfo->sin_addr, addrInfo->sin_port);
}

/*
 * void closeSocket (struct udpSocket*)
 * 
 * Closes the given udp socket and frees the structure, but not the struct sockaddr_in *
 * because it may not have been assigned. 
 *
 */ 

void closeSocket(struct udpSocket* udpSocket){
    close(udpSocket->sockfd);
    free(udpSocket);
}

