#ifndef STATEMAP_H
#define STATEMAP_H

#include <vector>
#include <memory>
#include "nfa_utilities.h"
#include "statedefinition.h"

namespace NFA {
    namespace state {


        template<typename alphabet, size_t max_unique_alphabet = 0, size_t max_unique_state = 0>
        class StateMap
        {
            public:
                // Using definitions
                using symbol_id = typename utilities::storage_type<max_unique_alphabet>; //numeric type
                using state_id = typename utilities::storage_type<max_unique_state>; //numeric type
                using transition_matrix = NFA::utilities::transition_matrix<state_id>;
                using epsilon_matrix = NFA::utilities::epsilon_matrix<state_id>;
                static constexpr symbol_id NULL_SYMBOL_ID = std::numeric_limits<symbol_id>::max();

                // Constructors and Operators
                StateMap(size_t state_count = 0);
                StateMap(const StateMap&);
                StateMap(StateMap&&);
                StateMap& operator=(const StateMap&);
                StateMap& operator=(StateMap&&);

                //Member Functions
                size_t size() const;

                state_id new_state(bool is_accept = false, bool is_start = false);
                void add_transition(state_id initial_state, state_id destination_state, alphabet symbol);
                void add_epsilon(state_id initial_state, state_id destination_state);

                inline const std::vector<state_id>& get_transitions(state_id initial_state, symbol_id symbol) const;
                inline const std::vector<state_id>& get_epsilon(state_id initial_state) const;

                inline symbol_id get_symbol_id(const alphabet&) const;
                inline state_id get_start_state() const;
                inline bool is_accept(state_id state) const;

            private:
                void add_symbol(alphabet);

                transition_matrix trans_;
                epsilon_matrix epsilon_;
                std::vector<state_definition<alphabet>> states_;
                std::vector<alphabet> symbols_;
                state_id start_state_;

        };

    }// namespace state
}// namespace NFA

#include "statemap.cpp"

#endif // STATEMAP_H
