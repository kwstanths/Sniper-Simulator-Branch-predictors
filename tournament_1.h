#ifndef TOURNAMENT_1_BRANCH_PREDICTOR_H
#define TOUTNAMENT_1_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include <cstring>
#include <iostream>


class Tournament_1_Predictor : public BranchPredictor
{
public:
   Tournament_1_Predictor(String name, core_id_t core_id): BranchPredictor(name, core_id)
   {
      /*
	* We implement a tournament predictor between a global (7,2) predictor with 7 bits global memory and a 2 bit prediction
	* and a 2 bit predictor with 4096 entries with 8K overhead to its predictor. The meta predictor will have 512 entries and
	* a 2bit predictor for each entry
      */

      // Initialising the global predictor
      pht_index_bits = 5;
      bhr_bits = 7;
      pht_length = 2;

      pht_table_entries = 1 << pht_index_bits;
      pht_table_columns = 1 << bhr_bits;
      PHT_TABLE = new unsigned long long[pht_table_entries*pht_table_columns];
      memset(PHT_TABLE, 0, pht_table_entries * pht_table_columns * sizeof(*PHT_TABLE));
      bhr=0;
      COUNTER_MAX=(1 << pht_length) - 1;

      //Initialising the 2 bit predictor
      nbit_index_bits = 12;
      nbit_length = 2;

      nbit_table_entries= 1 << nbit_index_bits;
      nbit_TABLE = new unsigned long long[nbit_table_entries];
      memset(nbit_TABLE, 0, nbit_table_entries*sizeof(*nbit_TABLE));

      //Initialising the meta predictor
      meta_index_bits = 9;
      meta_length = 2;

      meta_table_entries = 1 << meta_index_bits;
      META_TABLE = new unsigned long long[meta_table_entries];
      memset(META_TABLE, 0 ,meta_table_entries*sizeof(*META_TABLE));
      //Since they all are 2bit predictors we will use the same COUNTER_MAX
 	  	
   };
   ~Tournament_1_Predictor()
   {
      delete PHT_TABLE;
      delete nbit_TBALE;
      delete META_TABLE;
   };

   /**
    * Called from the Simulator to predict the result of branch instruction
    * in address 'ip' and target 'target'.
    **/
   bool predict(IntPtr ip, IntPtr target)
   {
      unsigned int pht_index = ip % pht_table_entries;
      unsigned long long pht_value = PHT_TABLE[pht_index*pht_table_columns + bhr];
      unsigned long long prediction_one = pht_value >> (pht_length-1);
      prediction_1 = prediction_one != 0;

      unsigned long long nbit_index = ip % nbit_table_entries;
      unsigned long long nbit_value = nbit_TABLE[nbit_index];
      unsigned long long prediction_two = nbit_value >> (nbit_length -1);
      prediction_2 = prediction_two != 0;

      unsigned long long meta_index = ip % meta_table_entries;
      unsigned long long meta_value = META_TABLE[meta_index];

      if (meta_value > 1) {
	   return (prediction_one != 0);
      } else return (prediction_two != 0);


   };

   /**
    * Called from the Simulator to update the internal state of the predictor.
    **/
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target)
   {

      //Update the global predictor
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

      //Update the 2bit predictor
      unsigned long long nbit_index = ip % nbit_table_entries;
      unsigned long long nbit_value = nbit_TABLE[nbit_index];
      if (actual){
            if (nbit_value < COUNTER_MAX){
                nbit_TABLE[nbit_index]++;
            }
      } else {
       	    if (nbit_value > 0){
            	nbit_TABLE[nbit_index]--;
        }
      }

     unsigned long long meta_index = ip % meta_table_entries;
     if (prediction_1 != prediction_2){
     	    if(prediction_1 == actual) {
		 if (META_TABLE[meta_index] < COUNTER_MAX){
                 	META_TABLE[meta_index]++;
		 }
 	    }else if (prediction_2 == actual) {
	 	  if (META_TABLE[meta_index] > 0) {
		  	META_TABLE[meta_index]--;
		  }
	    }
     }
     
      updateCounters(predicted, actual);
   };

private:
   unsigned int pht_index_bits, bhr_bits, pht_length, bhr;
   unsigned int nbit_index_bits, nbit_length;
   unsigned int meta_index_bits, meta_length;
   unsigned int COUNTER_MAX;

   /* Make this unsigned long long so as to support big numbers of cntr_bits. */

   unsigned long long *PHT_TABLE, *nbit_TABLE, *META_TABLE;
   unsigned int pht_table_entries, pht_table_columns;
   unsigned int nbit_table_entries;
   unsigned int meta_table_entries;

   bool prediction_1, prediction_2;
};

#endif
