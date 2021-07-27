# NonDeterministicAutomaton
## To start
` #include "nfa.h"`

## Constructing an Non-Deterministic Finite State Automaton
The full constructor is
```
NFA::NFA<Alphabet, max_symbol = 0, max_state = 0>(initial_state_size = 0)
```
#### Template Parameters:

###### Alphabet:
Alphabet is the variable type that represents the entire set of unique symbols.
###### max_symbol:
*Can be left blank.* Indicates the amount of unique symbols the NFA can receive. Used for optimization purposes. Leave blank if unknown.
###### max_state:
*Can be left blank.* Indicates the maximum possible states the NFA will have in its lifetime. Used for optimization purposes. Leave blank if unknown.

#### Parameters:

###### initial_state_size:
The amount of states the NFA will have on construction. Defaults to zero.

### Demonstration:
A basic NFA can be constructed as follows:
```
NFA::NFA<char> nfa()
```
The alphabet type is passed as the first template parameter. The argument indicates how many states should the NFA start with.


## Copying/Moving NFA
The NFA object is copy/move constructable and copy/move assignable.

Keep in mind that **Copying will link the symbols container.** This means inserting a symbol to either NFA will effect both of them.

If you don't wish this, call `nfa.break_links()`
on the copy you want made independent. (If copies are only linked together, both are made independent.)
