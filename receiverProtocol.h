#ifndef __RECEIVER_PROTOCOL_H__
#define __RECEIVER_PROTOCOL_H__

#include <string>
#include <cstring>
#include <vector>

#include "conf.h"
#include "dcomm.h"
#include "frame.h"
#include "receiverConnection.h"

class ReceiverProtocol {
public:
	ReceiverProtocol(ReceiverConnection* connection);

	void receive(const char* str);

private:
	ReceiverConnection* connection;
	vector<string> messageBuffer;
	vector<string> message;
	vector<bool> received;
	int head;

	void sendACK(int frameNumber);
	void sendNAK(int frameNumber);
};

ReceiverProtocol::ReceiverProtocol(ReceiverConnection* connection) {
	this->connection = connection;
	for (int i = 0; i < BUFFER_SIZE; i++) {
		messageBuffer.push_back("");
	}
	for (int i = 0; i < 10000; i++) {
		received.push_back(false);
	}
	head = 0;
}

void ReceiverProtocol::receive(const char* str) {
	if (!Frame::isValidFrame(str)) {
		sendNAK((int)str[1] - 1);
	} else {
		Frame* frame = Frame::constructFrameFromString(str);
		string s = frame->getText();
		int bufferIndex = frame->getFrameNumber();

		messageBuffer[bufferIndex] = s;
		sendACK(bufferIndex);

		int index = head - (head % BUFFER_SIZE) + bufferIndex;
		if (index < head) {
			index += BUFFER_SIZE;
		}

		if (index - head >= WINDOW_SIZE) {
			return;
		}
		received[index] = true;
		while (received[head]) {
			message.push_back(messageBuffer[head % BUFFER_SIZE]);
			cout << s << endl;
			head++;
		}
	}
}

void ReceiverProtocol::sendACK(int frameNumber) {
//	cout << "Send ACK for frame " << frameNumber << endl;

	char* str = new char(3);
	str[0] = ACK;
	str[1] = (char)frameNumber;
	str[2] = 0;

	connection->send(str, 2);
}

void ReceiverProtocol::sendNAK(int frameNumber) {
//	cout << "Send NAK for frame " << frameNumber << endl;

	char* str = new char(3);
	str[0] = NAK;
	str[1] = (char)frameNumber;
	str[2] = 0;

	connection->send(str, 2);
}

#endif