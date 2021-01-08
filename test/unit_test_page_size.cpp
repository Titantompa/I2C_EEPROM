//
//    FILE: unit_test_page_size.cpp
//  AUTHOR: Tomas Hübner
//    DATE: 2021-01-08
// PURPOSE: unit test for the page size calculations of the I2C_eeprom
//          library.
//
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

#include <ArduinoUnitTests.h>

class I2C_eeprom_wrapper;

#define UNIT_TEST_FRIEND friend class I2C_eeprom_wrapper

#include "Arduino.h"
#include "I2C_eeprom.h"

#define I2C_EEPROM_ADDR 0x50
#define I2C_EEPROM_SIZE 0x1000 // 4096

class I2C_eeprom_wrapper {
    public:
        static uint8_t pageSize(I2C_eeprom &eeprom) { return eeprom._pageSize; }
};

unittest_setup()
{
}

unittest_teardown()
{
}

/**
 * Verify that an instance gets the default page size
 * when the constructor without device size is used.
 */
unittest(i2c_eeprom_default)
{
  Wire.resetMocks();

  I2C_eeprom eeprom(I2C_EEPROM_ADDR);

  assertEqual(I2C_EEPROM_PAGESIZE, I2C_eeprom_wrapper::pageSize(eeprom));
}

// ------------------
