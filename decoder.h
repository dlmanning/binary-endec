#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "leb128.h"

namespace BinaryEndec {

enum class endian {
#ifdef _WIN32
    little = 0,
    big = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

template <typename V, size_t N = sizeof(V)>
static inline size_t read_unaligned_value(V* out, const std::byte* pc)
{
    memcpy(out, pc, N);
    return N;
}

template <typename V, size_t N = sizeof(V)>
static inline size_t read_unaligned_value_swap_endianness(
    V*               out,
    const std::byte* pc)
{
    std::byte* o = reinterpret_cast<std::byte*>(out);

    size_t hi, lo;
    for (hi = N - 1, lo = 0; hi > lo; lo++, hi--) {
        o[hi] = pc[lo];
        o[lo] = pc[hi];
    }

    return N;
}

template <typename V>
static inline size_t read_little_endian_value(V* out, const std::byte* pc)
{
    if constexpr (endian::native != endian::little) {
        return read_unaligned_value_swap_endianness(out, pc);
    }

    return read_unaligned_value(out, pc);
}

template <typename V>
static inline size_t read_big_endian_value(V* out, const std::byte* pc)
{
    if constexpr (endian::native != endian::big) {
        return read_unaligned_value_swap_endianness(out, pc);
    }

    return read_unaligned_value(out, pc);
}

enum class Endian { Big, Little };

template <Endian ENDIANNESS = Endian::Little>
class Decoder {
   public:
    Decoder(const std::byte* begin, const std::byte* end)
        : Decoder(begin, begin, end)
    {}

    Decoder(
        const std::byte* begin,
        const std::byte* current,
        const std::byte* end);

    template <typename T>
    size_t read_leb128(T* out, const std::byte* pc)
    {
        if constexpr (std::numeric_limits<T>::is_signed) {
            return leb128_decode_signed(out, pc);
        }

        return leb128_decode_unsigned(out, pc);
    }

    template <typename T>
    size_t consume_leb128(T* out)
    {
        size_t bytes_read = read_leb128(out, m_current);
        m_current += bytes_read;
        return bytes_read;
    }

    template <typename T>
    size_t read_bytes(T* out, const std::byte* pc, size_t length)
    {
        if (!check_available(length)) {
            return 0;
        }

        return read_unaligned_value(out, pc);
    }

    template <typename T>
    size_t consume_bytes(T* out, size_t length)
    {
        size_t bytes_read = read_bytes(out, m_current, length);
        m_current += bytes_read;
        return bytes_read;
    }

    template <typename T>
    size_t read(T* out, const std::byte* pc) const
    {
        static_assert(std::is_integral<T>::value);

        if (!check_available(sizeof(T))) {
            return 0;
        }

        if constexpr (ENDIANNESS == Endian::Big) {
            return read_big_endian_value(out, pc);
        } else {
            return read_little_endian_value(out, pc);
        }
    }

    template <typename T>
    size_t consume(T* out)
    {
        size_t bytes_read = read(out, m_current);
        ;
        m_current += bytes_read;
        return bytes_read;
    }

    const std::byte* end() const;
    bool             check_available(size_t size) const;
    size_t           get_pos() const;

   protected:
    const std::byte* m_current;
    const std::byte* m_begin;
    const std::byte* m_end;
};

template <Endian T>
Decoder<T>::Decoder(
    const std::byte* begin,
    const std::byte* current,
    const std::byte* end)
    : m_current(current), m_begin(begin), m_end(end)
{
    assert(end > begin);
    assert(current >= begin && current <= end);
}

template <Endian T>
const std::byte* Decoder<T>::end() const
{
    return m_end;
}

template <Endian T>
size_t Decoder<T>::get_pos() const
{
    return static_cast<size_t>(m_current - m_begin);
}

template <Endian T>
bool Decoder<T>::check_available(size_t size) const
{
    return (size <= static_cast<size_t>(m_end - m_current) + 1);
}

};  // namespace BinaryEndec
