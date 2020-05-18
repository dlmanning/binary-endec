#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace BinaryEndec {

constexpr std::byte ZERO{0x00};
constexpr std::byte HI_BIT_MASK{0x80};
constexpr std::byte LOWER_BITS_MASK{0x7f};

template <typename T>
size_t leb128_decode_unsigned(T *out, const std::byte *src) {
  static_assert(!std::numeric_limits<T>::is_signed);
  *out = 0;

  constexpr size_t length = sizeof(T);
  size_t i = 0;
  for (; i < length + 1; i++) {
    *out |= static_cast<T>((src[i] & LOWER_BITS_MASK)) << (i * 7);
    if ((src[i] & HI_BIT_MASK) == ZERO) {
      break;
    }
  }

  return i + 1;
}

template <typename T>
size_t leb128_decode_signed(T *out, std::byte *src) {
  static_assert(std::numeric_limits<T>::is_signed);
  *out = 0;

  // result = 0;
  // shift = 0;

  // /* the size in bits of the result variable, e.g., 64 if result's type is
  // int64_t */ size = number of bits in signed integer;

  // do {
  //   byte = next byte in input;
  //   result |= (low order 7 bits of byte << shift);
  //   shift += 7;
  // } while (high order bit of byte != 0);

  // /* sign bit of byte is second high order bit (0x40) */
  // if ((shift <size) && (sign bit of byte is set))
  //   /* sign extend */
  //   result |= (~0 << shift);
  // }

  return 0;
}
}  // namespace BinaryEndec