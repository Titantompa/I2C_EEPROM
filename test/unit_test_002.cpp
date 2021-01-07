//
//    FILE: unit_test_002.cpp
//  AUTHOR: Tomas Hübner
//    DATE: 2021-01-06
// PURPOSE: unit test for the I2C_EEPROM library
//          https://github.com/RobTillaart/
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// https://github.com/Arduino-CI/arduino_ci/blob/master/cpp/unittest/Assertion.h#L33-L42
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)
// assertNotNull(actual)

#include <ArduinoUnitTests.h>

#define assertEqualINF(arg)  assertOp("assertEqualINF", "expected", INFINITY, compareEqual, "==", "actual", arg)
#define assertEqualNAN(arg)  assertOp("assertEqualNAN", "expected", true, compareEqual, "==", "actual", isnan(arg))

#define I2C_EEPROM_ADDR 0x50
#define I2C_EEPROM_SIZE 0x1000 // 4096

#include "Arduino.h"
#include "I2C_eeprom.h"
#include "I2C_eeprom_cyclic_store.h"

struct DummyTestData {
  uint8_t padding;
};

unittest_setup()
{
}

unittest_teardown()
{
}

unittest(wire_begin)
{
  I2C_eeprom EE(0x50);
  EE.begin();
  
  assertTrue(Wire.didBegin());
}

unittest(wire_cs_begin)
{
  Wire.resetMocks();

  std::deque<uint8_t>* mosi = Wire.getMosi(I2C_EEPROM_ADDR);
  assertEqual(0, mosi->size());

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();
  
  assertTrue(Wire.didBegin());
  assertEqual(0, mosi->size());

  // It will read three times
  auto miso = Wire.getMiso(I2C_EEPROM_ADDR);
  miso->push_back(0x0);
  miso->push_back(0x0);
  miso->push_back(0x0);
  miso->push_back(0x0);
  miso->push_back(0xff);
  miso->push_back(0xff);
  miso->push_back(0xff);
  miso->push_back(0xff);

  I2C_eeprom_cyclic_store<DummyTestData> CS;
  auto success = CS.begin(EE, 32, 4);
  assertEqual(true, success);
  assertEqual(4, mosi->size());

  cerr << "Remaining on MISO " << miso->size() << endl;

  uint16_t slots;
  uint32_t writes;

  CS.getMetrics(slots, writes);

  cerr << "Slots = " << slots << " and Writes = " << writes << endl;




}

unittest_main()

// --------
