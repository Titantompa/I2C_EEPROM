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

/**
 * Verify that I2C_eeprom_cyclic_store successfully 
 * initializes on empty eeprom.
 */
unittest(cyclic_store_empty_begin)
{
  Wire.resetMocks();

  auto mosi = Wire.getMosi(I2C_EEPROM_ADDR);

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();
  
  auto miso = Wire.getMiso(I2C_EEPROM_ADDR);
  miso->push_back(0xff);
  miso->push_back(0xff);
  miso->push_back(0xff);
  miso->push_back(0xff);

  I2C_eeprom_cyclic_store<DummyTestData> CS;
  assertEqual(true, CS.begin(EE, 32, 4));
  assertEqual(2, mosi->size());
}

/**
 * Verify that I2C_eeprom_cyclic_store reports
 * the correct metrics on empty eeprom.
 */
unittest(cyclic_store_empty_metrics)
{
  Wire.resetMocks();

  auto mosi = Wire.getMosi(I2C_EEPROM_ADDR);

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  auto miso = Wire.getMiso(I2C_EEPROM_ADDR);
  miso->push_back(0xff);
  miso->push_back(0xff);
  miso->push_back(0xff);
  miso->push_back(0xff);

  I2C_eeprom_cyclic_store<DummyTestData> CS;
  assertEqual(true, CS.begin(EE, 32, 4));

  uint16_t slots;
  uint32_t writes;

  CS.getMetrics(slots, writes);

  assertEqual(4, slots);
  assertEqual(0, writes);
}

/**
 * Verify that I2C_eeprom_cyclic_store calculate
 * number of slots correctly
 */
unittest(cyclic_store_double_page_buffer)
{
  Wire.resetMocks();

  auto mosi = Wire.getMosi(I2C_EEPROM_ADDR);

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  auto miso = Wire.getMiso(I2C_EEPROM_ADDR);
  miso->push_back(0xff);
  miso->push_back(0xff);
  miso->push_back(0xff);
  miso->push_back(0xff);

  I2C_eeprom_cyclic_store<uint8_t[40]> CS;
  assertEqual(true, CS.begin(EE, 32, 4));

  uint16_t slots;
  uint32_t writes;

  CS.getMetrics(slots, writes);

  assertEqual(2, slots);
  assertEqual(0, writes);
}

/**
 * Verify that I2C_eeprom_cyclic_store fails to
 * initialize if buffer too large
 */
unittest(cyclic_store_fails_init_on_too_large_buffer)
{
  Wire.resetMocks();

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  I2C_eeprom_cyclic_store<uint8_t[400]> CS;
  assertEqual(false, CS.begin(EE, 32, 4));
}

/**
 * Verify that I2C_eeprom_cyclic_store fails to
 * fetch metrics if initialization has failed.
 */
unittest(cyclic_store_fails_metrics_when_failed_initialize)
{
  Wire.resetMocks();

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  I2C_eeprom_cyclic_store<uint8_t[400]> CS;
  assertEqual(false, CS.begin(EE, 32, 4));

  uint16_t slots;
  uint32_t writes;

  assertEqual(false, CS.getMetrics(slots, writes));
}

/**
 * Verify that I2C_eeprom_cyclic_store fails to
 * fetch metrics if initialization has failed.
 */
unittest(cyclic_store_fails_metrics_when_not_initialized)
{
  Wire.resetMocks();

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  I2C_eeprom_cyclic_store<uint8_t[400]> CS;

  uint16_t slots;
  uint32_t writes;

  assertEqual(false, CS.getMetrics(slots, writes));
}

unittest_main()

// --------
