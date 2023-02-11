/* RpiAccessory.h */

#ifndef ECE3220_RPIACCESSORY_H
#define ECE3220_RPIACCESSORY_H

namespace wiringPi {
#include <wiringPi.h>
}

#pragma GCC diagnostic error "-Wreturn-type"

namespace ece3220 {


///////////////////////////////////////////////////////////////////////////
// 
//   class  RpiAccessory
//
class RpiAccessory {

/*******************************************************************/
/*  Singleton Design Pattern                                       */
/*  See also [1] in file 'readme.txt'                              */
/*******************************************************************/

public:

    // Accessor method that gets (returns) a reference to the singleton
    // instance of the RpiAccessory class.
    static RpiAccessory&  GetInstance();


    // Class destructor. This method must be public.
    ~RpiAccessory();


    // Singleton design pattern.
    // Disable automatic synthesis of these class methods
    //
    // Note: Scott Meyers' /Effective Modern C++/ book states that deleted
    //       methods should be public because this usually yields more
    //       informative diagnostics from the C++ compiler.
    RpiAccessory(RpiAccessory const &) = delete;              // copy constructor
    RpiAccessory& operator=(RpiAccessory const &) = delete;   // copy assignment
    RpiAccessory(RpiAccessory &&) = delete;                   // move constructor
    RpiAccessory& operator=(RpiAccessory &&) = delete;        // move assignment

private:
 
    // Default constructor. This method must be private.
    RpiAccessory();


/*******************************************************************/
/*  Light-Emitting Diodes                                          */
/*******************************************************************/

public:

    // The value of each enumerator within the Led enumeration is a
    // wiringPi library virtual pin number that corresponds to the physical
    // pin number on the RPI's 40-pin header that's connected to the
    // specified LED color on the accessory board.
    //
    enum class Led {
        RED     = 8,
        YELLOW  = 9,    
        GREEN   = 7,
        BLUE    = 21
    };


    /** \brief Turns ON|OFF all LEDs on the accessory board. 
     * \param state [in] HIGH or 1 turns ON all LEDs. LOW or 0 turns OFF all LEDs.
     */
    void LedWriteAll ( int state );


    /** \brief Turns ON|OFF and individual LED on the accessory board.
     * \param color [in] Any one of this class's Led enumerated values.
     * \param state [in] HIGH or 1 turns ON the LED. LOW or 0 turns OFF the LED.
     */
    inline void LedWrite ( Led color, int state )
    {
        wiringPi::digitalWrite( static_cast<int>(color), state );
    }


    /** \brief Turns OFF the the specified LED color
     * \param color [in] Any one of this class's Led enumerated values.
     */
    inline void LedOff( Led color )
    {
        wiringPi::digitalWrite( static_cast<int>(color), 0 );
    }


    /** \brief Turns OFF all LEDs
     */
    inline void LedOffAll()
    {
        LedWriteAll( 0 );
    }


    /** \brief Turns ON the the specified LED color
     * \param color [in] Any one of this class's Led enumerated values.
     */
    inline void LedOn( Led color )
    {
        wiringPi::digitalWrite( static_cast<int>(color), 1 );
    }


    /** \brief Turns ON all LEDs
     */
    inline void LedOnAll()
    {
        LedWriteAll( 1 );
    }


/*******************************************************************/
/*  Pushbutton and Slider Switches                                 */
/*******************************************************************/

public:

    // The value of each enumerator within the `Switch' enumeration is
    // the wiringPi virtual pin number that corresponds to the physical pin
    // number on the RPI's 40-pin header that's connected to the
    // specified pushbutton switch (BTN?) or slider switch (SW?) on the
    // accessory board.
    //
    enum class Switch {
        BTN1 =  27,   // left-most pushbutton
        BTN2 =  0,
        BTN3 =  1,
        BTN4 =  24,
        BTN5 =  28,   // right-most pushbutton
        SW1  =  26,   // left slider switch
        SW2  =  23,   // right slider switch
    };


/*******************************************************************/
/*  Wrapper methods for wiringPi library calls                     */
/*******************************************************************/

public:

    // Map the wiringPi library's C-style enums into a C++ 'enum class'
    // type.
    enum class EdgeType { 
        BOTH    = INT_EDGE_BOTH,
        FALLING = INT_EDGE_FALLING,
        RISING  = INT_EDGE_RISING,
        SETUP   = INT_EDGE_SETUP,
    };

    inline int
    RegisterSwitchIsr ( Switch s, EdgeType edgeType,
            void (*isr_callback_function)(void) )
    {
        /* your code here... */
        return wiringPi::wiringPiISR(static_cast<int>(s), 
                                     static_cast<int>(EdgeType), 
                                     isr_callback_function);
    }

};  // class RpiAccessory

} // namespace ece3220

#endif // ECE3220_RPIACCESSORY_H

