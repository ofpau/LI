%% Problema C

numNutrients(8).
product(milk,  [2, 4, 6]).
product(meat,  [1, 8]).
product(choco, [1, 3, 5, 7]).
product(sushi, [1, 2, 3, 5, 7]).

shopping(0, _, NutRem):- length(NutRem, N), N > 0, fail.
shopping(K,[],[]):- K >= 0.

shopping(K, [Prod| L1], [N|NutRem]) :-
      product(Prod, ProdNut),
      member(N, ProdNut),
      subtract(NutRem, ProdNut, NewRem),
      K1 is K-1,
      shopping(K1, L1, NewRem).

shopping(K, L) :- numNutrients(N), numlist(1, N, NutrientsRemaining), shopping(K, L, NutrientsRemaining).

main :- shopping(3, L), write(L), nl.
