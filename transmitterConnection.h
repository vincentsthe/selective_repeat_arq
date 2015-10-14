#ifndef __TRANSMITTER_CONNECTION_H__
#define __TRANSMITTER_CONNECTION_H__

#include <iostream>
#include <thread>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

class TransmitterConnection {
public:
	TransmitterConnection(const char* destHost, const char* port);

	bool send(const char* str, int length);
	void registerListener(function<void(char*)> callback);

private:
	int socketConnection;
	struct sockaddr_in server;
	struct addrinfo *servAddr;
};

TransmitterConnection::TransmitterConnection(const char* destHost, const char* port) {
	struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;

    addrCriteria.ai_socktype = SOCK_DGRAM;
    addrCriteria.ai_protocol = IPPROTO_UDP;
    int rtnVal = getaddrinfo(destHost, port, &addrCriteria, &servAddr);
    if (rtnVal != 0) {
        cout << "Get address info failed" << endl;
    }
    
    socketConnection = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
    if (socketConnection < 0) {
        cout << "Failed connecting to socket" << endl;
    } else {
        cout << "socket created" << endl;
    }
}

bool TransmitterConnection::send(const char* str, int length) {
	ssize_t numB = sendto(socketConnection, str, length, 0, servAddr->ai_addr, servAddr->ai_addrlen);
	if (numB > 0) {
		return true;
	} else {
		return false;
	}
}

void TransmitterConnection::registerListener(function<void(char*)> callback) {
	char buffer[256];
    struct sockaddr_storage fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);
    do {
        recvfrom(socketConnection, buffer, 255, 0, (struct sockaddr *) &fromAddr, &fromAddrLen);

        callback(buffer);
    } while (true);
}

#endif