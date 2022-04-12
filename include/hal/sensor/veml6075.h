#pragma once

#include <cstdint>
#include <optional>

enum class Veml6075Trigger
{
    NO_TRIGGER,
    TRIGGER_ONE_OR_UV_TRIG,
};

enum class Veml6075IntegrationTime
{
    _50MS = 0,
    _100MS,
    _200MS,
    _400MS,
    _800MS,
};

struct Veml6075Data
{
   uint16_t uva;
   uint16_t uvb;
   uint16_t comp1;
   uint16_t comp2;
};

bool veml6075Init();
bool veml6075SetIntegrationTime(Veml6075IntegrationTime integration_time);
bool veml6075SetHighDynamic(bool enabled);
bool veml6075SetActiveForce(bool enabled);
bool Veml6075Trigger();
std::optional<Veml6075Data> veml6075Measure();
