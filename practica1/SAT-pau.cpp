#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <time.h>
#include <map>
using namespace std;

#define UNDEF -1
#define TRUE 1
#define FALSE 0 

uint numVars;
uint numClauses;
const uint LITS_PER_CLAUSE = 3;
vector<vector<int> > clauses;
vector<int> model;
vector<int> modelStack;
uint indexOfNextLitToPropagate;
uint decisionLevel;
uint propagations, decisions;
uint orderPositionOfNextLit;
vector<vector<vector<int> > > clausesWhereVarIs;
uint CONFLICTS_REFRESH_DELAY = 5000;
uint conflictsSinceLastRefresh = 0;
uint decisionsSinceLastRefresh = 0;
vector<uint> recentConflicts;
// clausesWhereVarIsPos[i][TRUE] contains
// the clauses where i appears with value TRUE

const bool D = false;

clock_t tStart;

inline bool influenceOrderFunction(int a, int b) {
  return clausesWhereVarIs[a][TRUE][0]+clausesWhereVarIs[a][FALSE][0] >
         clausesWhereVarIs[b][TRUE][0]+clausesWhereVarIs[b][FALSE][0];
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
  clausesWhereVarIs.resize(numVars+1, vector<vector<int> > (2, vector<int> (1,0)));
  recentConflicts.resize(numVars+1, 0);

  // Read clauses
  for (uint i = 0; i < numClauses; ++i) {
    int lit;
    while (cin >> lit and lit != 0) {
        clauses[i].push_back(lit);
        if (lit > 0) clausesWhereVarIs[lit][TRUE].push_back(i);
        else clausesWhereVarIs[-lit][FALSE].push_back(i);
    }
  }
  for (int i = 1; i < numVars+1; ++i) {
     clausesWhereVarIs[i][FALSE][0]   = clausesWhereVarIs[i][FALSE].size(); 
     clausesWhereVarIs[i][TRUE][0]    = clausesWhereVarIs[i][TRUE].size(); 
  }
}

inline int currentValueInModel(int lit){
  if (lit >= 0) return model[lit];
  else {
    if (model[-lit] == UNDEF) return UNDEF;
    else return 1 - model[-lit];
  }
}

inline void setLiteralToTrue(int lit){
  modelStack.push_back(lit);
  if (lit > 0) model[lit] = TRUE;
  else model[-lit] = FALSE;   
}

inline void updateRecentConflicts(int lit) {
  if (decisionsSinceLastRefresh > CONFLICTS_REFRESH_DELAY) {
    int s = numVars+1;
    for (uint i = 0; i < s; ++i) {
      if (D) cout << i << " " << recentConflicts[i] << ", ";
      recentConflicts[i] /= 2;
    }
    conflictsSinceLastRefresh = 0;
    decisionsSinceLastRefresh = 0;
  }
  ++conflictsSinceLastRefresh;
  recentConflicts[lit] += 10;
}

bool propagateGivesConflict ( ) {

  while ( indexOfNextLitToPropagate < modelStack.size() ) {

    int currLit = modelStack[indexOfNextLitToPropagate];
    ++indexOfNextLitToPropagate;
    ++propagations;

    vector<int>* clausesToPropagate = &clausesWhereVarIs[abs(currLit)][TRUE];
    if (currentValueInModel(abs(currLit)) == TRUE) {
      clausesToPropagate = &clausesWhereVarIs[abs(currLit)][FALSE];
    } 
    int s = clausesToPropagate->size();
    for (uint i = 1; i < s; ++i) {
      bool someLitTrue = false;
      int numUndefs = 0;
      int lastLitUndef = 0;
      vector<int>* clause = &clauses[(*clausesToPropagate)[i]];
      
      for (uint k = 0; not someLitTrue and k < LITS_PER_CLAUSE; ++k) {
       int val = currentValueInModel((*clause)[k]);
       if (val == TRUE) someLitTrue = true;
       else if (val == UNDEF){ ++numUndefs; lastLitUndef = (*clause)[k]; }
      }
      if (not someLitTrue and numUndefs == 0) {
        // conflict! all lits false
        updateRecentConflicts(abs(currLit));
        return true;
      }
      else if (not someLitTrue and numUndefs == 1) setLiteralToTrue(lastLitUndef);  
    }
  }
  return false;
}

inline void backtrack(){
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
inline int getNextDecisionLiteral(){
  uint numMaxConflicts = 0;
  uint maxScore = 0;
  int litWithMaxConflicts = 0;
  bool first = true;
  for (uint i = 1; i < numVars+1; ++i) {
    if (model[i] == UNDEF) {
      uint currentVal = recentConflicts[i] 
						+ clausesWhereVarIs[i][TRUE][0] 
						+ clausesWhereVarIs[i][FALSE][0];
	  // Entre literales con un numero de conflictos recientes similar
	  //  'desempatamos' con el numero de sus apariciones en clausulas 
      if (first or currentVal > maxScore) {
        first = false;
        maxScore = currentVal;
        litWithMaxConflicts = i;
      }
    }
  }
  return litWithMaxConflicts;
}

inline void checkmodel(){
  for (int i = 0; i < numClauses; ++i){
    bool someTrue = false;
    for (int j = 0; not someTrue and j < LITS_PER_CLAUSE; ++j)
      someTrue = (currentValueInModel(clauses[i][j]) == TRUE);
    if (not someTrue) {
      cout << "Error in model, clause is not satisfied:";
      for (int j = 0; j < LITS_PER_CLAUSE; ++j) cout << clauses[i][j] << " ";
      cout << endl;
      exit(1);
    }
  }  
}

void printStats() {
  double timeSpent = (double) (clock() - tStart)/CLOCKS_PER_SEC;
  //cout << "timeSpent" << "\t" << "\t" << "decisions" << "\t" << "propagations" 
  //      << "\t" << "propagations/timeSpent" << endl;
  cout << " " << timeSpent << " " << decisions << " " << propagations 
        << " " << propagations/timeSpent << endl;
        
}

int main(){ 
  readClauses(); // reads numVars, numClauses and clauses
  model.resize(numVars+1,UNDEF);

  tStart = clock();
    
  indexOfNextLitToPropagate = 0;  
  orderPositionOfNextLit	= 1;
  decisions = propagations = decisionLevel = 0;
  
  // Take care of initial unit clauses, if any
  for (uint i = 0; i < numClauses; ++i)
    if (clauses[i].size() == 1) {
      int lit = clauses[i][0];
      int val = currentValueInModel(lit);
      if (val == FALSE) {cout << "UNSATISFIABLE" ; printStats(); return 10;}
      else if (val == UNDEF) setLiteralToTrue(lit);
  }
  
  // DPLL algorithm
  while (true) {
    while ( propagateGivesConflict() ) {
      if (decisionLevel == 0) { cout << "UNSATISFIABLE" ; printStats(); return 10; }
      backtrack();
    }
    int decisionLit = getNextDecisionLiteral();
    ++decisions; ++decisionsSinceLastRefresh;
    if (decisionLit == 0) { checkmodel(); cout << "SATISFIABLE" ; printStats(); return 20; }
    // start new decision level:
    modelStack.push_back(0);  // push mark indicating new DL
    ++indexOfNextLitToPropagate;
    ++decisionLevel;
    setLiteralToTrue(decisionLit);    // now push decisionLit on top of the mark
  }
}  
