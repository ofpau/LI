
dados(0, 0, []).

dados(P, N, [K|L1]) :-
	P > 0, N > 0,
	between(1,6,K),
	P1 is P-K,
	N1 is N-1,
	dados(P1, N1, L1).

main:- dados(5,2,L), write(L), nl.
