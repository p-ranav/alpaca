#pragma once

namespace alpaca {

namespace detail {

static inline bool is_system_little_endian() {
  static int value{0x01};
  static auto address = static_cast<const void *>(&value);
  static auto least_significant_address =
      static_cast<const unsigned char *>(address);
  return (*least_significant_address == 0x01);
}

// Endian switching code taken from 
// https://github.com/dfelinto/blender/blob/master/source/blender/blenlib/BLI_endian_switch_inline.h

/* NOTE: using a temp char to switch endian is a lot slower,
 * use bit shifting instead. */

/* *** 16 *** */
static inline
void BLI_endian_switch_uint16(unsigned short *val)
{
#if (defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 408)) /* gcc4.8+ only */
  *val = __builtin_bswap16(*val);
#else
  unsigned short tval = *val;
  *val = (tval >> 8) | (tval << 8);
#endif
}

static inline
void BLI_endian_switch_int16(short *val)
{
  BLI_endian_switch_uint16((unsigned short *)val);
}

/* *** 32 *** */
static inline
void BLI_endian_switch_uint32(unsigned int *val)
{
#ifdef __GNUC__
  *val = __builtin_bswap32(*val);
#else
  unsigned int tval = *val;
  *val = ((tval >> 24)) | ((tval << 8) & 0x00ff0000) | ((tval >> 8) & 0x0000ff00) | ((tval << 24));
#endif
}

static inline
void BLI_endian_switch_int32(int *val)
{
  BLI_endian_switch_uint32((unsigned int *)val);
}

static inline
void BLI_endian_switch_float(float *val)
{
  BLI_endian_switch_uint32((unsigned int *)val);
}

/* *** 64 *** */
static inline
void BLI_endian_switch_uint64(uint64_t *val)
{
#ifdef __GNUC__
  *val = __builtin_bswap64(*val);
#else
  uint64_t tval = *val;
  *val = ((tval >> 56)) | ((tval << 40) & 0x00ff000000000000ll) |
         ((tval << 24) & 0x0000ff0000000000ll) | ((tval << 8) & 0x000000ff00000000ll) |
         ((tval >> 8) & 0x00000000ff000000ll) | ((tval >> 24) & 0x0000000000ff0000ll) |
         ((tval >> 40) & 0x000000000000ff00ll) | ((tval << 56));
#endif
}

static inline
void BLI_endian_switch_int64(int64_t *val)
{
  BLI_endian_switch_uint64((uint64_t *)val);
}

static inline
void BLI_endian_switch_double(double *val)
{
  BLI_endian_switch_uint64((uint64_t *)val);
}

} // namespace detail

} // namespace alpaca