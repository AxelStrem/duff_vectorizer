#include <emmintrin.h>
#include "duff_vect.hpp"

struct search_result
{
    char* x;
};

class find_char0
{
public:
    void operator()(char* x)
    {
        if (*x == 0)
            throw search_result{ x };
    }
};

template<> class vectorized_kernel<4, char, find_char0> : public vectorized_kernel<2, char, find_char0>
{
public:
    void operator()(char* x)
    {
        uint32_t *i = reinterpret_cast<uint32_t*>(x);
        uint32_t t1 = *i;
        t1 = ((t1 - 0x01010101) & ~t1 & 0x80808080);
        
        if (!t1)
            return;

        vectorized_kernel<2, char, find_char0>::operator()(x);
        vectorized_kernel<2, char, find_char0>::operator()(x+2);
    }

    vectorized_kernel(find_char0 kernel) : vectorized_kernel<2, char, find_char0>(kernel) {}
};

template<> class vectorized_kernel<16, char, find_char0> : public vectorized_kernel<8, char, find_char0>
{
public:
    void operator()(char* x)
    {
        __m128i *v = (reinterpret_cast<__m128i*>(x));
        static const __m128i zero = _mm_setzero_si128();
        uint32_t t = _mm_movemask_epi8(_mm_cmpeq_epi8(*v, zero));
        if (!t)
            return;

        vectorized_kernel<8, char, find_char0>::operator()(x);
        vectorized_kernel<8, char, find_char0>::operator()(x + 8);
    }

    vectorized_kernel(find_char0 kernel) : vectorized_kernel<8, char, find_char0>(kernel) {}
};

int strlen_sick(char* str, int max_len)
{
    try
    {
        traverse(str, str+max_len, create_vk<char>(find_char0{}));
    }
    catch (search_result sr)
    {
        return sr.x - str;
    }
    return -1;
}
