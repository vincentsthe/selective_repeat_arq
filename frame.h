#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#include <string>
#include <cstring>
#include <cstdlib>

#include "dcomm.h"

class Frame {
public:
	Frame(const char* str, int frameNumber);
	Frame(string str, int frameNumber);
	~Frame();

	string getText();
	int getFrameNumber();

	char* getFrameString();

	static bool isValidFrame(const char* str);
	static Frame* constructFrameFromString(const char* str);

private:
	char* text;
	int frameNumber;

	char getHash();

	static char getStringHash(const char* str);
};

Frame::Frame(const char* str, int frameNumber) {
	frameNumber++;
	int len = strlen(str);
	text = new char(len+1);
	strcpy(this->text, str);
	this->frameNumber = frameNumber;
}

Frame::Frame(string str, int frameNumber) {
	frameNumber++;
	int len = str.size();
	text = new char(len+1);
	strcpy(this->text, str.c_str());
	this->frameNumber = frameNumber;
}

Frame::~Frame() {
	delete text;
}

string Frame::getText() {
	return text;
}

int Frame::getFrameNumber() {
	return (frameNumber - 1);
}

char* Frame::getFrameString() {
	int len = strlen(this->text);

	char* ret = new char(len + 6);
	ret[0] = SOH;
	ret[1] = (char)this->frameNumber;
	ret[2] = STX;
	strcpy(ret + 3, this->text);
	ret[len + 3] = ETX;
	ret[len + 4] = this->getHash();
	ret[len + 5] = 0;

	return ret;
}

bool Frame::isValidFrame(const char* str) {
	int len = 3;
	while (str[len] != 3) {
		len++;
	}
	len += 2;

	if (str[0] != SOH) {
		return false;
	}

	if (str[2] != STX) {
		return false;
	}

	if (str[len - 2] != ETX) {
		return false;
	}

	char hash = str[len - 1];

	char* text = new char(len);

	memcpy(text, str + 3, len - 5);
	text[len - 5] = 0;

	if (Frame::getStringHash(text) != hash) {
		delete text;
		return false;
	}

	delete text;
	return true;
}

Frame* Frame::constructFrameFromString(const char* str) {
	int len = 3;
	while (str[len] != 3) {
		len++;
	}
	len += 2;
	char* text = new char(len);

	memcpy(text, str + 3, len - 5);
	text[len - 5] = 0;

	int frameNumber = (int) str[1];

	return new Frame(text, frameNumber - 1);
}

char Frame::getHash() {
	int len = strlen(text);

	int hash = 0;
	for (int i = 0; i < len; i++) {
		hash += (int) text[i];
		hash %= 128;
	}

	return (char)hash;
}

char Frame::getStringHash(const char* str) {
	int len = strlen(str);

	int hash = 0;
	for (int i = 0; i < len; i++) {
		hash += (int) str[i];
		hash %= 128;
	}

	return (char)hash;
}


#endif