%% Problema D

cities([1,2,3,4]).
road(1,2, 10). % road between cities 1 and 2 of 10km
road(4,1, 20).
road(2,3, 25).
road(3,4, 12).

mainroads(K, _, _):- K < 0, fail.
mainroads(K,[],[]):- K >= 0.

mainroads(K, [A-B-DistAB|M1], RemCities) :-
	road(A, B, DistAB),
	DistAB =< K,
	member(A, RemCities),
	\+member(B, RemCities),
	delete(RemCities, A, NewRem),
	K1 is K-DistAB,
	mainroads(K1, M1, NewRem).

mainroads(K, [A-B-DistAB|M1], RemCities) :-
	road(A, B, DistAB),
	DistAB =< K,
	\+member(A, RemCities),
	member(B, RemCities),
	delete(RemCities, B, NewRem),
	K1 is K-DistAB,
	mainroads(K1, M1, NewRem).

mainroads(K, [A-B-DistAB|M1]):-  
	cities(Cities),
	member(A, Cities),
	road(A,B, DistAB),
	subtract(Cities, [A,B], RemainingCities),
	K1 is K-DistAB,
	mainroads(K1,M1,RemainingCities).

main :- mainroads(59, L), write(L), nl.
