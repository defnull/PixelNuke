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

namespace
{

bool parseInt ( const char* arg, unsigned int &a )
{
    unsigned int i = 0, r = 0;
    char c;
    while ( ( c = arg[i++] ) != '\0' ) {
        if ( '0' <= c && c <= '9' ) {
            r = ( r*10 ) + c - '0';
        } else {
            return false;
        }
    }
    a=r;
    return true;
}

bool parseHex ( const char* arg, unsigned int &a )
{
    unsigned int i = 0, r = 0;
    char c;
    while ( ( c = arg[i++] ) != '\0' ) {
        if ( '0' <= c && c <= '9' ) {
            r = ( r*16 ) + c - '0';
        } else if ( 'a' <= c && c <= 'f' ) {
            r = ( r*16 ) + 10 + c - 'a';
        } else if ( 'A' <= c && c <= 'F' ) {
            r = ( r*16 ) + 10 + c - 'A';
        } else {
            return false;
        }
    }
    a=r;
    return true;
}

}


void PxCommand::set ( char * newline, size_t n )
{
    if ( line ) {
        free ( line );
        pos.clear();
    }
    line = newline;

    for ( size_t i=0; i<n; i++ ) {
        if ( line[i] == ' ' ) {
            line[i] = '\0';
        } else if ( i==0 || line[i-1] == '\0' ) {
            pos.push_back ( line+i );
        }
    }
}

bool PxCommand::parse ( size_t index, unsigned int &a, int base )
{
    if ( base == 10 ) {
        return parseInt ( get ( index ), a );
    } else if ( base == 16 ) {
        return parseHex ( get ( index ), a );
    } else {
        char * end;
        a = std::strtoul ( get ( index ), &end, base );
        return *end == '\0';
    }
}

size_t PxCommand::nargs()
{
    return pos.size();
}

PxCommand::~PxCommand()
{
    free ( line );
}

const char* PxCommand::get ( size_t n )
{
    if ( n >= pos.size() ) {
        throw PxParseError ( "Not enough arguments" );
    }
    return pos[n];
}

size_t PxCommand::len ( size_t n )
{
    return strlen ( get ( n ) );
}
