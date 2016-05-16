#ifndef LOCAL_HISTORY_BRANCH_PREDICTOR_H
#define LOCAL_HISTORY_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include <cstring>
#include <iostream>
#define PHT_BITS_MAX 3    /* h megisth timh twn 2 bits toy PHT table */

class LocalHistoryPredictor : public BranchPredictor
{
public:
   LocalHistoryPredictor(String name, core_id_t core_id, unsigned int index_bits_, unsigned int cntr_bits_): BranchPredictor(name, core_id), bht_index_bits(index_bits_) , bht_length(cntr_bits_)
   {
      bht_table_entries = 1 << bht_index_bits;
      BHT_TABLE = new unsigned long long[bht_table_entries];
      memset(BHT_TABLE, 0, bht_table_entries * sizeof(*BHT_TABLE));

      PHT_TABLE = new unsigned long long[8192];
      memset(PHT_TABLE, 0, 8192 * sizeof(*PHT_TABLE));

   };
   ~LocalHistoryPredictor()
   {
      delete BHT_TABLE;
      delete PHT_TABLE;
   };

   /**
    * Called from the Simulator to predict the result of branch instruction
    * in address 'ip' and target 'target'.
    **/
   bool predict(IntPtr ip, IntPtr target)
   {


      unsigned int bht_index = ip % bht_table_entries;
      unsigned long long bht_value = BHT_TABLE[bht_index];
      unsigned int pht_index = ip % (1 << (13-bht_length));
      pht_index = pht_index << bht_length;
      pht_index = pht_index + bht_value;
      unsigned long long prediction = PHT_TABLE[pht_index] >> 1;
      return (prediction != 0);


   };

   /**
    * Called from the Simulator to update the internal state of the predictor.
    **/
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target)
   {

      unsigned int bht_index = ip % bht_table_entries;
      unsigned long long bht_value = BHT_TABLE[bht_index];
      unsigned int pht_index = ip % (1 << (13-bht_length));
      pht_index = pht_index << bht_length;
      pht_index = pht_index + bht_value;


      unsigned long long pht_value = PHT_TABLE[pht_index];
      if (actual){
            if (pht_value < PHT_BITS_MAX){
                PHT_TABLE[pht_index]++;
            }
      } else {
        if (pht_value > 0){
            PHT_TABLE[pht_index]--;
        }
      }



      bht_value = bht_value << 1;

      if (actual) {
        bht_value++;
      }
      bht_value = bht_value % (1 << bht_index_bits);
      BHT_TABLE[bht_index] = bht_value;

      updateCounters(predicted, actual);
   };

private:
   unsigned int bht_index_bits, bht_length;
   unsigned int COUNTER_MAX;

   /* Make this unsigned long long so as to support big numbers of cntr_bits. */
   unsigned long long *BHT_TABLE;
   unsigned long long *PHT_TABLE;
   unsigned int bht_table_entries;
};

#endif
