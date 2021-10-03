#include "i2c_master_detail.h"
#include <hal/avr/pin_io.h>

#include <avr/io.h>
#include <util/twi.h>

#if defined __AVR_ATtiny84__ || defined __AVR_ATtiny84A__ || defined __AVR_ATtiny44__    \
   || defined __AVR_ATtiny44A__ || defined __AVR_ATtiny2313__                            \
   || defined __AVR_ATtiny2313A__
#error "Use USI driver for ATtiny devices"
#elif defined __AVR_ATmega644__
#define TW_SCL_PIN C, 0
#define TW_SDA_PIN C, 1
#else  // ATmega8, 168 et al
#define TW_SCL_PIN C, 5
#define TW_SDA_PIN C, 4
#endif

#ifndef I2C_BITRATE
#define I2C_BITRATE 76000UL
#endif

bool i2cMasterInit()
{
   confPinAsInput(TW_SDA_PIN);
   confPinAsInput(TW_SCL_PIN);

   TWSR &= ((1 << TWPS0) | (1 << TWPS1));  // no prescaler
   // Register setting for bus speed; prescaler is always 1.
   constexpr uint8_t twbr = static_cast<uint8_t>(((F_CPU / I2C_BITRATE) - 16UL) / 2UL);
   static_assert(F_CPU / I2C_BITRATE > 16 && twbr >= 11, "I2C speed too fast");
   TWBR = twbr;

   return true;
}

I2cStatus i2cStart(uint8_t address, I2cOperation operation)
{
   // Send START condition
   TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));
   // Wait until transmission completed
   while (!(TWCR & (1 << TWINT)))
      ;
   // Check value of TWI Status Register. Mask prescaler bits.
   const uint8_t twst = TW_STATUS;
   bool success = twst == TW_START || twst == TW_REP_START;
   if (!success)
   {
      return I2cStatus::START_ERROR;
   }
   const uint8_t addrbyte = (address << 1) | (static_cast<uint8_t>(operation) & 0x01);
   success = i2cWriteByte(addrbyte, I2cWriteType::ADDRESS);
   return success ? I2cStatus::SUCCESS : I2cStatus::ADDRESS_ERROR;
}

bool i2cWriteByte(uint8_t data, I2cWriteType type)
{
   // Send dat
   TWDR = data;
   TWCR = ((1 << TWINT) | (1 << TWEN));
   // Wait until transmission completed
   while (!(TWCR & (1 << TWINT)))
      ;
   // Get value of TWI Status Register. Mask prescaler bits.
   const uint8_t status = TW_STATUS;
   return type == I2cWriteType::ADDRESS
             ? status == TW_MR_SLA_ACK || status == TW_MT_SLA_ACK
             : status == TW_MT_DATA_ACK;
}

bool i2cReadByte(uint8_t* data, I2cAck ack)
{
   // Setup TWI
   TWCR = (1 << TWEN) | (1 << TWINT) | ((ack == I2cAck::ACK) ? (1 << TWEA) : 0);
   // Wait for TWI to complete.
   while (!(TWCR & (1 << TWINT)))
      ;
   // Get value of TWI Status Register. Mask prescaler bits.
   const uint8_t status = TW_STATUS;
   const bool success = (ack == I2cAck::ACK && status == TW_MR_DATA_ACK)
                        || (ack == I2cAck::NACK && status == TW_MR_DATA_NACK);
   if (success)
   {
      // Get data byte
      *data = TWDR;
   }
   return success;
}

void i2cStop()
{
   // Send stop condition
   TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO));
   /* Wait until stop condition is executed and bus released
      Note: After sending a stop condition, TWINT is _not_ set again;
      however, TWSTO is cleared after the condition has been executed. */
   while (TWCR & (1 << TWSTO))
      ;
}
