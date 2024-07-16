/* ESP8266 sensor on Wemos D1 mini
 *
 * SCL on D1 (GPIO5) green
 * SDA on D2 (GPIO4) yellow
 */

#include <hal/i2c_master.h>
#include <hal/sensor/veml6075.h>

#include <Arduino.h>

void setup()
{
   Serial.begin(57600);
   Serial.println("Start");

   i2cMasterInit();
   const bool has_veml6075 = veml6075Init();
   if (!has_veml6075)
   {
      Serial.println("Unable to initialise VEML6075");
   }
}

void loop()
{
   const auto data = veml6075Measure();
   if (data.has_value())
   {
      Serial.print("Measurement ok: UVA ");
      Serial.print(data->uva);
      Serial.print(" UVB ");
      Serial.print(data->uvb);
      Serial.println();
   }
   else
   {
      Serial.println("Measurement failed");
   }

   delay(500);
}
