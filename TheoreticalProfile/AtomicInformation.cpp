// Written by Anoop Mayampurath and Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "AtomicInformation.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMException.hpp>
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
#include<xercesc/util/XMLUni.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <string.h>
#include <iostream>
namespace Engine
{
	namespace TheoreticalProfile
	{
		const char* default_file_name = "Isotope.xml";

		ElementalIsotopes::ElementalIsotopes() 
		{
		} ;

		

		ElementalIsotopes::ElementalIsotopes(const ElementalIsotopes &a)
		{
			strcpy(marr_symbol, a.marr_symbol) ;
			strcpy(marr_name, a.marr_name);
			mint_num_isotopes = a.mint_num_isotopes ; 
			mint_atomicity = a.mint_atomicity;
			for (int i = 0 ; i < mint_num_isotopes ; i++)
			{
				marr_isotope_mass[i] = a.marr_isotope_mass[i] ; 
				marr_isotope_prob[i] = a.marr_isotope_prob[i] ; 
			}
			mdbl_average_mass = a.mdbl_average_mass ; 
			mdbl_mass_variance = a.mdbl_mass_variance ;
		}

		ElementalIsotopes& ElementalIsotopes::operator=(const ElementalIsotopes &a)
		{
			strcpy(marr_symbol, a.marr_symbol) ;
			strcpy(marr_name, a.marr_name);
			mint_atomicity = a.mint_atomicity;
			mint_num_isotopes = a.mint_num_isotopes ;
			for (int i = 0 ; i < mint_num_isotopes ; i++)
			{
				marr_isotope_mass[i] = a.marr_isotope_mass[i] ; 
				marr_isotope_prob[i] = a.marr_isotope_prob[i] ; 
			}
			mdbl_average_mass = a.mdbl_average_mass ; 
			mdbl_mass_variance = a.mdbl_mass_variance ;
			return (*this) ; 
		}


		ElementalIsotopes::~ElementalIsotopes() 
		{
		} ;

		AtomicInformation::AtomicInformation() 
		{
			SetDefaultIsotopeDistribution() ; 
		}

		AtomicInformation::AtomicInformation(AtomicInformation &a) {
			for (int i = 0; i < (int)a.mvect_elemental_isotopes.size(); i++) {
				mvect_elemental_isotopes.push_back(a.mvect_elemental_isotopes[i]);
			}
		}

		AtomicInformation::~AtomicInformation() {
		}

		AtomicInformation& AtomicInformation::operator=(const AtomicInformation &a)
		{
			int size_a = a.mvect_elemental_isotopes.size() ; 
			mvect_elemental_isotopes.clear();
			mvect_elemental_isotopes.insert(mvect_elemental_isotopes.begin(), a.mvect_elemental_isotopes.begin(), a.mvect_elemental_isotopes.end()) ; 
			int new_size = mvect_elemental_isotopes.size() ; 
			ElementalIsotopes elemIsotope = mvect_elemental_isotopes[0] ;
			return (*this) ; 
		}

		void AtomicInformation::SetDefaultIsotopeDistribution()
		{
			ElementalIsotopes isotopes ;	
			strcpy(isotopes.marr_symbol, "H") ;
			strcpy(isotopes.marr_name, "Hydrogen") ;

			isotopes.mdbl_average_mass = 1.007976;
			isotopes.mdbl_mass_variance = 0.000152 ;

			isotopes.mint_atomicity = 1;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 1.007825 ;
			isotopes.marr_isotope_prob[0] = 0.999850 ;
			isotopes.marr_isotope_mass[1] = 2.014102 ;
			isotopes.marr_isotope_prob[1] = 0.000150 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "He") ;
			strcpy(isotopes.marr_name, "Helium") ;

			isotopes.mdbl_average_mass = 4.002599;
			isotopes.mdbl_mass_variance = 0.000001 ;

			isotopes.mint_atomicity = 2;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 3.016030 ;
			isotopes.marr_isotope_prob[0] = 0.000001 ;
			isotopes.marr_isotope_mass[1] = 4.002600 ;
			isotopes.marr_isotope_prob[1] = 0.999999 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Li") ;
			strcpy(isotopes.marr_name, "Lithium") ;

			isotopes.mdbl_average_mass = 6.940937;
			isotopes.mdbl_mass_variance = 0.069497 ;

			isotopes.mint_atomicity = 3;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 6.015121 ;
			isotopes.marr_isotope_prob[0] = 0.075000 ;
			isotopes.marr_isotope_mass[1] = 7.016003 ;
			isotopes.marr_isotope_prob[1] = 0.925000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Be") ;
			strcpy(isotopes.marr_name, "Berellium") ;

			isotopes.mdbl_average_mass = 9.012182;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 4;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 9.012182 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "B") ;
			strcpy(isotopes.marr_name, "Boron") ;

			isotopes.mdbl_average_mass = 10.811028;
			isotopes.mdbl_mass_variance = 0.158243 ;

			isotopes.mint_atomicity = 5;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 10.012937 ;
			isotopes.marr_isotope_prob[0] = 0.199000 ;
			isotopes.marr_isotope_mass[1] = 11.009305 ;
			isotopes.marr_isotope_prob[1] = 0.801000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "C") ;
			strcpy(isotopes.marr_name, "Carbon") ;

			isotopes.mdbl_average_mass = 12.011107;
			isotopes.mdbl_mass_variance = 0.011021 ;

			isotopes.mint_atomicity = 6;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 12.000000 ;
			isotopes.marr_isotope_prob[0] = 0.988930 ;
			isotopes.marr_isotope_mass[1] = 13.003355 ;
			isotopes.marr_isotope_prob[1] = 0.011070 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "N") ;
			strcpy(isotopes.marr_name, "Nitrogen") ;

			isotopes.mdbl_average_mass = 14.006724;
			isotopes.mdbl_mass_variance = 0.003628 ;

			isotopes.mint_atomicity = 7;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 14.003072 ;
			isotopes.marr_isotope_prob[0] = 0.996337 ;
			isotopes.marr_isotope_mass[1] = 15.000109 ;
			isotopes.marr_isotope_prob[1] = 0.003663 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "O") ;
			strcpy(isotopes.marr_name, "Oxygen") ;

			isotopes.mdbl_average_mass = 15.999370;
			isotopes.mdbl_mass_variance = 0.008536 ;

			isotopes.mint_atomicity = 8;
			isotopes.mint_num_isotopes = 3;

			isotopes.marr_isotope_mass[0] = 15.994914 ;
			isotopes.marr_isotope_prob[0] = 0.997590 ;
			isotopes.marr_isotope_mass[1] = 16.999132 ;
			isotopes.marr_isotope_prob[1] = 0.000374 ;
			isotopes.marr_isotope_mass[2] = 17.999162 ;
			isotopes.marr_isotope_prob[2] = 0.002036 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "F") ;
			strcpy(isotopes.marr_name, "Fluorine") ;

			isotopes.mdbl_average_mass = 18.998403;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 9;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 18.998403 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ne") ;
			strcpy(isotopes.marr_name, "Neon") ;

			isotopes.mdbl_average_mass = 20.180041;
			isotopes.mdbl_mass_variance = 0.337122 ;

			isotopes.mint_atomicity = 10;
			isotopes.mint_num_isotopes = 3;

			isotopes.marr_isotope_mass[0] = 19.992435 ;
			isotopes.marr_isotope_prob[0] = 0.904800 ;
			isotopes.marr_isotope_mass[1] = 20.993843 ;
			isotopes.marr_isotope_prob[1] = 0.002700 ;
			isotopes.marr_isotope_mass[2] = 21.991383 ;
			isotopes.marr_isotope_prob[2] = 0.092500 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Na") ;
			strcpy(isotopes.marr_name, "Sodium") ;

			isotopes.mdbl_average_mass = 22.989767;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 11;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 22.989767 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Mg") ;
			strcpy(isotopes.marr_name, "Magnesium") ;

			isotopes.mdbl_average_mass = 24.305052;
			isotopes.mdbl_mass_variance = 0.437075 ;

			isotopes.mint_atomicity = 12;
			isotopes.mint_num_isotopes = 3;

			isotopes.marr_isotope_mass[0] = 23.985042 ;
			isotopes.marr_isotope_prob[0] = 0.789900 ;
			isotopes.marr_isotope_mass[1] = 24.985837 ;
			isotopes.marr_isotope_prob[1] = 0.100000 ;
			isotopes.marr_isotope_mass[2] = 25.982593 ;
			isotopes.marr_isotope_prob[2] = 0.110100 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Al") ;
			strcpy(isotopes.marr_name, "Almunium") ;

			isotopes.mdbl_average_mass = 26.981539;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 13;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 26.981539 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Si") ;
			strcpy(isotopes.marr_name, "Silicon") ;

			isotopes.mdbl_average_mass = 28.085509;
			isotopes.mdbl_mass_variance = 0.158478 ;

			isotopes.mint_atomicity = 14;
			isotopes.mint_num_isotopes = 3;

			isotopes.marr_isotope_mass[0] = 27.976927 ;
			isotopes.marr_isotope_prob[0] = 0.922300 ;
			isotopes.marr_isotope_mass[1] = 28.976495 ;
			isotopes.marr_isotope_prob[1] = 0.046700 ;
			isotopes.marr_isotope_mass[2] = 29.973770 ;
			isotopes.marr_isotope_prob[2] = 0.031000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "P") ;
			strcpy(isotopes.marr_name, "Phosphrous") ;

			isotopes.mdbl_average_mass = 30.973762;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 15;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 30.973762 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "S") ;
			strcpy(isotopes.marr_name, "Sulphur") ;

			isotopes.mdbl_average_mass = 32.064387;
			isotopes.mdbl_mass_variance = 0.169853 ;

			isotopes.mint_atomicity = 16;
			isotopes.mint_num_isotopes = 4;

			isotopes.marr_isotope_mass[0] = 31.972070 ;
			isotopes.marr_isotope_prob[0] = 0.950200 ;
			isotopes.marr_isotope_mass[1] = 32.971456 ;
			isotopes.marr_isotope_prob[1] = 0.007500 ;
			isotopes.marr_isotope_mass[2] = 33.967866 ;
			isotopes.marr_isotope_prob[2] = 0.042100 ;
			isotopes.marr_isotope_mass[3] = 35.967080 ;
			isotopes.marr_isotope_prob[3] = 0.000200 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Cl") ;
			strcpy(isotopes.marr_name, "Chlorine") ;

			isotopes.mdbl_average_mass = 35.457551;
			isotopes.mdbl_mass_variance = 0.737129 ;

			isotopes.mint_atomicity = 17;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 34.968853 ;
			isotopes.marr_isotope_prob[0] = 0.755290 ;
			isotopes.marr_isotope_mass[1] = 36.965903 ;
			isotopes.marr_isotope_prob[1] = 0.244710 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ar") ;
			strcpy(isotopes.marr_name, "Argon") ;

			isotopes.mdbl_average_mass = 39.947662;
			isotopes.mdbl_mass_variance = 0.056083 ;

			isotopes.mint_atomicity = 18;
			isotopes.mint_num_isotopes = 3;

			isotopes.marr_isotope_mass[0] = 35.967545 ;
			isotopes.marr_isotope_prob[0] = 0.003370 ;
			isotopes.marr_isotope_mass[1] = 37.962732 ;
			isotopes.marr_isotope_prob[1] = 0.000630 ;
			isotopes.marr_isotope_mass[2] = 39.962384 ;
			isotopes.marr_isotope_prob[2] = 0.996000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "K") ;
			strcpy(isotopes.marr_name, "Potassium") ;

			isotopes.mdbl_average_mass = 39.098301;
			isotopes.mdbl_mass_variance = 0.250703 ;

			isotopes.mint_atomicity = 19;
			isotopes.mint_num_isotopes = 3;

			isotopes.marr_isotope_mass[0] = 38.963707 ;
			isotopes.marr_isotope_prob[0] = 0.932581 ;
			isotopes.marr_isotope_mass[1] = 39.963999 ;
			isotopes.marr_isotope_prob[1] = 0.000117 ;
			isotopes.marr_isotope_mass[2] = 40.961825 ;
			isotopes.marr_isotope_prob[2] = 0.067302 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ca") ;
			strcpy(isotopes.marr_name, "Calcium") ;

			isotopes.mdbl_average_mass = 40.078023;
			isotopes.mdbl_mass_variance = 0.477961 ;

			isotopes.mint_atomicity = 20;
			isotopes.mint_num_isotopes = 6;

			isotopes.marr_isotope_mass[0] = 39.962591 ;
			isotopes.marr_isotope_prob[0] = 0.969410 ;
			isotopes.marr_isotope_mass[1] = 41.958618 ;
			isotopes.marr_isotope_prob[1] = 0.006470 ;
			isotopes.marr_isotope_mass[2] = 42.958766 ;
			isotopes.marr_isotope_prob[2] = 0.001350 ;
			isotopes.marr_isotope_mass[3] = 43.955480 ;
			isotopes.marr_isotope_prob[3] = 0.020860 ;
			isotopes.marr_isotope_mass[4] = 45.953689 ;
			isotopes.marr_isotope_prob[4] = 0.000040 ;
			isotopes.marr_isotope_mass[5] = 47.952533 ;
			isotopes.marr_isotope_prob[5] = 0.001870 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Sc") ;
			strcpy(isotopes.marr_name, "Scandium") ;

			isotopes.mdbl_average_mass = 44.955910;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 21;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 44.955910 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ti") ;
			strcpy(isotopes.marr_name, "Titanium") ;

			isotopes.mdbl_average_mass = 47.878426;
			isotopes.mdbl_mass_variance = 0.656425 ;

			isotopes.mint_atomicity = 22;
			isotopes.mint_num_isotopes = 5;

			isotopes.marr_isotope_mass[0] = 45.952629 ;
			isotopes.marr_isotope_prob[0] = 0.080000 ;
			isotopes.marr_isotope_mass[1] = 46.951764 ;
			isotopes.marr_isotope_prob[1] = 0.073000 ;
			isotopes.marr_isotope_mass[2] = 47.947947 ;
			isotopes.marr_isotope_prob[2] = 0.738000 ;
			isotopes.marr_isotope_mass[3] = 48.947871 ;
			isotopes.marr_isotope_prob[3] = 0.055000 ;
			isotopes.marr_isotope_mass[4] = 49.944792 ;
			isotopes.marr_isotope_prob[4] = 0.054000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "V") ;
			strcpy(isotopes.marr_name, "Vanadium") ;

			isotopes.mdbl_average_mass = 50.941470;
			isotopes.mdbl_mass_variance = 0.002478 ;

			isotopes.mint_atomicity = 23;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 49.947161 ;
			isotopes.marr_isotope_prob[0] = 0.002500 ;
			isotopes.marr_isotope_mass[1] = 50.943962 ;
			isotopes.marr_isotope_prob[1] = 0.997500 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Cr") ;
			strcpy(isotopes.marr_name, "Chromium") ;

			isotopes.mdbl_average_mass = 51.996125;
			isotopes.mdbl_mass_variance = 0.359219 ;

			isotopes.mint_atomicity = 24;
			isotopes.mint_num_isotopes = 4;

			isotopes.marr_isotope_mass[0] = 49.946046 ;
			isotopes.marr_isotope_prob[0] = 0.043450 ;
			isotopes.marr_isotope_mass[1] = 51.940509 ;
			isotopes.marr_isotope_prob[1] = 0.837900 ;
			isotopes.marr_isotope_mass[2] = 52.940651 ;
			isotopes.marr_isotope_prob[2] = 0.095000 ;
			isotopes.marr_isotope_mass[3] = 53.938882 ;
			isotopes.marr_isotope_prob[3] = 0.023650 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Mn") ;
			strcpy(isotopes.marr_name, "Manganese") ;

			isotopes.mdbl_average_mass = 54.938047;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 25;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 54.938047 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Fe") ;
			strcpy(isotopes.marr_name, "Iron") ;

			isotopes.mdbl_average_mass = 55.843820;
			isotopes.mdbl_mass_variance = 0.258796 ;

			isotopes.mint_atomicity = 26;
			isotopes.mint_num_isotopes = 4;

			isotopes.marr_isotope_mass[0] = 53.939612 ;
			isotopes.marr_isotope_prob[0] = 0.059000 ;
			isotopes.marr_isotope_mass[1] = 55.934939 ;
			isotopes.marr_isotope_prob[1] = 0.917200 ;
			isotopes.marr_isotope_mass[2] = 56.935396 ;
			isotopes.marr_isotope_prob[2] = 0.021000 ;
			isotopes.marr_isotope_mass[3] = 57.933277 ;
			isotopes.marr_isotope_prob[3] = 0.002800 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Co") ;
			strcpy(isotopes.marr_name, "Cobalt") ;

			isotopes.mdbl_average_mass = 58.933198;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 27;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 58.933198 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ni") ;
			strcpy(isotopes.marr_name, "Nickle") ;

			isotopes.mdbl_average_mass = 58.687889;
			isotopes.mdbl_mass_variance = 1.473521 ;

			isotopes.mint_atomicity = 28;
			isotopes.mint_num_isotopes = 5;

			isotopes.marr_isotope_mass[0] = 57.935346 ;
			isotopes.marr_isotope_prob[0] = 0.682700 ;
			isotopes.marr_isotope_mass[1] = 59.930788 ;
			isotopes.marr_isotope_prob[1] = 0.261000 ;
			isotopes.marr_isotope_mass[2] = 60.931058 ;
			isotopes.marr_isotope_prob[2] = 0.011300 ;
			isotopes.marr_isotope_mass[3] = 61.928346 ;
			isotopes.marr_isotope_prob[3] = 0.035900 ;
			isotopes.marr_isotope_mass[4] = 63.927968 ;
			isotopes.marr_isotope_prob[4] = 0.009100 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Cu") ;
			strcpy(isotopes.marr_name, "Copper") ;

			isotopes.mdbl_average_mass = 63.552559;
			isotopes.mdbl_mass_variance = 0.842964 ;

			isotopes.mint_atomicity = 29;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 62.939598 ;
			isotopes.marr_isotope_prob[0] = 0.691700 ;
			isotopes.marr_isotope_mass[1] = 64.927793 ;
			isotopes.marr_isotope_prob[1] = 0.308300 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Zn") ;
			strcpy(isotopes.marr_name, "Zinc") ;

			isotopes.mdbl_average_mass = 65.396363;
			isotopes.mdbl_mass_variance = 2.545569 ;

			isotopes.mint_atomicity = 30;
			isotopes.mint_num_isotopes = 5;

			isotopes.marr_isotope_mass[0] = 63.929145 ;
			isotopes.marr_isotope_prob[0] = 0.486000 ;
			isotopes.marr_isotope_mass[1] = 65.926034 ;
			isotopes.marr_isotope_prob[1] = 0.279000 ;
			isotopes.marr_isotope_mass[2] = 66.927129 ;
			isotopes.marr_isotope_prob[2] = 0.041000 ;
			isotopes.marr_isotope_mass[3] = 67.924846 ;
			isotopes.marr_isotope_prob[3] = 0.188000 ;
			isotopes.marr_isotope_mass[4] = 69.925325 ;
			isotopes.marr_isotope_prob[4] = 0.006000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ga") ;
			strcpy(isotopes.marr_name, "Galium") ;

			isotopes.mdbl_average_mass = 69.723069;
			isotopes.mdbl_mass_variance = 0.958287 ;

			isotopes.mint_atomicity = 31;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 68.925580 ;
			isotopes.marr_isotope_prob[0] = 0.601080 ;
			isotopes.marr_isotope_mass[1] = 70.924700 ;
			isotopes.marr_isotope_prob[1] = 0.398920 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ge") ;
			strcpy(isotopes.marr_name, "Germanium") ;

			isotopes.mdbl_average_mass = 72.632250;
			isotopes.mdbl_mass_variance = 3.098354 ;

			isotopes.mint_atomicity = 32;
			isotopes.mint_num_isotopes = 5;

			isotopes.marr_isotope_mass[0] = 69.924250 ;
			isotopes.marr_isotope_prob[0] = 0.205000 ;
			isotopes.marr_isotope_mass[1] = 71.922079 ;
			isotopes.marr_isotope_prob[1] = 0.274000 ;
			isotopes.marr_isotope_mass[2] = 72.923463 ;
			isotopes.marr_isotope_prob[2] = 0.078000 ;
			isotopes.marr_isotope_mass[3] = 73.921177 ;
			isotopes.marr_isotope_prob[3] = 0.365000 ;
			isotopes.marr_isotope_mass[4] = 75.921401 ;
			isotopes.marr_isotope_prob[4] = 0.078000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "As") ;
			strcpy(isotopes.marr_name, "Arsenic") ;

			isotopes.mdbl_average_mass = 74.921594;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 33;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 74.921594 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Se") ;
			strcpy(isotopes.marr_name, "Selenium") ;

			isotopes.mdbl_average_mass = 78.977677;
			isotopes.mdbl_mass_variance = 2.876151 ;

			isotopes.mint_atomicity = 34;
			isotopes.mint_num_isotopes = 6;

			isotopes.marr_isotope_mass[0] = 73.922475 ;
			isotopes.marr_isotope_prob[0] = 0.009000 ;
			isotopes.marr_isotope_mass[1] = 75.919212 ;
			isotopes.marr_isotope_prob[1] = 0.091000 ;
			isotopes.marr_isotope_mass[2] = 76.919912 ;
			isotopes.marr_isotope_prob[2] = 0.076000 ;
			isotopes.marr_isotope_mass[3] = 77.919000 ;
			isotopes.marr_isotope_prob[3] = 0.236000 ;
			isotopes.marr_isotope_mass[4] = 79.916520 ;
			isotopes.marr_isotope_prob[4] = 0.499000 ;
			isotopes.marr_isotope_mass[5] = 81.916698 ;
			isotopes.marr_isotope_prob[5] = 0.089000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Br") ;
			strcpy(isotopes.marr_name, "Bromine") ;

			isotopes.mdbl_average_mass = 79.903527;
			isotopes.mdbl_mass_variance = 0.997764 ;

			isotopes.mint_atomicity = 35;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 78.918336 ;
			isotopes.marr_isotope_prob[0] = 0.506900 ;
			isotopes.marr_isotope_mass[1] = 80.916289 ;
			isotopes.marr_isotope_prob[1] = 0.493100 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Kr") ;
			strcpy(isotopes.marr_name, "Krypton") ;

			isotopes.mdbl_average_mass = 83.800003;

			isotopes.mdbl_mass_variance = 1.741449 ;

			isotopes.mint_atomicity = 36;
			isotopes.mint_num_isotopes = 6;

			isotopes.marr_isotope_mass[0] = 77.914000 ;
			isotopes.marr_isotope_prob[0] = 0.003500 ;
			isotopes.marr_isotope_mass[1] = 79.916380 ;
			isotopes.marr_isotope_prob[1] = 0.022500 ;
			isotopes.marr_isotope_mass[2] = 81.913482 ;
			isotopes.marr_isotope_prob[2] = 0.116000 ;
			isotopes.marr_isotope_mass[3] = 82.914135 ;
			isotopes.marr_isotope_prob[3] = 0.115000 ;
			isotopes.marr_isotope_mass[4] = 83.911507 ;
			isotopes.marr_isotope_prob[4] = 0.570000 ;
			isotopes.marr_isotope_mass[5] = 85.910616 ;
			isotopes.marr_isotope_prob[5] = 0.173000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Rb") ;
			strcpy(isotopes.marr_name, "Rubidium") ;

			isotopes.mdbl_average_mass = 85.467668;
			isotopes.mdbl_mass_variance = 0.801303 ;

			isotopes.mint_atomicity = 37;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 84.911794 ;
			isotopes.marr_isotope_prob[0] = 0.721700 ;
			isotopes.marr_isotope_mass[1] = 86.909187 ;
			isotopes.marr_isotope_prob[1] = 0.278300 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Sr") ;
			strcpy(isotopes.marr_name, "Strontium") ;

			isotopes.mdbl_average_mass = 87.616651;
			isotopes.mdbl_mass_variance = 0.468254 ;

			isotopes.mint_atomicity = 38;
			isotopes.mint_num_isotopes = 4;

			isotopes.marr_isotope_mass[0] = 83.913430 ;
			isotopes.marr_isotope_prob[0] = 0.005600 ;
			isotopes.marr_isotope_mass[1] = 85.909267 ;
			isotopes.marr_isotope_prob[1] = 0.098600 ;
			isotopes.marr_isotope_mass[2] = 86.908884 ;
			isotopes.marr_isotope_prob[2] = 0.070000 ;
			isotopes.marr_isotope_mass[3] = 87.905619 ;
			isotopes.marr_isotope_prob[3] = 0.825800 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Y") ;
			strcpy(isotopes.marr_name, "Yttrium") ;

			isotopes.mdbl_average_mass = 88.905849;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 39;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 88.905849 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Zr") ;
			strcpy(isotopes.marr_name, "Zirconium") ;

			isotopes.mdbl_average_mass = 91.223646;
			isotopes.mdbl_mass_variance = 2.851272 ;

			isotopes.mint_atomicity = 40;
			isotopes.mint_num_isotopes = 5;

			isotopes.marr_isotope_mass[0] = 89.904703 ;
			isotopes.marr_isotope_prob[0] = 0.514500 ;
			isotopes.marr_isotope_mass[1] = 90.905644 ;
			isotopes.marr_isotope_prob[1] = 0.112200 ;
			isotopes.marr_isotope_mass[2] = 91.905039 ;
			isotopes.marr_isotope_prob[2] = 0.171500 ;
			isotopes.marr_isotope_mass[3] = 93.906314 ;
			isotopes.marr_isotope_prob[3] = 0.173800 ;
			isotopes.marr_isotope_mass[4] = 95.908275 ;
			isotopes.marr_isotope_prob[4] = 0.028000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Nb") ;
			strcpy(isotopes.marr_name, "Niobium") ;

			isotopes.mdbl_average_mass = 92.906377;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 41;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 92.906377 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Mo") ;
			strcpy(isotopes.marr_name, "Molybdenum") ;

			isotopes.mdbl_average_mass = 95.931290;
			isotopes.mdbl_mass_variance = 5.504460 ;

			isotopes.mint_atomicity = 42;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 91.906808 ;
			isotopes.marr_isotope_prob[0] = 0.148400 ;
			isotopes.marr_isotope_mass[1] = 93.905085 ;
			isotopes.marr_isotope_prob[1] = 0.092500 ;
			isotopes.marr_isotope_mass[2] = 94.905840 ;
			isotopes.marr_isotope_prob[2] = 0.159200 ;
			isotopes.marr_isotope_mass[3] = 95.904678 ;
			isotopes.marr_isotope_prob[3] = 0.166800 ;
			isotopes.marr_isotope_mass[4] = 96.906020 ;
			isotopes.marr_isotope_prob[4] = 0.095500 ;
			isotopes.marr_isotope_mass[5] = 97.905406 ;
			isotopes.marr_isotope_prob[5] = 0.241300 ;
			isotopes.marr_isotope_mass[6] = 99.907477 ;
			isotopes.marr_isotope_prob[6] = 0.096300 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Tc") ;
			strcpy(isotopes.marr_name, "Technetium") ;

			isotopes.mdbl_average_mass = 98.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 43;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 98.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ru") ;
			strcpy(isotopes.marr_name, "Ruthenium") ;

			isotopes.mdbl_average_mass = 101.066343;
			isotopes.mdbl_mass_variance = 4.148678 ;

			isotopes.mint_atomicity = 44;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 95.907599 ;
			isotopes.marr_isotope_prob[0] = 0.055400 ;
			isotopes.marr_isotope_mass[1] = 97.905287 ;
			isotopes.marr_isotope_prob[1] = 0.018600 ;
			isotopes.marr_isotope_mass[2] = 98.905939 ;
			isotopes.marr_isotope_prob[2] = 0.127000 ;
			isotopes.marr_isotope_mass[3] = 99.904219 ;
			isotopes.marr_isotope_prob[3] = 0.126000 ;
			isotopes.marr_isotope_mass[4] = 100.905582 ;
			isotopes.marr_isotope_prob[4] = 0.171000 ;
			isotopes.marr_isotope_mass[5] = 101.904348 ;
			isotopes.marr_isotope_prob[5] = 0.316000 ;
			isotopes.marr_isotope_mass[6] = 103.905424 ;
			isotopes.marr_isotope_prob[6] = 0.186000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Rh") ;
			strcpy(isotopes.marr_name, "Rhodium") ;

			isotopes.mdbl_average_mass = 102.905500;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 45;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 102.905500 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Pd") ;
			strcpy(isotopes.marr_name, " Palladium") ;

			isotopes.mdbl_average_mass = 106.415327;
			isotopes.mdbl_mass_variance = 3.504600 ;

			isotopes.mint_atomicity = 46;
			isotopes.mint_num_isotopes = 6;

			isotopes.marr_isotope_mass[0] = 101.905634 ;
			isotopes.marr_isotope_prob[0] = 0.010200 ;
			isotopes.marr_isotope_mass[1] = 103.904029 ;
			isotopes.marr_isotope_prob[1] = 0.111400 ;
			isotopes.marr_isotope_mass[2] = 104.905079 ;
			isotopes.marr_isotope_prob[2] = 0.223300 ;
			isotopes.marr_isotope_mass[3] = 105.903478 ;
			isotopes.marr_isotope_prob[3] = 0.273300 ;
			isotopes.marr_isotope_mass[4] = 107.903895 ;
			isotopes.marr_isotope_prob[4] = 0.264600 ;
			isotopes.marr_isotope_mass[5] = 109.905167 ;
			isotopes.marr_isotope_prob[5] = 0.117200 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ag") ;
			strcpy(isotopes.marr_name, "Silver") ;

			isotopes.mdbl_average_mass = 107.868151;
			isotopes.mdbl_mass_variance = 0.998313 ;

			isotopes.mint_atomicity = 47;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 106.905092 ;
			isotopes.marr_isotope_prob[0] = 0.518390 ;
			isotopes.marr_isotope_mass[1] = 108.904757 ;
			isotopes.marr_isotope_prob[1] = 0.481610 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Cd") ;
			strcpy(isotopes.marr_name, "Cadmium") ;

			isotopes.mdbl_average_mass = 112.411552;
			isotopes.mdbl_mass_variance = 3.432071 ;

			isotopes.mint_atomicity = 48;
			isotopes.mint_num_isotopes = 8;

			isotopes.marr_isotope_mass[0] = 105.906461 ;
			isotopes.marr_isotope_prob[0] = 0.012500 ;
			isotopes.marr_isotope_mass[1] = 107.904176 ;
			isotopes.marr_isotope_prob[1] = 0.008900 ;
			isotopes.marr_isotope_mass[2] = 109.903005 ;
			isotopes.marr_isotope_prob[2] = 0.124900 ;
			isotopes.marr_isotope_mass[3] = 110.904182 ;
			isotopes.marr_isotope_prob[3] = 0.128000 ;
			isotopes.marr_isotope_mass[4] = 111.902758 ;
			isotopes.marr_isotope_prob[4] = 0.241300 ;
			isotopes.marr_isotope_mass[5] = 112.904400 ;
			isotopes.marr_isotope_prob[5] = 0.122200 ;
			isotopes.marr_isotope_mass[6] = 113.903357 ;
			isotopes.marr_isotope_prob[6] = 0.287300 ;
			isotopes.marr_isotope_mass[7] = 115.904754 ;
			isotopes.marr_isotope_prob[7] = 0.074900 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "In") ;
			strcpy(isotopes.marr_name, "Indium") ;

			isotopes.mdbl_average_mass = 114.817888;
			isotopes.mdbl_mass_variance = 0.164574 ;

			isotopes.mint_atomicity = 49;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 112.904061 ;
			isotopes.marr_isotope_prob[0] = 0.043000 ;
			isotopes.marr_isotope_mass[1] = 114.903880 ;
			isotopes.marr_isotope_prob[1] = 0.957000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Sn") ;
			strcpy(isotopes.marr_name, "Tin") ;

			isotopes.mdbl_average_mass = 118.710213;
			isotopes.mdbl_mass_variance = 4.707888 ;

			isotopes.mint_atomicity = 50;
			isotopes.mint_num_isotopes = 10;

			isotopes.marr_isotope_mass[0] = 111.904826 ;
			isotopes.marr_isotope_prob[0] = 0.009700 ;
			isotopes.marr_isotope_mass[1] = 113.902784 ;
			isotopes.marr_isotope_prob[1] = 0.006500 ;
			isotopes.marr_isotope_mass[2] = 114.903348 ;
			isotopes.marr_isotope_prob[2] = 0.003600 ;
			isotopes.marr_isotope_mass[3] = 115.901747 ;
			isotopes.marr_isotope_prob[3] = 0.145300 ;
			isotopes.marr_isotope_mass[4] = 116.902956 ;
			isotopes.marr_isotope_prob[4] = 0.076800 ;
			isotopes.marr_isotope_mass[5] = 117.901609 ;
			isotopes.marr_isotope_prob[5] = 0.242200 ;
			isotopes.marr_isotope_mass[6] = 118.903310 ;
			isotopes.marr_isotope_prob[6] = 0.085800 ;
			isotopes.marr_isotope_mass[7] = 119.902200 ;
			isotopes.marr_isotope_prob[7] = 0.325900 ;
			isotopes.marr_isotope_mass[8] = 121.903440 ;
			isotopes.marr_isotope_prob[8] = 0.046300 ;
			isotopes.marr_isotope_mass[9] = 123.905274 ;
			isotopes.marr_isotope_prob[9] = 0.057900 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Sb") ;
			strcpy(isotopes.marr_name, "Antimony") ;

			isotopes.mdbl_average_mass = 121.755989;
			isotopes.mdbl_mass_variance = 0.978482 ;

			isotopes.mint_atomicity = 51;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 120.903821 ;
			isotopes.marr_isotope_prob[0] = 0.574000 ;
			isotopes.marr_isotope_mass[1] = 122.904216 ;
			isotopes.marr_isotope_prob[1] = 0.426000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Te") ;
			strcpy(isotopes.marr_name, "Tellerium") ;

			isotopes.mdbl_average_mass = 127.590074;
			isotopes.mdbl_mass_variance = 4.644177 ;

			isotopes.mint_atomicity = 52;
			isotopes.mint_num_isotopes = 8;

			isotopes.marr_isotope_mass[0] = 119.904048 ;
			isotopes.marr_isotope_prob[0] = 0.000950 ;
			isotopes.marr_isotope_mass[1] = 121.903054 ;
			isotopes.marr_isotope_prob[1] = 0.025900 ;
			isotopes.marr_isotope_mass[2] = 122.904271 ;
			isotopes.marr_isotope_prob[2] = 0.009050 ;
			isotopes.marr_isotope_mass[3] = 123.902823 ;
			isotopes.marr_isotope_prob[3] = 0.047900 ;
			isotopes.marr_isotope_mass[4] = 124.904433 ;
			isotopes.marr_isotope_prob[4] = 0.071200 ;
			isotopes.marr_isotope_mass[5] = 125.903314 ;
			isotopes.marr_isotope_prob[5] = 0.189300 ;
			isotopes.marr_isotope_mass[6] = 127.904463 ;
			isotopes.marr_isotope_prob[6] = 0.317000 ;
			isotopes.marr_isotope_mass[7] = 129.906229 ;
			isotopes.marr_isotope_prob[7] = 0.338700 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "I") ;
			strcpy(isotopes.marr_name, "Iodine") ;

			isotopes.mdbl_average_mass = 126.904473;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 53;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 126.904473 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Xe") ;
			strcpy(isotopes.marr_name, "Xenon") ;

			isotopes.mdbl_average_mass = 131.293076;
			isotopes.mdbl_mass_variance = 4.622071 ;

			isotopes.mint_atomicity = 54;
			isotopes.mint_num_isotopes = 9;

			isotopes.marr_isotope_mass[0] = 123.905894 ;
			isotopes.marr_isotope_prob[0] = 0.001000 ;
			isotopes.marr_isotope_mass[1] = 125.904281 ;
			isotopes.marr_isotope_prob[1] = 0.000900 ;
			isotopes.marr_isotope_mass[2] = 127.903531 ;
			isotopes.marr_isotope_prob[2] = 0.019100 ;
			isotopes.marr_isotope_mass[3] = 128.904780 ;
			isotopes.marr_isotope_prob[3] = 0.264000 ;
			isotopes.marr_isotope_mass[4] = 129.903509 ;
			isotopes.marr_isotope_prob[4] = 0.041000 ;
			isotopes.marr_isotope_mass[5] = 130.905072 ;
			isotopes.marr_isotope_prob[5] = 0.212000 ;
			isotopes.marr_isotope_mass[6] = 131.904144 ;
			isotopes.marr_isotope_prob[6] = 0.269000 ;
			isotopes.marr_isotope_mass[7] = 133.905395 ;
			isotopes.marr_isotope_prob[7] = 0.104000 ;
			isotopes.marr_isotope_mass[8] = 135.907214 ;
			isotopes.marr_isotope_prob[8] = 0.089000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Cs") ;
			strcpy(isotopes.marr_name, "Casium") ;

			isotopes.mdbl_average_mass = 132.905429;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 55;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 132.905429 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ba") ;
			strcpy(isotopes.marr_name, "Barium") ;

			isotopes.mdbl_average_mass = 137.326825;
			isotopes.mdbl_mass_variance = 1.176556 ;

			isotopes.mint_atomicity = 56;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 129.906282 ;
			isotopes.marr_isotope_prob[0] = 0.001060 ;
			isotopes.marr_isotope_mass[1] = 131.905042 ;
			isotopes.marr_isotope_prob[1] = 0.001010 ;
			isotopes.marr_isotope_mass[2] = 133.904486 ;
			isotopes.marr_isotope_prob[2] = 0.024200 ;
			isotopes.marr_isotope_mass[3] = 134.905665 ;
			isotopes.marr_isotope_prob[3] = 0.065930 ;
			isotopes.marr_isotope_mass[4] = 135.904553 ;
			isotopes.marr_isotope_prob[4] = 0.078500 ;
			isotopes.marr_isotope_mass[5] = 136.905812 ;
			isotopes.marr_isotope_prob[5] = 0.112300 ;
			isotopes.marr_isotope_mass[6] = 137.905232 ;
			isotopes.marr_isotope_prob[6] = 0.717000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "La") ;
			strcpy(isotopes.marr_name, "Lanthanum") ;

			isotopes.mdbl_average_mass = 138.905448;
			isotopes.mdbl_mass_variance = 0.000898 ;

			isotopes.mint_atomicity = 57;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 137.907110 ;
			isotopes.marr_isotope_prob[0] = 0.000900 ;
			isotopes.marr_isotope_mass[1] = 138.906347 ;
			isotopes.marr_isotope_prob[1] = 0.999100 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ce") ;
			strcpy(isotopes.marr_name, "Cerium") ;

			isotopes.mdbl_average_mass = 140.115861;
			isotopes.mdbl_mass_variance = 0.442985 ;

			isotopes.mint_atomicity = 58;
			isotopes.mint_num_isotopes = 4;

			isotopes.marr_isotope_mass[0] = 135.907140 ;
			isotopes.marr_isotope_prob[0] = 0.001900 ;
			isotopes.marr_isotope_mass[1] = 137.905985 ;
			isotopes.marr_isotope_prob[1] = 0.002500 ;
			isotopes.marr_isotope_mass[2] = 139.905433 ;
			isotopes.marr_isotope_prob[2] = 0.884300 ;
			isotopes.marr_isotope_mass[3] = 141.909241 ;
			isotopes.marr_isotope_prob[3] = 0.111300 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Pr") ;
			strcpy(isotopes.marr_name, "Praseodymium") ;

			isotopes.mdbl_average_mass = 140.907647;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 59;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 140.907647 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Nd") ;
			strcpy(isotopes.marr_name, "Neodynium") ;

			isotopes.mdbl_average_mass = 144.242337;
			isotopes.mdbl_mass_variance = 4.834797 ;

			isotopes.mint_atomicity = 60;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 141.907719 ;
			isotopes.marr_isotope_prob[0] = 0.271300 ;
			isotopes.marr_isotope_mass[1] = 142.909810 ;
			isotopes.marr_isotope_prob[1] = 0.121800 ;
			isotopes.marr_isotope_mass[2] = 143.910083 ;
			isotopes.marr_isotope_prob[2] = 0.238000 ;
			isotopes.marr_isotope_mass[3] = 144.912570 ;
			isotopes.marr_isotope_prob[3] = 0.083000 ;
			isotopes.marr_isotope_mass[4] = 145.913113 ;
			isotopes.marr_isotope_prob[4] = 0.171900 ;
			isotopes.marr_isotope_mass[5] = 147.916889 ;
			isotopes.marr_isotope_prob[5] = 0.057600 ;
			isotopes.marr_isotope_mass[6] = 149.920887 ;
			isotopes.marr_isotope_prob[6] = 0.056400 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Pm") ;
			strcpy(isotopes.marr_name, "Promethium") ;

			isotopes.mdbl_average_mass = 145.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 61;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 145.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Sm") ;
			strcpy(isotopes.marr_name, "Samarium") ;

			isotopes.mdbl_average_mass = 150.360238;
			isotopes.mdbl_mass_variance = 7.576609 ;

			isotopes.mint_atomicity = 62;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 143.911998 ;
			isotopes.marr_isotope_prob[0] = 0.031000 ;
			isotopes.marr_isotope_mass[1] = 146.914895 ;
			isotopes.marr_isotope_prob[1] = 0.150000 ;
			isotopes.marr_isotope_mass[2] = 147.914820 ;
			isotopes.marr_isotope_prob[2] = 0.113000 ;
			isotopes.marr_isotope_mass[3] = 148.917181 ;
			isotopes.marr_isotope_prob[3] = 0.138000 ;
			isotopes.marr_isotope_mass[4] = 149.917273 ;
			isotopes.marr_isotope_prob[4] = 0.074000 ;
			isotopes.marr_isotope_mass[5] = 151.919729 ;
			isotopes.marr_isotope_prob[5] = 0.267000 ;
			isotopes.marr_isotope_mass[6] = 153.922206 ;
			isotopes.marr_isotope_prob[6] = 0.227000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Eu") ;
			strcpy(isotopes.marr_name, "Europium") ;

			isotopes.mdbl_average_mass = 151.964566;
			isotopes.mdbl_mass_variance = 0.999440 ;

			isotopes.mint_atomicity = 63;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 150.919847 ;
			isotopes.marr_isotope_prob[0] = 0.478000 ;
			isotopes.marr_isotope_mass[1] = 152.921225 ;
			isotopes.marr_isotope_prob[1] = 0.522000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Gd") ;
			strcpy(isotopes.marr_name, "Gadolinium") ;

			isotopes.mdbl_average_mass = 157.252118;
			isotopes.mdbl_mass_variance = 3.157174 ;

			isotopes.mint_atomicity = 64;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 151.919786 ;
			isotopes.marr_isotope_prob[0] = 0.002000 ;
			isotopes.marr_isotope_mass[1] = 153.920861 ;
			isotopes.marr_isotope_prob[1] = 0.021800 ;
			isotopes.marr_isotope_mass[2] = 154.922618 ;
			isotopes.marr_isotope_prob[2] = 0.148000 ;
			isotopes.marr_isotope_mass[3] = 155.922118 ;
			isotopes.marr_isotope_prob[3] = 0.204700 ;
			isotopes.marr_isotope_mass[4] = 156.923956 ;
			isotopes.marr_isotope_prob[4] = 0.156500 ;
			isotopes.marr_isotope_mass[5] = 157.924099 ;
			isotopes.marr_isotope_prob[5] = 0.248400 ;
			isotopes.marr_isotope_mass[6] = 159.927049 ;
			isotopes.marr_isotope_prob[6] = 0.218600 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Tb") ;
			strcpy(isotopes.marr_name, "Terbium") ;

			isotopes.mdbl_average_mass = 158.925342;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 65;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 158.925342 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Dy") ;
			strcpy(isotopes.marr_name, "Dysprosium") ;

			isotopes.mdbl_average_mass = 162.497531;
			isotopes.mdbl_mass_variance = 1.375235 ;

			isotopes.mint_atomicity = 66;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 155.925277 ;
			isotopes.marr_isotope_prob[0] = 0.000600 ;
			isotopes.marr_isotope_mass[1] = 157.924403 ;
			isotopes.marr_isotope_prob[1] = 0.001000 ;
			isotopes.marr_isotope_mass[2] = 159.925193 ;
			isotopes.marr_isotope_prob[2] = 0.023400 ;
			isotopes.marr_isotope_mass[3] = 160.926930 ;
			isotopes.marr_isotope_prob[3] = 0.189000 ;
			isotopes.marr_isotope_mass[4] = 161.926795 ;
			isotopes.marr_isotope_prob[4] = 0.255000 ;
			isotopes.marr_isotope_mass[5] = 162.928728 ;
			isotopes.marr_isotope_prob[5] = 0.249000 ;
			isotopes.marr_isotope_mass[6] = 163.929171 ;
			isotopes.marr_isotope_prob[6] = 0.282000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ho") ;
			strcpy(isotopes.marr_name, "Holmium") ;

			isotopes.mdbl_average_mass = 164.930319;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 67;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 164.930319 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Er") ;
			strcpy(isotopes.marr_name, "Erbium") ;

			isotopes.mdbl_average_mass = 167.255701;
			isotopes.mdbl_mass_variance = 2.024877 ;

			isotopes.mint_atomicity = 68;
			isotopes.mint_num_isotopes = 6;

			isotopes.marr_isotope_mass[0] = 161.928775 ;
			isotopes.marr_isotope_prob[0] = 0.001400 ;
			isotopes.marr_isotope_mass[1] = 163.929198 ;
			isotopes.marr_isotope_prob[1] = 0.016100 ;
			isotopes.marr_isotope_mass[2] = 165.930290 ;
			isotopes.marr_isotope_prob[2] = 0.336000 ;
			isotopes.marr_isotope_mass[3] = 166.932046 ;
			isotopes.marr_isotope_prob[3] = 0.229500 ;
			isotopes.marr_isotope_mass[4] = 167.932368 ;
			isotopes.marr_isotope_prob[4] = 0.268000 ;
			isotopes.marr_isotope_mass[5] = 169.935461 ;
			isotopes.marr_isotope_prob[5] = 0.149000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Tm") ;
			strcpy(isotopes.marr_name, "Thulium") ;

			isotopes.mdbl_average_mass = 168.934212;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 69;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 168.934212 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Yb") ;
			strcpy(isotopes.marr_name, "Ytterbium") ;

			isotopes.mdbl_average_mass = 173.034187;
			isotopes.mdbl_mass_variance = 2.556093 ;

			isotopes.mint_atomicity = 70;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 167.933894 ;
			isotopes.marr_isotope_prob[0] = 0.001300 ;
			isotopes.marr_isotope_mass[1] = 169.934759 ;
			isotopes.marr_isotope_prob[1] = 0.030500 ;
			isotopes.marr_isotope_mass[2] = 170.936323 ;
			isotopes.marr_isotope_prob[2] = 0.143000 ;
			isotopes.marr_isotope_mass[3] = 171.936378 ;
			isotopes.marr_isotope_prob[3] = 0.219000 ;
			isotopes.marr_isotope_mass[4] = 172.938208 ;
			isotopes.marr_isotope_prob[4] = 0.161200 ;
			isotopes.marr_isotope_mass[5] = 173.938859 ;
			isotopes.marr_isotope_prob[5] = 0.318000 ;
			isotopes.marr_isotope_mass[6] = 175.942564 ;
			isotopes.marr_isotope_prob[6] = 0.127000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Lu") ;
			strcpy(isotopes.marr_name, "Lutetium") ;

			isotopes.mdbl_average_mass = 174.966719;
			isotopes.mdbl_mass_variance = 0.025326 ;

			isotopes.mint_atomicity = 71;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 174.940770 ;
			isotopes.marr_isotope_prob[0] = 0.974100 ;
			isotopes.marr_isotope_mass[1] = 175.942679 ;
			isotopes.marr_isotope_prob[1] = 0.025900 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Hf") ;
			strcpy(isotopes.marr_name, "Hafnium") ;

			isotopes.mdbl_average_mass = 178.486400;
			isotopes.mdbl_mass_variance = 1.671265 ;

			isotopes.mint_atomicity = 72;
			isotopes.mint_num_isotopes = 6;

			isotopes.marr_isotope_mass[0] = 173.940044 ;
			isotopes.marr_isotope_prob[0] = 0.001620 ;
			isotopes.marr_isotope_mass[1] = 175.941406 ;
			isotopes.marr_isotope_prob[1] = 0.052060 ;
			isotopes.marr_isotope_mass[2] = 176.943217 ;
			isotopes.marr_isotope_prob[2] = 0.186060 ;
			isotopes.marr_isotope_mass[3] = 177.943696 ;
			isotopes.marr_isotope_prob[3] = 0.272970 ;
			isotopes.marr_isotope_mass[4] = 178.945812 ;
			isotopes.marr_isotope_prob[4] = 0.136290 ;
			isotopes.marr_isotope_mass[5] = 179.946545 ;
			isotopes.marr_isotope_prob[5] = 0.351000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ta") ;
			strcpy(isotopes.marr_name, "Tantalum") ;

			isotopes.mdbl_average_mass = 180.947872;
			isotopes.mdbl_mass_variance = 0.000120 ;

			isotopes.mint_atomicity = 73;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 179.947462 ;
			isotopes.marr_isotope_prob[0] = 0.000120 ;
			isotopes.marr_isotope_mass[1] = 180.947992 ;
			isotopes.marr_isotope_prob[1] = 0.999880 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "W") ;
			strcpy(isotopes.marr_name, "Tungsten") ;

			isotopes.mdbl_average_mass = 183.849486;
			isotopes.mdbl_mass_variance = 2.354748 ;

			isotopes.mint_atomicity = 74;
			isotopes.mint_num_isotopes = 5;

			isotopes.marr_isotope_mass[0] = 179.946701 ;
			isotopes.marr_isotope_prob[0] = 0.001200 ;
			isotopes.marr_isotope_mass[1] = 181.948202 ;
			isotopes.marr_isotope_prob[1] = 0.263000 ;
			isotopes.marr_isotope_mass[2] = 182.950220 ;
			isotopes.marr_isotope_prob[2] = 0.142800 ;
			isotopes.marr_isotope_mass[3] = 183.950928 ;
			isotopes.marr_isotope_prob[3] = 0.307000 ;
			isotopes.marr_isotope_mass[4] = 185.954357 ;
			isotopes.marr_isotope_prob[4] = 0.286000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Re") ;
			strcpy(isotopes.marr_name, "Rhenium") ;

			isotopes.mdbl_average_mass = 186.206699;
			isotopes.mdbl_mass_variance = 0.939113 ;

			isotopes.mint_atomicity = 75;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 184.952951 ;
			isotopes.marr_isotope_prob[0] = 0.374000 ;
			isotopes.marr_isotope_mass[1] = 186.955744 ;
			isotopes.marr_isotope_prob[1] = 0.626000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Os") ;
			strcpy(isotopes.marr_name, "Osmium") ;

			isotopes.mdbl_average_mass = 190.239771;
			isotopes.mdbl_mass_variance = 2.665149 ;

			isotopes.mint_atomicity = 76;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 183.952488 ;
			isotopes.marr_isotope_prob[0] = 0.000200 ;
			isotopes.marr_isotope_mass[1] = 185.953830 ;
			isotopes.marr_isotope_prob[1] = 0.015800 ;
			isotopes.marr_isotope_mass[2] = 186.955741 ;
			isotopes.marr_isotope_prob[2] = 0.016000 ;
			isotopes.marr_isotope_mass[3] = 187.955860 ;
			isotopes.marr_isotope_prob[3] = 0.133000 ;
			isotopes.marr_isotope_mass[4] = 188.958137 ;
			isotopes.marr_isotope_prob[4] = 0.161000 ;
			isotopes.marr_isotope_mass[5] = 189.958436 ;
			isotopes.marr_isotope_prob[5] = 0.264000 ;
			isotopes.marr_isotope_mass[6] = 191.961467 ;
			isotopes.marr_isotope_prob[6] = 0.410000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ir") ;
			strcpy(isotopes.marr_name, "Iridium") ;

			isotopes.mdbl_average_mass = 192.216047;
			isotopes.mdbl_mass_variance = 0.937668 ;

			isotopes.mint_atomicity = 77;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 190.960584 ;
			isotopes.marr_isotope_prob[0] = 0.373000 ;
			isotopes.marr_isotope_mass[1] = 192.962917 ;
			isotopes.marr_isotope_prob[1] = 0.627000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Pt") ;
			strcpy(isotopes.marr_name, "Platinum") ;

			isotopes.mdbl_average_mass = 195.080105;
			isotopes.mdbl_mass_variance = 1.293292 ;

			isotopes.mint_atomicity = 78;
			isotopes.mint_num_isotopes = 6;

			isotopes.marr_isotope_mass[0] = 189.959917 ;
			isotopes.marr_isotope_prob[0] = 0.000100 ;
			isotopes.marr_isotope_mass[1] = 191.961019 ;
			isotopes.marr_isotope_prob[1] = 0.007900 ;
			isotopes.marr_isotope_mass[2] = 193.962655 ;
			isotopes.marr_isotope_prob[2] = 0.329000 ;
			isotopes.marr_isotope_mass[3] = 194.964766 ;
			isotopes.marr_isotope_prob[3] = 0.338000 ;
			isotopes.marr_isotope_mass[4] = 195.964926 ;
			isotopes.marr_isotope_prob[4] = 0.253000 ;
			isotopes.marr_isotope_mass[5] = 197.967869 ;
			isotopes.marr_isotope_prob[5] = 0.072000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Au") ;
			strcpy(isotopes.marr_name, "Gold") ;

			isotopes.mdbl_average_mass = 196.966543;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 79;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 196.966543 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Hg") ;
			strcpy(isotopes.marr_name, "Mercury") ;

			isotopes.mdbl_average_mass = 200.596438;
			isotopes.mdbl_mass_variance = 2.625230 ;

			isotopes.mint_atomicity = 80;
			isotopes.mint_num_isotopes = 7;

			isotopes.marr_isotope_mass[0] = 195.965807 ;
			isotopes.marr_isotope_prob[0] = 0.001500 ;
			isotopes.marr_isotope_mass[1] = 197.966743 ;
			isotopes.marr_isotope_prob[1] = 0.100000 ;
			isotopes.marr_isotope_mass[2] = 198.968254 ;
			isotopes.marr_isotope_prob[2] = 0.169000 ;
			isotopes.marr_isotope_mass[3] = 199.968300 ;
			isotopes.marr_isotope_prob[3] = 0.231000 ;
			isotopes.marr_isotope_mass[4] = 200.970277 ;
			isotopes.marr_isotope_prob[4] = 0.132000 ;
			isotopes.marr_isotope_mass[5] = 201.970617 ;
			isotopes.marr_isotope_prob[5] = 0.298000 ;
			isotopes.marr_isotope_mass[6] = 203.973467 ;
			isotopes.marr_isotope_prob[6] = 0.068500 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Tl") ;
			strcpy(isotopes.marr_name, "Thallium") ;

			isotopes.mdbl_average_mass = 204.383307;
			isotopes.mdbl_mass_variance = 0.834026 ;

			isotopes.mint_atomicity = 81;
			isotopes.mint_num_isotopes = 2;

			isotopes.marr_isotope_mass[0] = 202.972320 ;
			isotopes.marr_isotope_prob[0] = 0.295240 ;
			isotopes.marr_isotope_mass[1] = 204.974401 ;
			isotopes.marr_isotope_prob[1] = 0.704760 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Pb") ;
			strcpy(isotopes.marr_name, "Lead") ;

			isotopes.mdbl_average_mass = 207.216883;
			isotopes.mdbl_mass_variance = 0.834636 ;

			isotopes.mint_atomicity = 82;
			isotopes.mint_num_isotopes = 4;

			isotopes.marr_isotope_mass[0] = 203.973020 ;
			isotopes.marr_isotope_prob[0] = 0.014000 ;
			isotopes.marr_isotope_mass[1] = 205.974440 ;
			isotopes.marr_isotope_prob[1] = 0.241000 ;
			isotopes.marr_isotope_mass[2] = 206.975872 ;
			isotopes.marr_isotope_prob[2] = 0.221000 ;
			isotopes.marr_isotope_mass[3] = 207.976627 ;
			isotopes.marr_isotope_prob[3] = 0.524000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Bi") ;
			strcpy(isotopes.marr_name, "Bismuth") ;

			isotopes.mdbl_average_mass = 208.980374;
			isotopes.mdbl_mass_variance = 0.000000 ;


			isotopes.mint_atomicity = 83;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 208.980374 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Po") ;
			strcpy(isotopes.marr_name, "Polonium") ;

			isotopes.mdbl_average_mass = 209.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 84;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 209.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "At") ;
			strcpy(isotopes.marr_name, "Astatine") ;

			isotopes.mdbl_average_mass = 210.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 85;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 210.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Rn") ;
			strcpy(isotopes.marr_name, "Radon") ;

			isotopes.mdbl_average_mass = 222.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 86;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 222.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Fr") ;
			strcpy(isotopes.marr_name, "Francium") ;

			isotopes.mdbl_average_mass = 223.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 87;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 223.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ra") ;
			strcpy(isotopes.marr_name, "Radium") ;

			isotopes.mdbl_average_mass = 226.025000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 88;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 226.025000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Ac") ;
			strcpy(isotopes.marr_name, "Actinium") ;

			isotopes.mdbl_average_mass = 227.028000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 89;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 227.028000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Th") ;
			strcpy(isotopes.marr_name, "Thorium") ;

			isotopes.mdbl_average_mass = 232.038054;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 90;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 232.038054 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Pa") ;
			strcpy(isotopes.marr_name, "Protactinium") ;

			isotopes.mdbl_average_mass = 231.035900;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 91;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 231.035900 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "U") ;
			strcpy(isotopes.marr_name, "Uranium") ;

			isotopes.mdbl_average_mass = 238.028914;
			isotopes.mdbl_mass_variance = 0.065503 ;

			isotopes.mint_atomicity = 92;
			isotopes.mint_num_isotopes = 3;

			isotopes.marr_isotope_mass[0] = 234.040946 ;
			isotopes.marr_isotope_prob[0] = 0.000055 ;
			isotopes.marr_isotope_mass[1] = 235.043924 ;
			isotopes.marr_isotope_prob[1] = 0.007200 ;
			isotopes.marr_isotope_mass[2] = 238.050784 ;
			isotopes.marr_isotope_prob[2] = 0.992745 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Np") ;
			strcpy(isotopes.marr_name, "Neptunium") ;

			isotopes.mdbl_average_mass = 237.048000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 93;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 237.048000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Pu") ;
			strcpy(isotopes.marr_name, "Plutonium") ;

			isotopes.mdbl_average_mass = 244.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 94;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 244.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Am") ;
			strcpy(isotopes.marr_name, "Americium") ;

			isotopes.mdbl_average_mass = 243.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 95;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 243.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Cm") ;
			strcpy(isotopes.marr_name, "Curium") ;

			isotopes.mdbl_average_mass = 247.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 96;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 247.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Bk") ;
			strcpy(isotopes.marr_name, "Berkelium") ;

			isotopes.mdbl_average_mass = 247.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 97;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 247.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Cm") ;
			strcpy(isotopes.marr_name, "Californium") ;

			isotopes.mdbl_average_mass = 251.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 98;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 251.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Es") ;
			strcpy(isotopes.marr_name, "Einsteinium") ;

			isotopes.mdbl_average_mass = 252.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 99 ;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 252.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Fm") ;
			strcpy(isotopes.marr_name, "Fernium") ;

			isotopes.mdbl_average_mass = 257.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 100;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 257.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "Md") ;
			strcpy(isotopes.marr_name, "Medelevium") ;

			isotopes.mdbl_average_mass = 258.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 101 ;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 258.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 

			strcpy(isotopes.marr_symbol, "No") ;
			strcpy(isotopes.marr_name, "Nobelium") ;

			isotopes.mdbl_average_mass = 259.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 102 ;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 259.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 


			strcpy(isotopes.marr_symbol, "Lr") ;
			strcpy(isotopes.marr_name, "Lawrencium") ;

			isotopes.mdbl_average_mass = 260.000000;
			isotopes.mdbl_mass_variance = 0.000000 ;

			isotopes.mint_atomicity = 103 ;
			isotopes.mint_num_isotopes = 1;

			isotopes.marr_isotope_mass[0] = 260.000000 ;
			isotopes.marr_isotope_prob[0] = 1.000000 ;

			mvect_elemental_isotopes.push_back(isotopes) ; 


			//Engine::TheoreticalProfile::ElementalIsotopes isotopes ; 
			//std::cout.precision(6) ; 
			//std::cout.setf(std::ios::fixed, std::ios::floatfield);

			//for (int element_num = 0 ; element_num < mvect_elemental_isotopes.size() ; element_num++)
			//{
			//	isotopes = mvect_elemental_isotopes[element_num] ; 

			//	std::cout<<"strcpy(isotopes.marr_symbol, \""<<isotopes.marr_symbol<<"\") ;\n" ; 
			//	std::cout<<"strcpy(isotopes.marr_name, \""<<isotopes.marr_name<<"\") ;\n\n" ; 
			//	std::cout<<"isotopes.mdbl_average_mass = "<<isotopes.mdbl_average_mass<<";\n" ; 
			//	std::cout<<"isotopes.mdbl_mass_variance = "<<isotopes.mdbl_mass_variance<<" ;\n\n" ; 
			//	std::cout<<"isotopes.mint_atomicity = "<<isotopes.mint_atomicity<<";\n" ; 
			//	std::cout<<"isotopes.mint_num_isotopes = "<<isotopes.mint_num_isotopes<<";\n\n" ; 
			//	for (int isotope_num = 0 ; isotope_num < isotopes.mint_num_isotopes ; isotope_num++)
			//	{
			//		std::cout<<"isotopes.marr_isotope_mass["<<isotope_num<<"] = "<<isotopes.marr_isotope_mass[isotope_num]<<" ;\n" ; 
			//		std::cout<<"isotopes.marr_isotope_prob["<<isotope_num<<"] = "<<isotopes.marr_isotope_prob[isotope_num]<<" ;\n" ; 
			//	}
			//	std::cout<<"\nmvect_elemental_isotopes.push_back(isotopes) ; \n\n" ; 
			//}
		}

		void AtomicInformation::LoadData(const char *file_name)
		{
			mvect_elemental_isotopes.clear() ; 
			/** Function that reads from xml file, creates a DOM tree and writes the isotope config 
			into mvect_elemental_isotopes **/
			ElementalIsotopes isotopes ;	

			bool	doNamespaces = false;
			bool	doSchema = false;
			bool	schemaFullChecking = false;			
			bool	doCreate = false;
			bool	bFailed = false;		
			const char* element_tag = "element";
			const char* symbol_tag = "symbol";
			const char* num_isotopes_tag = "num_isotopes";
			const char* isotope_tag = "isotope";
			const char* atomicity_tag = "atomicity";
			const char* mass_tag = "mass";
			const char* probability_tag = "probability";
			const char *isotopes_tag = "isotopes";
			const char *name_tag = "name";
			int num_isotopes = 0;
			int atomicity = 0;
			int i = 0;

			mvect_elemental_isotopes.clear();

			static XERCES_CPP_NAMESPACE::XercesDOMParser::ValSchemes    valScheme = XERCES_CPP_NAMESPACE::XercesDOMParser::Val_Auto ;
			//Initialize the XML 
			try
			{
				XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();			
			}
			catch(const XERCES_CPP_NAMESPACE::XMLException &toCatch)
			{
				throw toCatch.getMessage() ; 
			}

			XERCES_CPP_NAMESPACE::XercesDOMParser *parser = new XERCES_CPP_NAMESPACE::XercesDOMParser();
			if (parser)
			{
				parser->setValidationScheme(valScheme);
				parser->setDoNamespaces(doNamespaces);
				parser->setDoSchema(doSchema);
				parser->setValidationSchemaFullChecking(schemaFullChecking);
				try
				{
					parser->parse(file_name);
				}
				catch(const XERCES_CPP_NAMESPACE::XMLException &toCatch)
				{
					SetDefaultIsotopeDistribution() ; 
					delete parser ; 
					throw toCatch.getMessage() ; 
				}

				//create DOM tree
				XERCES_CPP_NAMESPACE::DOMDocument *doc = NULL ;				
				try
				{
					doc = parser->getDocument();				
				}
				catch(const XERCES_CPP_NAMESPACE::XMLException &toCatch)
				{
					SetDefaultIsotopeDistribution() ; 
					delete parser ; 
					throw toCatch.getMessage() ; 
				}
				catch(const XERCES_CPP_NAMESPACE::DOMException &toCatch)
				{
					SetDefaultIsotopeDistribution() ; 
					delete parser ; 
					throw toCatch.getMessage() ; 
				}
				catch(const char *toCatch)
				{
					SetDefaultIsotopeDistribution() ; 
					delete parser ; 
					throw toCatch ; 
				}
				catch(const exception &toCatch)
				{
					SetDefaultIsotopeDistribution() ; 
					delete parser ; 
					throw toCatch.what() ; 
				}

				XERCES_CPP_NAMESPACE::DOMNode *nRoot = NULL;			

				if (doc)
				{
					//start walking down the tree
					nRoot = (XERCES_CPP_NAMESPACE::DOMNode*) doc->getDocumentElement();
					XERCES_CPP_NAMESPACE::DOMNode *nCurrent = NULL;					
					XERCES_CPP_NAMESPACE::DOMTreeWalker *walker = doc->createTreeWalker(nRoot, XERCES_CPP_NAMESPACE::DOMNodeFilter::SHOW_ELEMENT, NULL, false); //at elements
					nCurrent = walker->nextNode();//first "element" get the element name
					/*DOMNamedNodeMap *attr =  nCurrent->getAttributes();
					DOMNode *att= attr->item(0);
					char *element_name = XMLString::transcode(att->getNodeValue());
					strcpy(isotopes.marr_name, element_name );*/
		
					nCurrent = walker->nextNode(); //goes to "name"

					while (nCurrent!=0)
					{
						char *nName = XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getNodeName());
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, name_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);
							char *element_name = XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getTextContent());
							strcpy(isotopes.marr_name, element_name );
							XERCES_CPP_NAMESPACE::XMLString::release(&element_name) ; 
							nCurrent = walker->nextNode();
							continue;
						}
						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, symbol_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);
							char *element_symbol = XERCES_CPP_NAMESPACE::XMLString::transcode(nCurrent->getTextContent());
							strcpy(isotopes.marr_symbol, element_symbol );
							XERCES_CPP_NAMESPACE::XMLString::release(&element_symbol) ; 
							nCurrent = walker->nextNode();
							continue;
						}

						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, atomicity_tag))
						{	
							//can be modified to store in atomicity if needed
							atomicity =  XERCES_CPP_NAMESPACE::XMLString::parseInt(nCurrent->getTextContent());
							isotopes.mint_atomicity = atomicity;
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);
							nCurrent = walker->nextNode();
							continue;
						}

						if(XERCES_CPP_NAMESPACE::XMLString::equals(nName, num_isotopes_tag))
						{	
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);		
							num_isotopes = XERCES_CPP_NAMESPACE::XMLString::parseInt(nCurrent->getTextContent());
							isotopes.mint_num_isotopes = num_isotopes;
							isotopes.mdbl_average_mass = 0 ; 
							isotopes.mdbl_mass_variance = 0 ; 
							nCurrent = walker->nextNode();
							continue;
						}

						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, isotopes_tag))
						{
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);
							nCurrent = walker->nextNode();
							continue;
						}						

						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, isotope_tag))
						{	
							//keeping track of i
							nCurrent = walker->nextNode();
							XERCES_CPP_NAMESPACE::XMLString::release(&nName);
							const XMLCh*  text_context = nCurrent->getTextContent() ;

							char *ch_text = XERCES_CPP_NAMESPACE::XMLString::transcode(text_context) ; 
							double mass = atof(ch_text) ;
							isotopes.marr_isotope_mass[i] = mass ; 
							XERCES_CPP_NAMESPACE::XMLString::release(&ch_text) ; 

							nCurrent = walker->nextNode();
							text_context = nCurrent->getTextContent() ;
							ch_text = XERCES_CPP_NAMESPACE::XMLString::transcode(text_context) ; 
							double prob = atof(ch_text) ;
							isotopes.marr_isotope_prob[i] = prob ; 
							XERCES_CPP_NAMESPACE::XMLString::release(&ch_text) ; 

							isotopes.mdbl_average_mass += isotopes.marr_isotope_mass[i] * isotopes.marr_isotope_prob[i];
							nCurrent = walker->nextNode();	
							i = i+1;
							continue;
						}

						if (XERCES_CPP_NAMESPACE::XMLString::equals(nName, element_tag))
						{
							if (i != num_isotopes)
								return; //error if unequal

							isotopes.mdbl_mass_variance = 0.0 ; 
							for (int j = 0 ; j < num_isotopes ; j++)
							{
								double mass_diff = isotopes.marr_isotope_mass[j] - isotopes.mdbl_average_mass ; 
								isotopes.mdbl_mass_variance += mass_diff * mass_diff * isotopes.marr_isotope_prob[j] ; 
							 }
							
							//next element							
							i = 0;
							/*DOMNamedNodeMap *attrMap =  nCurrent->getAttributes();
							DOMNode *attItem = attr->item(0);
							char *element_name = XERCES_CPP_NAMESPACE::XMLString::transcode(att->getNodeValue());
							strcpy(isotopes.marr_name, element_name );*/
							nCurrent = walker->nextNode(); //goes to "name"
							mvect_elemental_isotopes.push_back(isotopes) ;
						}

						XERCES_CPP_NAMESPACE::XMLString::release(&nName) ; 
					}
				}
				else
				{
					throw "Unable to open isotopes.xml. Perhaps the file does not exist ?" ; 
				}

				doc->release() ; 
				delete doc ; 
			}
			if (i != num_isotopes)
				return; //error if unequal
			else
			{
				for (int j = 0 ; j < num_isotopes ; j++)
				{
					double mass_diff = isotopes.marr_isotope_mass[j] - isotopes.mdbl_average_mass ; 
					isotopes.mdbl_mass_variance += mass_diff * mass_diff * isotopes.marr_isotope_prob[j] ; 
				}
				mvect_elemental_isotopes.push_back(isotopes) ;
			}

			if (parser)
				delete parser ; 
			XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();

		}

		void AtomicInformation::LoadData() {
			LoadData(TheoreticalProfile::default_file_name);
		}

		void AtomicInformation::WriteData(const char *file_name)
		{			
			ElementalIsotopes isotopes ;
			const char* name_tag = "name";
			const char* elements_tag = "elements";
			const char* element_tag = "element";
			const char* symbol_tag = "symbol";
			const char* num_isotopes_tag = "num_isotopes";
			const char* isotope_tag = "isotope";
			const char* atomicity_tag = "atomicity";
			const char* mass_tag = "mass";
			const char* probability_tag = "probability";
			const char *isotopes_tag = "isotopes";


			try
			{
				XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();			
			}
			catch(const XERCES_CPP_NAMESPACE::XMLException &toCatch)
			{
				std::cerr<<toCatch.getMessage()<<std::endl ; 
				//Do something
				return;
			}

			XERCES_CPP_NAMESPACE::DOMImplementation* impl =  XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(XERCES_CPP_NAMESPACE::XMLString::transcode("Core"));
			XERCES_CPP_NAMESPACE::DOMWriter *theSerializer = impl->createDOMWriter();	

			 if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTSplitCdataSections, true))
                theSerializer->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTSplitCdataSections, true);

            if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTDiscardDefaultContent, true))
                theSerializer->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTDiscardDefaultContent, true);

            if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTFormatPrettyPrint, true))
                theSerializer->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTFormatPrettyPrint, true);

            if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTBOM, true))
                theSerializer->setFeature(XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTBOM, true);



			if (impl != NULL)
			{

				XERCES_CPP_NAMESPACE::DOMDocument* doc = impl->createDocument(0, XERCES_CPP_NAMESPACE::XMLString::transcode(elements_tag), 0);
				XERCES_CPP_NAMESPACE::DOMElement* rootElem = doc->getDocumentElement();				

				int size = mvect_elemental_isotopes.size();

				for (int j = 0; j<size; j++)
				{

					isotopes = mvect_elemental_isotopes[j];

					XERCES_CPP_NAMESPACE::DOMElement* elementElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(element_tag));
					XERCES_CPP_NAMESPACE::DOMElement* nameElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(name_tag));
					XERCES_CPP_NAMESPACE::DOMElement* symbolElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(symbol_tag));
					XERCES_CPP_NAMESPACE::DOMElement* atomicityElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(atomicity_tag));
					XERCES_CPP_NAMESPACE::DOMElement* numisotopesElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(num_isotopes_tag));
					XERCES_CPP_NAMESPACE::DOMElement* isotopesElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(isotopes_tag));

					rootElem->appendChild(elementElem);				


					elementElem->appendChild(nameElem);
					char *element_name = NULL;			
					element_name = isotopes.marr_name;	
					XERCES_CPP_NAMESPACE::DOMText* nameElemValue = doc->createTextNode(XERCES_CPP_NAMESPACE::XMLString::transcode(element_name));
					nameElem->appendChild(nameElemValue);
					//elementElem->setAttribute(XERCES_CPP_NAMESPACE::XMLString::transcode("id"), XERCES_CPP_NAMESPACE::XMLString::transcode(element_name));				

					elementElem->appendChild(symbolElem);
					char *elemental_symbol = isotopes.marr_symbol;
					XERCES_CPP_NAMESPACE::DOMText* symbolElemValue = doc->createTextNode(XERCES_CPP_NAMESPACE::XMLString::transcode(elemental_symbol));
					symbolElem->appendChild(symbolElemValue);				

					elementElem->appendChild(atomicityElem);				
					XMLCh* temp1 = new XMLCh[10];
					XERCES_CPP_NAMESPACE::XMLString::binToText(isotopes.mint_atomicity, temp1, 9 ,10);
					XERCES_CPP_NAMESPACE::DOMText* atomicityElemValue = doc->createTextNode(temp1);
					atomicityElem->appendChild(atomicityElemValue);

					elementElem->appendChild(numisotopesElem);
					XERCES_CPP_NAMESPACE::XMLString::binToText(isotopes.mint_num_isotopes, temp1, 9 ,10);
					XERCES_CPP_NAMESPACE::DOMText* numisotopesElemValue = doc->createTextNode(temp1);
					numisotopesElem->appendChild(numisotopesElemValue);

					elementElem->appendChild(isotopesElem);
					for( int i=0;i<isotopes.mint_num_isotopes;i++)
					{
						XERCES_CPP_NAMESPACE::DOMElement* isotopeElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(isotope_tag));
						XERCES_CPP_NAMESPACE::DOMElement* massElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(mass_tag));	
						XERCES_CPP_NAMESPACE::DOMElement* probElem = doc->createElement(XERCES_CPP_NAMESPACE::XMLString::transcode(probability_tag));
						char buf[100];

						sprintf(buf, "%f",isotopes.marr_isotope_mass[i]);
						XERCES_CPP_NAMESPACE::DOMText* massElemValue = doc->createTextNode(XERCES_CPP_NAMESPACE::XMLString::transcode(buf));					
						massElem->appendChild(massElemValue);
						isotopeElem->appendChild(massElem);

						sprintf(buf, "%f",isotopes.marr_isotope_prob[i]);
						XERCES_CPP_NAMESPACE::DOMText* probElemValue = doc->createTextNode(XERCES_CPP_NAMESPACE::XMLString::transcode(buf));
						probElem->appendChild(probElemValue);
						isotopeElem->appendChild(probElem);

						isotopesElem->appendChild(isotopeElem);						
					}				
				}	
				XERCES_CPP_NAMESPACE::XMLFormatTarget *myFormTarget;
				myFormTarget = new XERCES_CPP_NAMESPACE::LocalFileFormatTarget(file_name);
				theSerializer->writeNode(myFormTarget, *doc);
				theSerializer->release() ; 
				delete myFormTarget ; 
				theSerializer = NULL ; 
				if (doc)
				{
					doc->release() ; 
					delete doc ; 
				}
				if (impl)
				{
					delete impl ; 
				}
			}

			XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();

		}

		int AtomicInformation::GetNumElements()
		{
			return mvect_elemental_isotopes.size() ; 
		}
		int AtomicInformation::GetElementIndex(char *symbol)
		{
			int num_elements = mvect_elemental_isotopes.size() ; 
			for (int element_num = 0 ; element_num < num_elements ; element_num++)
			{
				if (_stricmp(mvect_elemental_isotopes[element_num].marr_symbol, symbol) == 0)
				{
					// found the element. Return the index.
					return element_num ; 
				}
			}
			return -1 ; 
		}
	}
}