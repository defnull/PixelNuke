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
using namespace std;

int main(int argc, char** argv) {
        printf("bar");

    auto win = UIWindow();
    auto net = Net();
    net.watch(8080);
    
    //std::thread netThread (&Net::loop, net);
    //netThread.detach();
    printf("bar");
    win.loop();
    return 0;
}

