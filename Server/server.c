/*
 * Author: Simone McHugh
 * File: server.c
 * Date: 11/4/19
 * Description: Socket Server using TCP as the transport and entry point for a client connection
 * Note:
 *  The inputs are: port number.
 *  Ex. server 5555
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
	SOCKET listensocket = INVALID_SOCKET;
	SOCKET acceptsocket = INVALID_SOCKET;
#else
    int listensocket;
    int acceptsocket;
#endif

    int rc = 0;
    int port = 0;
    int clientaddrlen = 0;
    int fromaddresslength = 0;
    char buffer[255] = {'\0'};

    struct sockaddr_in serveraddress, clientaddress;
    struct sockaddr recv_from;

    if (argc < 2) {
        fprintf(stdout,"Usage: server <port>\n");
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
    if ( strcmp(argv[1],"") ) {
        port = 4444;
    } else {
        port = atoi(argv[2]);
    }

    /* The socket function is used for file descriptors for a socket that can be used for a connection
       AF_INET is where you put the IP address and SOCK_STREAM is used to establish a connection between two parties
       If successfully done, listensock will have a non negative integer value
       If not, it will have a value of -1 and will return as an error */
    listensocket = socket(AF_INET, SOCK_STREAM, 0);
    if ( listensocket < 0 ) {
        perror("Error opening socket");
        return rc;
    } else {
        fprintf(stdout, "Server socket has been created...\n");
    }

    /* The function memset is used to fill some memory space with a particular value
       In this instance, the space of &serveraddress is filled with '\0' for sizeof(serveraddress) blocks */
    memset(&serveraddress,'\0',sizeof(serveraddress));

    /* sin_family is a short that contains code for the address family, and is set to equal AF_NET
       sin_addr is the IP address in the socket that can be accessed as a one 4-byte integer, which is equivalent to INADDR_ANY
       INADDR_ANY is used to tell the socket to listen on any available interfaces
       sin_port contains the port number, which is converted to network byte order using htons() */
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_addr.s_addr = INADDR_ANY;
    serveraddress.sin_port = htons(port);

    /* The bind() function assigns the local socket address (struct sockaddr *) &serveraddress to listensocket
       It binds a name to a socket, and should return 0 upon successful completion

       If rc, which is equivalent to the value of bind(), is less than 0, then it will result in an error involving the connection
       If one is using Windows, then it will print out the error
       rc is then returned if it is less than 0; otherwise, it will print that the server socket can bind a port */
    rc = bind(listensocket, (struct sockaddr *) &serveraddress, sizeof(serveraddress));
    if ( rc < 0 ) {
        perror("Error binding");
        return rc;
    } else {
        fprintf(stdout, "Server socket can bind a port...\n");
    }

    /* The listen() function listens for socket connections through listensocket,
       with a limiting number of connections defined by the 5. This value is stored into rc

       If rc's value is less than 0, then it will result in an error involving the listening
       rc is then returned; otherwise, it will print that the server socket is listening
       clientaddress's length value is stored into clientaddrlen as an inetger */
    rc = listen(listensocket, 5);
    if ( rc < 0 ) {
        perror("Error Listening");
        return rc;
    } else {
        fprintf(stdout, "Server socket is listening...\n");
    }

    clientaddrlen = sizeof(clientaddress);

    while(1) {
        /* The accept() function accepts a new connection on listensocket through the address (struct sockaddr *) &clientaddress
           This integer value is stored in acceptsocket

           If acceptsocket's value is less than 0, then it will result in an error involving the creation of the client socket
           rc is returned; otherwise, it will print that the client socket is connected */
        acceptsocket = accept(listensocket, (struct sockaddr *) &clientaddress, &clientaddrlen);
        if ( acceptsocket < 0 ) {
           perror("Error creating client socket");
           return rc;
       } else {
           fprintf(stdout, "----> Client socket is connected...\n");
       }

       /* recv_from's length value is stored in fromaddresslength as an integer
          The recvfrom() function receives a message from acceptsocket. The message comes from buffer, its length being sizeof(buffer)
          It has 0 flags, which specifies the type of message reception. The source of the message come from (struct sockaddr *)&recv_from
          This data is stored in rc

          If rc's value is less than 0, then it will result in an error involving reading the client socket
          rc is returned; otherwise, it will print the received data from the client */
       fromaddresslength = sizeof(recv_from);
       rc = recvfrom(acceptsocket, buffer, sizeof(buffer),0, (struct sockaddr *)&recv_from, &fromaddresslength);
       if ( rc < 0 ) {
           perror("Error reading client socket");
           return rc;
       } else {
           fprintf(stdout, "----> Received data from client: %s\n", buffer);
       }

#ifdef WIN32
    /* If one is running Windows, then rc is equivalent to closesocket(acceptsocket)
    which means that the existing socket gets closed,
    as well as freeing the resources attached to said socket */
    rc = closesocket(acceptsocket);
#else
    /* If one is running anything but Windows, then rc is equivalent toclose(acceptsocket),
    which essentially does the same function as the above function */
    rc = close(acceptsocket);
#endif

       if ( rc < 0 ) {
           perror("Error closing client socket");
           return rc;
       } else {
           fprintf(stdout, "----> Client socket is closed...\n");
       }
    } // End While Loop

#ifdef WIN32
    /* The WSACleanup() function terminates use of the Winsock 2 DLL
       Sockets opened when WSACleanup is called are reset like if closesocket was called */
	WSACleanup();
#endif
    return rc;
}



