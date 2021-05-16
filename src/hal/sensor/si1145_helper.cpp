#include <hal/sensor/si1145.h>
#include <utils/programspace.h>

uint8_t logicalValue(const Si1145Gain gain)
{
   switch (gain)
   {
   case Si1145Gain::DIV_1:
      return 1;
   case Si1145Gain::DIV_2:
      return 2;
   case Si1145Gain::DIV_4:
      return 4;
   case Si1145Gain::DIV_8:
      return 8;
   case Si1145Gain::DIV_16:
      return 16;
   case Si1145Gain::DIV_32:
      return 32;
   case Si1145Gain::DIV_64:
      return 64;
   case Si1145Gain::DIV_128:
      return 128;
   }
   return 0;
}

const char* asString(const Si1145Range range)
{
   switch (range)
   {
   case Si1145Range::NORMAL:
      return PSTR("normal");
   case Si1145Range::HIGH:
      return PSTR("high");
   }
   return nullptr;
}

const char* asString(const Si1145IrPhotodiode ir_photodiode)
{
   switch (ir_photodiode)
   {
   case Si1145IrPhotodiode::SMALL:
      return PSTR("small");
   case Si1145IrPhotodiode::LARGE:
      return PSTR("large");
   }

   return nullptr;
}
