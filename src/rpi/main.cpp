#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

int main() {
  std::cout << "AutoPlow starting..." << std::endl;

  std::cout << "Attempting to set proper device configuration on ARD1" << std::endl;
  system("stty -F /dev/ttyACM0 9600 raw -clocal -echo");

  std::cout << "Attempting to begin serial data read on ARD1 (Motor Controller) at /dev/ttyACM0" << std::endl;
  std::fstream ttyACM0;
  ttyACM0.open("/dev/ttyACM0", std::fstream::in | std::fstream::out | std::fstream::app);
  if (ttyACM0.good()) {
    std::cout << "Successfully opened serial stream" << std::endl;
    std::cout << "Attempting device reset" << std::endl;
    std::cout << "Dumping contents:" << std::endl;
    ttyACM0 << "Restart;";
    char buffer[256];
    std::string string;
    ttyACM0 << "b1;";
    while (ttyACM0.getline(buffer, sizeof(buffer))) {
      string = std::string(buffer);
      std::cout << string << std::endl;
    }
    std::cout << "Closing file stream" << std::endl;
    ttyACM0.close();
  } else {
    std::cout << "Unsuccessful in opening serial stream. Make sure the Arduino is plugged in to the RPI." << std::endl;
    return 1;
  }
  return 0;
}
