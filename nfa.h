#ifndef NFA_H
#define NFA_H
#include "nfacontainer.h"
namespace NFA {
    template<typename alphabet, size_t max_unique_alphabet = 0, size_t max_unique_state = 0>
    using NFA = container::NFAContainer<alphabet, max_unique_alphabet, max_unique_state>;
}
#endif // NFA_H
