#ifndef GLOBAL_HISTORY_BRANCH_PREDICTOR_H
#define GLOBAL_HISTORY_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include <cstring>
#include <iostream>


class GlobalHistoryPredictor : public BranchPredictor
{
public:
   GlobalHistoryPredictor(String name, core_id_t core_id, unsigned int index_bits_, unsigned int bhr_bits_, unsigned int pht_length_): BranchPredictor(name, core_id), pht_index_bits(index_bits_) , bhr_bits(bhr_bits_) , pht_length(pht_length_)
   {
      pht_table_entries = 1 << pht_index_bits;
      pht_table_columns = 1 << bhr_bits;
      PHT_TABLE = new unsigned long long[pht_table_entries*pht_table_columns];
      memset(PHT_TABLE, 0, pht_table_entries * pht_table_columns * sizeof(*PHT_TABLE));
      bhr=0;
      COUNTER_MAX=(1 << pht_length) - 1;
   };
   ~GlobalHistoryPredictor()
   {
      delete PHT_TABLE;
   };

   /**
    * Called from the Simulator to predict the result of branch instruction
    * in address 'ip' and target 'target'.
    **/
   bool predict(IntPtr ip, IntPtr target)
   {


      unsigned int pht_index = ip % pht_table_entries;
      unsigned long long pht_value = PHT_TABLE[pht_index*pht_table_columns + bhr];
      unsigned long long prediction = pht_value >> (pht_length-1);
      return (prediction != 0);


   };

   /**
    * Called from the Simulator to update the internal state of the predictor.
    **/
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target)
   {

      unsigned int pht_index = ip % pht_table_entries;
      unsigned long long pht_value = PHT_TABLE[pht_index*pht_table_columns + bhr];


      if (actual){
            if (pht_value < COUNTER_MAX){
                PHT_TABLE[pht_index*pht_table_columns + bhr]++;
            }
      } else {
        if (pht_value > 0){
            PHT_TABLE[pht_index*pht_table_columns + bhr]--;
        }
      }



      bhr = bhr << 1;

      if (actual) {
        bhr++;
      }
      bhr = bhr % (1 << bhr_bits);

      updateCounters(predicted, actual);
   };

private:
   unsigned int pht_index_bits, bhr_bits, pht_length, bhr;
   unsigned int COUNTER_MAX;

   /* Make this unsigned long long so as to support big numbers of cntr_bits. */

   unsigned long long *PHT_TABLE;
   unsigned int pht_table_entries, pht_table_columns;
};

#endif
