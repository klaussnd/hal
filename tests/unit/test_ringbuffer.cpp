#include <gmock/gmock.h>

#include <utils/ringbuffer.h>

using namespace testing;

namespace
{
   const uint8_t BUFFER_SIZE = 8;
   const uint8_t ARBITRARY_VALUE = 42;
}

class ARingBuffer : public testing::Test
{
public:
   RingBuffer<uint8_t, BUFFER_SIZE> buffer;
};

TEST_F(ARingBuffer, capacityIsBufferSize)
{
   ASSERT_THAT(buffer.capacity(), Eq(BUFFER_SIZE));
}

TEST_F(ARingBuffer, isEmptyAfterConstruction)
{
   ASSERT_THAT(buffer.elementCount(), Eq(0));
}

TEST_F(ARingBuffer, hasOneElementAfterInsertion)
{
   buffer.add(ARBITRARY_VALUE);

   ASSERT_THAT(buffer.elementCount(), Eq(1));
}

TEST_F(ARingBuffer, addingMoreThanCapacityElementsWrapsAround)
{
   for (auto i = 0; i < BUFFER_SIZE + 1; ++i)
   {
      buffer.add(i);
   }

   ASSERT_THAT(buffer.lastInsertedElementIndex(), Eq(0));
   ASSERT_THAT(buffer.elementAt(0), Eq(BUFFER_SIZE));
}

TEST_F(ARingBuffer, withAddingMoreThanCapacityEntriesIsFull)
{
   for (auto i = 0; i < BUFFER_SIZE + 2; ++i)
   {
      buffer.add(i);
   }

   ASSERT_TRUE(buffer.isFull());
   ASSERT_THAT(buffer.elementCount(), Eq(BUFFER_SIZE));
}
