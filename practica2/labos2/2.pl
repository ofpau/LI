prod([X], P):- P is X, !.
prod([X|L], P):- prod(L, P1), P is X*P1.

main:- prod([2,5,3], N), write(N), nl.