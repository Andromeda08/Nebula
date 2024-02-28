#include <iostream>
#include "common/RingArray.hpp"

using namespace Nebula;

#define ARR_SIZE 4

struct SomeType
{
    size_t      id;
    std::string name;
};

std::ostream& operator<<(std::ostream& os, const SomeType& rhs)
{
    return os << std::format("#{}: {}", rhs.id, rhs.name);
}

int main()
{
    RingArray<SomeType, ARR_SIZE> ring_array {};

    std::cout << "RingArray::for_each(T&, S)" << std::endl; ring_array.for_each([](SomeType& val, const size_t i){
        val = SomeType { i, std::format("SomeType#{}", i) };
        std::cout << "\tSet value at index " << i << " to " << val << std::endl;
    });

    std::cout << "RingArray::for_each(T&)" << std::endl; ring_array.for_each([](SomeType& val){
       std::cout << "\t" << val << std::endl;
    });

    std::cout << "RingArray::next()" << std::endl;
    for (size_t i = 0; i < ARR_SIZE * 2; i++)
    {
        if (i != 0) std::cout << ", ";
        std::cout << ring_array.next();
    }
    std::cout << std::endl;

    std::cout << "RingArray::operator[](S)" << std::endl;
    for (size_t i = 0; i <= ARR_SIZE; i++)
    {
        std::cout << "\t array[" << i << "] -> ";
        try {
            auto& val = ring_array[i];
            std::cout << val;
        } catch (std::out_of_range& ex) {
            std::cout << ex.what();
        }
        std::cout << std::endl;
    }

    return 0;
}