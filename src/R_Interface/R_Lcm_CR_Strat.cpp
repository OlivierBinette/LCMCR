/* 
 * Copyright (C) 2007-2019 Daniel Manrique-Vallier
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#include <string>
#include <vector>
#include <set>
#include "daniel2/Model_Environ.h"
#include "daniel2/R_Environ_Simple.h"
#include "daniel2/dan_sys.h"
#include "NPLCM_CR_Strat_Freq.h"
#include "R_Lcm_CR_Strat.h"


//CR_Basic: Classes for this sampler.
typedef CData_DM_Strat DB;
typedef CNPLCM_CR_Strat_Freq MB;
typedef CParams_NPLCM_CR_Strat_Freq PB;

//template definition
class LNPLCM_CR_Strat_env : public CModel_Environ_Simple<DB, MB, PB>{
public:
	typedef CModel_Environ_Simple<DB, MB, PB> mit_t;
	LNPLCM_CR_Strat_env(DB* _data, PB* _par, int _len_buffer, int _subsamp=1, bool _del_objects=true) 
			:	mit_t(_data, _par, _len_buffer, _subsamp, _del_objects){
	}
	~LNPLCM_CR_Strat_env(){
	}
protected:
	void Other_Tracing_Options(){
	}
	void Other_Updating_Options(){
		R_CheckUserInterrupt();
	}
};

SEXP R_Create_LCM_CR_Strat(
	SEXP x_flat, SEXP n_strat_plus_NA, SEXP J_plus_strat,
	SEXP n, SEXP K,
	SEXP a_alpha, SEXP b_alpha, SEXP a_lambda, SEXP b_lambda, 
	SEXP min_n, SEXP min_lists, SEXP N_Max_factor,
	SEXP len_buffer, SEXP subsamp)
{
	//objects creation inside "try" block. This allows stack unwinding.
	
	LNPLCM_CR_Strat_env* e;
	try {
		DB* d = new DB();
		std::vector<int> levels(*INTEGER(J_plus_strat)); // Space for Stratification column
		levels[0] = *INTEGER(n_strat_plus_NA); //Numbe of strata + NA
		std::fill(levels.begin() + 1, levels.end(), 2);
		d->Set_Manually(INTEGER(x_flat), *INTEGER(J_plus_strat), *INTEGER(n), &(levels[0]));
		e = new LNPLCM_CR_Strat_env(d, 
			new PB(d, *INTEGER(K), 
			*REAL(a_alpha), *REAL(b_alpha), *REAL(a_lambda), *REAL(b_lambda),
			*INTEGER(min_n), *INTEGER(min_lists), *REAL(N_Max_factor)), 
			*INTEGER(len_buffer), *INTEGER(subsamp), true);	
	} catch (const std::exception& e){
		DAN_ERR_EXIT(e.what());
	}
	SEXP ext = PROTECT(R_MakeExternalPtr((void*)e, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(ext, finalizer_Env, TRUE);
    UNPROTECT(1);
	return(ext);
}
