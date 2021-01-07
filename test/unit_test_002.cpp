//
//    FILE: unit_test_002.cpp
//  AUTHOR: Tomas Hübner
//    DATE: 2021-01-06
// PURPOSE: unit test for the I2C_eeprom_cyclic_store class of the I2C_EEPROM library
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

#include <ArduinoUnitTests.h>

#include "Arduino.h"
#include "I2C_eeprom.h"
#include "I2C_eeprom_cyclic_store.h"

#define I2C_EEPROM_ADDR 0x50
#define I2C_EEPROM_SIZE 0x1000 // 4096

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
 * Verify that I2C_eeprom_cyclic_store fails to 
 * initialize when unable to read from eeprom.
 */
unittest(cyclic_store_fails_begin)
{
  Wire.resetMocks();

  auto mosi = Wire.getMosi(I2C_EEPROM_ADDR);

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  I2C_eeprom_cyclic_store<DummyTestData> CS;
  assertEqual(false, CS.begin(EE, 32, 4));
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
 * Verify that I2C_eeprom_cyclic_store successfully finds
 * the last entry before the entire eeprom has been filled.
 */
unittest(cyclic_store_double_finds_last_half_filled_eeprom)
{
  Wire.resetMocks();

  auto mosi = Wire.getMosi(I2C_EEPROM_ADDR);

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  auto miso = Wire.getMiso(I2C_EEPROM_ADDR);

  uint32_t tmp = 1;
  miso->push_back(((uint8_t*)&tmp)[0]);
  miso->push_back(((uint8_t*)&tmp)[1]);
  miso->push_back(((uint8_t*)&tmp)[2]);
  miso->push_back(((uint8_t*)&tmp)[3]);

  tmp = 2;
  miso->push_back(((uint8_t*)&tmp)[0]);
  miso->push_back(((uint8_t*)&tmp)[1]);
  miso->push_back(((uint8_t*)&tmp)[2]);
  miso->push_back(((uint8_t*)&tmp)[3]);

  tmp = 3;
  miso->push_back(((uint8_t*)&tmp)[0]);
  miso->push_back(((uint8_t*)&tmp)[1]);
  miso->push_back(((uint8_t*)&tmp)[2]);
  miso->push_back(((uint8_t*)&tmp)[3]);

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
  assertEqual(4, writes);
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

/**
 * Verify that I2C_eeprom_cyclic_store correctly 
 * increments the number of writes when saving
 * new versions.
 */
unittest(cyclic_store_increments_write_counter)
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
  uint32_t originalWrites;

  CS.getMetrics(slots, originalWrites);

  DummyTestData data;

  CS.write(data);
  CS.write(data);

  uint32_t newWrites;

  CS.getMetrics(slots, newWrites);

  assertEqual(4, slots);
  assertEqual(2, newWrites);
  assertNotEqual(originalWrites, newWrites);
}

/**
 * Check that the format() call of I2C_eeprom_cyclic_store
 * writes to the correct places when buffer size is smaller
 * than page size
 */
unittest(cyclic_store_format_single_page)
{
  Wire.resetMocks();

  auto mosi = Wire.getMosi(I2C_EEPROM_ADDR);

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  I2C_eeprom_cyclic_store<uint8_t[20]> CS;
  CS.begin(EE, 32, 4);

  mosi->clear();

  CS.format();

  // It should write exactly 24 bytes to the eeprom
  assertEqual(24, mosi->size());

  // CHeck that it writes empty marker to 0, 32, 64 and 96
  uint8_t expected[24] = {0,0,0xff,0xff,0xff,0xff,0,32,0xff,0xff,0xff,0xff,0,64,0xff,0xff,0xff,0xff,0,96,0xff,0xff,0xff,0xff};

  for(int i = 0; i < 24; i++)
  {
    assertEqual(expected[i], mosi->front());
    mosi->pop_front();
  }
}

/**
 * Check that the format() call of I2C_eeprom_cyclic_store
 * writes to the correct places when buffer is two pages large.
 */
unittest(cyclic_store_format_double_page)
{
  Wire.resetMocks();

  auto mosi = Wire.getMosi(I2C_EEPROM_ADDR);

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  I2C_eeprom_cyclic_store<uint8_t[40]> CS;
  CS.begin(EE, 32, 4);

  mosi->clear();

  CS.format();

  // It should write exactly 12 bytes to the eeprom
  assertEqual(12, mosi->size());

  // CHeck that it writes empty marker to 0 and 64
  uint8_t expected[12] = {0,0,0xff,0xff,0xff,0xff,0,64,0xff,0xff,0xff,0xff};

  for(int i = 0; i < 12; i++)
  {
    assertEqual(expected[i], mosi->front());
    mosi->pop_front();
  }
}

/**
 * Check that the format() call of I2C_eeprom_cyclic_store
 * writes to the correct places when the buffer is two pages
 * large and an odd number of pages is available.
 */
unittest(cyclic_store_format_double_page_odd_space)
{
  Wire.resetMocks();

  auto mosi = Wire.getMosi(I2C_EEPROM_ADDR);

  I2C_eeprom EE(I2C_EEPROM_ADDR, I2C_EEPROM_SIZE);
  EE.begin();

  I2C_eeprom_cyclic_store<uint8_t[40]> CS;
  CS.begin(EE, 32, 5);

  mosi->clear();

  CS.format();

  // It should write exactly 12 bytes to the eeprom
  assertEqual(12, mosi->size());

  // CHeck that it writes empty marker to 0 and 64
  uint8_t expected[12] = {0,0,0xff,0xff,0xff,0xff,0,64,0xff,0xff,0xff,0xff};

  for(int i = 0; i < 12; i++)
  {
    assertEqual(expected[i], mosi->front());
    mosi->pop_front();
  }
}

/**
 * Check that writing buffers to the store makes it wrap
 * to the start of the memory once it has been filled
 * using a single page buffer.
 */
unittest(cyclic_store_wrapping_single_page)
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

  I2C_eeprom_cyclic_store<uint8_t[20]> CS;
  assertEqual(true, CS.begin(EE, 32, 4));
  assertEqual(2, mosi->size());

  uint8_t dummy[20];

  CS.write(dummy);
  CS.write(dummy);
  CS.write(dummy);

  mosi->clear();

  CS.write(dummy);

  // It should write exactly 22 bytes to the eeprom
  assertEqual(22, mosi->size());

  // check that the address is 0
  assertEqual(0, mosi->front());
  mosi->pop_front();
  assertEqual(0, mosi->front());
  mosi->pop_front();
}

/**
 * Check that writing buffers to the store makes it wrap
 * to the start of the memory once it has been filled
 * using a single page buffer.
 */
unittest(cyclic_store_wrapping_double_page)
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

  uint8_t dummy[40];

  CS.write(dummy);
  CS.write(dummy);

  mosi->clear();

  CS.write(dummy);

  // It should write exactly 42 bytes to the eeprom
  assertEqual(42, mosi->size());

  // check that the address is 0
  assertEqual(0, mosi->front());
  mosi->pop_front();
  assertEqual(0, mosi->front());
  mosi->pop_front();
}

/**
 * Check that writing buffers to the store makes it wrap
 * to the start of the memory once it has been filled
 * using a single page buffer.
 */
unittest(cyclic_store_wrapping_double_page_odd_space)
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
  assertEqual(true, CS.begin(EE, 32, 5));

  uint8_t dummy[40];

  CS.write(dummy);
  CS.write(dummy);

  mosi->clear();

  CS.write(dummy);

  // It should write exactly 42 bytes to the eeprom
  assertEqual(42, mosi->size());

  // check that the address is 0
  assertEqual(0, mosi->front());
  mosi->pop_front();
  assertEqual(0, mosi->front());
  mosi->pop_front();
}

unittest_main()

// --------
