/* 
 * File:   PxCommand.h
 * Author: marc
 *
 * Created on March 22, 2015, 2:52 PM
 */

#ifndef PXCOMMAND_H
#define	PXCOMMAND_H

#include <string>
#include <vector>
#include <exception>
#include "utils.h"
#include "NetSession.h"
#include <memory>

class PxCommand : NonCopyable {
public:
    PxCommand(NetSession *client, char * line, size_t n);
    ~PxCommand();

    bool parse(size_t i, unsigned int &a, int base=10);

    size_t nargs();
    size_t len(size_t n);
    const char* get(size_t n);
    NetSession &getClient();

private:
    NetSession *client;
    char * line;
    std::vector<char*> pos;
};

class PxParseError: public std::exception {
public:
	explicit PxParseError(const char * what): msg_(what) {};
    explicit PxParseError(const std::string& message): msg_(message){};

    virtual const char* what() const throw() {
        return msg_.c_str();
    }

protected:
    std::string msg_;
};

#endif	/* PXCOMMAND_H */

