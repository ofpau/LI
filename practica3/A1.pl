% Problema(s) A1

camino(E,E, C,C).
camino(EstadoActual, EstadoFinal, CaminoHastaAhora, CaminoTotal) :- 
	unPaso(EstadoActual, EstSiguiente),
	\+member(EstSiguiente, CaminoHastaAhora),
	camino(EstSiguiente, EstadoFinal, [EstSiguiente|CaminoHastaAhora], CaminoTotal).

unPaso(EstadoActual, EstSiguiente):- llenaCubo(EstadoActual, EstSiguiente).
unPaso(EstadoActual, EstSiguiente):- vaciaCubo(EstadoActual, EstSiguiente).
unPaso(EstadoActual, EstSiguiente):- verte(EstadoActual, EstSiguiente).

llenaCubo([_,L], [S, L]):- S is 5.
llenaCubo([S,_], [S, L]):- L is 8.

vaciaCubo([_,L], [S, L]):- S is 0.
vaciaCubo([S,_], [S, L]):- L is 0.

% verte del cubo5 al cubo8
verte([S, L], [Snext, Lnext]):- (S + L) =< 8, Snext is 0, Lnext is S + L.
verte([S, L], [Snext, Lnext]):- (S + L) > 8, Snext is S-(8-L), Lnext is 8.

% verte del cubo8 al cubo5
verte([S, L], [Snext, Lnext]):- (S + L) =< 5, Snext is S + L, Lnext is 0.
verte([S, L], [Snext, Lnext]):- (S + L) > 5, Snext is 5, Lnext is L-(5-S).

nat(0).
nat(N):- nat(N1), N is N1 + 1.

solucionOptima :-
	nat(N),				    
	camino([0,0], [0,4], [[0,0]], C),   
	length(C,N),			    
	write(C).
% Buscamos solucion de "coste" 0; si no, de 1, etc.
% En "hacer aguas": -un estado es [cubo5,cubo8], y
% -el coste es la longitud de C.

main:- solucionOptima, nl.
