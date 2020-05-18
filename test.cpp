#include <cstddef>
#include <iostream>

#include "decoder.h"

using namespace BinaryEndec;
int main()
{
    std::byte buffer[] = {
        std::byte{0xE5}, std::byte{0x8E}, std::byte{0x26}, std::byte{0x01}};

    auto decoder = Decoder(&buffer[0], &buffer[4]);

    uint32_t x;
    uint8_t  y;

    auto bytes_read = 0;
    bytes_read += decoder.consume_leb128(&x);
    bytes_read += decoder.consume(&y);

    std::cout << "Number " << unsigned(x) << std::endl
              << "Number " << unsigned(y) << std::endl
              << "Bytes read: " << bytes_read << std::endl;
}