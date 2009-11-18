// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#include <iostream>
#include <fstream>
#include <float.h>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <complex>

#include "FeatureSpace.h"
#include "../Utilities/Matrix.h"
#include "../PeakProcessor/PeakData.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMEntity.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMTreeWalker.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>



namespace Engine
{
	namespace ChargeDetermination
	{
		const int num_bins = 4;
		class SVMChargeDetermine
		{
			
			//Param file and params
			char mchar_svm_param_xml_file[256];						
			std::vector <double> mvect_w;
			std::vector <double> mvect_w_aux;
            std::vector <double> mvect_b;
            std::vector <double> mvect_b_aux;
			std::vector <double> mvect_nbsv;
			std::vector <double> mvect_aux ; 
			MATRIX *mmat_vote ;
			double **mmat_vote_val ; 
			MATRIX *mmat_discriminant_scores ; 
			double ** mmat_discriminant_scores_val ; 

			
			std::vector <Engine::ChargeDetermination::FeatureList> mvect_xsup ;
			Engine::ChargeDetermination::FeatureList *mobj_support_features;
			std::vector <Engine::ChargeDetermination::FeatureList> mvect_xsup_aux ;
			


			//Test and result
			std::vector<Engine::ChargeDetermination::FeatureList> mvect_xtest;	
			std::vector<Engine::ChargeDetermination::FeatureList> mvect_xtest_aux;	
			
			std::vector <double> mvect_ypredict;
			double marr_min_values[num_features];
			double marr_max_values[num_features];
		
			// Input scan
			Engine::ChargeDetermination::FeatureList *mobj_scan_features;
			std::vector<double> mvect_mzs;			
			std::vector<double> mvect_intensities;
			int marr_pk_distribution[num_bins];
			int marr_intensity_distribution[num_bins]; 


			//For features
			MATRIX* marr_mean_charge2;
			MATRIX* marr_mean_charge3;			
			MATRIX *mmat_C;
			double mdbl_mass_CO;
			double mdbl_mass_H2O;
			double mdbl_mass_NH3;

			public:

				SVMChargeDetermine();
				~SVMChargeDetermine();

				void LoadSVMFromXml();
				void InitializeLDA();
				void InitVectors();
				//void GetFeaturesForSpectra(PeakProcessing::PeakData &msN_pk_data, PeakProcessing::Peak &parentPeak, int parentScan) ; 
				void GetFeaturesForSpectra(std::vector<double> *mzs, std::vector <double> *intensities, PeakProcessing::Peak &parentPeak, int parentScan) ; 
				void NormalizeDataSet();	
				void ClearMemory() ; 
				void SetSVMParamFile(char *file_name) ; 



				
				void GetXScores(double parent_Mz, double &xscore2, double &xscore3, double neutralLoss) ;
				void GetFisherScores(double &fscore2, double &fscore3);				
				void CalculatePeakProbabilities(double parent_Mz, double &pk1, double &pk2, double &pk3, double &pk4);
				void GetBScores(double parent_Mz, double &bscore2, double &bscore3);
				bool IdentifyIfChargeOne(std::vector<double> *mzs, std::vector <double> *intensities, PeakProcessing::Peak &parentPeak, int parentScan);				
				void NormalizeSpectra();
				void ReadValues(int scan_num, double parent_Mz, double x_score2, double x_score3, double x_score_ratio,
								double b_score2, double b_score3, double pk1_prb, double pk2_prb, double pk3_prob,
								double pk4_prb,	double fscore2, double fscore3, double x_score2_CO, double x_score3_CO, double x_score2_H2O, 
								double x_score3_H20, double x_score2_NH3, double x_score3_NH3 );
		

				int GetClassAtScanIndex(int index);
				void ResolveIntoClass();
				
				void DetermineClassForDataSet();	
				void SVMClassification(int startIndex, int stopIndex, int k_bias) ;				
				void CalculateCumSum() ; 
				double GetScoreAtScanIndex( int index ) ;


				MATRIX *GetKernel(std::vector<Engine::ChargeDetermination::FeatureList> &vect_xtest, std::vector<Engine::ChargeDetermination::FeatureList> &vect_xsup);		
		};
	}
}