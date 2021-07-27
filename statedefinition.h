#ifndef STATEDEFINITION_H
#define STATEDEFINITION_H
#include <vector>
#include <cassert>
namespace NFA {
    namespace state {
        template<typename alphabet>
        struct state_definition{
                state_definition(bool accept): is_accept(accept){};
                //std::vector<transition_definition<alphabet>> transitions;
                //size_t id;
                bool is_accept = false;
                //bool is_start = false;
        };
    }




}



#endif // STATEDEFINITION_H
