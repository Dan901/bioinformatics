#include "anchor.h"

void Anchor::calculateConflictIndex(){
  if(firstNP != -1 && secondNP != -1){
      conflictIndex = secondNP / (double)firstNP;
  }
};