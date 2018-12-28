#include "ESPWebSetup.h"

ESPWebSetup webSetup("ESPSetup","12345678");


void setup()
{
  // put your setup code here, to run once:
  webSetup.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
  webSetup.update();
}
