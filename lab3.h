#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <assert.h>

#ifndef lab3_h
#define lab3_h

#define BUFF_LEN 96
#define CORRUPTION_RATE 0.8 

typedef enum messagetype { SYNC, FILENAME, FILECONTENT, FILEEND } m_type;

typedef struct udpSocket{
    int sockfd;                     //represents the successfully connected socket structure
    struct sockaddr remoteInfo;     //represents the specifications passed in for this socket
    socklen_t socketlen;            //socklen for the last arg of sendto
} UDPSocket;

typedef struct segment {
    uint8_t sequence;               //current sequence number of the packet, either 0 or 1
    uint16_t checksum;              //calculated checksum
    char data[BUFF_LEN];            //actual data to be sent
    uint8_t numBytes;               //number of data bytes in segment
    m_type type;
} Segment;


/*
 * void checkResult(int retval, char *message)
 *
 * Prints the given message to stdout if retval is -1
 *
 */

void checkResult(int, char*);

/*
 * u_int16_t checksumSimple(char *data);
 *
 * Calculates complementing the 16 bit representation of the data. Every 2 bytes is taken
 * as a single u_int16t and added together for the checksum. Overflow is welcome.
 *
 */

u_int16_t checksumSimple(char *data);

/*
 * u_int16_t checksum(struct udpSocket *sock);
 *
 * Calculates the actual UDP checksum.
 *
 */

u_int16_t checksum(struct udpSocket* sock);


/*
 * void printSegment(Segment *s, int receiving)
 *
 * Prints out the contents of the given segment. If the second argument is -1, indicates
 * a sending packet. If the second argument is 1, indicates a receiving packet. Any 
 * other number is ignored.
 *
 */

void printSegment(Segment *s, int receiving);

/*
 * void printAddrInfo(struct sockaddr_in*)
 *
 * Prints out the addrinfo 
 *
 */

void printAddrInfo(struct sockaddr_in* addrInfo);


void closeSocket(struct udpSocket* udpSocket);
#endif /* lab3_h */

