/*
 * Author: Simone McHugh
 * File: client.c
 * Date: 11/5/19
 * Description: Socket Client using TCP as transport
 * Note:
 *  The inputs are: ip address, port number and a string to pass to the server.
 *  Ex. client 127.0.0.1 5555 "Welcome to CSCI 345!"
 *
 * Edits
 *
 * 10/01/2019 SDN - Code creation
 *
 */
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
 #include <windows.h>
 #include <winsock2.h>
 #include <ws2tcpip.h>
#else
 #include <string.h>
 #include <unistd.h>
 #include <netdb.h>
#endif

int main(int argc, char *argv[])
{

#ifdef WIN32
	WSADATA wsaData;
	SOCKET clientsock = INVALID_SOCKET;
#else
    int clientsock;
#endif

    int rc = 0;
    int port = 0;
    char inputbuffer[512] = {'\0'};
    /* int addresslength = 0; */
    /* char outputbuffer[512] = {'\0'}; */

    struct sockaddr_in send_to;
    /* struct sockaddr recv_from; */

    if (argc < 4) {
        fprintf(stdout,"Usage: %s <ip address> <port> <string>\n", argv[0]);
        return -1;
    }

#ifdef WIN32
    /* WSAStartup allows an application to specify what version of Windows Sockets is required
       It returns a value of 0 if it is successfully run
       If the value is not 0, then it will return an error message */
	rc = WSAStartup(MAKEWORD(2,2), &wsaData);
	if ( rc != 0 ) {
       fprintf(stdout,"WSAStartup failed with error: %d\n",rc);
	   return rc;
	}
#endif

    /* The function strcmp takes two strings and compares them through ASCII value
       It returns an integer value based on the comparison
       In this if else statement, strcmp is comparing a one-dimensional array of strings
       In the else statement, the atoi function converts the string into an integer type for the port number */
    if ( strcmp(argv[2],"") ) {
        port = 4444;
    } else {
        port = atoi(argv[2]);
    }

    /* The socket function is used for file descriptors for a socket that can be used for a connection
       AF_INET is where you put the IP address and SOCK_STREAM is used to establish a connection between two parties
       If successfully done, clientsock will have a non negative integer value
       If not, it will have a value of -1 and will return as an error */
    clientsock = socket(AF_INET, SOCK_STREAM, 0);
    if ( clientsock < 0 ) {
        perror("Error opening socket");
        return rc;
    } else {
        fprintf(stdout, "Client socket created...\n");
    }

    /* The function memset is used to fill some memory space with a particular value
       In this instance, the space of send_to is filled with '\0' for sizeof(send_to) blocks */
    memset(&send_to,'\0',sizeof(send_to));

    /* sin_addr is the IP address in the socket that can be accessed as a one 4-byte integer
       inet_addr converts the IPv4 address from a string to an integer, which is argv[1]
       sin_family is a short that contains code for the address family, and is set to equal AF_NET
       sin_port contains the port number, which is converted to network byte order using htons() */
    send_to.sin_addr.s_addr = inet_addr(argv[1]);
    send_to.sin_family = AF_INET;
    send_to.sin_port = htons(port);

    /* The connect() function initiates a connection on a socket, that being clientsock
       It connects clientsock and (struct sockaddr *)&send_to, the length of the latter being sizeof(send_to)
       The data from connect is converted into an integer, which is equal to rc

       If rc's value is less than 0, then it will result in an error involving the connection
       If one is using Windows, then it will print out the error
       rc is then returned if it is less than 0; otherwise, it will print that the client socket connected successfully */
    rc = connect(clientsock, (struct sockaddr *)&send_to, sizeof(send_to));
    if ( rc < 0 ) {
        perror("Error connecting");
#ifdef WIN32
        fprintf(stdout,"Error from winsock: %d\n",WSAGetLastError());
#endif
        return rc;
    } else {
        fprintf(stdout, "Client socket connected...\n");
    }

    /* If the two strings argv[3] and "" are compared and are equivalent to the value 0,
       then the "" string is copied to the inputbuffer through the strcpy() function
       Otherwise, the argv[3] string is copied to the inputbuffer through the strcpy() function */
    if ( strcmp(argv[3], "") == 0 ) {
        strcpy(inputbuffer, "");
    } else {
        strcpy(inputbuffer, argv[3]);
    }

    /* The function sendto() sends data on the socket with clientsocket, using inputbuffer as the pointer to the buffer
       containing a message. The sizeof(inputbuffer) is the length of the message, the 0 represents the flags but is not supported by AF_UNIX.
       The address of the target is (struct socketaddr *)&send_to, and sizeof(send_to) is the length of the address

       The rc variable is equivalent to the data from sendto(), which is an integer. If rc's value is less than 0,
       then the window will display an error
       If one's computer runs on Windows, then it will print out the error
       rc is returned if it complies with the if statement; otherwise, it will print out inputbuffer,
       along with "Client socket write to server" */
    rc = sendto(clientsock, inputbuffer, sizeof(inputbuffer), 0, (struct sockaddr *)&send_to, sizeof(send_to) );
    if ( rc < 0 ) {
        perror("Error writing socket");
#ifdef WIN32
        fprintf(stdout,"Error from Winsock: %d\n",WSAGetLastError());
#endif
        return rc;
    } else {
        fprintf(stdout, "----> Client socket write to server: %s\n", inputbuffer);
    }

     /*
    addresslength = sizeof(recv_from);
    rc = recvfrom(clientsock, outputbuffer, sizeof(outputbuffer), 0, &recv_from, &addresslength);
    if ( rc < 0 ) {
        perror("Error reading socket");
#ifdef WIN32
        fprintf(stdout,"Error from Winsock: %d\n",WSAGetLastError());
#endif
        return rc;
    } else {
        fprintf(stdout, "----> Client socket read: %s\n", outputbuffer);
    }
    */

#ifdef WIN32
    /* If one is running Windows, then rc is equivalent to closesocket(clientsocket)
    which means that the existing socket gets closed,
    as well as freeing the resources attached to said socket */
    rc = closesocket(clientsock);
#else
    /* If one is running anything but Windows, then rc is equivalent toclose(clientsocket),
    which essentially does the same function as the above function */
    rc = close(clientsock);
#endif

    if ( rc < 0 ) {
        perror("Error closing socket");
        return rc;
    } else {
        fprintf(stdout, "Client socket closed...\n");
    }

#ifdef WIN32
	WSACleanup();
#endif

    return rc;
}



