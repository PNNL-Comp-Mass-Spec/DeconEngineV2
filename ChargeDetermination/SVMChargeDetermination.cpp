// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "SVMChargeDetermination.h"

namespace Engine
{
	namespace ChargeDetermination
	{
		SVMChargeDetermine::SVMChargeDetermine(void)
		{
			mobj_scan_features = new Engine::ChargeDetermination::FeatureList();	
			mobj_support_features = new Engine::ChargeDetermination::FeatureList();

			mmat_C = matrix_allocate(2, 2, sizeof(double));
			marr_mean_charge2 = matrix_allocate(1, 2, sizeof(double));
			marr_mean_charge3 = matrix_allocate(1, 2, sizeof(double));
			for (int i = 0; i < num_bins; i++)
			{
				marr_pk_distribution[i] = 0;
				marr_intensity_distribution[i] = 0;
			}

			mdbl_mass_CO = 27.9949141;
			mdbl_mass_H2O =  18.0105633;
			mdbl_mass_NH3 =  17.0265458;			

			for (int i = 0; i < mobj_scan_features->GetNumFeatures(); i++)
			{
				marr_min_values[i] = DBL_MAX ;
				marr_max_values[i] = 0 ; //DBL_MIN ;
			}

			mvect_nbsv.push_back(0) ; 
		}

		SVMChargeDetermine::~SVMChargeDetermine(void)
		{
			if (mobj_scan_features != NULL)
                delete mobj_scan_features;
			if (mobj_support_features !=NULL)
				delete mobj_support_features;
			if (mmat_C != NULL)
				matrix_free(mmat_C);
			if (marr_mean_charge2 != NULL)
				matrix_free(marr_mean_charge2);
			if (marr_mean_charge3 != NULL)
				matrix_free(marr_mean_charge3);			 
			/*delete marr_min_values;
			delete marr_max_values;
			delete marr_pk_distribution;
			delete marr_intensity_distribution;*/
			if (mvect_nbsv.size() != 0)
				mvect_nbsv.clear() ; 
			
		}

		void SVMChargeDetermine::SetSVMParamFile(char *file_name)
		{
			strcpy(mchar_svm_param_xml_file, file_name);						
		}


		void SVMChargeDetermine::InitVectors()
		{
			mvect_intensities.clear();
			mvect_mzs.clear();
			for (int bin_num = 0; bin_num < num_bins; bin_num++)
			{
				marr_pk_distribution[bin_num] = 0;
				marr_intensity_distribution[bin_num] = 0;
			}
		}

		void SVMChargeDetermine::InitializeLDA()
		{	
			double **C;
			double **mean2, **mean3;
			
			C =(double **) mmat_C->ptr;
			mean2 = (double **) marr_mean_charge2->ptr;
			mean3 = (double **) marr_mean_charge3->ptr;
			
			//notation (row, col)
			//These values were acquired from training across 3 datasets - Shew, QC, Human_tipid
			mean2[0][0] = 202.54;
			mean2[0][1] = 0.31098;			
			mean3[0][0] = 254.84;
			mean3[0][1] = 14.152;
						
			C[0][0] = 8422;
			C[0][1] = 102.4;
			C[1][0] = 102.4;
			C[1][1] = 23.746;
		}

		bool SVMChargeDetermine::IdentifyIfChargeOne(std::vector<double> *mzs, std::vector <double> *intensities, PeakProcessing::Peak &parentPeak, int parentScan)
		{
			int numPeaks = (*mzs).size();;
			double noLoss = 0;
			double parent_mz = parentPeak.mdbl_mz;			
			double temp1 = 0;
			double temp2 = 0;
			double temp3 = 0;
			double temp4 = 0;

			//Get input spectra
			InitVectors();			
			for (int i = 0 ; i < numPeaks; i++)
			{	
				mvect_intensities.push_back((*intensities)[i]);
				mvect_mzs.push_back((*mzs)[i]);
			}

			CalculatePeakProbabilities(parent_mz, temp1, temp2, temp3, temp4);	
	

			// If all frag peaks lie in first bin [0 - parent_Mz]
			if (marr_pk_distribution[0] > 0.9* numPeaks)
				return true;
			else
				return false ; 
			
		}

		void SVMChargeDetermine::GetFisherScores(double &fscore2, double &fscore3)
		{
			MATRIX *m1, *m2, *m3, *m4;
			MATRIX *first_term2, *second_term2;
			MATRIX *first_term3, *second_term3;
			MATRIX *invC, *x, *x_T;
			double prob[2];
			double totalPeaks = 0;
			
			for (int i = 0; i < num_bins; i ++)
				totalPeaks += marr_pk_distribution[i];
			
			prob[0] = (double) marr_pk_distribution[1]/totalPeaks;
			prob[1] = (double) marr_pk_distribution[2]/totalPeaks;

			x = matrix_allocate(1, 2, sizeof(double));		
			double **xvalue;
			xvalue = (double **) x->ptr;	
			
			xvalue[0][0] = (double) marr_pk_distribution[1];
			xvalue[0][1] = (double) marr_pk_distribution[2];	


			x_T = matrix_transpose(x);
			invC = matrix_invert(mmat_C);
			m1 = matrix_mult(marr_mean_charge2, invC);
			m2 = matrix_transpose(marr_mean_charge2);
			first_term2 = matrix_mult(m1, x_T);
			second_term2 = matrix_mult(m1, m2);	

			double **first_term2_value;
			double **second_term2_value;
			first_term2_value = (double **)first_term2->ptr;
			second_term2_value = (double **)second_term2->ptr;

			fscore2 = first_term2_value[0][0] - 0.5*second_term2_value[0][0] + prob[0];

			m3 = matrix_mult(marr_mean_charge3, invC);
			m4 = matrix_transpose(marr_mean_charge3);
			first_term3 = matrix_mult(m3, x_T);
			second_term3 = matrix_mult(m3, m4);

			double **first_term3_value;
			double **second_term3_value;
			first_term3_value = (double **)first_term3->ptr;
			second_term3_value = (double **)second_term3->ptr;
            fscore3 = first_term3_value[0][0] - 0.5*second_term3_value[0][0] + prob[1];
			
		}


		void SVMChargeDetermine::GetFeaturesForSpectra(std::vector<double> *mzs, std::vector <double> *intensities, PeakProcessing::Peak &parentPeak, int msNscan)
		{					
			double xscore2;
			double xscore3;
			double xscore_ratio;
			double bscore2;
			double bscore3;			
			double pk1_prob;
			double pk2_prob;
			double pk3_prob;
			double pk4_prob;
			double fscore2;
			double fscore3;
			double xscore2_CO;
			double xscore3_CO;
			double xscore2_H2O;
			double xscore3_H2O;
			double xscore2_NH3;
			double xscore3_NH3;
		
			int numPeaks = (*mzs).size();						
						
			double noLoss = 0;
			double parent_mz = parentPeak.mdbl_mz;			

			//Get input spectra
			InitVectors();			
			for (int i = 0 ; i < numPeaks; i++)
			{	
				mvect_intensities.push_back((*intensities)[i]);
				mvect_mzs.push_back((*mzs)[i]);
			}

			//Start with feature detection
			CalculatePeakProbabilities(parent_mz, pk1_prob, pk2_prob, pk3_prob, pk4_prob) ;
			GetFisherScores(fscore2, fscore3);
			NormalizeSpectra();
			GetXScores(parent_mz, xscore2, xscore3, noLoss);
			if (xscore2 != 0)
				xscore_ratio = xscore3/xscore2;
			else
				xscore_ratio = 0;
			GetBScores(parent_mz, bscore2, bscore3);
			GetXScores(parent_mz, xscore2_CO, xscore3_CO, mdbl_mass_CO);
			GetXScores(parent_mz, xscore2_H2O, xscore3_H2O, mdbl_mass_H2O);
			GetXScores(parent_mz, xscore2_NH3, xscore3_NH3, mdbl_mass_NH3);

			ReadValues(msNscan, parent_mz, xscore2, xscore3, xscore_ratio, bscore2, bscore3, pk1_prob, 
						 pk2_prob, pk3_prob, pk4_prob, fscore2, fscore3, xscore2_CO, xscore3_CO, xscore2_H2O, 
						 xscore3_H2O, xscore2_NH3, xscore3_NH3 );
			
			

		}

		void SVMChargeDetermine::ReadValues(int scan_num, double parent_Mz, double x_score2, double x_score3, double x_score_ratio,
								double b_score2, double b_score3, double pk1_prb, double pk2_prb, double pk3_prob,
								double pk4_prb, double fscore2, double fscore3, double x_score2_CO, double x_score3_CO, double x_score2_H2O, 
								double x_score3_H20, double x_score2_NH3, double x_score3_NH3 )
		{
			std::vector <double> vect_features;			
			double val = 0;
			vect_features.push_back(scan_num);
			vect_features.push_back(parent_Mz);
			vect_features.push_back(x_score2);
			vect_features.push_back(x_score3);
			vect_features.push_back(x_score_ratio);
			vect_features.push_back(b_score2);
			vect_features.push_back(b_score3);
			vect_features.push_back(pk1_prb);
			vect_features.push_back(pk2_prb);
			vect_features.push_back(pk3_prob);
			vect_features.push_back(pk4_prb); 
			vect_features.push_back(fscore2);
			vect_features.push_back(fscore3);
			vect_features.push_back(x_score2_CO);
			vect_features.push_back(x_score3_CO);
			vect_features.push_back(x_score2_H2O);
			vect_features.push_back(x_score3_H20);
			vect_features.push_back(x_score2_NH3);
			vect_features.push_back(x_score3_NH3);
			
			mobj_scan_features->InitValues(vect_features);
			mvect_xtest.push_back(*mobj_scan_features);	

			for (int i = 0; i < num_features; i++)
			{
				val = vect_features[i];
				if (val <= marr_min_values[i])
					marr_min_values[i] = val;
				if (val >= marr_max_values[i])
					marr_max_values[i] = val;
			}

		}


		void SVMChargeDetermine::NormalizeSpectra()
		{
			double threshold = 5;
			double median = 0;
			int size = mvect_intensities.size();
			std::vector<double> vect_sort_intensities;

			for (int i = 0; i < size; i++)
			{
				vect_sort_intensities.push_back(mvect_intensities[i]);
			}

			std::sort(vect_sort_intensities.begin(), vect_sort_intensities.end());			
			
			if(size%2 == 0)
				median = vect_sort_intensities[size/2];
			else
				median = (vect_sort_intensities[(size-1)/2] + vect_sort_intensities[(size+1)/2])/2;

			for (int i = 0; i < size; i++)
			{
				double val = mvect_intensities[i]/median;
				if (val > threshold)
					mvect_intensities[i] = val;
				else
					mvect_intensities[i] = 0;
			}
		}

	

		void SVMChargeDetermine::GetXScores(double parent_Mz, double &xscore2, double &xscore3, double neutralLoss)
		{

			parent_Mz = parent_Mz-neutralLoss;
			xscore2 = 0;
			xscore3 = 0;		
			double SumMForwardCS2 = 0;				
			double mzForwardBeginCS2 = 0;
			double mzForwardEndCS2 = 0;
				
			//for xscore3
			double SumMForwardCS3_1 = 0;
			double SumMForwardCS3_2 = 0;				
			double mzForwardBeginCS3_1 = 0;
			double mzForwardEndCS3_1 = 0;
			double mzForwardBeginCS3_2 = 0;
			double mzForwardEndCS3_2 = 0;

			double SumMReverse = 0;
			double mzReverseBegin = 0;
			double mzReverseEnd = 0;
			double mz = 0;
				

			for (int i = 0; i < parent_Mz ; i ++)
			{
				//for xscore2
				SumMForwardCS2 = 0;				
				mzForwardBeginCS2 = (parent_Mz + i) - 0.5 ;
				mzForwardEndCS2 = (parent_Mz+i)+0.5 ;				
				
				//for xscore3
				SumMForwardCS3_1 = 0;
				SumMForwardCS3_2 = 0;				
				mzForwardBeginCS3_1 = (3*parent_Mz - 2*i) - 0.5 ;
				mzForwardEndCS3_1 = (3*parent_Mz - 2*i) + 0.5 ;
				mzForwardBeginCS3_2 =  (3*parent_Mz - i)/2 - 0.5;
				mzForwardEndCS3_2 = (3*parent_Mz - i)/2 + 0.5;
				
				//for both
				SumMReverse = 0;
				mzReverseBegin = (parent_Mz - i) - 0.5 ;
				mzReverseEnd = (parent_Mz - i) + 0.5 ;

				
				
				for (int j = 0; j <(int) mvect_mzs.size(); j++)
				{
					mz = mvect_mzs[j];
					if (mz >= mzForwardBeginCS2 && mz <= mzForwardEndCS2)
						SumMForwardCS2 += (mvect_intensities[j]);
					if (mz >= mzForwardBeginCS3_1 && mz <= mzForwardEndCS3_1)
						SumMForwardCS3_1 += (mvect_intensities[j]);
					if (mz >= mzForwardBeginCS3_2 && mz <= mzForwardEndCS3_2)
						SumMForwardCS3_2 += (mvect_intensities[j]);
					if (mz >= mzReverseBegin && mz <= mzReverseEnd)
						SumMReverse += (mvect_intensities[j]);
				}
				xscore2 += (SumMForwardCS2 * SumMReverse);
				xscore3 += ((SumMForwardCS3_1 * SumMReverse) + (SumMForwardCS3_2 * SumMReverse));
			}
		}

		double SVMChargeDetermine::GetScoreAtScanIndex( int index ) 
		{
			double val = mmat_discriminant_scores_val[index][3] ;
			return val ; 
		}

		int SVMChargeDetermine::GetClassAtScanIndex(int index)
		{		
			//return values 1 - +1, 2 - +2, 3 - +3, 4 - +4, 0 - +2 or +3
			int num_class = mmat_vote->cols ; 
			int charge = 0 ; 
			for (int col = 0; col<num_class; col++)
			{
				int rank = (int) mmat_vote_val[index][col] ; 
				if (mmat_vote_val[index][col] == 3)
				{
					charge = col+1;					
				}				
			}	
			if (charge == 2 || charge == 3)
			{
				// As column 3 gives score between 2 and 3
				double val = mmat_discriminant_scores_val[index][3] ;
				if (val > -5 && val < 2.5) //these thresholds were found using the score distribution curves
					charge = 0 ;
			}
			return charge ; 
		}

		void SVMChargeDetermine::ClearMemory()
		{
			// clearing the matrices/ vectors (separate from the destructor as
			// initialization is done separetely from the constructor)
			matrix_free(mmat_vote) ; 
			matrix_free(mmat_discriminant_scores) ;
			if (mvect_b.size()!= 0)
				mvect_b.clear() ; 
			if(mvect_b_aux.size() != 0)
				mvect_b_aux.clear() ; 
			if (mvect_aux.size() != 0)
				mvect_aux.clear() ; 
			if(mvect_w.size() != 0)
				mvect_w.clear() ; 
			if (mvect_w_aux.size() != 0 )
				mvect_w_aux.clear() ; 
			if (mvect_xsup.size() != 0)
				mvect_xsup.clear() ;
			if (mvect_xtest.size() != 0)
				mvect_xtest.clear() ; 
			if (mvect_xsup_aux.size() != 0)
				mvect_xsup_aux.clear() ; 
			if (mvect_xtest_aux.size() != 0)
				mvect_xtest_aux.clear() ;
			if (mvect_ypredict.size() != 0)
				mvect_ypredict.clear() ; 			
		}

		void SVMChargeDetermine::ResolveIntoClass()
		{
			
			int num_rows = mmat_vote->rows;
			int num_cols = mmat_vote->cols;
			for(int row = 0; row < num_rows; row++)
			{
				int charge = 1;
				for (int col = 0; col<num_cols; col++)
				{
					if (mmat_vote_val[row][col] == 3)
					{
						charge = col+1;
						break;
					}
					mvect_ypredict[row] = charge;
				}				
			}
		}


		void SVMChargeDetermine::GetBScores(double parent_Mz, double &bscore2, double &bscore3)
		{
			bscore2 = 0;
			bscore3 = 0;

			double sumLeft = 0;
			double sumRight_CS2 = 0;
			double sumRight_CS3 = 0;
			double sumTotal_CS2 = 0;
			double sumTotal_CS3 = 0;
			double mz = 0;

			for (int i = 0; i < (int)mvect_mzs.size(); i++)
			{				
				mz = mvect_mzs[i];
				if (mz < parent_Mz - 1)
					sumLeft += mvect_intensities[i];
				if (mz > parent_Mz + 1 && mz <= (2 *parent_Mz))
					sumRight_CS2 += mvect_intensities[i];
				if (mz > parent_Mz + 1 && mz <= (3 *parent_Mz))
					sumRight_CS3 += mvect_intensities[i];
				if (mz <= (2 *parent_Mz))
					sumTotal_CS2 +=  mvect_intensities[i];
				if (mz <= (3*parent_Mz))
					sumTotal_CS3 +=  mvect_intensities[i];
			}

			if (sumTotal_CS2 > 0)
				bscore2 = (sumLeft - sumRight_CS2)/sumTotal_CS2;
			if (sumTotal_CS3 > 0 )
				bscore3 = (sumLeft - sumRight_CS3)/sumTotal_CS3;

			bscore2 = std::abs(bscore2);
			bscore3 = std::abs(bscore3);

		}

			
		void SVMChargeDetermine::CalculatePeakProbabilities(double parent_Mz, double &pk1, double &pk2, double &pk3, double &pk4)
		{

			int numPeaks = mvect_intensities.size();			
			int sum = 0;
			double mz = 0;
			double intensity = 0;
			pk1 = -1;
			pk2 = -1;
			pk3 = -1;
			pk4 = -1;

			for (int i = 1; i < numPeaks-1; i++)
			{
				mz = mvect_mzs[i];
				intensity = mvect_intensities[i];
				if (mz > 0 && mz <= parent_Mz)
				{
					marr_pk_distribution[0]++;
					marr_intensity_distribution[0] += mvect_intensities[i];
				}
				if (mz > parent_Mz && mz <= (2*parent_Mz))
				{
					marr_pk_distribution[1]++;
					marr_intensity_distribution[1] += mvect_intensities[i];
				}
				if (mz > (2*parent_Mz) && mz <= (3*parent_Mz))
				{
					marr_pk_distribution[2]++;
					marr_intensity_distribution[2] += mvect_intensities[i];
				}
				if (mz > (3* parent_Mz) && mz < (4*parent_Mz))
				{
					marr_pk_distribution[3]++;				
					marr_intensity_distribution[3] += mvect_intensities[i];
				}					
			}
			
			
			for (int i=0; i < num_bins; i++)
			{
				int val = marr_pk_distribution [i] ;				
				sum += marr_pk_distribution[i];
			}			

			
			pk1 = (double) marr_pk_distribution[0]/(double) sum;
			pk2 = (double)marr_pk_distribution[1]/(double)sum;
			pk3 = (double)marr_pk_distribution[2]/(double)sum;
			pk4 = (double)marr_pk_distribution[3]/(double)sum;
		}

		void SVMChargeDetermine::NormalizeDataSet()
		{				
			// Normalize Data Set
			int num_vectors = (int) mvect_xtest.size();		
			
			if (num_vectors < 2)
				return ;
		
			double val = 0;			
			Engine::ChargeDetermination::FeatureList this_test_vector;
			
			for (int vector_num = 0; vector_num < num_vectors; vector_num++)
			{
				this_test_vector = mvect_xtest[vector_num];
		
				for (int feature_num = 0; feature_num < num_features; feature_num++)
				{
					double max_feature = marr_max_values[feature_num] ; 
					double min_feature = marr_min_values[feature_num] ; 
					val = this_test_vector.GetValueAt(feature_num);
					if(marr_max_values[feature_num] > marr_min_values[feature_num])
						val  = (val- marr_min_values[feature_num])/ (marr_max_values[feature_num] - marr_min_values[feature_num]);
					else
						val = 0 ;
						
					this_test_vector.SetValueAt(feature_num, val);						
				}	
				mvect_xtest[vector_num] = this_test_vector;
			}
			
		}

		MATRIX *SVMChargeDetermine::GetKernel(std::vector<Engine::ChargeDetermination::FeatureList> &vect_xtest, std::vector<Engine::ChargeDetermination::FeatureList> &vect_xsup)
		{
			Engine::ChargeDetermination::FeatureList this_test_vector;
			Engine::ChargeDetermination::FeatureList this_support_vector;

			MATRIX *norm_x;
			MATRIX *norm_xsup;	
			MATRIX *norm_xsup_t;
			MATRIX *metric;
			MATRIX *xsup;
			MATRIX *xsup_t;
			MATRIX *xsup2;
			MATRIX *x;
			MATRIX *x2;
			MATRIX *ps;
			MATRIX *psTemp;			
			MATRIX *m1;
			MATRIX *m2;
			MATRIX *m3;
			MATRIX *kernel;

			double **norm_x_value;
			double **norm_xsup_value;
			double **metric_value;
			double **xsup_value;			
			double **x_value;
			double **x2_value;
			double **ps_value;			
			double **kernel_value;
			double **m1_value;
			double **m2_value;
			double **m3_value;
			

			
			int num_vectors = (int)vect_xsup.size();
			int num_test = (int) vect_xtest.size();
			int ps_rows;
			int ps_cols;

			//get vectors
			xsup = matrix_allocate(num_vectors, num_features, sizeof(double));			
			xsup_value = (double **)xsup->ptr;
			for (int i = 0; i <num_vectors;i++)
			{
				this_support_vector = vect_xsup[i];
				for (int j = 0; j < num_features; j++)
				{
					double val_sup = this_support_vector.GetValueAt(j);				
					xsup_value[i][j] = val_sup;
				}
			}
			
			x = matrix_allocate(num_test, num_features, sizeof(double));
			x_value = (double **)x->ptr;				
			for (int i = 0; i <num_test;i++)
			{
				this_test_vector = vect_xtest[i];				
				for (int j = 0; j < num_features; j++)
				{
					double val_x= this_test_vector.GetValueAt(j);					
					x_value[i][j] = val_x;
				}
			}
		
			//Init metric
			metric = matrix_allocate(num_features, num_features, sizeof(double));
			metric_value = (double**)metric->ptr;
			for (int i = 0; i < num_features; i++)
				metric_value[i][i] = 1;

			// Get ps
			m1 = matrix_mult(x, metric);
			m1_value = (double **)m1->ptr;			
			xsup_t = matrix_transpose(xsup);
			double **xsup_t_value;
			xsup_t_value = (double **) xsup_t->ptr;
			

			ps = matrix_mult(m1, xsup_t); 
			double **ps_v;
			ps_v = (double **) ps->ptr;
			ps_rows = ps->rows; //num_test
			ps_cols = ps->cols; //num_support
			
						
			// Get norms           
			norm_x = matrix_allocate(ps_rows, ps_cols, sizeof(double));
			norm_x_value = (double **)norm_x->ptr;

			norm_xsup = matrix_allocate(ps_cols, ps_rows, sizeof(double));
			norm_xsup_value = (double **)norm_xsup->ptr;
						
			x2 = matrix_mult_pwise(x, x);
			x2_value = (double **) x2->ptr;
			m2 = matrix_mult(x2, metric);
			m2_value = (double **)m2->ptr;

			for(int row_num = 0; row_num <m2->rows; row_num++)
			{
				double sumX = 0;		
				for(int col_num = 0; col_num < m2->cols; col_num++)
				{
					double val_m2 = m2_value[row_num][col_num];					
					sumX = sumX + val_m2 ;										
				}		
				//This is for ease of matrix addition
				for (int col_num=0; col_num < norm_x->cols; col_num++)
                    norm_x_value[row_num][col_num] = sumX;
			}

			xsup2 = matrix_mult_pwise(xsup, xsup);
			m3 = matrix_mult(xsup2, metric);
			m3_value = (double **)m3->ptr;
			for(int row_num = 0; row_num < m3->rows; row_num ++)
			{
				double sumXsup = 0;
				for(int col_num = 0; col_num < m3->cols; col_num++)
				{
					double val_m3 = m3_value[row_num][col_num];
					sumXsup = sumXsup+val_m3;		
				}		
				
				for(int col_num = 0; col_num < norm_xsup->cols; col_num++)
					norm_xsup_value[row_num][col_num] = sumXsup;
			}
			
			norm_xsup_t = matrix_transpose(norm_xsup);
			double **norm_xsup_t_val;
			norm_xsup_t_val = (double **) norm_xsup_t->ptr;

			double scale = -2.0;
            psTemp = matrix_scale(ps, scale);
			double **psTemp_val;
			psTemp_val = (double **) psTemp->ptr;
			
			MATRIX *ps2;
			MATRIX *ps3;			
			ps2 = matrix_add(psTemp, norm_x);
			double **ps2_val;
			ps2_val = (double **) ps2->ptr;
			ps3 = matrix_add(ps2, norm_xsup_t);
			double **ps3_val;
			ps3_val = (double **) ps3->ptr;

			matrix_free(ps);
			ps = matrix_scale(ps3, 1/scale);
			ps_value = (double **) ps->ptr;
			matrix_free(psTemp);
			matrix_free(ps2);
			matrix_free(ps3);

			kernel = matrix_allocate(ps->rows, ps->cols,ps->element_size);
			kernel_value = (double **)kernel->ptr;

			for (int row_num = 0; row_num < ps->rows; row_num ++)
			{
				for(int col_num = 0; col_num < ps->cols; col_num++)
				{
					double val = ps_value[row_num][col_num];
					double eval = exp(val);
					kernel_value[row_num][col_num] = eval;
				}
			}

		 
			matrix_free(norm_x);
			matrix_free(norm_xsup);	
			matrix_free(norm_xsup_t);
			matrix_free(metric);
			matrix_free(xsup);
			matrix_free(xsup_t);
			matrix_free(xsup2);
			matrix_free(x);
			matrix_free(x2);
			matrix_free(ps);			
			matrix_free(m1);
			matrix_free(m2);
			matrix_free(m3);	

			return (kernel) ; 
		}

		void SVMChargeDetermine::DetermineClassForDataSet()
		{
			int length_nbsv = 0 ; 
			int num_class = 0;
			int num_test = 0 ; 
			int k = 0;
			int num_iterations  = 6 ;
			int iter_num = 0 ; 
			

			length_nbsv = mvect_nbsv.size() ;
			num_class = 4 ; //(int)(1 + (int)(sqrt(1+4*2*length_nbsv)))/2 ; 
			num_test = (int) mvect_xtest.size() ; 
			mmat_vote = matrix_allocate(num_test, num_class, sizeof(double)) ; 
			mmat_vote_val = (double **) mmat_vote->ptr ;	
			mmat_discriminant_scores = matrix_allocate(num_test, num_iterations, sizeof(double)); 
			mmat_discriminant_scores_val = (double **) mmat_discriminant_scores->ptr ; 
			CalculateCumSum() ; 

			for (int row = 0; row < (int)mvect_xtest.size(); row++)
				mvect_ypredict.push_back(0) ; 
			
			for (int row = 0; row < mmat_vote->rows ; row++)
			{
				for(int col = 0; col < mmat_vote->cols ; col++)
				{
					mmat_vote_val[row][col] = 0 ; 
				}
				for (int col = 0; col < mmat_discriminant_scores->cols ; col++)
				{
					mmat_discriminant_scores_val[row][col] = 0 ; 
				}
			}

			for(int i = 0; i < num_class; i++)
			{
				for (int j = i+1; j < num_class; j++)
				{
					int startIndexToConsider = (int) mvect_aux[k] ; 
					int stopIndexToConsider = (int) (mvect_aux[k] + mvect_nbsv[k+1]) - 1 ; 
					SVMClassification(startIndexToConsider, stopIndexToConsider, k) ; 
					for (int row = 0; row < mmat_vote->rows; row++)
					{
						double val = mvect_ypredict[row];
						
						mmat_discriminant_scores_val[row][iter_num] = val ; 	

						mvect_ypredict[row] = 0 ; 
						if (val >= 0) 
						{
							double val2 = mmat_vote_val[row][i] ;
							val2++;
							mmat_vote_val[row][i] = val2 ; 
						}
						else
						{
							double val1 = mmat_vote_val[row][j] ;
							val1++ ; 
							mmat_vote_val[row][j] = val1 ;
						}
					}
					k++;
					iter_num++ ; 
				}
			}		
		}

		void SVMChargeDetermine::CalculateCumSum()
		{
			double sum = 0;
			for (int i = 0; i < (int)mvect_nbsv.size(); i++)
			{
				sum = 0 ; 				
				for (int j = 0 ; j <= i ; j++)
				{
					sum += mvect_nbsv[j] ; 
				}
				mvect_aux.push_back(sum) ; 
			}
		}

		void SVMChargeDetermine::SVMClassification(int startIndex, int stopIndex, int k_bias)
		{
			std::vector<double> span;
			std::vector<double> y;
			std::vector <Engine::ChargeDetermination::FeatureList> chunk_xtest;
			std::vector <Engine::ChargeDetermination::FeatureList> chunk_xsup;
			Engine::ChargeDetermination::FeatureList this_test_vector;
			Engine::ChargeDetermination::FeatureList this_support_vector;			
			
			int num_support = stopIndex - startIndex;
			int num_test = (int)mvect_xtest.size();
			const int chunksize = 100;
			std::vector<int> ind1;
			std::vector<int> ind2;
						
			int chunks1 = int(num_support/chunksize)+1;
			int chunks2 = int(num_test/chunksize)+1;
			
			
			// Performing y2(ind2)=y2(ind2)+ kchunk*w(ind1) ;	
			for(int ch1 = 1; ch1 <= chunks1; ch1++)
			{
				//Get ind1
				int low_ind1_index = (ch1-1)*chunksize + startIndex;
				int high_ind1_index = (ch1*chunksize) - 1 + startIndex;				
				if(high_ind1_index > stopIndex)					
					high_ind1_index = stopIndex; 
				ind1.clear();
				for(int index = 0; index <= (high_ind1_index-low_ind1_index); index++)
					ind1.push_back(index + low_ind1_index);					

				//Get support vectors
				chunk_xsup.clear();
				for (int j = 0; j < (int)ind1.size(); j++)
				{
					int xsupIndex = ind1[j];					
					this_support_vector = mvect_xsup[xsupIndex];					
					chunk_xsup.push_back(this_support_vector);
				}
				
				for(int ch2 = 1; ch2 <= chunks2; ch2++)
				{
				
					//Get ind2
					int low_ind2_index = (ch2-1)*chunksize ;
					int high_ind2_index = (ch2*chunksize) - 1;
					if(high_ind2_index > num_test)
						high_ind2_index=num_test-1;		
					ind2.clear();
					for (int index2 = 0; index2<=(high_ind2_index-low_ind2_index); index2++)
						ind2.push_back(index2 + low_ind2_index);			

					 //Get X vector
					chunk_xtest.clear();
					for(int j=0; j< (int)ind2.size(); j++)
					{
						int xIndex = ind2[j];						
						this_test_vector = mvect_xtest[xIndex];
						chunk_xtest.push_back(this_test_vector);						
					}

					//Get the kernel										
					MATRIX *svm_kernel;
					double **svm_kernel_val;
					svm_kernel = GetKernel(chunk_xtest, chunk_xsup);
					svm_kernel_val = (double **) svm_kernel->ptr;
			
					
					//Read in the weights w(ind1)
					MATRIX *w;
					double **w_value;
					w = matrix_allocate((int)ind1.size(), 1, sizeof(double));
					w_value = (double **) w->ptr;					
					for (int i=0; i<(int)ind1.size(); i++)
					{
						int index = ind1[i];
						w_value[i][0] = mvect_w[index];						
					}
					
					// m1 = kchunk*w(ind1)
					MATRIX *m1;					
					double **m1_value;										
					m1 = matrix_mult(svm_kernel, w);
					m1_value = (double**)m1->ptr;
					
					
					//y2(ind2) += m1;							
					for(int i = 0; i < (int)ind2.size(); i++)
					{
						int index = ind2[i];	
						
						int vale = (int) mvect_ypredict[index] ; 
						mvect_ypredict[index]+= m1_value[i][0];								
						vale = mvect_ypredict[index] ; 
					}		

					
					matrix_free(m1);
					matrix_free(svm_kernel);
					matrix_free(w);
				}
				
			}			
			
			//Add w0			
			for (int i = 0; i < (int) mvect_ypredict.size(); i++)
			{			
				double debug = mvect_ypredict[i];
				if (i == 6312)
				{
					debug ++ ; 
					double b = mvect_b[k_bias] ; 
					b++ ; 
				}
				mvect_ypredict[i]+= mvect_b[k_bias];
				
			}
		
		}

		void SVMChargeDetermine::LoadSVMFromXml()
		{
			static XERCES_CPP_NAMESPACE::XercesDOMParser::ValSchemes    valScheme = XERCES_CPP_NAMESPACE::XercesDOMParser::Val_Never;
			bool	doNamespaces = false;
			bool	doSchema = false;
			bool	schemaFullChecking = false;			
			bool	doCreate = false;
			bool	bFailed = false;

			const char* svm_tag = "SVMParams";
			const char* b_tag = "b";
			const char* w_tag = "w";
			const char* nbsv_tag = "nbsv";
			const char* bias_tag = "Bias";
			const char* weights_tag = "Support_Weights";
			const char* support_tag = "Support_Vectors";
			const char* bias_support_tag = "Support_Bias" ; 
			const char* xsup_tag = "xsup";
			const char* feature_tag = "feature";
			int weight_count = 0;
			int feature_count = 0;
			int support_count = 0;


			
			char *pEnd;
			//Initialize the XML 
			try
			{
				XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();			
			}
			catch(const XERCES_CPP_NAMESPACE::XMLException &toCatch)
			{
				//Do something
				return;
			}

			XERCES_CPP_NAMESPACE::XercesDOMParser *parser = new XERCES_CPP_NAMESPACE::XercesDOMParser;
			if (parser)
			{
				parser->setValidationScheme(valScheme);
				parser->setDoNamespaces(doNamespaces);
				parser->setDoSchema(doSchema);
				parser->setValidationSchemaFullChecking(schemaFullChecking);
				try
				{
					parser->parse(mchar_svm_param_xml_file);				
				}
				catch(const XERCES_CPP_NAMESPACE::XMLException &toCatch)
				{
					throw toCatch.getMessage();
					printf((char*)toCatch.getMessage());
				}

				//create DOM tree
				XERCES_CPP_NAMESPACE::DOMDocument *doc = NULL ;				
				try
				{
					doc = parser->getDocument();				
				}
				catch(const XERCES_CPP_NAMESPACE::XMLException &toCatch)
				{
					throw toCatch.getMessage() ; 
				}
				catch(const XERCES_CPP_NAMESPACE::DOMException &toCatch)
				{
					throw toCatch.getMessage() ; 
				}
				catch(const char *toCatch)
				{
					throw toCatch ; 
				}
				catch(const std::exception &toCatch)
				{
					throw toCatch.what() ; 
				}
				
							
				XERCES_CPP_NAMESPACE::DOMNode *nRoot = NULL;			

				if (doc)
				{
					//start walking down the tree					
					nRoot = (XERCES_CPP_NAMESPACE::DOMNode*) doc->getDocumentElement();	
					char *rootName = XERCES_CPP_NAMESPACE::XMLString::transcode(nRoot->getNodeName()); // svm_params
					XERCES_CPP_NAMESPACE::DOMNode *nCurrent = NULL;						
					XERCES_CPP_NAMESPACE::DOMTreeWalker *walker = doc->createTreeWalker(nRoot, XERCES_CPP_NAMESPACE::DOMNodeFilter::SHOW_ELEMENT, NULL, false); //at elements
					
					nCurrent = walker->nextNode(); //first bias

					while(nCurrent!=0)
					{
						char *nName = XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getNodeName());
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, bias_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);													
							nCurrent = walker->nextNode();
							continue;
						}
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, b_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);													
							double b = strtod(XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getTextContent()), &pEnd);	
							mvect_b.push_back(b) ;
							nCurrent = walker->nextNode();
							continue;
						}
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, bias_support_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);													
							nCurrent = walker->nextNode();
							continue;
						}
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, nbsv_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);													
							double nbsv = strtod(XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getTextContent()), &pEnd);	
							mvect_nbsv.push_back(nbsv) ;
							nCurrent = walker->nextNode();
							continue;
						}
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, weights_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);													
							nCurrent = walker->nextNode();
							continue;
						}
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, w_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);													
							double w = strtod(XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getTextContent()), &pEnd);	
							mvect_w.push_back(w);
							nCurrent = walker->nextNode();
							weight_count++;
							continue;
						}
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, support_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);													
							nCurrent = walker->nextNode();
							continue;
						}
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, xsup_tag))
						{
							std::vector <double> vect_xsup;
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);	
							nCurrent = walker->nextNode();
							nName = XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getNodeName());
							feature_count = 0;
							while (XERCES_CPP_NAMESPACE::XMLString::equals(nName, feature_tag))
							{
								XERCES_CPP_NAMESPACE::XMLString::release(&nName);
                                double feature = strtod(XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getTextContent()), &pEnd);	
								vect_xsup.push_back(feature);
								nCurrent = walker->nextNode();
								if (nCurrent!=0)
									nName = XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getNodeName());
								else
									nName = NULL;
								feature_count++;
							}
							mobj_support_features->InitValues(vect_xsup) ;
							mvect_xsup.push_back(*mobj_support_features);
							support_count++;
							continue;
						}
					}
				}
			}
			XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();
		}

		

	}
}