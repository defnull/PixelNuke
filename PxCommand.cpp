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

PxCommand::PxCommand(NetSession *client, char * line, const size_t n):
	client(client) {

    std::string str(line, n);
    std::stringstream ss(str);
    std::string buf;

    while (ss >> buf)
		args.push_back(std::move(buf));

}

bool PxCommand::parse(size_t index, unsigned int &a, int base) {
	if(index >= args.size())
		throw PxParseError("Not enough arguments");
	size_t p;
	a = std::stoul(args[index], &p, base);
	return p != args[index].size();
}

size_t PxCommand::nargs() {
	return args.size();
}

PxCommand::~PxCommand() {
}

const std::string& PxCommand::get(size_t n) {
	if(n >= args.size())
		throw PxParseError("Not enough arguments");
	return args[n];
}

size_t PxCommand::len(size_t n) {
	if(n >= args.size())
		throw PxParseError("Not enough arguments");
	return args[n].size();
}

NetSession& PxCommand::getClient() {
	return *client;
}
