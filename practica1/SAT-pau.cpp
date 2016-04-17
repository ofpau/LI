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
vector<int> orderByInfluence;  // could be vector<uint>
uint CONFLICTS_REFRESH_DELAY = 100000;
uint conflictsSinceLastRefresh = 0;
uint decisionsSinceLastRefresh = 0;
vector<pair<uint, uint> > recentConflictsOrdered;
map<uint, uint> litToConflictsVector;
// clausesWhereVarIsPos[i][TRUE] contains
// the clauses where i appears with value TRUE

const bool D = false;

clock_t tStart;

bool influenceOrderFunction(int a, int b) {
  // TODO: don't call size(), store the size
  //     in the element 0 of the vector
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
  //clausesWhereVarIsNeg.resize(numVars+1, vector<int>(0));
  orderByInfluence.resize(numVars+1, 0);
  recentConflictsOrdered.resize(numVars+1);

  uint j = 1;
  // Read clauses
  for (uint i = 0; i < numClauses; ++i) {
    int lit;
    while (cin >> lit and lit != 0) {
        if (j <= numVars) {
          // (a bit dirty) way of avoiding a 
          // new loop of numVars iterations.
          orderByInfluence[j] = j;
          recentConflictsOrdered[j] = make_pair(j, 0);
          litToConflictsVector[j] = j;
          ++j;
        }
        clauses[i].push_back(lit);
        if (lit > 0) clausesWhereVarIs[lit][TRUE].push_back(i);
        else clausesWhereVarIs[-lit][FALSE].push_back(i);
    }
  }
  for (int i = 1; i < numVars+1; ++i) {
     clausesWhereVarIs[i][FALSE][0]   = clausesWhereVarIs[i][FALSE].size(); 
     clausesWhereVarIs[i][TRUE][0]    = clausesWhereVarIs[i][TRUE].size(); 
  }
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

bool recentConflictsOrderFunction(pair<uint, uint> a, pair<uint, uint> b) {
  return a.second > b.second;
}

void updateRecentConflicts(int lit) {
  if (decisionsSinceLastRefresh > CONFLICTS_REFRESH_DELAY) {
    int s = numVars+1;
    sort(recentConflictsOrdered.begin()+1, recentConflictsOrdered.end(), recentConflictsOrderFunction);
    for (uint i = 0; i < s; ++i) {
      if (D) cout << recentConflictsOrdered[i].first << " " << recentConflictsOrdered[i].second << ", ";
      recentConflictsOrdered[i].second /= 2;
    }
    //cout << ".done." << endl;
    conflictsSinceLastRefresh = 0;
    decisionsSinceLastRefresh = 0;
  }
  ++conflictsSinceLastRefresh;
  uint vecPos = litToConflictsVector[lit];
  //if (conflictsSinceLastRefresh%128 == 0) cout << "vecPos: " << vecPos << endl;
  recentConflictsOrdered[vecPos].second += 20;
  /*
  int pre = vecPos-1;
  while(pre > 0 and recentConflictsOrdered[vecPos].second > recentConflictsOrdered[pre].second) {
    pre--; // Millor ordena cada cert temps en lloc d'aixo
  }
  pre++;
  if (pre > 0) {
    pair<uint, uint> temp = recentConflictsOrdered[vecPos];
    recentConflictsOrdered[vecPos] = recentConflictsOrdered[pre];
    recentConflictsOrdered[pre] = temp;
    litToConflictsVector[lit] = pre;
  }*/
}

bool propagateGivesConflict ( ) {

  while ( indexOfNextLitToPropagate < modelStack.size() ) {

    int currLit = modelStack[indexOfNextLitToPropagate];
    //cout << "Propagating " << abs(currLit);
    ++indexOfNextLitToPropagate;
    ++propagations;

    vector<int>* clausesToPropagate = &clausesWhereVarIs[abs(currLit)][TRUE];
    if (currentValueInModel(abs(currLit)) == TRUE) {
      clausesToPropagate = &clausesWhereVarIs[abs(currLit)][FALSE];
    //  cout << " = true, " ;
    } //else cout << " = false, ";
    //cout << "to " << clausesToPropagate.size() << " clauses." << endl;
    int s = clausesToPropagate->size();
    for (uint i = 1; i < s; ++i) {
      bool someLitTrue = false;
      int numUndefs = 0;
      int lastLitUndef = 0;
      vector<int>* clause = &clauses[(*clausesToPropagate)[i]];
      //cout << "   Present in clause " << clause << ": ";
      
      for (uint k = 0; not someLitTrue and k < LITS_PER_CLAUSE; ++k) {
       //cout << clauses[clause][k] << " ";
       int val = currentValueInModel((*clause)[k]);
       if (val == TRUE) someLitTrue = true;
       else if (val == UNDEF){ ++numUndefs; lastLitUndef = (*clause)[k]; }
      }
      //cout << " -> " ;
      //if (someLitTrue) cout << "true"; else cout << "false";
      //cout << ". Last undefined lit: " << lastLitUndef << endl;
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
  /*
  while(orderPositionOfNextLit <= numVars+1) {
    int l = (int) recentConflictsOrdered[orderPositionOfNextLit++].first;
    if (model[l] == UNDEF) return l;
  }*/
  //return orderByInfluence[modelStack.size()+1];
  // returns most influent UNDEF vars, positively
  while(orderPositionOfNextLit <= numVars+1) {
    int nl = orderByInfluence[orderPositionOfNextLit++];
    if (model[nl] == UNDEF) return nl; 
  }
  return 0; // reurns 0 when all literals are defined
}

void checkmodel(){
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
  cout << ". ";
  double timeSpent = (double) (clock() - tStart)/CLOCKS_PER_SEC;
  cout << "time: " << timeSpent << endl;
  cout << "dec: " << decisions 
        << ",\t prop: " << propagations 
        << ",\t prop/s: " << propagations/timeSpent << endl;

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
      if (val == FALSE) {cout << "UNSATISFIABLE" << endl; /*printStats();*/ return 10;}
      else if (val == UNDEF) setLiteralToTrue(lit);
  }
  //for (uint i = 0; i < orderByInfluence.size(); ++i) cout << orderByInfluence[i] << " ";
  
  // DPLL algorithm
  while (true) {
    while ( propagateGivesConflict() ) {
      if (decisionLevel == 0) { cout << "UNSATISFIABLE" << endl; /*printStats();*/ return 10; }
      backtrack();
    }
    int decisionLit = getNextDecisionLiteral();
    ++decisions; ++decisionsSinceLastRefresh;
    if (decisionLit == 0) { checkmodel(); cout << "SATISFIABLE" << endl;/* printStats(); */return 20; }
    // start new decision level:
    modelStack.push_back(0);  // push mark indicating new DL
    ++indexOfNextLitToPropagate;
    ++decisionLevel;
    setLiteralToTrue(decisionLit);    // now push decisionLit on top of the mark
  }
}  
