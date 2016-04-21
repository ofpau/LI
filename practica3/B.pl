% Problema B
    
jump([AY, AX], [VY, VX], [BY, BX], E) :- 
    VY =:= AY - 1,
    VX =:= AX - 1,
    VY > 1, VX > 1,
    BY is VY-1,
    BX is VX-1,
    \+member([BY, BX], E).

jump([AY, AX], [VY, VX], [BY, BX], E) :- 
    VY =:= AY - 1,
    VX =:= AX,
    VY > 1, VX < VY,
    BY is VY-1,
    BX is VX,
    \+member([BY, BX], E).

jump([AY, AX], [VY, VX], [BY, BX], E) :- 
    VY =:= AY + 1,
    VX =:= AX,
    VY < 5,
    BY is VY+1, 
    BX is VX,
    \+member([BY, BX], E).

jump([AY, AX], [VY, VX], [BY, BX], E) :- 
    VY =:= AY + 1,
    VX =:= AX + 1,
    VY < 5,
    BY is VY+1, 
    BX is VX+1,
    \+member([BY, BX], E).

jump([AY, AX], [VY, VX], [BY, BX], E) :- 
    VY =:= AY,
    VX =:= AX+1,
    VX < VY,
    BY is VY, 
    BX is VX+1,
    \+member([BY, BX], E).

jump([AY, AX], [VY, VX], [BY, BX], E) :- 
    VY =:= AY,
    VX =:= AX - 1,
    VX > 1,
    BY is VY,
    BX is VX-1,
    \+member([BY, BX], E).

unSalto(EA, []) :- length(EA, 1). 
unSalto(EA, [[BolaA, BolaV]|Saltos]) :-
    member(BolaA, EA),
    member(BolaV, EA),
    jump(BolaA, BolaV, BolaB, EA),
    delete(EA, BolaV, Eaux1),
    delete(Eaux1, BolaA, Eaux2),
    append(Eaux2, [BolaB], Enext),
    unSalto(Enext, Saltos).
    
escribe([]).
escribe([[A,B] | L]) :- write(A), write(" jumps over "), write(B), nl, escribe(L).

solve(E) :-
    unSalto(E, S),
    escribe(S).

level3([[4,1], [4,2], [5,2], [5,3]]).
level4([[1,1], [2,1], [2,2], [4,2], [4,3]]).
level5([[2,1], [2,2], [3,1], [3,2], [4,1], [4,2], [4,3], [4,4], [5,2], [5,4]]).
levelT([[1,1], [2,1], [2,2], [3,2], [3,3], [4,1], [4,2], [4,3], [4,4], [5,1], [5,2], [5,3], [5,4], [5,5]]).

main :- levelT(LVL), solve(LVL).
