% A matrix which contains zeroes and ones gets "x-rayed" vertically and
% horizontally, giving the total number of ones in each row and column.
% The problem is to reconstruct the contents of the matrix from this
% information. Sample run:
%
%	?- p.
%	    0 0 7 1 6 3 4 5 2 7 0 0
%	 0                         
%	 0                         
%	 8      * * * * * * * *    
%	 2      *             *    
%	 6      *   * * * *   *    
%	 4      *   *     *   *    
%	 5      *   *   * *   *    
%	 3      *   *         *    
%	 7      *   * * * * * *    
%	 0                         
%	 0                         
%	

:- use_module(library(clpfd)).

ejemplo1( [0,0,8,2,6,4,5,3,7,0,0], [0,0,7,1,6,3,4,5,2,7,0,0] ).
ejemplo2( [10,4,8,5,6], [5,3,4,0,5,0,5,2,2,0,1,5,1] ).
ejemplo3( [11,5,4], [3,2,3,1,1,1,1,2,3,2,1] ).

split_at(0, L, [], L).
split_at(N, [X|L], [X|F1], Rest):- N > 0, N1 is N-1, split_at(N1, L, F1, Rest).

matrixByRows([], _, []).
matrixByRows(L, NumCols, [Row|RestOfRows]):-
	split_at(NumCols, L, Row, Rest),
	matrixByRows(Rest, NumCols, RestOfRows).

listVars(0, []):-!.
listVars(N, [_|L1]):-N1 is N-1, listVars(N1, L1).

p:-	ejemplo1(RowSums,ColSums),
	length(RowSums,NumRows),
	length(ColSums,NumCols),
	NVars is NumRows*NumCols,
	listVars(NVars,L),  % generate a list of Prolog vars (their names do not matter)
	L ins 0..1,
	matrixByRows(L,NumCols,MatrixByRows),
	transpose(MatrixByRows, Ts),
	declareConstraints(MatrixByRows, RowSums),
	declareConstraints(Ts, ColSums),
	labeling([ff], L),
	pretty_print(RowSums,ColSums,MatrixByRows).

declareConstraints([], []).
declareConstraints([List|Mat], [ListSum|RS]):- sum(List, #=, ListSum), declareConstraints(Mat, RS).

pretty_print(_,ColSums,_):- write('     '), member(S,ColSums), writef('%2r ',[S]), fail.
pretty_print(RowSums,_,M):- nl,nth1(N,M,Row), nth1(N,RowSums,S), nl, writef('%3r   ',[S]), member(B,Row), wbit(B), fail.
pretty_print(_,_,_).
wbit(1):- write('*  '),!.
wbit(0):- write('   '),!.
    
main:- p, halt.