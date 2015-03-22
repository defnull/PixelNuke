/* 
 * File:   main.cpp
 * Author: marc
 *
 * Created on March 20, 2015, 3:20 PM
 */

#include <cstdlib>
#include <thread>

#include "UIWindow.h"
#include "Net.h"
#include "PixelServer.h"
using namespace std;

int main(int argc, char** argv) {
    auto *px = new PixelServer();
    px->run();
    return 0;
}

