#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <thread>

#include "transmitterConnection.h"
#include "transmitterProtocol.h"

using namespace std;

vector<string> getStringsFromFile(const char* file) {
    vector<string> input;
    string tmp;

    ifstream fileReader;
    fileReader.open(file);
    while (fileReader >> tmp) {
        input.push_back(tmp);
    }

    return input;
}

void listen1(TransmitterConnection* connection, TransmitterProtocol* transmitter) {
	connection->registerListener([&transmitter](char* str) {
		transmitter->receive(str);
	});
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
        cout << "Wrong number of argument\n" << endl;
    } else {
        vector<string> message = getStringsFromFile(argv[3]);

        TransmitterConnection* connection = new TransmitterConnection(argv[1], argv[2]);
        TransmitterProtocol* transmitter = new TransmitterProtocol(connection);

        thread listener(listen1, connection, transmitter);
        cout << "sending" << endl;
        transmitter->transmit(message);
        listener.join();
    }

	return 0;
}
