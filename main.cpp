#include <iostream>
#include "nfa.h"
#include <vector>
#include <chrono>

//TODO add max_current_states

void test(NFA::NFA<char, 5, 5>& machine){
    machine.evaluate();
}

NFA::NFA<char, 5, 5> get_machine(){
    auto machine = NFA::NFA<char, 5, 5>();
    auto q0 = machine.new_state();
    auto q1 = machine.new_state(true);
    auto q2 = machine.new_state();
    machine.add_transition(q0, q0, 0,-1);
    machine.add_transition(q0, q1, 1);
    machine.add_transition(q0, q2,2);
    machine.add_transition(q1, q0, -1);
    machine.add_transition(q1, q0,2);
    machine.add_transition(q1, q1,0);
    machine.add_transition(q1, q2,1);
    machine.add_transition(q2, q0,1);
    machine.add_transition(q2, q0, -1);
    machine.add_transition(q2, q1,2);
    machine.add_transition(q2, q2,0);

    for (int i = 0; i<1000000; ++i){
        machine.input_symbols(0,1,2,-1,1,0,1,2,-1,1);
        /*
        machine.input(0);
        machine.input(1);
        machine.input(2);
        machine.input(-1);
        machine.input(1);
        machine.input(0);
        machine.input(1);
        machine.input(2);
        machine.input(-1);
        machine.input(1);
*/
    }
    return machine;
}

int main()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;

    auto machine = get_machine();
    std::cout << "created machine" << std::endl;
    std::cout << "testing" << std::endl;
    auto start = high_resolution_clock::now();
    test(machine);

    auto stop = high_resolution_clock::now();
        std::cout << machine << std::endl;
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << duration.count() << "ms" << std::endl;
    return 0;
}
