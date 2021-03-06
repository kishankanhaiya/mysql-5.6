/*
   Portions Copyright (c) 2016-Present, Facebook, Inc.
   Portions Copyright (c) 2012,2013 Monty Program Ab

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
#pragma once

#include <algorithm>
#include <string>

namespace myrocks {

/*
  Basic composition functions for a network buffer presented as a MySQL String
  ("netstr") which stores data in Network Byte Order (Big Endian).
*/

inline void rdb_netstr_append_uint64(my_core::String *out_netstr, uint64 val)
{
  DBUG_ASSERT(out_netstr != nullptr);

  // Convert from host machine byte order (usually Little Endian) to network
  // byte order (Big Endian).
  uint64 net_val= htobe64(val);
  out_netstr->append(reinterpret_cast<char*>(&net_val), sizeof(net_val));
}

inline void rdb_netstr_append_uint32(my_core::String *out_netstr, uint32 val)
{
  DBUG_ASSERT(out_netstr != nullptr);

  // Convert from host machine byte order (usually Little Endian) to network
  // byte order (Big Endian).
  uint32 net_val= htobe32(val);
  out_netstr->append(reinterpret_cast<char*>(&net_val), sizeof(net_val));
}

inline void rdb_netstr_append_uint16(my_core::String *out_netstr, uint16 val)
{
  DBUG_ASSERT(out_netstr != nullptr);

  // Convert from host machine byte order (usually Little Endian) to network
  // byte order (Big Endian).
  uint16 net_val= htobe16(val);
  out_netstr->append(reinterpret_cast<char*>(&net_val), sizeof(net_val));
}


/*
  Basic network buffer ("netbuf") write helper functions.
*/

inline void rdb_netbuf_store_uint32(uchar *dst_netbuf, uint32 n)
{
  DBUG_ASSERT(dst_netbuf != nullptr);

  // Convert from host byte order (usually Little Endian) to network byte order
  // (Big Endian).
  uint32 net_val= htobe32(n);
  memcpy(dst_netbuf, &net_val, sizeof(net_val));
}

inline void rdb_netbuf_store_uint16(uchar *dst_netbuf, uint16 n)
{
  DBUG_ASSERT(dst_netbuf != nullptr);

  // Convert from host byte order (usually Little Endian) to network byte order
  // (Big Endian).
  uint16 net_val= htobe16(n);
  memcpy(dst_netbuf, &net_val, sizeof(net_val));
}

inline void rdb_netbuf_store_byte(uchar *dst_netbuf, uchar c)
{
  DBUG_ASSERT(dst_netbuf != nullptr);

  *dst_netbuf= c;
}

inline void rdb_netbuf_store_index(uchar *dst_netbuf, uint32 number)
{
  DBUG_ASSERT(dst_netbuf != nullptr);

  rdb_netbuf_store_uint32(dst_netbuf, number);
}


/*
  Basic conversion helper functions from network byte order (Big Endian) to host
  machine byte order (usually Little Endian).
*/

inline uint64 rdb_netbuf_to_uint64(const uchar *netbuf)
{
  DBUG_ASSERT(netbuf != nullptr);

  uint64 net_val;
  memcpy(&net_val, netbuf, sizeof(net_val));

  // Convert from network byte order (Big Endian) to host machine byte order
  // (usually Little Endian).
  return be64toh(net_val);
}

inline uint32 rdb_netbuf_to_uint32(const uchar *netbuf)
{
  DBUG_ASSERT(netbuf != nullptr);

  uint32 net_val;
  memcpy(&net_val, netbuf, sizeof(net_val));

  // Convert from network byte order (Big Endian) to host machine byte order
  // (usually Little Endian).
  return be32toh(net_val);
}

inline uint16 rdb_netbuf_to_uint16(const uchar *netbuf)
{
  DBUG_ASSERT(netbuf != nullptr);

  uint16 net_val;
  memcpy(&net_val, netbuf, sizeof(net_val));

  // Convert from network byte order (Big Endian) to host machine byte order
  // (usually Little Endian).
  return be16toh(net_val);
}

inline uchar rdb_netbuf_to_byte(const uchar* netbuf)
{
  DBUG_ASSERT(netbuf != nullptr);

  return(uchar)netbuf[0];
}


/*
  Basic network buffer ("netbuf") read helper functions.
  Network buffer stores data in Network Byte Order (Big Endian).
  NB: The netbuf is passed as an input/output param, hence after reading,
      the netbuf pointer gets advanced to the following byte.
*/

inline uint64 rdb_netbuf_read_uint64(const uchar **netbuf_ptr)
{
  DBUG_ASSERT(netbuf_ptr != nullptr);

  // Convert from network byte order (Big Endian) to host machine byte order
  // (usually Little Endian).
  uint64 host_val= rdb_netbuf_to_uint64(*netbuf_ptr);

  // Advance pointer.
  *netbuf_ptr += sizeof(host_val);

  return host_val;
}

inline uint32 rdb_netbuf_read_uint32(const uchar **netbuf_ptr)
{
  DBUG_ASSERT(netbuf_ptr != nullptr);

  // Convert from network byte order (Big Endian) to host machine byte order
  // (usually Little Endian).
  uint32 host_val= rdb_netbuf_to_uint32(*netbuf_ptr);

  // Advance pointer.
  *netbuf_ptr += sizeof(host_val);

  return host_val;
}

inline uint16 rdb_netbuf_read_uint16(const uchar **netbuf_ptr)
{
  DBUG_ASSERT(netbuf_ptr != nullptr);

  // Convert from network byte order (Big Endian) to host machine byte order
  // (usually Little Endian).
  uint16 host_val= rdb_netbuf_to_uint16(*netbuf_ptr);

  // Advance pointer.
  *netbuf_ptr += sizeof(host_val);

  return host_val;
}

inline void rdb_netbuf_read_gl_index(const uchar **netbuf_ptr,
                                     GL_INDEX_ID *gl_index_id)
{
  DBUG_ASSERT(gl_index_id != nullptr);
  DBUG_ASSERT(netbuf_ptr != nullptr);

  gl_index_id->cf_id=    rdb_netbuf_read_uint32(netbuf_ptr);
  gl_index_id->index_id= rdb_netbuf_read_uint32(netbuf_ptr);
}

/*
  A simple string reader:
  - it keeps position within the string that we read from
  - it prevents one from reading beyond the end of the string.
*/

class Rdb_string_reader
{
  const char* m_ptr;
  uint m_len;
 public:
  explicit Rdb_string_reader(const std::string &str)
  {
    m_len= str.length();
    if (m_len)
    {
      m_ptr= &str.at(0);
    }
    else
    {
      /*
        One can a create a Rdb_string_reader for reading from an empty string
        (although attempts to read anything will fail).
        We must not access str.at(0), since len==0, we can set ptr to any
        value.
      */
      m_ptr= nullptr;
    }
  }

  explicit Rdb_string_reader(const rocksdb::Slice *slice)
  {
    m_ptr= slice->data();
    m_len= slice->size();
  }

  /*
    Read the next @param size bytes. Returns pointer to the bytes read, or
    nullptr if the remaining string doesn't have that many bytes.
  */
  const char *read(uint size)
  {
    const char *res;
    if (m_len < size)
    {
      res= nullptr;
    }
    else
    {
      res= m_ptr;
      m_ptr += size;
      m_len -= size;
    }
    return res;
  }

  uint remaining_bytes() const { return m_len; }

  /*
    Return pointer to data that will be read by next read() call (if there is
    nothing left to read, returns pointer to beyond the end of previous read()
    call)
  */
  const char *get_current_ptr() const { return m_ptr; }
};

class Rdb_bit_writer
{
  uchar* m_ptr;
  uint m_len;
  uchar m_offset;
 public:
  // TODO(mung): Writing into a buffer without length checking is not ideal.
  // There should be a Rdb_string_writer class that Rdb_bit_writer can wrap
  // around (similar to the reader case).
  explicit Rdb_bit_writer(uchar *ptr)
    : m_ptr(ptr),
      m_len(0),
      m_offset(0)
  {
  }

  void write(uint size, uint value)
  {
    DBUG_ASSERT((value & ((1 << size) - 1)) == value);

    while (size > 0)
    {
      if (m_offset == 0)
      {
        m_ptr[m_len++]= 0;
      }
      // number of bits to put in this byte
      uint bits = std::min(size, (uint)(8 - m_offset));
      m_ptr[m_len - 1] |=
        (uchar) ((value >> (size - bits)) & ((1 << bits) - 1)) << m_offset;
      size -= bits;
      m_offset = (m_offset + bits) & 0x7;
    }
  }

  uchar* get_ptr() const { return m_ptr; }
  uint get_len() const { return m_len; }
};

class Rdb_bit_reader
{
  const uchar *m_cur;
  uchar m_offset;
  uint m_ret;
  Rdb_string_reader *m_reader;
 public:
  explicit Rdb_bit_reader(Rdb_string_reader *reader)
    : m_cur(nullptr),
      m_offset(0),
      m_reader(reader)
  {
  }

  // Returns a pointer to an uint containing the bits read. On subsequent
  // reads, the value being pointed to will be overwritten.  Returns nullptr
  // on failure.
  uint *read(uint size)
  {
    m_ret= 0;
    DBUG_ASSERT(size <= 32);

    while (size > 0)
    {
      if (m_offset == 0)
      {
        m_cur= (const uchar *)m_reader->read(1);
        if (m_cur == nullptr)
        {
          return nullptr;
        }
      }
      // how many bits from the current byte?
      uint bits = std::min((uint)(8 - m_offset), size);
      m_ret <<= bits;
      m_ret |= (*m_cur >> m_offset) & ((1 << bits) - 1);
      size -= bits;
      m_offset = (m_offset + bits) & 0x7;
    }

    return &m_ret;
  }
};

}  // namespace myrocks
