#ifndef NFACONTAINER_H
#define NFACONTAINER_H
#include "statemap.h"
#include <ostream>
namespace NFA {
    namespace container {
        template<typename alphabet, size_t max_symbol = 0, size_t max_state = 0>
        class NFAContainer;

        template<typename symbol_id>
        class InputLink{
            private:
                InputLink(std::shared_ptr<std::vector<symbol_id>>& symbols);
                std::shared_ptr<std::vector<symbol_id>> symbols_ptr_;
        };

        template<typename alphabet, size_t msy = 0, size_t mst = 0>
        std::ostream& operator<<(std::ostream& os, NFAContainer<alphabet, msy, mst> const& nfa);

        template<typename alphabet, size_t max_symbol, size_t max_state>
        class NFAContainer
        {
            public:
                using symbol_id = typename state::StateMap<alphabet,max_symbol,max_state>::symbol_id; //numeric type
                using state_id = typename state::StateMap<alphabet,max_symbol,max_state>::state_id; //numeric type

                //Constructors
                NFAContainer(state_id initial_state_size = 0);
                NFAContainer(const NFAContainer&);
                NFAContainer(NFAContainer&&);

                //Operators
                NFAContainer& operator=(const NFAContainer&);
                NFAContainer& operator=(NFAContainer&&);

                //Manipulate States
                state_id new_state(bool is_accept = false, bool is_start = false);
                template<typename... Args>
                void add_transition(state_id initial_state, state_id destination_state, alphabet symbol, Args... args);
                void add_epsilon(state_id initial_state, state_id destination_state);

                //Insert symbols
                template<typename... Args>
                void input_symbols(const alphabet&, Args...);
                template<typename Iterator>
                void input_iterator(Iterator iter_start, Iterator iter_end);

                //Copy and link symbols
                void copy_input(const NFAContainer&); //Copy input from another container
                void copy_input(const InputLink<symbol_id>&); //Copy input from InputLink
                void link_input(const NFAContainer&); //Share pointer to same input
                void link_input(const InputLink<symbol_id>&); //Share pointer to InputLink's symbols

                //get symbols
                InputLink<symbol_id> get_inputs() const; //Get a memento of inputs



                //Query
                size_t size() const;
                bool is_accept() const;
                state_id get_start_state() const;
                std::vector<state_id> get_states() const;

                //Evaluate
                void evaluate();


            private:
                void eval_symbol(symbol_id);

                state::StateMap<alphabet,max_symbol,max_state> state_map_;
                std::shared_ptr<std::vector<symbol_id>> symbols_ptr_;
                std::vector<state_id> current_states_;
                friend std::ostream& operator<<<alphabet, max_symbol, max_state>(std::ostream& os, const NFAContainer& nfa);
        };


    } // container
}// NFA

#include "nfacontainer.cpp"

#endif // NFACONTAINER_H
