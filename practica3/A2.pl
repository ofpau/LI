% Problema(s) A1

camino(E,E, C,C, _).
camino(EstadoActual, EstadoFinal, CaminoHastaAhora, CaminoTotal, N) :- 
	unPaso(EstadoActual, EstSiguiente, N),
	\+member(EstSiguiente, CaminoHastaAhora),
	camino(EstSiguiente, EstadoFinal, [EstSiguiente|CaminoHastaAhora], CaminoTotal, N).

unPaso([I, J], [Inext, Jnext], _):- 
            I > 1, 
            J > 0, 
            Inext is I-2, Jnext is J-1.

unPaso([I, J], [Inext, Jnext], _):- 
            I > 0, 
            J > 1, 
            Inext is I-1, Jnext is J-2.

unPaso([I, J], [Inext, Jnext], N):- 
            I > 1, 
            J < N-1, 
            Inext is I-2, Jnext is J+1.

unPaso([I, J], [Inext, Jnext], N):- 
            I > 0, 
            J < N-2, 
            Inext is I-1, Jnext is J+2.

unPaso([I, J], [Inext, Jnext], N):- 
            I < N-2, 
            J > 0, 
            Inext is I+2, Jnext is J-1.

unPaso([I, J], [Inext, Jnext], N):- 
            I < N-1,
            J > 1, 
            Inext is I+1, Jnext is J-2.

unPaso([I, J], [Inext, Jnext], N):- 
            I < N-2, 
            J < N-1, 
            Inext is I+2, Jnext is J+1.

unPaso([I, J], [Inext, Jnext], N):- 
            I < N-1, 
            J < N-2, 
            Inext is I+1, Jnext is J+2.

nat(0).
nat(N):- nat(N1), N is N1 + 1.

solucionOptima :-
        P is 3,
        N is 5,
	camino([0,0], [0,4], [[0,0]], C, N),
	length(C,P),
	write(C).
% Buscamos solucion de "coste" 0; si no, de 1, etc.
% En "hacer aguas": -un estado es [cubo5,cubo8], y
% -el coste es la longitud de C.

main:- solucionOptima, nl, write("done"), nl.
