#ifndef UNWRAP_REFWRAPPER_H_
#define UNWRAP_REFWRAPPER_H_

#include <functional>
#include <type_traits>

template <class T>
struct unwrap_refwrapper {
    using type = T;
};

template <class T>
struct unwrap_refwrapper<std::reference_wrapper<T>> {
    using type = T&;
};

template <class T>
using special_decay_t = typename ::unwrap_refwrapper<typename std::decay<T>::type>::type;

#endif  // UNWRAP_REFWRAPPER_H_
