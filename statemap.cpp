#ifndef STATEMAP_CPP
#define STATEMAP_CPP

#include "statemap.h"
#include <cassert>
#include <algorithm>
#include <execution>
#include <iterator>
namespace NFA {
    namespace state {

        //Constructors and Operators
        template<typename alphabet, size_t max_a, size_t max_s>
        StateMap<alphabet, max_a, max_s>::StateMap(size_t state_count): start_state_(0){
            for (size_t i = 0; i < state_count; ++i){
                new_state();
            }
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        StateMap<alphabet, max_a, max_s>::StateMap(const StateMap& other):
            trans_(other.trans_), epsilon_(other.epsilon_), states_(other.states_), symbols_(other.symbols_), start_state_(other.start_state_) {}

        template<typename alphabet, size_t max_a, size_t max_s>
        StateMap<alphabet, max_a, max_s>& StateMap<alphabet, max_a, max_s>::operator=(const StateMap& other){
            if (this != &other){
                trans_ = other.trans_;
                epsilon_ = other.epsilon_;
                states_ = other.states_;
                symbols_ = other.symbols_;
            }
            return *this;
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        StateMap<alphabet, max_a, max_s>::StateMap(StateMap&& other ):
            trans_(std::move(other.trans_)),
            epsilon_(std::move(other.epsilon_)),
            states_(std::move(other.states_)),
            symbols_(std::move(other.symbols_)),
            start_state_(other.start_state_)
        {
            other.start_state_ = 0;
        }


        template<typename alphabet, size_t max_a, size_t max_s>
        StateMap<alphabet, max_a, max_s>& StateMap<alphabet, max_a, max_s>::operator=(StateMap&& other){
            if (this != &other){
                trans_ = std::move(other.trans_);
                epsilon_ = std::move(other.epsilon_);
                states_ = std::move(other.states_);
                symbols_ = std::move(other.symbols_);
                start_state_ = other.start_state_;
                other.start_state_ = 0;
            }
            return *this;
        }


        //Functions
        template<typename alphabet, size_t max_a, size_t max_s>
        size_t StateMap<alphabet, max_a, max_s>::size() const{
            return states_.size();
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        typename StateMap<alphabet, max_a, max_s>::state_id StateMap<alphabet, max_a, max_s>::new_state(bool is_accept, bool is_start){
            trans_.emplace_back(symbols_.size());
            epsilon_.emplace_back();
            states_.emplace_back(is_accept);
            if (is_start) start_state_ = states_.size() - 1;
            return states_.size() - 1;
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        void StateMap<alphabet, max_a, max_s>::add_transition(state_id init, state_id dest, alphabet symbol){
            symbol_id sid = get_symbol_id(symbol);
            if (sid == NULL_SYMBOL_ID){
                add_symbol(symbol);
                sid = symbols_.size() - 1;
            }
            trans_[init][sid].push_back(dest);
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        void StateMap<alphabet, max_a, max_s>::add_epsilon(state_id init, state_id dest){
            epsilon_[init].push_back(dest);
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        inline const std::vector<typename StateMap<alphabet, max_a, max_s>::state_id>& StateMap<alphabet, max_a, max_s>::get_transitions(state_id initial_state, symbol_id symbol) const{
            return trans_[initial_state][symbol];
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        inline const std::vector<typename StateMap<alphabet, max_a, max_s>::state_id>& StateMap<alphabet, max_a, max_s>::get_epsilon(state_id initial_state) const{
            return epsilon_[initial_state];
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        inline typename StateMap<alphabet, max_a, max_s>::symbol_id StateMap<alphabet, max_a, max_s>::get_symbol_id(const alphabet& symbol) const{
            auto alphabet_iter = std::find(std::execution::par_unseq, symbols_.cbegin(), symbols_.cend(), symbol);
            if (alphabet_iter != symbols_.cend()) return std::distance(symbols_.cbegin(), alphabet_iter);
            else return NULL_SYMBOL_ID;
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        void StateMap<alphabet, max_a, max_s>::add_symbol(alphabet symbol){
            symbols_.push_back(symbol);
            size_t new_size = symbols_.size();
            std::for_each(std::execution::par_unseq, trans_.begin(), trans_.end(), [new_size](std::vector<std::vector<state_id>>& col){
                col.resize(new_size);
            });
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        inline typename StateMap<alphabet, max_a, max_s>::state_id StateMap<alphabet, max_a, max_s>::get_start_state() const{
            return start_state_;
        }

        template<typename alphabet, size_t max_a, size_t max_s>
        inline bool StateMap<alphabet, max_a, max_s>::is_accept(state_id state) const{
            return states_[state].is_accept;
        }

    } // state
} // NFA

#endif // STATEMAP_CPP
