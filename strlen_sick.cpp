#include <emmintrin.h>
#include "duff_vect.hpp"

class find_char0
{
public:
    char* search_result = nullptr;
    bool operator()(char* x)
    {
        return (*x == 0);
    }
};

template<> class vectorized_check<4, char, find_char0> : public vectorized_check<2, char, find_char0>
{
public:
    bool operator()(char* x)
    {
        uint32_t *i = reinterpret_cast<uint32_t*>(x);
        uint32_t t1 = *i;
        t1 = ((t1 - 0x01010101) & ~t1 & 0x80808080);
        
        if (!t1)
            return false;

        if(vectorized_check<2, char, find_char0>::operator()(x)) return true;
        return vectorized_check<2, char, find_char0>::operator()(x+2);
    }

    vectorized_check(find_char0 kernel) : vectorized_check<2, char, find_char0>(kernel) {}
};

template<> class vectorized_check<16, char, find_char0> : public vectorized_check<8, char, find_char0>
{
public:
    bool operator()(char* x)
    {
        __m128i *v = (reinterpret_cast<__m128i*>(x));
        static const __m128i zero = _mm_setzero_si128();
        uint32_t t = _mm_movemask_epi8(_mm_cmpeq_epi8(*v, zero));
        if (!t)
            return false;

        if(vectorized_check<8, char, find_char0>::operator()(x)) return true;
        return vectorized_check<8, char, find_char0>::operator()(x + 8);
    }

    vectorized_check(find_char0 kernel) : vectorized_check<8, char, find_char0>(kernel) {}
};

int strlen_sick(char* str, int max_len)
{
    find_char0 fc;
    traverse_until(str, str+max_len, create_vc<char>(fc));
    return fc.search_result - str;
}
