#include <gmock/gmock.h>

#include <hal/communication/can_message.h>
#include <utils/can_message.h>

using testing::Eq;

namespace
{
const uint8_t ARBITRARY_SOURCE_DEVICE = 0x0a;
const uint8_t ARBITRARY_TARGET_DEVICE = 0x01;
}  // namespace

namespace CanMsg
{
constexpr CanId WITH_SOURCE_AND_TARGET = {0x630000, true, true, 1, 0};
constexpr CanId WITH_ONLY_SOURCE = {0x810100, true, false, 3, 2};
constexpr CanId ANOTHER_WITH_SOURCE = {0x810200, true, false, 2, 0};
}  // namespace CanMsg

TEST(MakeCanId, forIdWithSourceAndTarget)
{
   const uint32_t id = makeCanId(CanMsg::WITH_SOURCE_AND_TARGET, ARBITRARY_SOURCE_DEVICE,
                                 ARBITRARY_TARGET_DEVICE);

   ASSERT_THAT(id, Eq(0x63010a));
}

TEST(MakeCanID, forIdWithOnlySource)
{
   const uint32_t id = makeCanId(CanMsg::WITH_ONLY_SOURCE, ARBITRARY_SOURCE_DEVICE,
                                 ARBITRARY_TARGET_DEVICE);

   ASSERT_THAT(id, Eq(0x81010a));
}

TEST(CheckCanID, forIdWithSource)
{
   const uint8_t another_device = 2;
   const uint32_t id = makeCanId(CanMsg::ANOTHER_WITH_SOURCE, ARBITRARY_SOURCE_DEVICE,
                                 ARBITRARY_TARGET_DEVICE);

   ASSERT_TRUE(isMatchingCanId(CanMsg::ANOTHER_WITH_SOURCE, another_device, id));
}

TEST(CheckCanID, doesNotMatchForDifferentTypeAndIdWithSource)
{
   const uint32_t id = makeCanId(CanMsg::WITH_ONLY_SOURCE, ARBITRARY_SOURCE_DEVICE,
                                 ARBITRARY_TARGET_DEVICE);

   ASSERT_FALSE(
      isMatchingCanId(CanMsg::ANOTHER_WITH_SOURCE, ARBITRARY_TARGET_DEVICE, id));
}

TEST(MakeCanMessage, makesMessage)
{
   const auto msg = CanMsg::makeBasicCanMessage(
      CanMsg::WITH_SOURCE_AND_TARGET, ARBITRARY_SOURCE_DEVICE, ARBITRARY_TARGET_DEVICE);

   ASSERT_THAT(msg.id, Eq(0x63010a));
   ASSERT_FALSE(msg.flags.rtr);
   ASSERT_TRUE(msg.flags.extended);
   ASSERT_THAT(msg.length, Eq(CanMsg::WITH_SOURCE_AND_TARGET.length));
}

TEST(SerialiseAndDeserialise, reobtainsOriginalValue)
{
   const uint16_t arbitrary_value = 0x1f34;

   uint8_t buf[2];
   CanMsg::serialise(arbitrary_value, buf);
   const auto obtained_value = CanMsg::deserialise<uint16_t>(buf);

   ASSERT_THAT(obtained_value, Eq(arbitrary_value));
}
