// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "ReaderFactory.h"
#include "BrukerRawData.h"
#include "BrukerAsciiRawData.h"
#include "AsciiRawData.h"
#include "SunExtrelRawData.h"
#include "FinniganRawData.h" 
#include "AgilentRawData.h" 
#include "Icr2lsRawData.h" 
#include "MicromassRawData.h"
#include "MZXmlRawData.h" 
#include "IMSRawData.h" 
#include "UIMFRawData.h" 
#include "YafmsRawData.h"

namespace Engine 
{
	namespace Readers
	{
		RawData* ReaderFactory::GetRawData(FileType file_type, char *file_name)
		{
			char *header_n = "header" ; 
			switch(file_type)
			{
				case BRUKER:
					BrukerRawData *bruker_raw_data ; 
					bruker_raw_data = new BrukerRawData() ; 
					bruker_raw_data->Load(file_name) ; 
					return bruker_raw_data ; 
					break ; 
				case BRUKER_ASCII:
					BrukerAsciiRawData *bruker_baf_raw_data ; 
					bruker_baf_raw_data = new BrukerAsciiRawData() ; 
					bruker_baf_raw_data->Load(file_name) ; 
					return bruker_baf_raw_data ; 
					break ; 
				case ASCII:
					AsciiRawData *ascii_raw_data ; 
					ascii_raw_data = new AsciiRawData() ; 
					ascii_raw_data->Load(file_name) ; 
					return ascii_raw_data ; 
					break ; 
				case AGILENT_TOF:
#ifdef ANALYST_INSTALLED
					AgilentRawData *agilent_raw_data ; 
					agilent_raw_data = new AgilentRawData() ; 
					agilent_raw_data->Load(file_name) ; 
					return agilent_raw_data ; 
#endif 
					break ; 
				case SUNEXTREL:
					SunExtrelRawData *sun_extrel_raw_data  ;
					sun_extrel_raw_data = new SunExtrelRawData() ; 
					sun_extrel_raw_data->Load(file_name) ; 
					return sun_extrel_raw_data ; 
					break ; 
				case ICR2LSRAWDATA:
					Icr2lsRawData *icr2ls_raw_data  ;
					icr2ls_raw_data = new Icr2lsRawData() ; 
					icr2ls_raw_data->Load(file_name) ; 
					return icr2ls_raw_data ; 
					break ; 
				case FINNIGAN:
#ifdef XCALIBUR_INSTALLED
					FinniganRawData *finnigan_raw_data ; 
					finnigan_raw_data = new FinniganRawData() ;
					finnigan_raw_data->Load(file_name) ; 
					return finnigan_raw_data ; 
#endif 
					break ; 
				case MICROMASSRAWDATA:
					MicromassRawData *micro_raw_data ; 
					micro_raw_data = new MicromassRawData() ;
					micro_raw_data->Load(file_name) ; 
					return micro_raw_data ; 
					break ; 
				case MZXMLRAWDATA:
					MZXmlRawData *mzxml_raw_data ; 
					mzxml_raw_data = new MZXmlRawData() ; 
					mzxml_raw_data->Load(file_name) ; 
					return mzxml_raw_data ; 
					break ; 
				case PNNL_IMS:
					IMSRawData *ims_raw_data ; 
					ims_raw_data = new IMSRawData() ; 
					ims_raw_data->Load(file_name) ; 
					return ims_raw_data ; 
					break ; 
				case PNNL_UIMF:
					UIMFRawData *uimf_raw_data ; 
					uimf_raw_data = new UIMFRawData() ; 
					uimf_raw_data->Load(file_name) ; 
					return uimf_raw_data ; 
					break ; 
				case YAFMS:
					YafmsRawData *yafms_raw_data ; 
					yafms_raw_data = new YafmsRawData() ; 
					yafms_raw_data->Load(file_name) ; 
					return yafms_raw_data ; 
					break ; 
				default:
					break ; 
			}
			return NULL ; 
		}

		void ReaderFactory::GetRawData(RawData **raw_data, FileType file_type)
		{
			char *header_n = "header" ; 
			*raw_data = NULL ; 
			switch(file_type)
			{
				case BRUKER:
					*raw_data = new BrukerRawData() ; 
					return ; 
					break ; 
				case AGILENT_TOF:
#ifdef ANALYST_INSTALLED
					*raw_data = new AgilentRawData() ; 
					return ; 
#endif 
					break ; 
				case FINNIGAN:
#ifdef XCALIBUR_INSTALLED
					*raw_data = new FinniganRawData() ; 
					return ; 
#endif 
					break ; 
				case SUNEXTREL:
					*raw_data = new SunExtrelRawData() ;
					return ; 
					break ; 
				case ICR2LSRAWDATA:
					*raw_data = new Icr2lsRawData() ; 
					return ; 
					break ; 
				case MICROMASSRAWDATA:
					*raw_data = new MicromassRawData() ; 
					return ; 
					break ; 
				case MZXMLRAWDATA:
					*raw_data = new MZXmlRawData() ; 
					return ; 
					break ; 
				case PNNL_IMS:
					*raw_data = new IMSRawData() ; 
					return ;
					break ; 
				case PNNL_UIMF:
					*raw_data = new UIMFRawData() ; 
					return ;
					break ; 
				case YAFMS:
					*raw_data = new YafmsRawData() ; 
					return ;
					break ; 
				case BRUKER_ASCII:
					*raw_data = new BrukerAsciiRawData() ; 
					return ; 
					break ; 
				case ASCII:
					*raw_data = new AsciiRawData() ; 
					return ; 
					break ; 
				default:
					break ; 
			}
			return  ; 
		}

		RawData* ReaderFactory::GetRawData(FileType file_type)
		{
			char *header_n = "acqu" ; 
			switch(file_type)
			{
				case BRUKER:
					BrukerRawData *bruker_raw_data ; 
					bruker_raw_data = new BrukerRawData() ; 
					return bruker_raw_data ; 
					break ; 
				case AGILENT_TOF:
#ifdef ANALYST_INSTALLED
					AgilentRawData *agilent_raw_data ; 
					agilent_raw_data= new AgilentRawData() ; 
					return agilent_raw_data ; 
#endif
					break ; 
				case FINNIGAN:
#ifdef XCALIBUR_INSTALLED
					FinniganRawData *finnigan_raw_data ; 
					finnigan_raw_data = new FinniganRawData() ;
					return finnigan_raw_data ; 
#endif 
					break ; 
				case SUNEXTREL:
					SunExtrelRawData *sun_extrel_raw_data  ;
					sun_extrel_raw_data = new SunExtrelRawData() ; 
					return sun_extrel_raw_data ; 
					break ; 
				case ICR2LSRAWDATA:
					Icr2lsRawData *icr2ls_raw_data  ;
					icr2ls_raw_data = new Icr2lsRawData() ; 
					return icr2ls_raw_data ; 
					break ; 
				case MICROMASSRAWDATA:
					MicromassRawData *micro_raw_data ; 
					micro_raw_data = new MicromassRawData() ; 
					return micro_raw_data ; 
					break ; 
				case MZXMLRAWDATA:
					MZXmlRawData *mzxml_raw_data ; 
					mzxml_raw_data = new MZXmlRawData() ; 
					return mzxml_raw_data ; 
					break ; 
				case PNNL_IMS:
					IMSRawData *ims_raw_data ; 
					ims_raw_data = new IMSRawData() ; 
					return ims_raw_data ; 
					break ; 
				case PNNL_UIMF:
					UIMFRawData *uimf_raw_data ; 
					uimf_raw_data = new UIMFRawData() ; 
					return uimf_raw_data ; 
					break ; 
				case YAFMS:
					YafmsRawData *yafms_raw_data ; 
					yafms_raw_data = new YafmsRawData() ; 
					return yafms_raw_data ; 
					break ; 
				case BRUKER_ASCII:
					BrukerAsciiRawData *bruker_baf_raw_data ; 
					bruker_baf_raw_data = new BrukerAsciiRawData() ; 
					return bruker_baf_raw_data ; 
					break ; 
				case ASCII:
					AsciiRawData *ascii_raw_data ; 
					ascii_raw_data = new AsciiRawData() ; 
					return ascii_raw_data ; 
					break ; 
				default:
					break ; 
			}
			return NULL ; 
		}
	}
}