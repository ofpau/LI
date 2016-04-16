union([X], [], U):- U is [X].
union([X], [Y], U):- U is [X].

main :- union([1,2,3], [2,5,4], U), write(U), nl.
