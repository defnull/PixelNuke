/* 
 * File:   PxCommand.cpp
 * Author: marc
 * 
 * Created on March 22, 2015, 2:52 PM
 */

#include "PxCommand.h"
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <climits>

PxCommand::PxCommand(char * line, const size_t n): line(line) {

	for(size_t i=0; i<n; i++) {
		if(line[i] == ' ') {
			line[i] = '\0';
		} else if(i==0 || line[i-1] == '\0') {
			pos.push_back(line+i);
		}
	}
}

bool PxCommand::parse(size_t index, unsigned int &a, int base) {
	char * end;
	a = std::strtoul(get(index), &end, base);
	return *end == '\0';
}

size_t PxCommand::nargs() {
	return pos.size();
}

PxCommand::~PxCommand() {
	delete line;
}

const char* PxCommand::get(size_t n) {
	if(n >= pos.size())
		throw PxParseError("Not enough arguments");
	return pos[n];
}

size_t PxCommand::len(size_t n) {
	return strlen(get(n));
}
