#ifndef __TRANSMITTER_PROTOCOL_H__
#define __TRANSMITTER_PROTOCOL_H__

#include <iostream>
#include <vector>
#include <string>

#include "conf.h"
#include "dcomm.h"
#include "frame.h"

class TransmitterProtocol {
public:
	TransmitterProtocol(TransmitterConnection* connection);
	void transmit(vector<string> message);
	void receive(char* str);

private:
	TransmitterConnection* connection;
	vector<bool> accepted;
	vector<string> message;
	int tail;
	int head;
	int messageLength;

	void send();
	void sendFrame(int messageIndex);
};

TransmitterProtocol::TransmitterProtocol(TransmitterConnection* connection) {
	this->connection = connection;
}

void TransmitterProtocol::transmit(vector<string> message) {
	this->messageLength = message.size();
	accepted.clear();
	this->message = message;
	for (int i = 0; i < this->messageLength; i++) {
		accepted.push_back(false);
	}
	head = 0;
	tail = 0;
	send();
}

void TransmitterProtocol::send() {
	while (tail < this->messageLength) {
		while ((tail < this->messageLength) && (tail - head < WINDOW_SIZE)) {
			cout << "sending frame " << tail << ": " << message[tail] << endl;
			sendFrame(tail);
			tail++;
			usleep(500000);
		}

		usleep(500000);
	}
}

void TransmitterProtocol::receive(char* str) {
	if (str[0] == ACK) {
		int bufferIndex = (int)str[1];
		int index = head - (head % BUFFER_SIZE) + bufferIndex;
		if (index < head) {
			index += BUFFER_SIZE;
		}

		if (index - head >= WINDOW_SIZE) {
			return;
		}
		accepted[index] = true;
		while (accepted[head]) {
			head++;
		}
	} else if (str[0] == NAK) {
		int bufferIndex = (int)str[1];
		int index = head - (head % BUFFER_SIZE) + bufferIndex;
		sendFrame(index);
	}
}

void TransmitterProtocol::sendFrame(int messageIndex) {
	Frame* frame = new Frame(this->message[messageIndex], messageIndex % BUFFER_SIZE);
	char* str = frame->getFrameString();
	this->connection->send(frame->getFrameString(), message[messageIndex].size() + 5);
}

#endif