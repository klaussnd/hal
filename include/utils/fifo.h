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

#include <stddef.h>

/** FIFO
 *
 *  @note The size of the ring buffer _must_ be a power of 2!
 */
template <typename T, size_t buffer_size, typename S = uint8_t>
class Fifo
{
public:
   using size_type = S;
   using value_type = T;

   Fifo();

   bool isEmpty() const volatile;
   bool isFull() const volatile;
   /// Return the number of entries in the FIFO
   size_type count() const volatile;
   constexpr size_type capacity() const volatile
   { return static_cast<size_type>(buffer_size); }
   /// Write a value into the FIFO.
   /// @return false if the FIFO is full and the value could not be added
   bool write(T value) volatile;
   /// Read a value from the FIFO. If the FIFO is empty, returns zero.
   T read() volatile;

private:
   /// new value of the write index when inserting @e itemCount values
   size_type writeIndexWhenAddingItems(size_type itemCount) const volatile
   { return (m_writeIndex + itemCount) & (capacity() - 1); }

   size_type m_readIndex;
   size_type m_writeIndex;
   T m_buffer[buffer_size];
};

template <typename T, size_t buffer_size, typename S>
Fifo<T, buffer_size, S>::Fifo()
      : m_readIndex(0)
      , m_writeIndex(0)
{
   static_assert(buffer_size % 2 == 0, "Buffer size must be a multiple of 2");
}

template <typename T, size_t buffer_size, typename S>
bool Fifo<T, buffer_size, S>::isEmpty() const volatile
{
   return m_readIndex == m_writeIndex;
}

template <typename T, size_t buffer_size, typename S>
bool Fifo<T, buffer_size, S>::isFull() const volatile
{
   const auto newWriteIndex = writeIndexWhenAddingItems(1);
   return newWriteIndex == m_readIndex;
}

template <typename T, size_t buffer_size, typename S>
typename Fifo<T, buffer_size, S>::size_type Fifo<T, buffer_size, S>::count() const volatile
{
   return (m_writeIndex - m_readIndex) & (capacity() - 1);
}

template <typename T, size_t buffer_size, typename S>
bool Fifo<T, buffer_size, S>::write(T value) volatile
{
   const auto newWriteIndex = writeIndexWhenAddingItems(1);
   const bool isFull = newWriteIndex == m_readIndex;
   if (isFull)
   {
      return false;
   }
   else
   {
      m_writeIndex = newWriteIndex;
      m_buffer[newWriteIndex] = value;
      return true;
   }
}

template <typename T, size_t buffer_size, typename S>
T Fifo<T, buffer_size, S>::read() volatile
{
   if (isEmpty())
   {
      return {};
   }
   else
   {
      m_readIndex = (m_readIndex + 1) & (capacity() - 1);
      return m_buffer[m_readIndex];
   }
}
