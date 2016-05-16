#include "simulator.h"
#include "branch_predictor.h"
#include "one_bit_branch_predictor.h"
#include "pentium_m_branch_predictor.h"
#include "config.hpp"
#include "stats.h"

/* advcomparch: include file with nbit predictor. */
#include "nbit_branch_predictor.h"
#include "not_taken_branch_predictor.h"
#include "btfnt_branch_predictor.h"
#include "local_history.h"
#include "global_history.h"
#include "tournament_1.h"
#include "tournament_2.h"

BranchPredictor::BranchPredictor()
   : m_correct_predictions(0)
   , m_incorrect_predictions(0)
{
}

BranchPredictor::BranchPredictor(String name, core_id_t core_id)
   : m_correct_predictions(0)
   , m_incorrect_predictions(0)
{
  registerStatsMetric(name, core_id, "num-correct", &m_correct_predictions);
  registerStatsMetric(name, core_id, "num-incorrect", &m_incorrect_predictions);
}

BranchPredictor::~BranchPredictor()
{ }

UInt64 BranchPredictor::m_mispredict_penalty;

BranchPredictor* BranchPredictor::create(core_id_t core_id)
{
   try
   {
      config::Config *cfg = Sim()->getCfg();
      assert(cfg);

      m_mispredict_penalty = cfg->getIntArray("perf_model/branch_predictor/mispredict_penalty", core_id);

      String type = cfg->getStringArray("perf_model/branch_predictor/type", core_id);
      if (type == "none")
      {
         return 0;
      }
      else if (type == "one_bit")
      {
         UInt32 size = cfg->getIntArray("perf_model/branch_predictor/size", core_id);
         return new OneBitBranchPredictor("branch_predictor", core_id, size);
      }
      else if (type == "pentium_m")
      {
         return new PentiumMBranchPredictor("branch_predictor", core_id);
      }
      else if (type == "nbit")
      {
         /* advcomparch: create new NbitBranchPredictor */
         UInt32 index_bits = cfg->getIntArray("perf_model/branch_predictor/index_bits", core_id);
         UInt32 cntr_bits = cfg->getIntArray("perf_model/branch_predictor/cntr_bits", core_id);
         return new NbitBranchPredictor("branch_predictor", core_id, index_bits, cntr_bits);
      }
      else if (type == "not_taken"){
         return new Not_Taken_BranchPredictor("branch_predictor", core_id);
      }else if(type == "btfnt"){
         return new BTFNT_BranchPredictor("branch_predictor", core_id);
	}
         else if (type == "local_history")
      {
	 printf("I am implementing a local history predictor\n");
         UInt32 bht_index_bits = cfg->getIntArray("perf_model/branch_predictor/bht_index_bits", core_id);
         UInt32 bht_length = cfg->getIntArray("perf_model/branch_predictor/bht_length", core_id);
         return new LocalHistoryPredictor("branch_predictor", core_id, bht_index_bits, bht_length);
      }
      else if (type == "global_history")
      {
 	 printf("I am implementing a global history predictor\n");
         UInt32 pht_index_bits = cfg->getIntArray("perf_model/branch_predictor/pht_index_bits", core_id);
         UInt32 bhr_bits = cfg->getIntArray("perf_model/branch_predictor/bhr_bits", core_id);
         UInt32 pht_length = cfg->getIntArray("perf_model/branch_predictor/pht_length", core_id);
         return new GlobalHistoryPredictor("branch_predictor", core_id, pht_index_bits, bhr_bits, pht_length);
      }else if (type == "tournament_1"){
	 printf("I am implementing a tournament predictor\n");	
         return new Tournament_1_Predictor("branch_predictor", core_id);
	}else if (type == "tournament_2") {

	 UInt32 pht_index_bits = cfg->getIntArray("perf_model/branch_predictor/pht_index_bits", core_id);
         UInt32 bhr_bits = cfg->getIntArray("perf_model/branch_predictor/bhr_bits", core_id);
         UInt32 pht_length = cfg->getIntArray("perf_model/branch_predictor/pht_length", core_id);

 	 UInt32 bht_index_bits = cfg->getIntArray("perf_model/branch_predictor/bht_index_bits", core_id);
         UInt32 bht_length = cfg->getIntArray("perf_model/branch_predictor/bht_length", core_id);
	 
         return new Tournament_2_Predictor("branch_predictor", core_id, pht_index_bits, bhr_bits, pht_length, bht_index_bits, bht_length);
	}else
      {
         LOG_PRINT_ERROR("Invalid branch predictor type.");
         return 0;
      }
   }
   catch (...)
   {
      LOG_PRINT_ERROR("Config info not available while constructing branch predictor.");
      return 0;
   }

   return 0;
}

UInt64 BranchPredictor::getMispredictPenalty()
{
   return m_mispredict_penalty;
}

void BranchPredictor::resetCounters()
{
  m_correct_predictions = 0;
  m_incorrect_predictions = 0;
}

void BranchPredictor::updateCounters(bool predicted, bool actual)
{
   if (predicted == actual)
      ++m_correct_predictions;
   else
      ++m_incorrect_predictions;
}
