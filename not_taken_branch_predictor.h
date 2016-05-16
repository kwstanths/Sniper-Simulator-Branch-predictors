#ifndef NOT_TAKEN_BRANCH_PREDICTOR_H
#define NOT_TAKEN_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include <cstring>
#include <iostream>

class Not_Taken_BranchPredictor : public BranchPredictor
{
public:
   Not_Taken_BranchPredictor(String name, core_id_t core_id): BranchPredictor(name, core_id)
   {

   };
   ~Not_Taken_BranchPredictor()
   {

   };

   /**
    * Called from the Simulator to predict the result of branch instruction
    * in address 'ip' and target 'target'.
    **/
   bool predict(IntPtr ip, IntPtr target)
   {
      return 0;
   };

   /**
    * Called from the Simulator to update the internal state of the predictor.
    **/
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target)
   {
      updateCounters(predicted, actual);
   };

private:

};

#endif
