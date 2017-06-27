#pragma once

#define TS_BRANCH_(offset, label_up, label_down) next = start + offset; \
                                                 if (next > end) goto label_down; \
                                                 (K::kernel_width<offset>(kernel))(start); \
                                                 start = next; \
                                                 goto label_up;

template<class T, class K>
void traverse(T* start, T* end, K kernel)
{
    int alignment = ((reinterpret_cast<int>(start)) / sizeof(T)) & 0xF;
    T* next = nullptr;
    switch (alignment)
    {
    case 0:   l0:  TS_BRANCH_(16, l0, l8);
    case 1:        if (start == end) return;
                   (K::kernel_single(kernel))(start++);
    case 2:        TS_BRANCH_(2, l4, l3);
    case 3:   l3:  if (start == end) return;
                   (K::kernel_single(kernel))(start++);
    case 4:   l4:  TS_BRANCH_(4, l8, l6);
    case 5:        if (start == end) return;
                   (K::kernel_single(kernel))(start++);
    case 6:   l6:  TS_BRANCH_(2, l8, l7);
    case 7:   l7:  if (start == end) return;
                   (K::kernel_single(kernel))(start++);
    case 8:   l8:  TS_BRANCH_(8, l0, l12);
    case 9:        if (start == end) return;
                   (K::kernel_single(kernel))(start++);
    case 10:       TS_BRANCH_(2, l12, l11);
    case 11: l11:  if (start == end) return;
                   (K::kernel_single(kernel))(start++);
    case 12: l12:  TS_BRANCH_(4, l0, l14);
    case 13:       if (start == end) return;
                   (K::kernel_single(kernel))(start++);
    case 14: l14:  TS_BRANCH_(2, l0, l15);
    case 15: l15:  if (start == end) return;
                   (K::kernel_single(kernel))(start++);
                   goto l0;
    }
    return;
}

template<int width, class T, class K> class vectorized_kernel : public vectorized_kernel<width/2, T, K>
{
    typedef vectorized_kernel<width / 2,T, K> superclass;
public:
    void operator()(T* ptr)
    {
        superclass::operator()(ptr);
        superclass::operator()(ptr + (width / 2));
    }

    template<int w> using kernel_width = vectorized_kernel<w, T,K>;
                    using kernel_single = K;

    vectorized_kernel(K kernel) : superclass(kernel) {}
};

template<class T, class K> class vectorized_kernel<1,T,K> : public K
{
public:
    vectorized_kernel(K kernel) : K(kernel) {}
    template<int w> using kernel_width = vectorized_kernel<w,T,K>;
                    using kernel_single = K;
};

template<class T, class K> vectorized_kernel<16, T, K> create_vk(K kernel)
{
    return vectorized_kernel<16, T, K>(kernel);
}
