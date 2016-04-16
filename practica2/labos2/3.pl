pescalar([X], [Y], P):- P is X*Y.
pescalar([X|L1], [Y|L2], P) :- 
		pescalar(L1, L2, P1), 
		P is X*Y + P1.

main:- pescalar([2,5,3], [4,1,2], N), write(N), nl.

%% 2,5,3   
%% 4,1,2 
%% = 8+5+6 = 19