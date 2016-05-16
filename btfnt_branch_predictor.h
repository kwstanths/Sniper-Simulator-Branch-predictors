#ifndef BTFNT_PREDICTOR_H
#define BTFNT_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include <cstring>
#include <iostream>

class BTFNT_BranchPredictor : public BranchPredictor
{
public:
   BTFNT_BranchPredictor(String name, core_id_t core_id): BranchPredictor(name, core_id)
   {

    };
   ~BTFNT_BranchPredictor()
   {

   };

   /**
    * Called from the Simulator to predict the result of branch instruction
    * in address 'ip' and target 'target'.
    **/
   bool predict(IntPtr ip, IntPtr target)
   {
      /**
       * Backwards branch is taken and forward branch is not taken
       */
      if (target < ip) {
            // Taken
            return 1;
      } else {
            // Not Taken
            return 0;
      }
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
