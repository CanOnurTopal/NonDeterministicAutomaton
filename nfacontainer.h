#ifndef NFACONTAINER_H
#define NFACONTAINER_H
#include "statemap.h"
#include <variant>
#include <ostream>
namespace NFA {

    enum class special_symbol{
        EPSILON,
        STOP
    };

    namespace container {
        template<typename alphabet, size_t max_symbol = 0, size_t max_state = 0>
        class NFAContainer;

        template<typename symbol_id>
        class InputLink;

        template<typename alphabet, size_t msy = 0, size_t mst = 0>
        std::ostream& operator<<(std::ostream& os, NFAContainer<alphabet, msy, mst> const& nfa);

        template<typename symbol_id>
        using cached_symbol = std::variant<symbol_id, special_symbol>;

        template<typename alphabet, size_t max_symbol, size_t max_state>
        class NFAContainer
        {
            public:
                using symbol_id = typename state::StateMap<alphabet,max_symbol,max_state>::symbol_id; //numeric type
                using state_id = typename state::StateMap<alphabet,max_symbol,max_state>::state_id; //numeric type

                //Constructors
                NFAContainer(size_t initial_state_size = 0);
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
                template<typename... Args>
                void input_symbols(special_symbol, Args...);
                template<typename Iterator>
                void input_iterator(Iterator iter_start, Iterator iter_end);

                //Copy and link symbols
                void copy_input(const NFAContainer&); //Copy input from another container
                void copy_input(const InputLink<symbol_id>&); //Copy input from InputLink
                void link_input(const NFAContainer&); //Share pointer to same input
                void link_input(const InputLink<symbol_id>&); //Share pointer to InputLink's symbols

                //get symbols
                InputLink<symbol_id> get_inputs() const; //Get a memento of inputs

                //get copy
                NFAContainer get_copy() const;
                NFAContainer get_linked_copy() const;

                //Query
                size_t size() const; // amount of states
                bool is_accept() const;
                state_id get_start_state() const;
                std::vector<state_id> get_states() const;

                //clear
                void clear_states();
                void clear_inputs(); // does not break linked inputs
                
                //Break input links with other NFAcontainers
                void break_links();

                //Evaluate
                bool evaluate();

                //set parallel states_
                void set_max_parallel_states(size_t);


            private:
                state::StateMap<alphabet,max_symbol,max_state> state_map_;
                std::shared_ptr<std::vector<cached_symbol<symbol_id>>> symbols_ptr_;
                std::vector<state_id> current_states_;
                size_t current_input_;
                size_t max_parallel_states_;

                bool eval_(cached_symbol<symbol_id>&);

                template<typename state_container> size_t eval_special(special_symbol, state_container&);
                template<typename state_container> size_t eval_symbol(symbol_id, state_container&);
                template<typename state_container> size_t eval_epsilon(state_container&);


                friend std::ostream& operator<<<alphabet, max_symbol, max_state>(std::ostream& os, const NFAContainer& nfa);

        };

        template<typename symbol_id>
        class InputLink{
            private:
                InputLink(std::shared_ptr<std::vector<cached_symbol<symbol_id>>>& symbols);
                std::shared_ptr<std::vector<symbol_id>> symbols_ptr_;
        };


    } // container
}// NFA

#include "nfacontainer.cpp"

#endif // NFACONTAINER_H
