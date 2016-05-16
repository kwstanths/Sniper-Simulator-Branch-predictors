#ifndef NBIT_BRANCH_PREDICTOR_H
#define NBIT_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include <cstring>
#include <iostream>

class NbitBranchPredictor : public BranchPredictor
{
public:
   NbitBranchPredictor(String name, core_id_t core_id, unsigned int index_bits_, unsigned int cntr_bits_): BranchPredictor(name, core_id), index_bits(index_bits_) , cntr_bits(cntr_bits_)
   {
      table_entries = 1 << index_bits;
      TABLE = new unsigned long long[table_entries];
      memset(TABLE, 0, table_entries * sizeof(*TABLE));

      COUNTER_MAX = (1 << cntr_bits) - 1;
   };
   ~NbitBranchPredictor()
   {
      delete TABLE;
   };

   /**
    * Called from the Simulator to predict the result of branch instruction
    * in address 'ip' and target 'target'.
    **/
   bool predict(IntPtr ip, IntPtr target)
   {
      unsigned int ip_table_index = ip % table_entries;
      unsigned long long ip_table_value = TABLE[ip_table_index];
      unsigned long long prediction = ip_table_value >> (cntr_bits - 1);
      return (prediction != 0);
   };

   /**
    * Called from the Simulator to update the internal state of the predictor.
    **/
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target)
   {
      unsigned int ip_table_index = ip % table_entries;
      if (actual) {
         if (TABLE[ip_table_index] < COUNTER_MAX)
            TABLE[ip_table_index]++;
      } else {
         if (TABLE[ip_table_index] > 0)
            TABLE[ip_table_index]--;
      }

      updateCounters(predicted, actual);
   };

private:
   unsigned int index_bits, cntr_bits;
   unsigned int COUNTER_MAX;

   /* Make this unsigned long long so as to support big numbers of cntr_bits. */
   unsigned long long *TABLE;
   unsigned int table_entries;
};

#endif
