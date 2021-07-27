#ifndef NFA_UTILITIES_H
#define NFA_UTILITIES_H
#include <utility>
#include <limits>
#include <memory>
#include <vector>

namespace NFA {
    class utilities {
        public:
            template<std::size_t max_num>
            static constexpr auto get_storage_type(){
                return get_storage_type_default<max_num, std::size_t, char, int, std::size_t>();
            }

            template<std::size_t max_size>
            using storage_type = typename std::conditional<max_size == 0, std::size_t,
                typename std::conditional<max_size <= std::numeric_limits<char>::max(), char,
                    typename std::conditional<max_size <= std::numeric_limits<short int>::max(), short int,
                        typename std::conditional<max_size <= std::numeric_limits<int>::max(), int, std::size_t>::type>::type>::type>::type;


            template<typename state_id>
            using transition_matrix = std::vector<std::vector<std::vector<state_id>>>; //state_id then symbol_id then transitions

            template<typename state_id>
            using epsilon_matrix = std::vector<std::vector<state_id>>;



        private:
            template <typename inner_type>
            struct type_container{
                    using type = inner_type;
            };

            template<std::size_t max_num, typename smallest_type, typename... other_types>
            static constexpr auto get_storage_type_inner(){
                if constexpr (max_num <= std::numeric_limits<smallest_type>::max() || sizeof... (other_types) == 0) return type_container<smallest_type>();
                else return get_storage_type_inner<max_num, other_types...>();
            }

            template<std::size_t max_num, typename default_type, typename... other_types>
            static constexpr auto get_storage_type_default(){
                if constexpr (max_num == 0) return type_container<default_type>();
                else return get_storage_type_inner<max_num, other_types...>();
            }
        }; // utilities
}//NFA







#endif // NFA_UTILITIES_H
