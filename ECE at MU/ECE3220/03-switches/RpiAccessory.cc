/* RpiAccessory.cc */

#include "RpiAccessory.h"

namespace ece3220 {

/** \brief Default constructor
 */
RpiAccessory :: RpiAccessory ()
{
    // wiringPi library setup() call
    wiringPi::wiringPiSetup();

    /* LEDs */

    // Configure the GPIO pins that are connected to the four LEDs on the
    // accessory board, and then turn OFF the LEDs.
    wiringPi::pinMode( static_cast<int>(Led::RED), OUTPUT );
    wiringPi::pinMode( static_cast<int>(Led::YELLOW), OUTPUT );
    wiringPi::pinMode( static_cast<int>(Led::GREEN), OUTPUT );
    wiringPi::pinMode( static_cast<int>(Led::BLUE), OUTPUT );
    LedOffAll();
    
    /* Push buttons and slider switches */
    
    //config GPIO pins connected to buttons and switches as inputs
    wiringPi::pinMode(static_cast<int>(Switch::BTN1), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::BTN2), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::BTN3), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::BTN4), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::BTN5), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::SW1), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::SW2), INPUT);
}


/** \brief Destructor
 */
RpiAccessory :: ~RpiAccessory()
{
    /*
     * Configure all GPIO pins as INPUTs
     */

    // LED GPIO pins
    wiringPi::pinMode( static_cast<int>(Led::RED), INPUT );
    wiringPi::pinMode( static_cast<int>(Led::YELLOW), INPUT );
    wiringPi::pinMode( static_cast<int>(Led::GREEN), INPUT );
    wiringPi::pinMode( static_cast<int>(Led::BLUE), INPUT );
    
    //Switch and button GPIO pins
    wiringPi::pinMode(static_cast<int>(Switch::BTN1), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::BTN2), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::BTN3), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::BTN4), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::BTN5), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::SW1), INPUT);
    wiringPi::pinMode(static_cast<int>(Switch::SW2), INPUT);
}


/** \brief Gets a reference to the singleton instance of the RpiAccessory class.
 *  
 *  The singleton instance of the RpiAccessory class is constructed in a
 *  thread-safe way the first time this method is called. See also [1] in
 *  `readme.txt'.
 * 
 *  \return Reference to the static sintleton instance.
 */
RpiAccessory &
RpiAccessory :: GetInstance()
{
    static RpiAccessory  sInstance ;
    return  sInstance ;
}


/** \brief Turns ON|OFF all LEDs on the accessory board. 
 *
 * \param state HIGH or 1 turns ON all LEDs. LOW or 0 turns OFF all LEDs.
 */
void
RpiAccessory :: LedWriteAll ( int state )
{
    // Write the value `state' to all LEDs
    LedWrite( Led::RED, state );
    LedWrite( Led::YELLOW, state );
    LedWrite( Led::GREEN, state );
    LedWrite( Led::BLUE, state );
}

} // namespace ece3220

