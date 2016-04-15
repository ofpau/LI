#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
using namespace std;

#define UNDEF -1
#define TRUE 1
#define FALSE 0 

uint numVars;
uint numClauses;
const uint litsPerClause = 3;
vector<vector<int> > clauses;
vector<int> model;
vector<int> modelStack;
uint indexOfNextLitToPropagate;
uint decisionLevel;
uint propagations, decisions;
uint orderPositionOfNextLit;
vector<vector<int> > clausesWhereVarIsPos;
vector<vector<int> > clausesWhereVarIsNeg;
vector<int> orderByInfluence;  // could be vector<uint>
// clausesWhereVarIsPos[i] contains
// the clauses where i appears positively
bool influenceOrderFunction(int a, int b) {
	// TODO: don't call size(), store the size
	//		 in the element 0 of the vector
  return clausesWhereVarIsPos[a].size()+clausesWhereVarIsNeg[a].size() >
         clausesWhereVarIsPos[b].size()+clausesWhereVarIsNeg[b].size();
}

void readClauses( ){
  // Skip comments
  char c = cin.get();
  while (c == 'c') {
    while (c != '\n') c = cin.get();
    c = cin.get();
  }  
  // Read "cnf numVars numClauses"
  string aux;
  cin >> aux >> numVars >> numClauses;
  clauses.resize(numClauses);  
  clausesWhereVarIsPos.resize(numVars+1, vector<int>(0));
  clausesWhereVarIsNeg.resize(numVars+1, vector<int>(0));
  orderByInfluence.resize(numVars+1, 0);

  uint j = 1;
  // Read clauses
  for (uint i = 0; i < numClauses; ++i) {
    int lit;
    while (cin >> lit and lit != 0) {
        if (j <= numVars) {
          // (a bit dirty) way of avoiding a 
          // new loop of numVars iterations.
          orderByInfluence[j] = j;
          ++j;
        }
        clauses[i].push_back(lit);
        if (lit > 0) clausesWhereVarIsPos[lit].push_back(i);
        else clausesWhereVarIsNeg[-lit].push_back(i);
    }
  }
  // for (int i = 1; i < numVars+1; ++i) {
  //   clausesWhereVarIsNeg[i][0] = clausesWhereVarIsNeg[i].size(); 
  //   clausesWhereVarIsPos[i][0] = clausesWhereVarIsPos[i].size(); 
  // }
  sort(orderByInfluence.begin()+1, orderByInfluence.end(), influenceOrderFunction);
}

int currentValueInModel(int lit){
  if (lit >= 0) return model[lit];
  else {
    if (model[-lit] == UNDEF) return UNDEF;
    else return 1 - model[-lit];
  }
}


void setLiteralToTrue(int lit){
  modelStack.push_back(lit);
  if (lit > 0) model[lit] = TRUE;
  else model[-lit] = FALSE;		
}


bool propagateGivesConflict ( ) {

  while ( indexOfNextLitToPropagate < modelStack.size() ) {

    int currLit = modelStack[indexOfNextLitToPropagate];
    //cout << "Propagating " << abs(currLit);
    ++indexOfNextLitToPropagate;
    ++propagations;

    vector<int>* clausesToPropagate = &clausesWhereVarIsPos[abs(currLit)];
    if (currentValueInModel(abs(currLit)) == TRUE) {
      clausesToPropagate = &clausesWhereVarIsNeg[abs(currLit)];
    //  cout << " = true, " ;
    } //else cout << " = false, ";
    //cout << "to " << clausesToPropagate.size() << " clauses." << endl;
    int s = clausesToPropagate->size();
    for (uint i = 0; i < s; ++i) {
      bool someLitTrue = false;
      int numUndefs = 0;
      int lastLitUndef = 0;
      vector<int>* clause = &clauses[(*clausesToPropagate)[i]];
      //cout << "   Present in clause " << clause << ": ";
      
      for (uint k = 0; not someLitTrue and k < litsPerClause; ++k) {
       //cout << clauses[clause][k] << " ";
       int val = currentValueInModel((*clause)[k]);
       if (val == TRUE) someLitTrue = true;
       else if (val == UNDEF){ ++numUndefs; lastLitUndef = (*clause)[k]; }
      }
      //cout << " -> " ;
      //if (someLitTrue) cout << "true"; else cout << "false";
      //cout << ". Last undefined lit: " << lastLitUndef << endl;
      if (not someLitTrue and numUndefs == 0) return true; // conflict! all lits false
      else if (not someLitTrue and numUndefs == 1) setLiteralToTrue(lastLitUndef);  
    }
  }
  return false;
}


void backtrack(){
  uint i = modelStack.size() -1;
  int lit = 0;
  while (modelStack[i] != 0){ // 0 is the DL mark
    lit = modelStack[i];
    model[abs(lit)] = UNDEF;
    modelStack.pop_back();
    --i;
  }
  // at this point, lit is the last decision
  modelStack.pop_back(); // remove the DL mark
  --decisionLevel;
  indexOfNextLitToPropagate = modelStack.size();
  setLiteralToTrue(-lit);  // reverse last decision
  orderPositionOfNextLit = 1;
}


// Heuristic for finding the next decision literal:
int getNextDecisionLiteral(){
  //return orderByInfluence[modelStack.size()+1];
  // returns most influent UNDEF vars, positively
  for (uint i = orderPositionOfNextLit; i <= numVars+1; ++i) 
    if (model[orderByInfluence[i]] == UNDEF) return orderByInfluence[i];  
  return 0; // reurns 0 when all literals are defined

}

void checkmodel(){
  for (int i = 0; i < numClauses; ++i){
    bool someTrue = false;
    for (int j = 0; not someTrue and j < litsPerClause; ++j)
      someTrue = (currentValueInModel(clauses[i][j]) == TRUE);
    if (not someTrue) {
      cout << "Error in model, clause is not satisfied:";
      for (int j = 0; j < litsPerClause; ++j) cout << clauses[i][j] << " ";
      cout << endl;
      exit(1);
    }
  }  
}

void printStats() {
  //cout << "decisions: " << decisions << ", propagations: " << propagations << endl;
}

int main(){ 
  readClauses(); // reads numVars, numClauses and clauses
  model.resize(numVars+1,UNDEF);
    
  indexOfNextLitToPropagate = 0;  
  orderPositionOfNextLit	= 1;
  decisions = propagations = decisionLevel = 0;
  
  // Take care of initial unit clauses, if any
  for (uint i = 0; i < numClauses; ++i)
    if (clauses[i].size() == 1) {
      int lit = clauses[i][0];
      int val = currentValueInModel(lit);
      if (val == FALSE) {cout << "UNSATISFIABLE" << endl; return 10;}
      else if (val == UNDEF) setLiteralToTrue(lit);
  }
  //for (uint i = 0; i < orderByInfluence.size(); ++i) cout << orderByInfluence[i] << " ";
  
  // DPLL algorithm
  while (true) {
    while ( propagateGivesConflict() ) {
      if (decisionLevel == 0) { printStats(); cout << "UNSATISFIABLE" << endl; return 10; }
      backtrack();
    }
    int decisionLit = getNextDecisionLiteral();
    ++decisions;
    if (decisionLit == 0) { checkmodel(); printStats(); cout << "SATISFIABLE" << endl; return 20; }
    // start new decision level:
    modelStack.push_back(0);  // push mark indicating new DL
    ++indexOfNextLitToPropagate;
    ++decisionLevel;
    setLiteralToTrue(decisionLit);    // now push decisionLit on top of the mark
  }
}  
