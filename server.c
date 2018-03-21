/* 
 * server.c
 *
 * Sean Karstein
 * COEN 146L
 * Lab 3
 *
 * 2/9/18
 * This program runs a server on a specified port number
 * and recieves a packet over a udp socket from client.c
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

#define BUFF_LEN 10              // num bytes to transfer per iteration

//void checkResult(int, char*);
void udpserver(char*);
int setupSingleConnection(char*);
int UDPsetup(char*);

typedef struct {
    int32_t seq_ack;
    int32_t len;
    int32_t checksum;
}HEADER;

typedef struct{
    HEADER head;
    char data[10];
}PACKET;

int stop=0;


/*
 * int main(int argc, char **argv);
 *
 * @argc{int} - number of arguments
 * @argv{char**} - array of arguments
 */
int main(int argc, char **argv){

    //check proper number of arguments
    if (argc < 2){
        printf("Usage: ./%s PORT_NO\n", argv[0]);
        exit(-1);
    }

    //check port number is greater 1024
    if (atoi(argv[1]) < 1024){
        printf("PORT_NO must be greater than 1024\n");
        exit(-1);
    }

    udpserver(argv[1]);
 }


/*
 * void udpserver(char *port)
 *
 * Starts an ftp server on the given port and waits to receive and write to the given file.
 *
 * @port{char*} - string containing the port number
 */

void udpserver(char *port){
    int sock,nBytes,i;
    char buffer[BUFF_LEN];
    FILE *fp;
    char fname[10];
    PACKET spack;
    PACKET *q=&spack;
    PACKET c_ack;
    PACKET *p=&c_ack;

    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size, client_addr_size;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((short)atoi (port));
    serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
    memset ((char *)serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));
    addr_size = sizeof (serverStorage);
    int check;

    //create socket
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket error\n");
        return;
    }

    //bind
    if (bind (sock, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) != 0) {
        printf("bind error\n");
        return;
    }

    //recieve new filename
    recvfrom (sock, q, (12+BUFF_LEN), 0, (struct sockaddr *)&serverStorage, &addr_size);
    printf("recieved string: %s\n",spack.data);
    strcpy(fname,spack.data);
    printf("new filename is: %s\n",fname);
    fp=fopen(fname,"wb");
    sendto (sock, p, sizeof(c_ack), 0, (struct sockaddr *)&serverStorage, addr_size);

    while (1){
        // receive  datagrams
        nBytes = recvfrom (sock, q, (12+BUFF_LEN), 0, (struct sockaddr *)&serverStorage, &addr_size);
        if(spack.head.len==0)
            break;
        //check checksum
        check=spack.head.checksum;
        spack.head.checksum=0;
        c_ack.head.checksum=checksum(nBytes,q);
        if(c_ack.head.checksum == check) {
            c_ack.head.seq_ack=spack.head.seq_ack;
            sendto (sock, p, sizeof(c_ack), 0, (struct sockaddr *)&serverStorage, addr_size);
            fwrite(spack.data,1,spack.head.len,fp);
        }
        else {
            c_ack.head.seq_ack=!spack.head.seq_ack;
            sendto (sock, p, sizeof(c_ack), 0, (struct sockaddr *)&serverStorage, addr_size);
            printf("send nack\n");
        }
        printf("ACK %d\n",spack.head.seq_ack);
    }
    fclose(fp);
} 

int checksum(int nbytes,char *head) {
    int i;
    char sum = *head;

    for(i=0;i<nbytes;i++) {
        sum = sum ^ head[i];
    }

    return((int) sum);
}

