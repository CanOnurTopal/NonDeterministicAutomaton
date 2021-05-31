#ifndef STATEDEFINITION_H
#define STATEDEFINITION_H
#include <vector>
#include <cassert>
namespace NFA {
    enum class special_symbol{epsilon, null};


    template <typename alphabet>
    struct transition_definition{
            transition_definition(alphabet symbol_input, size_t next_state_input):
                symbol(symbol_input), s_symbol(special_symbol::null), next_state(next_state_input){}
            transition_definition(special_symbol special_symbol_input, size_t next_state_input):
                s_symbol(special_symbol_input), next_state(next_state_input){
                assert(special_symbol_input != special_symbol::null && "null is not a valid symbol");
            }
            alphabet symbol;
            special_symbol s_symbol;
            size_t next_state;
    };

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
