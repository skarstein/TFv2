/* 
 * client.c
 * Sean Karstein
 * COEN 146L
 * Lab 3
 *
 * 2/9/18
 * This program sends a file over a udp socket to a specified ip and port
 * of server.c
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define BUFF_LEN 10             // num bytes to transfer per iteration

//void checkResult(int, char*);
int udpClient(char *filename, char* newFileName, char* host, char* port);
struct sockaddr_in setupConnection(char* host, char* port, int *sockdes);

struct HEADER{
    int32_t seq_ack;
    int32_t len;
    int32_t cksum;
}HEADER;

struct PACKET{
    struct HEADER head;
    char data[10];
}PACKET;

int main(int argc, char **argv){
    if (argc < 5){
        printf("Usage: %s FILE_TO_TRANSFER NEW_FILENAME HOST PORT\n", argv[0]);
        exit(-1);
     }

    if (access(argv[1], R_OK) != 0){
        printf("Make sure you have read access to the file\n");
        exit(-1);
    }

    if (atoi(argv[4]) < 5000){
        printf("Please make sure PORT_NO is above 5000\n");
        exit(-1);
    }

    udpClient(argv[1], argv[2], argv[3], argv[4]);
}

/*
 * int udpserver(char *port)
 *
 * Starts an ftp client connecting to the given host:port, transferring over the contents
 * of filename into newFileName.
 *
 * @filename{char*} - name of the file to be sent
 * @newFileName{char*} - name of the file to be written
 * @host{char*} - IP address of the host server
 * @port{char*} - port number of host
 */

int udpClient(char *filename, char* newFileName, char* host, char* port){
    int sockfd,nBytes;
    char buffer[BUFF_LEN];
    socklen_t addr_size;
    FILE *fp=fopen(filename,"r");
    int fildes=fileno(fp);
    int bytes_written,bytes_read;
    int state = 0;
    //create UDP socket
    struct sockaddr_in  s = setupConnection(host, port, &sockfd);
    printf("socket bound to server\n");
    printf("s: %d\nsockfd: %d\n",s,sockfd);
    addr_size = sizeof s;

    //created header
    struct PACKET cpack;
    struct PACKET s_ack;
    struct PACKET *q=&cpack;

    //create file name packet
    strcpy(cpack.data,newFileName);
    cpack.head.len=sizeof(newFileName);
    cpack.head.cksum=0;
    sendto (sockfd, q, (12+BUFF_LEN), 0, (struct sockaddr *)&s, addr_size);
    recvfrom (sockfd, &s_ack, sizeof(s_ack),0,NULL,NULL);

    while(1) {
        bytes_read=read(fildes,cpack.data,10);
        cpack.head.seq_ack=state;
        cpack.head.len=bytes_read;
        if(bytes_read==0) {
            //done transferring file, end
            cpack.head.len=0;
            sendto (sockfd, q, (12+BUFF_LEN), 0, (struct sockaddr *)&s, addr_size);
            break;
        }
        do {
            cpack.head.cksum=0;
            cpack.head.cksum=checksum(12+bytes_read,q);
            if(rand()%2==1)
                cpack.head.cksum=rand()%40;
            //send
            sendto (sockfd, q, (12+bytes_read), 0, (struct sockaddr *)&s, addr_size);
            //recv
            recvfrom (sockfd, &s_ack, (12+BUFF_LEN),0,NULL,NULL);
            //if(s_ack.head.seq_ack!=cpack.head.seq_ack){
                //printf("failed ACK\n");
                //sendto (sockfd, q, (12+BUFF_LEN), 0, (struct sockaddr *)&s, addr_size);
        }while(s_ack.head.seq_ack!=cpack.head.seq_ack);
        //update state
        //if(s_ack.head.seq_ack==state) {
        if(state==0)
            state=1;
        else
            state=0;
        //continue;
        //}
        //else
        //    sendto (sockfd, q, (12+bytes_read), 0, (struct sockaddr *)&s, addr_size);
    }
    printf("done sending file\n");
    return 0;
}

/*
 * int setupConnection(char* host, char* port);
 *
 * Initializes client socket using passed in host and port
 * then attempt to connect to server.
 *
 * @host{char*} - string containing the ip address of the host
 * @port{char*} - string containing the port number
 *
 * @return{int} - socket file descriptor
 *
 */
struct sockaddr_in setupConnection(char* host, char* port, int *sockdes){
    int sock;
    int success;
    struct addrinfo *sockList;
    struct sockaddr_in servAddr;

    //setup client socket
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    fprintf(stderr, "Client socket is set up\n");

    //Resolve the IP Address given the hostname and the port number 
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr=inet_addr(host);

    char buffer[50];
    inet_ntop(AF_INET, &(servAddr.sin_addr.s_addr),buffer,50);
    printf("\n\nAddress: %s\n\n",buffer);

    servAddr.sin_port=htons(atoi(port));

    struct sockaddr_in* dave = &servAddr;
    //printAddrInfo(dave);
    *sockdes=sock;
    return servAddr;
}

int checksum(int nbytes,char *head) {
    int i;
    char sum = *head;

    for(i=0;i<nbytes;i++) {
        sum = sum ^ head[i];
    }

    return((int) sum);
}
