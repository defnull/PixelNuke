/*
 * File:   main.cpp
 * Author: marc
 *
 * Created on March 20, 2015, 3:20 PM
 */

#include <cstdlib>
#include <exception>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

#include "PixelServer.h"
namespace {
    class OptParser {
    public:
        OptParser(int argc, char ** argv): argc(argc), argv(argv) {}
        bool get(const char* name, int &a) {
            for(int i=1; i<argc-1; i++) {
                if(strcmp(name, argv[i]))
                    continue;
                a = std::stoi(argv[i+1]);
                return true;
            }
            return false;
        }
    private:
        int argc;
        char ** argv;
    };
}


int main(int argc, char* argv[]) {
    OptParser opt(argc, argv);

    int port = 0;
    opt.get("-p", port) || opt.get("--port", port);

    try {
        PixelServer px;
        px.run();
    } catch (std::exception const& x) {
        std::cerr << "Exception: " << x.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

