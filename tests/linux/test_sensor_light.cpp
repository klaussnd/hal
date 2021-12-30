#include <hal/i2c_master.h>
#include <hal/sensor/si1145.h>

#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <thread>

#include <cstdint>
#include <ctime>
#include <unistd.h>

constexpr int DEFAULT_INTERVAL = 5;
const std::string DEFAULT_I2C_DEVICE = "/dev/i2c-2";

enum class MeasurementType
{
   NORMAL,
   EXPOSURE_TEST
};

struct Param
{
   std::string i2c_device{DEFAULT_I2C_DEVICE};
   MeasurementType type{MeasurementType::NORMAL};
   int interval{DEFAULT_INTERVAL};
};

Param parseArguments(const int argc, const char* const argv[]);
void standardMeasurement(int interval);
void exposureTest();
template <typename T>
void checkEqual(const T& nominal_value, const std::optional<T>& read_value,
                const std::string& message);

std::ostream& operator<<(std::ostream& os, const Si1145Gain gain);
std::ostream& operator<<(std::ostream& os, const Si1145Range range);
std::ostream& operator<<(std::ostream& os, const Si1145IrPhotodiode ir_photodiode);

int main(int argc, char** argv)
{
   Param param = parseArguments(argc, argv);

   if (!i2cMasterInit(param.i2c_device.c_str()))
   {
      std::cerr << "Unable to initialise i2c " << param.i2c_device << "\n";
      return EXIT_FAILURE;
   }
   if (!si1145Init())
   {
      std::cerr << "Unable to initialise SI1145\n";
      return EXIT_FAILURE;
   }

   switch (param.type)
   {
   case MeasurementType::NORMAL:
      standardMeasurement(param.interval);
      break;
   case MeasurementType::EXPOSURE_TEST:
      exposureTest();
      break;
   }

   return EXIT_SUCCESS;
}

void standardMeasurement(int interval)
{
   while (1)
   {
      si1145StartMeasurement();
      const auto data = si1145ReadMeasurement();
      if (data)
      {
         std::cout << std::time(nullptr) << " VIS " << data->vis << " IR " << data->ir
                   << std::endl;
      }
      else
      {
         std::cout << "measurement error" << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::seconds(interval));
   }
}

void exposureTest()
{
   std::random_device randev;
   std::mt19937 rng(randev());

   std::array<Si1145Gain, 8> gains = {
      Si1145Gain::DIV_1,  Si1145Gain::DIV_2,  Si1145Gain::DIV_4,  Si1145Gain::DIV_8,
      Si1145Gain::DIV_16, Si1145Gain::DIV_32, Si1145Gain::DIV_64, Si1145Gain::DIV_128};
   std::shuffle(gains.begin(), gains.end(), rng);
   std::array<Si1145Range, 2> ranges = {Si1145Range::NORMAL, Si1145Range::HIGH};
   std::shuffle(ranges.begin(), ranges.end(), rng);
   std::array<Si1145IrPhotodiode, 2> ir_photodiodes = {Si1145IrPhotodiode::SMALL,
                                                       Si1145IrPhotodiode::LARGE};
   std::shuffle(ir_photodiodes.begin(), ir_photodiodes.end(), rng);

   std::cout << "range,gain,ir_photodiode,vis,ir" << std::endl;
   for (const auto gain : gains)
   {
      for (const auto range : ranges)
      {
         for (const auto ir_photodiode : ir_photodiodes)
         {
            if (!si1145SetVisMode(range, gain))
            {
               std::cerr << "Unable to set VIS mode!" << std::endl;
               return;
            }
            if (!si1145SetIrMode(range, gain, ir_photodiode))
            {
               std::cerr << "Unable to set IR mode!" << std::endl;
               return;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(64));

            checkEqual(range, si1145GetVisRange(), "VIS range");
            checkEqual(gain, si1145GetVisGain(), "VIS gain");
            checkEqual(range, si1145GetIrRange(), "IR range");
            checkEqual(gain, si1145GetIrGain(), "IR gain");
            checkEqual(ir_photodiode, si1145GetIrPhotoduiode(), "IR photodiode");

            for (int i = 0; i < 10; ++i)
            {
               bool ok = false;
               for (int try_count = 0; !(ok = si1145StartMeasurement()) && try_count < 10;
                    ++try_count)
               {
                  std::cerr << "Unable to start measurement" << std::endl;
                  std::this_thread::sleep_for(std::chrono::microseconds(64));
               }
               if (!ok)
               {
                  std::cerr << "Failed" << std::endl;
                  return;
               }
               const auto meas = si1145ReadMeasurement();
               if (meas)
               {
                  std::cout << range << ',' << gain << ',' << ir_photodiode << ','
                            << meas.value().vis << ',' << meas.value().ir << std::endl;
               }
               else
               {
                  std::cerr << "Unable to read measurements" << std::endl;
               }
               std::this_thread::sleep_for(std::chrono::microseconds(64));
            }
         }
      }
   }
}

Param parseArguments(const int argc, const char* const argv[])
{
   namespace po = boost::program_options;

   try
   {
      Param param;

      // argument specification
      // clang-format off
      po::options_description desc_gen("Options");
      desc_gen.add_options()
            ("help,h", "Help")
            ("device,d", po::value<std::string>(&param.i2c_device), "i2c device")
            ("interval,i", po::value<int>(&param.interval), "Measurement interval")
            ("exposure,e", "Run exposure time series")
            ;
      // clang-format on

      po::variables_map vm;
      po::command_line_parser parser(argc, argv);
      po::store(parser.options(desc_gen).style(0).run(), vm);
      po::notify(vm);

      if (vm.count("help"))
      {
         std::cout << desc_gen << std::endl;
         exit(0);
      }
      if (vm.count("exposure"))
      {
         param.type = MeasurementType::EXPOSURE_TEST;
      }

      return param;
   }
   catch (po::error& err)
   {
      std::cerr << "Argument error: " << err.what() << std::endl;
      exit(1);
   }
   catch (std::exception& err)
   {
      std::cerr << "Error in argument parsing: " << err.what() << std::endl;
      exit(1);
   }
}

template <typename T>
void checkEqual(const T& nominal_value, const std::optional<T>& read_value,
                const std::string& message)
{
   if (!read_value.has_value())
   {
      std::cerr << "Error: Unable to read " << message << std::endl;
   }
   else if (read_value.value() != nominal_value)
   {
      std::cerr << "Error: " << message << " could not be set: nominal " << nominal_value
                << " vs. actual " << read_value.value() << std::endl;
   }
}

std::ostream& operator<<(std::ostream& os, const Si1145Gain gain)
{
   os << static_cast<int>(logicalValue(gain));
   return os;
}

std::ostream& operator<<(std::ostream& os, const Si1145Range range)
{
   os << asString(range);
   return os;
}

std::ostream& operator<<(std::ostream& os, const Si1145IrPhotodiode ir_photodiode)
{
   os << asString(ir_photodiode);
   return os;
}
