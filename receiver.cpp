#include <thread>
#include <string>
#include <iostream>

#include "receiverConnection.h"
#include "receiverProtocol.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Wrong number of argument" << endl;
	} else {
		ReceiverConnection* connection = new ReceiverConnection(argv[1]);
		ReceiverProtocol* receiver = new ReceiverProtocol(connection);

		connection->registerListener([&receiver](char* str) {
			receiver->receive(str);
		});
	}

	return 0;
}