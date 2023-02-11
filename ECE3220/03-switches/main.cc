/* main.cc */

#include <atomic>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include "RpiAccessory.h"

// NOTES
// 1) On the accessory board, switch contact bounce is debounced in
//    hardware; therefore, there's no need to implement software debouncing
//    herein.
// 2) When using the wiringPi library, user-defined "interrupt service
//    routines" (ISR) are not true ISRs--i.e., they do not execute within
//    the Linux kernel's execution context. These functions are, in fact,
//    application-layer callback functions that execute in a separate
//    thread of execution from the main() program thread.
// 3) According to the wiringPi documentation, the ISR callback function
//    "runs concurrently with the main program", which implies this
//    function executes in a separate thread of exection from the main
//    program.
// 4) It is entirely possible to have multiple threads executing the same
//    callback function concurrently.  Therefore, your callback function
//    must be written so that it is thread-safe.
//    See also: https://en.wikipedia.org/wiki/Thread_safety
//


using namespace ece3220;
using namespace std;

atomic_bool gPushButton1Pressed {false};

mutex gMutexCout;

void ISR_PushButton1Press(void){
    gPushButton1Pressed.store(true);
    
    {//critical section for cout mutex
        lock_guard<std::mutex> lg{ gMutexCout };
        cout << "ISR_PushButton1Press()" << endl;
    }
}
    
void ISR_PushButton5Press(void){
    gPushButton5Pressed.store(true);
    
    {
        lock_guard<std::mutex> lg{ gMutexCout };
        cout << "ISR_PushButton5Press()" << endl;
    }
}

//========================================================================
//  main()
//========================================================================

int main()
{
    int exit_code = 0;

    RpiAccessory &a = RpiAccessory::GetInstance();

    try {
        //Register ISR with the wiringPi library
        
        a.RegisterSwitchIsr(RpiAccessory::Switch::BTN1, 
                            RpiAccessory::EdgeType::RISING, 
                            ISR_PushButton1Press );
        
        a.RegisterSwitchIsr(RpiAccessory::Switch::BTN5, 
                            RpiAccessory::EdgeType::RISING, 
                            ISR_PushButton5Press );
        //while loop to detect button presses
        do{
            if(gPushButton5Pressed.load()){
                //reset flag for gPushButton5Pressed
                gPushButton5Pressed.store(false);
                
                lock_guard<std::mutex> lg{ gMutexCout };
                //output message to stdout
                cout << "main(): Push button 5 pressed" << endl;
            }
        }while(!gPushButton1Pressed.load());
        
    }
    catch ( std::runtime_error const &e ) {
        cerr << "\n:: ERROR :: " << e.what() << "; aborting...\n"; 
        exit_code = 1;
    }
    catch ( ... ) {
        cerr << "\n:: ERROR :: Unexpected exception thrown; aborting...\n";
        exit_code = 1;
    }

    return exit_code;
}

