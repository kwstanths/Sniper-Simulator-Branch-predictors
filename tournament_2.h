#ifndef TOURNAMENT_2_BRANCH_PREDICTOR_H
#define TOURNAMENT_2_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include <cstring>
#include <iostream>

class Tournament_2_Predictor : public BranchPredictor
{
public:
   Tournament_2_Predictor(String name, core_id_t core_id, unsigned int index_bits_, unsigned int bhr_bits_, unsigned int pht_length_, unsigned int local_index_bits_, unsigned int cntr_bits_): BranchPredictor(name, core_id), global_pht_index_bits(index_bits_), bhr_bits(bhr_bits_), global_pht_length(pht_length_), local_bht_index_bits(local_index_bits_), local_bht_length(cntr_bits_) 
   {

      // Initialising the global predictor
      global_pht_table_entries = 1 << global_pht_index_bits;
      global_pht_table_columns = 1 << bhr_bits;
      GLOBAL_PHT_TABLE = new unsigned long long[global_pht_table_entries*global_pht_table_columns];
      memset(GLOBAL_PHT_TABLE, 0, global_pht_table_entries*global_pht_table_columns*sizeof(GLOBAL_PHT_TABLE));
      bhr =0;
      global_COUNTER_MAX =(1 << global_pht_length) - 1;

      //Initialising the local predictor
      local_bht_table_entries = 1 << local_bht_index_bits;
      local_pht_table_entries = 4096;
      LOCAL_BHT_TABLE = new unsigned long long[local_bht_table_entries];
      memset(LOCAL_BHT_TABLE,0 ,local_bht_table_entries*sizeof(LOCAL_BHT_TABLE));
      LOCAL_PHT_TABLE = new unsigned long long[local_pht_table_entries];
      memset(LOCAL_PHT_TABLE, 0, local_pht_table_entries*sizeof(LOCAL_PHT_TABLE));
      local_COUNTER_MAX = 3;

      //Initialising the meta predictor
      meta_index_bits = 9;
      meta_length = 2;

      meta_table_entries = 1 << meta_index_bits;
      META_TABLE = new unsigned long long[meta_table_entries];
      memset(META_TABLE, 0 ,meta_table_entries*sizeof(*META_TABLE));
      meta_COUNTER_MAX=(1 << meta_length) - 1;
      //Since they all are 2bit predictors we will use the same COUNTER_MAX
 	  	
   };
   ~Tournament_2_Predictor()
   {
      delete META_TABLE;
      delete GLOBAL_PHT_TABLE;
      delete LOCAL_PHT_TABLE;
      delete LOCAL_BHT_TABLE;
   };

   /**
    * Called from the Simulator to predict the result of branch instruction
    * in address 'ip' and target 'target'.
    **/
   bool predict(IntPtr ip, IntPtr target)
   {
      unsigned int global_pht_index = ip % global_pht_table_entries;     
      unsigned long long global_pht_value = GLOBAL_PHT_TABLE[global_pht_index*global_pht_table_columns + bhr];
      unsigned long long prediction_one = global_pht_value >> (global_pht_length -1);
      prediction_1 = prediction_one != 0;

      unsigned int local_bht_index = ip % local_bht_table_entries;
      unsigned long long local_bht_value = LOCAL_BHT_TABLE[local_bht_index];
      unsigned int local_pht_index = ip % (1 << (12 - local_bht_length));
      local_pht_index = local_pht_index << local_bht_length;
      local_pht_index = local_pht_index + local_bht_value;
      unsigned long long local_pht_value  = LOCAL_PHT_TABLE[local_pht_index];
      unsigned long long prediction_two = local_pht_value >> 1;
      prediction_2 = prediction_two != 0;

      unsigned long long meta_index = ip % meta_table_entries;
      unsigned long long meta_value = META_TABLE[meta_index];

      if (meta_value > 1) {
	   return (prediction_2);
      } else return (prediction_1);

   };

   /**
    * Called from the Simulator to update the internal state of the predictor.
    **/
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target)
   {

      //Update the global predictor
      unsigned int global_pht_index = ip % global_pht_table_entries;
      unsigned long long global_pht_value = GLOBAL_PHT_TABLE[global_pht_index*global_pht_table_columns + bhr];
      if (actual){
      	   if (global_pht_value < global_COUNTER_MAX){
               GLOBAL_PHT_TABLE[global_pht_index*global_pht_table_columns + bhr]++;   
	   }
      }else {
           if (global_pht_value > 0){
               GLOBAL_PHT_TABLE[global_pht_index*global_pht_table_columns + bhr]--;
	   }
      }
      bhr = bhr << 1;
      if (actual){
          bhr++;
      }
      bhr = bhr % (1 << bhr_bits);
 
      //Update the local predictor
      unsigned int local_bht_index = ip % local_bht_table_entries;
      unsigned long long local_bht_value = LOCAL_BHT_TABLE[local_bht_index];
      unsigned int local_pht_index = ip % (1 << (12 - local_bht_length));    
      local_pht_index = local_pht_index << local_bht_length;
      local_pht_index = local_pht_index + local_bht_value;
      unsigned long long local_pht_value = LOCAL_PHT_TABLE[local_pht_index];
      if (actual){
          if (local_pht_value < local_COUNTER_MAX){
              LOCAL_PHT_TABLE[local_pht_index]++; 
          }
      }else {
          if (local_pht_value > 0){
              LOCAL_PHT_TABLE[local_pht_index]--;
          }
      }
 
      local_bht_value = local_bht_value << 1;
      if (actual){
	  local_bht_value++;	
       }
       local_bht_value = local_bht_value % local_bht_table_entries;
       LOCAL_BHT_TABLE[local_bht_index] = local_bht_value;

      //Update the meta predictor
     unsigned long long meta_index = ip % meta_table_entries;
     unsigned long long meta_value = META_TABLE[meta_index];
     if (prediction_1 != prediction_2){
     	    if(prediction_2 == actual) {
		   if (meta_value < meta_COUNTER_MAX){
			  META_TABLE[meta_index]++;
		   }
 	    }else if (prediction_1 == actual) {
		  if (meta_value > 0) {
			  META_TABLE[meta_index]--;
		  }
	    }
     }
     
      updateCounters(predicted, actual);
   };

private:
   //Global predictor variables
   unsigned int global_pht_index_bits, bhr_bits, global_pht_length, bhr;
   unsigned int global_COUNTER_MAX;
   unsigned long long *GLOBAL_PHT_TABLE;
   unsigned int global_pht_table_entries, global_pht_table_columns;
   
   //Local predictor variables 
   unsigned int local_bht_index_bits, local_bht_length;
   unsigned int local_COUNTER_MAX;
   unsigned long long *LOCAL_BHT_TABLE, *LOCAL_PHT_TABLE;
   unsigned int local_bht_table_entries, local_pht_table_entries;

    //Meta predictor variables
   unsigned int meta_index_bits, meta_length;
   unsigned int meta_COUNTER_MAX;  
   unsigned long long *META_TABLE;
   unsigned int meta_table_entries;

   bool prediction_1, prediction_2;
};

#endif
