

#include <ctime>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

// Function prototypes
void device_close( int device_id );
void device_configure( int device_id );
void device_message( const std::string& funcname, int device_id );
int  device_open();
void device_sleep( int device_id );

// Output formatting
static const std::size_t COLUMN1_WIDTH = 20;

// DO NOT MODIFY ANY CODE ABOVE THIS LINE

class device_guard {
	int id_ = device_open();
	
	device_guard(int id){
		id_ = id;
	}
	
	
	

	public device_id(){
		return id;
	}
	
	~device_guard(){
		device_close(id_);
	}
}
private:
  // These class methods are intentionally disabled.
  // Do not use these class methods.
  device_guard() = delete;
  device_guard( const device_guard& ) = delete;
  device_guard( const device_guard&& ) = delete;
  device_guard& operator=( const device_guard& ) = delete;
  device_guard& operator=( const device_guard&& ) = delete;
};

// DO NOT MODIFY ANY CODE BELOW THIS LINE
int main()
{
  int exit_code = 0;
  try {
    device_guard  dg{ device_open() };
    device_configure( dg.device_id() );
    // ...
    throw std::runtime_error("my error message");
    // ...
    device_sleep( dg.device_id() );
  }
  catch ( const std::runtime_error& e ) {
    std::cerr << std::left << std::setw(COLUMN1_WIDTH) 
      << "std::runtime_error" << ": " << e.what() << '\n';
    exit_code = 1;
  }
  catch ( ... ) {
    std::cerr << "Whoops! Unknown exception was thrown!\n";
  }
  return exit_code;
}
//-------------------------------------------------------------------------
void
device_close( int device_id )
{
  device_message( "device_close", device_id );
}
//-------------------------------------------------------------------------
void
device_configure( int device_id )
{
  device_message( "device_configure", device_id );
}
//-------------------------------------------------------------------------
int
device_open()
{
  unsigned int seed = static_cast<unsigned int>( time(NULL) );
  srand( seed );
  int device_id = rand();
  device_message( "device_open", device_id );
  return device_id;
}
//-------------------------------------------------------------------------
void
device_sleep( int device_id )
{
  device_message( "device_sleep", device_id );
}
//-------------------------------------------------------------------------
void
device_message( const std::string& funcname, int device_id )
{
  std::cout << std::left << std::setw(COLUMN1_WIDTH) << funcname
    << ": " << device_id << std::endl;
}
