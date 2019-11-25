# String-Pattern-Matching-Algorithm

The algorithm matches multiple patterns simultaneously, by first constructing a Deterministic Finite Automaton (DFA)
representing the patterns set, and then, with this DFA on its disposal, processing the text in a single pass. 
Specifically, the DFA construction is done in two phases. First, the algorithm builds a DFA of the patterns set:
All the patterns are added from the root as chains, where each state corresponds to one symbol.
When patterns share a common prefix, they also share the corresponding set of states in the DFA.
The edges of the first phase are called forward transitions.   The edges of the second phase are called failure transitions.
These edges deal with situations where, given an input symbol b and a state s, there is no forward transition from s using b.
In such a case, the DFA should follow the failure transition to some state s' and take a forward transition from there.
This process is repeated until a forward transition is found.
