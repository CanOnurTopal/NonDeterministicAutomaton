#ifndef NFACONTAINER_CPP
#define NFACONTAINER_CPP
#include "nfacontainer.h"
#include <algorithm>
#include <execution>
namespace NFA {
    namespace container {

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>::NFAContainer(size_t initial_state_size):
            state_map_(initial_state_size),
            current_input_(0),
            max_parallel_states_(0)
        {
            symbols_ptr_ = std::make_shared<std::vector<cached_symbol<symbol_id>>>();
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>::NFAContainer(const NFAContainer& other):
            state_map_(other.state_map_),
            current_states_(other.current_states_),
            current_input_(other.current_input_),
            max_parallel_states_(other.max_parallel_states_)
        {
            symbols_ptr_ = std::make_shared<std::vector<cached_symbol<symbol_id>>>(*other.symbols_ptr_);
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>::NFAContainer(NFAContainer&& other):
            state_map_(std::move(other.state_map_)),
            symbols_ptr_(std::move(other.symbols_ptr_)),
            current_states_(std::move(other.current_states_)),
            current_input_(other.current_input_),
            max_parallel_states_(other.max_parallel_states_)
        {
            other.symbols_ptr_ = std::make_shared<std::vector<cached_symbol<symbol_id>>>();
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>& NFAContainer<alphabet, msy, mst>::operator=(const NFAContainer& other){
            if (this != &other){
                state_map_ = other.state_map_;
                symbols_ptr_ = std::make_shared<std::vector<cached_symbol<symbol_id>>>(other.symbols_ptr_);
                current_states_ = other.current_states_;
                current_input_ = other.current_input_;
                max_parallel_states_ = other.max_parallel_states_;
            }
            return *this;
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>& NFAContainer<alphabet, msy, mst>::operator=(NFAContainer&& other){
            if (this != &other){
                state_map_ = std::move(other.state_map_);
                symbols_ptr_ = std::move(other.symbols_ptr_);
                other.symbols_ptr_ = std::make_shared<std::vector<cached_symbol<symbol_id>>>();
                current_states_ = std::move(other.current_states_);
                current_input_ = other.current_input_;
                max_parallel_states_ = other.max_parallel_states_;
            }
            return *this;
        }

        //Manipulate States
        template<typename alphabet, size_t msy, size_t mst>
        typename NFAContainer<alphabet, msy, mst>::state_id NFAContainer<alphabet, msy, mst>::new_state(bool accept, bool start){
            return state_map_.new_state(accept, start);
        }

        template<typename alphabet, size_t msy, size_t mst>
        template<typename... Args>
        void NFAContainer<alphabet, msy, mst>::add_transition(state_id initial_state, state_id destination_state, alphabet symbol, Args... args){
            state_map_.add_transition(initial_state, destination_state, symbol);
            if constexpr (sizeof... (args) != 0) return add_transition(initial_state, destination_state, args...);
        }

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::add_epsilon(state_id initial_state, state_id destination_state){
            state_map_.add_epsilon(initial_state, destination_state);
        }

        //Insert symbols
        template<typename alphabet, size_t msy, size_t mst>
        template<typename... Args>
        void NFAContainer<alphabet, msy, mst>::input_symbols(const alphabet& symbol, Args... args){
            cached_symbol<symbol_id> id = state_map_.get_symbol_id(symbol);
            symbols_ptr_->push_back(id);
            if constexpr (sizeof... (args) != 0) return input_symbols(args...);
        }

        template<typename alphabet, size_t msy, size_t mst>
        template<typename... Args>
        void NFAContainer<alphabet, msy, mst>::input_symbols(special_symbol symbol, Args... args){
            cached_symbol<symbol_id> special = symbol;
            symbols_ptr_->push_back(special);
            if constexpr (sizeof... (args) != 0) return input_symbols(args...);
        }

        template<typename alphabet, size_t msy, size_t mst>
        template<typename Iterator>
        void NFAContainer<alphabet, msy, mst>::input_iterator(Iterator iter_start, Iterator iter_end){
            while (iter_start != iter_end){
                input_symbols(*(iter_start++));
            }
        }

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::copy_input(const NFAContainer& other){
            symbols_ptr_ = std::make_shared<std::vector<cached_symbol<symbol_id>>>(*(other.symbols_ptr_));
        } //Copy input from another container

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::copy_input(const InputLink<symbol_id>& other){
            symbols_ptr_ = std::make_shared<std::vector<cached_symbol<symbol_id>>>(*(other.symbols_ptr_));
        } //Copy input from InputLink

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::link_input(const NFAContainer& other){
            symbols_ptr_ = other.symbols_ptr_;
        } //Share pointer to same input

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::link_input(const InputLink<symbol_id>& other){
            symbols_ptr_ = other.symbols_ptr_;
        } //Share pointer to same input

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst> NFAContainer<alphabet, msy, mst>::get_copy() const{
            return NFAContainer(*this);
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst> NFAContainer<alphabet, msy, mst>::get_linked_copy() const{
            NFAContainer new_NFA(0);
            new_NFA.state_map_ = state_map_;
            new_NFA.symbols_ptr_ = symbols_ptr_;
            if (current_states_.size() > 0) new_NFA.current_states_ = current_states_;
            return new_NFA;
        }

        template<typename alphabet, size_t msy, size_t mst>
        InputLink<typename NFAContainer<alphabet, msy, mst>::symbol_id> NFAContainer<alphabet, msy, mst>::get_inputs() const{
            return InputLink(symbols_ptr_);
        } //Retrieve InputLink memento of the symbols

        template<typename alphabet, size_t msy, size_t mst>
        std::size_t NFAContainer<alphabet, msy, mst>::size() const{
            return state_map_.size();
        }

        //Query current state
        template<typename alphabet, size_t msy, size_t mst>
        bool NFAContainer<alphabet, msy, mst>::is_accept() const{
            return std::any_of(std::execution::par_unseq, current_states_.cbegin(), current_states_.cend(), [this](auto state){return state_map_.is_accept(state);});
        }

        template<typename alphabet, size_t msy, size_t mst>
        typename NFAContainer<alphabet, msy, mst>::state_id NFAContainer<alphabet, msy, mst>::get_start_state() const{
            return state_map_.get_start_state();
        }

        template<typename alphabet, size_t msy, size_t mst>
        std::vector<typename NFAContainer<alphabet, msy, mst>::state_id> NFAContainer<alphabet, msy, mst>::get_states() const{
            return current_states_;
        }

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::clear_states(){
            current_states_.clear();
            return;
        }

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::clear_inputs(){
            symbols_ptr_->clear();
            return;
        }

        // break_links
        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::break_links(){
            symbols_ptr_ = std::make_shared<std::vector<cached_symbol<symbol_id>>>(*symbols_ptr_);
            return;
        }


        //Evaluate
        template<typename alphabet, size_t msy, size_t mst> //Comeback TODO
        bool NFAContainer<alphabet, msy, mst>::evaluate(){
            if (current_input_ >= symbols_ptr_->size()) return false;
            if (current_states_.empty()) current_states_.push_back(get_start_state());
            bool resume = true;
            size_t symbol_size = symbols_ptr_->size();
            while (resume && current_input_ < symbol_size){
                if (max_parallel_states_ != 0 && current_states_.size() > max_parallel_states_) return false;
                resume = eval_(symbols_ptr_->operator[](current_input_));
                ++current_input_;
            }
            return true;
        }

        template<typename alphabet, size_t msy, size_t mst>
        bool NFAContainer<alphabet, msy, mst>::eval_(cached_symbol<symbol_id>& symbol){
            std::vector<std::vector<state_id>const *> next_states;
            size_t next_states_size;
            auto symbol_contains = symbol.index();
            switch (symbol_contains) {
                case 0: //Contains symbol_id
                    next_states_size = eval_symbol(std::get<0>(symbol), next_states);
                    break;
                case 1: //Contains special symbol
                    special_symbol special = std::get<1>(symbol);
                    switch (special) {
                        case special_symbol::STOP:
                            return false;
                        case special_symbol::EPSILON:
                            next_states_size = eval_epsilon(next_states);
                            break;}
                    break;
            }
            current_states_.resize(next_states_size);
            auto current_states_iter = current_states_.begin();
            for (auto& item: next_states){
                auto item_size = item->size();
                std::move(item->begin(),item->end(), current_states_iter);
                std::advance(current_states_iter, item_size);
            }
            return true;
        }

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::set_max_parallel_states(size_t val){
           max_parallel_states_ = val;
        }

        template<typename alphabet, size_t msy, size_t mst>
        template<typename state_container>
        size_t NFAContainer<alphabet, msy, mst>::eval_symbol(symbol_id symbol, state_container& next_states ){ //will replace init states
            next_states.reserve(current_states_.size() * 2);
            size_t next_states_size = 0;
            std::vector<state_id> const* next;
            for(auto oldstate : current_states_){
                if (oldstate != state_map_.NULL_SYMBOL_ID) {
                    next = &(state_map_.get_transitions(oldstate,symbol));
                    next_states.push_back(next);
                    next_states_size += next->size();
                }
                next = &(state_map_.get_epsilon(oldstate));
                next_states.push_back(next);
                next_states_size += next->size();
            }
            return next_states_size;
        }

        template<typename alphabet, size_t msy, size_t mst>
        template<typename state_container>
        size_t NFAContainer<alphabet, msy, mst>::eval_epsilon(state_container& next_states){ //process epsilons
            next_states.reserve(current_states_.size());
            size_t next_states_size = 0;
            std::vector<state_id> const* next;
            for(auto oldstate : current_states_){
                next = &(state_map_.get_epsilon(oldstate));
                next_states.push_back(next);
                next_states_size += next->size();
            }
            return next_states_size;
        }

        //InputLink
        template<typename symbol_id>
        InputLink<symbol_id>::InputLink(std::shared_ptr<std::vector<cached_symbol<symbol_id>>>& symbols):symbols_ptr_(symbols){}

        //ostream <<
        template<typename alphabet, size_t msy, size_t mst>
        std::ostream& operator<<(std::ostream& os, NFAContainer<alphabet, msy, mst> const& nfa){
            auto nfa_states = nfa.get_states();
            os << "Current States: ";
            if (nfa_states.size() == 0) os << "None";
            else{
                bool first_print = true;
                for (size_t state: nfa_states){
                    if (!first_print) os << ", ";
                    else first_print = false;
                    os << state;}
            }
            os << " --- ";
            os << "Input Accepted: ";
            if(nfa.is_accept()) os << "True";
            else os << "False";
            os << ".";
            return os;
        }

    } // container
} // NFA

#endif

