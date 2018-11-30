#include "stdio.h"

int main() {
  printf("");
  printf("AutoPlow starting...\n\n");

  printf("Attempting to begin serial data read on ARD1 (Motor Controller) at /dev/ttyACM0\n");
  FILE * ttyACM0;
  ttyACM0 = fopen("/dev/ttyACM0", "r");
  if (ttyACM0) {
    printf("Successfully opened serial stream\n\n");
    printf("Dumping contents:\n");
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), ttyACM0) != NULL) {
      printf("Input: ");
      printf(buffer);
    }
    printf("Closing file stream\n");
    fclose(ttyACM0);
  } else {
    printf("Unsuccessful in opening serial stream. Make sure the Arduino is plugged in to the RPI.\n");
    return 1;
  }
  return 0;
}