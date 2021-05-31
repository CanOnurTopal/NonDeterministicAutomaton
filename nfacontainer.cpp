#ifndef NFACONTAINER_CPP
#define NFACONTAINER_CPP
#include "nfacontainer.h"
#include <algorithm>
#include <execution>
namespace NFA {
    namespace container {

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>::NFAContainer(state_id initial_state_size):
            state_map_(initial_state_size)
        {
            symbols_ptr_ = std::make_shared<std::vector<symbol_id>>();
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>::NFAContainer(const NFAContainer& other):
            state_map_(other.state_map_), current_states_(other.current_states_)
        {
            symbols_ptr_ = std::make_shared<std::vector<symbol_id>>(other.symbols_ptr_);
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>::NFAContainer(NFAContainer&& other):
            state_map_(std::move(other.state_map_)),
            symbols_ptr_(std::move(other.symbols_ptr_)),
            current_states_(std::move(other.current_states_))
        {
            other.symbols_ptr_ = std::make_shared<std::vector<symbol_id>>();
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>& NFAContainer<alphabet, msy, mst>::operator=(const NFAContainer& other){
            if (this != &other){
                state_map_ = other.state_map_;
                symbols_ptr_ = std::make_shared<std::vector<symbol_id>>(other.symbols_ptr_);
                current_states_ = other.current_states_;
            }
            return *this;
        }

        template<typename alphabet, size_t msy, size_t mst>
        NFAContainer<alphabet, msy, mst>& NFAContainer<alphabet, msy, mst>::operator=(NFAContainer&& other){
            if (this != &other){
                state_map_ = std::move(other.state_map_);
                symbols_ptr_ = std::move(other.symbols_ptr_);
                other.symbols_ptr_ = std::make_shared<std::vector<symbol_id>>();
                current_states_ = std::move(other.current_states_);
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
            symbol_id id = state_map_.get_symbol_id(symbol);
            symbols_ptr_->push_back(id);
            if constexpr (sizeof... (args) != 0) return input_symbols(args...);
        }

        template<typename alphabet, size_t msy, size_t mst>
        template<typename Iterator>
        void NFAContainer<alphabet, msy, mst>::input_iterator(Iterator iter_start, Iterator iter_end){
            while (iter_start != iter_end){
                input(*(iter_start++));
            }
        }

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::copy_input(const NFAContainer& other){
            symbols_ptr_ = std::make_shared<std::vector<symbol_id>>(*(other.symbols_ptr_));
        } //Copy input from another container

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::copy_input(const InputLink<symbol_id>& other){
            symbols_ptr_ = std::make_shared<std::vector<symbol_id>>(*(other.symbols_ptr_));
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

        //Evaluate
        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::evaluate(){
            if (current_states_.empty()) current_states_.push_back(get_start_state());
            for (symbol_id symbol: *symbols_ptr_){
                eval_symbol(symbol);
            }

        }

        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::eval_symbol(symbol_id symbol){ //will replace init states
            std::vector<std::vector<state_id>const *> next_states;
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
            current_states_.resize(next_states_size);
            auto current_states_iter = current_states_.begin();
            for (auto& item: next_states){
                auto item_size = item->size();
                std::move(item->begin(),item->end(), current_states_iter);
                std::advance(current_states_iter, item_size);
            }
        }

        /*
        template<typename alphabet, size_t msy, size_t mst>
        void NFAContainer<alphabet, msy, mst>::eval_symbol(symbol_id symbol){ //will replace init states
            std::vector<std::vector<state_id>> next_states;
            next_states.reserve(current_states_.size() * 2);
            for(auto oldstate : current_states_){
                if (oldstate != state_map_.NULL_SYMBOL_ID) next_states.push_back(state_map_.get_transitions(oldstate,symbol));
                next_states.push_back(state_map_.get_epsilon(oldstate));
            }
            current_states_.clear();
            for (auto& item: next_states){
                std::move(item.begin(),item.end(), std::back_inserter(current_states_));
            }
        }
        */

        //InputLink
        template<typename symbol_id>
        InputLink<symbol_id>::InputLink(std::shared_ptr<std::vector<symbol_id>>& symbols):symbols_ptr_(symbols){}

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

