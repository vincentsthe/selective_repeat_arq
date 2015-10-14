#ifndef __RECEIVER_CONNECTION_H__
#define __RECEIVER_CONNECTION_H__

#include <iostream>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

class ReceiverConnection {
public:
	ReceiverConnection(const char* port);

	void registerListener(function<void(char*)> callback);
	void send(const char* str, int length);

private:
	int socketConnection;
	struct sockaddr_in server;
	struct addrinfo *servAddr;
	struct sockaddr_in clntAddr;
	socklen_t clntAddrLen;
};

ReceiverConnection::ReceiverConnection(const char* port) {
	cout << "Binding pada localhost:" << port << endl;

	clntAddrLen = sizeof(clntAddr);

	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_flags = AI_PASSIVE;
	addrCriteria.ai_socktype = SOCK_DGRAM;
	addrCriteria.ai_protocol = IPPROTO_UDP;
	
	
	struct addrinfo *servAddr; // List of server addresses
	int rtnVal = getaddrinfo(NULL, port, &addrCriteria, &servAddr);
	if (rtnVal != 0) {
		cout << "get address info failed" << endl;
	}
	
	// create socket
	socketConnection = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
	if (socketConnection < 0) {
		cout << "failed create socket" << endl;
	}
	
	// Bind to local address
	if (::bind(socketConnection, servAddr->ai_addr, servAddr->ai_addrlen) == -1) {
		cout << "error binding" << endl;
	} else {
		cout << "binding success" << endl;
	}

	freeaddrinfo(servAddr);
}

void ReceiverConnection::registerListener(function<void(char*)> callback) {
	char buffer[256];
	cout << "Listening...." << endl;
    do {
        recvfrom(socketConnection, buffer, 255, 0, (struct sockaddr *) &clntAddr, &clntAddrLen);
        callback(buffer);
    } while (true);
}

void ReceiverConnection::send(const char* str, int length) {
	sendto(socketConnection, str, length, 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr));
}

#endif