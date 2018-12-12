/*  Hardware abstraction layer for embedded systems
 *
 *  (c) Klaus Schneider-Zapp klaus underscore snd at web dot de
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 */

#pragma once

#include <stdint.h>

/** Ring buffer (circular buffer)
 *
 *  @note The size of the ring buffer _must_ be a power of 2!
 */
template <typename T, size_t buffer_size, typename S = uint8_t>
class RingBuffer
{
public:
   using size_type = S;
   using value_type = T;

   RingBuffer();

   void add(T value);
   T elementAt(size_type index) const;
   size_type lastInsertedElementIndex() const;
   size_type elementCount() const;
   constexpr size_type capacity() const
   { return static_cast<size_type>(buffer_size); }
   bool isFull() const;

private:
   size_type getPreviousIndex(size_type index) const
   { return (index - 1) & (capacity() - 1); }
   size_type getNextIndex(size_type index)
   { return (index + 1) & (capacity() - 1); }

private:
   uint8_t m_writeIndex; ///< write position
   uint8_t m_count;      ///< number of elements filled
   T m_buffer[buffer_size];
};

template <typename T, size_t buffer_size, typename S>
RingBuffer<T, buffer_size, S>::RingBuffer()
   : m_writeIndex(0)
   , m_count(0)
{
   static_assert(buffer_size % 2 == 0, "Buffer size must be a multiple of 2");
}

template <typename T, size_t buffer_size, typename S>
void RingBuffer<T, buffer_size, S>::add(T value)
{
   m_buffer[m_writeIndex] = value;
   m_writeIndex = getNextIndex(m_writeIndex); // set write position
   if (m_count < capacity()) // set number of entries filled
      ++m_count;
}

template <typename T, size_t buffer_size, typename S>
typename RingBuffer<T, buffer_size, S>::size_type
    RingBuffer<T, buffer_size, S>::lastInsertedElementIndex() const
{
   return getPreviousIndex(m_writeIndex);
}

template <typename T, size_t buffer_size, typename S>
T RingBuffer<T, buffer_size, S>::elementAt(const size_type index) const
{
   return m_buffer[index];
}

template <typename T, size_t buffer_size, typename S>
typename RingBuffer<T, buffer_size, S>::size_type
    RingBuffer<T, buffer_size, S>::elementCount() const
{
   return m_count;
}

template <typename T, size_t buffer_size, typename S>
bool RingBuffer<T, buffer_size, S>::isFull() const
{
   return capacity() == m_count;
}
