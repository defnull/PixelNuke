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

#include "Net.h"
#include "PixelServer.h"
#include "UIWindow.h"

int main(int argc, char* argv[]) {
    try {
        PixelServer px;
        px.run();
    } catch (std::exception const& x) {
        std::cerr << "Exception: " << x.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

