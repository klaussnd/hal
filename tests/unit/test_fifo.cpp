#include <gmock/gmock.h>

#include <utils/fifo.h>

using namespace testing;

class AFifo : public testing::Test
{
public:
   static const uint8_t BUFFER_SIZE = 8;
   static const uint8_t ARBITRARY_VALUE = 10;

   Fifo<uint8_t, BUFFER_SIZE> fifo;
};

TEST_F(AFifo, isEmptyOnConstruction)
{
   ASSERT_TRUE(fifo.isEmpty());
   ASSERT_FALSE(fifo.isFull());
   ASSERT_THAT(fifo.count(), Eq(0));
}

TEST_F(AFifo, readingFromEmptyObtainsZero)
{
   ASSERT_THAT(fifo.read(), Eq(0));
}

TEST_F(AFifo, isNonEmptyAfterAddingOneElement)
{
   fifo.write(ARBITRARY_VALUE);

   ASSERT_FALSE(fifo.isEmpty());
   ASSERT_THAT(fifo.count(), Eq(1));
}

TEST_F(AFifo, writeAndReadObtainsOriginalValue)
{
   fifo.write(ARBITRARY_VALUE);

   ASSERT_THAT(fifo.read(), Eq(ARBITRARY_VALUE));
}

TEST_F(AFifo, readingAfterWritingMultipleValuesObtainsFirstValue)
{
   fifo.write(0);
   fifo.write(1);

   ASSERT_THAT(fifo.read(), Eq(0));
}

TEST_F(AFifo, readingTwiceObtainsSecondValue)
{
   fifo.write(0);
   fifo.write(1);
   fifo.read();

   ASSERT_THAT(fifo.read(), Eq(1));
}

TEST_F(AFifo, addingBufferSizeMinusOneValuesMakesItFull)
{
   for (uint8_t i = 0; i < BUFFER_SIZE - 1; ++i)
   {
      ASSERT_TRUE(fifo.write(i));
   }

   ASSERT_TRUE(fifo.isFull());
}

TEST_F(AFifo, tryingToWriteToFullReturnsFalse)
{
   for (uint8_t i = 0; i < BUFFER_SIZE - 1; ++i)
   {
      ASSERT_TRUE(fifo.write(i));
   }

   ASSERT_FALSE(fifo.write(ARBITRARY_VALUE));
}

class AFilledAndEmptiedFifo : public AFifo
{
   void SetUp() override
   {
      for (uint8_t i = 0; i < BUFFER_SIZE - 1; ++i)
      {
         fifo.write(i);
      }
      ASSERT_TRUE(fifo.isFull());
      while (!fifo.isEmpty())
      {
         fifo.read();
      }
   }
};

TEST_F(AFilledAndEmptiedFifo, readObtainsFirstValue)
{
   fifo.write(33);
   fifo.write(34);

   ASSERT_THAT(fifo.read(), Eq(33));
}

TEST_F(AFilledAndEmptiedFifo, readingTwiceObtainsSecondValue)
{
   fifo.write(33);
   fifo.write(34);
   fifo.read();

   ASSERT_THAT(fifo.read(), Eq(34));
}
