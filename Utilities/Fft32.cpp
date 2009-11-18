
#include <math.h>
#include "fft32.h"

namespace Engine
{
	namespace FFT
	{
		const double PI = 3.14159265358979323846 ; 
		const double TWOPI = 2*3.14159265358979323846 ;		

		/*
		* Replaces data by its discrete Fourier transfor, if isign is input as 1;
		* or replaces data by nn times its inverse discrete Fourier transform, if
		* isign is input as -1. data is a complex array of length nn, input as a
		* real array data[1...2*nn]. nn must be an integer power of 2 (this is not
		* checked for !).
		*
		*  Numerical Recipes in C.
		*  William H. Press
		*  Brian P. Flannery
		*  Saul A. Teukolsky
		*  William T. Vetterling
		*/

		void four(int nn, float *data, int isign)
		{
			long      n,mmax,m,j,istep,i,im1,jm1;
			double    wtemp,wr,wpr,wpi,wi,theta;
			float     tempr,tempi;

			n = nn << 1;
			j = 1;
			for(i=1;i<n;i+=2)
			{
				if(j > i)
				{
					SWAP(data[j-1],data[i-1]);
					SWAP(data[j],data[i]);
				}
				m = n >> 1;
				while(m >= 2 && j > m)
				{
					j -= m;
					m >>= 1;
				}
				j += m;
			}
			mmax = 2;
			while(n > mmax)
			{
				istep = 2*mmax;
				theta = 6.28318530717959/(isign * mmax);
				wtemp = sin(0.5*theta);
				wpr = -2.0*wtemp*wtemp;
				wpi = sin(theta);
				wr = 1.0;
				wi = 0.0;
				for(m=1;m<mmax;m+=2)
				{
					for(i=m;i<=n;i+=istep)
					{
						j = i+mmax;
						jm1=j-1;
						im1=i-1;
						tempr = (float) (wr*data[jm1]-wi*data[j]);
						tempi = (float) (wr*data[j]+wi*data[jm1]);
						data[jm1]= (float) (data[im1]-tempr);
						data[j]= (float) (data[i]-tempi);
						data[im1] += tempr;
						data[i] += tempi;
					}
					wr=(wtemp=wr)*wpr-wi*wpi+wr;
					wi=wi*wpr+wtemp*wpi+wi;
				}
				mmax = istep;
			}
		}


		/*
		* Replaces data by its discrete Fourier transfor, if isign is input as 1;
		* or replaces data by nn times its inverse discrete Fourier transform, if
		* isign is input as -1. data is a complex array of length nn, input as a
		* real array data[1...2*nn]. nn must be an integer power of 2 (this is not
		* checked for !).
		*
		*  Numerical Recipes in C.
		*  William H. Press
		*  Brian P. Flannery
		*  Saul A. Teukolsky
		*  William T. Vetterling
		*/
		// hArray is a 16 bit selector for a 16:32 buffer
		int four1(int nn, float *lpdata, int isign)
		{
			float *data;

			data = lpdata;
			four(nn,data,isign);
			return(0);
		}


//		void four(int nn, std::vector<float> &data, int isign)
		//{
		//	long      n,mmax,m,j,istep,i,im1,jm1;
		//	double    wtemp,wr,wpr,wpi,wi,theta;
		//	float     tempr,tempi;

		//	n = nn << 1;
		//	j = 1;
		//	for(i=1;i<n;i+=2)
		//	{
		//		if(j > i)
		//		{
		//			SWAP(data[j-1],data[i-1]);
		//			SWAP(data[j],data[i]);
		//		}
		//		m = n >> 1;
		//		while(m >= 2 && j > m)
		//		{
		//			j -= m;
		//			m >>= 1;
		//		}
		//		j += m;
		//	}
		//	mmax = 2;
		//	while(n > mmax)
		//	{
		//		istep = 2*mmax;
		//		theta = 6.28318530717959/(isign * mmax);
		//		wtemp = sin(0.5*theta);
		//		wpr = -2.0*wtemp*wtemp;
		//		wpi = sin(theta);
		//		wr = 1.0;
		//		wi = 0.0;
		//		for(m=1;m<mmax;m+=2)
		//		{
		//			for(i=m;i<=n;i+=istep)
		//			{
		//				j = i+mmax;
		//				jm1=j-1;
		//				im1=i-1;
		//				tempr = (float) (wr*data[jm1]-wi*data[j]);
		//				tempi = (float) (wr*data[j]+wi*data[jm1]);
		//				data[jm1]= (float) (data[im1]-tempr);
		//				data[j]= (float) (data[i]-tempi);
		//				data[im1] += tempr;
		//				data[i] += tempi;
		//			}
		//			wr=(wtemp=wr)*wpr-wi*wpi+wr;
		//			wi=wi*wpr+wtemp*wpi+wi;
		//		}
		//		mmax = istep;
		//	}
		//}

		void Four1(unsigned long nn, std::vector<double> &Data, int isign)
		{
			unsigned long i, j, m, n, mmax, istep;
			double wr, wpr, wpi, wi, theta;
			double wtemp, tempr, tempi;

			/* Perform bit reversal of Data[] */
			n = nn << 1;
			j=1;
			for (i=1; i<n; i+=2)
			{
				if (j > i)
				{
					wtemp = Data[i];
					Data[i] = Data[j];
						Data[j] = wtemp;
					wtemp = Data[i+1];
					Data[i+1] = Data[j+1];
					Data[j+1] = wtemp;
				}
				m = n >> 1;
				while (m >= 2 && j > m)
				{
					j -= m;
					m >>= 1;
				}
				j += m;
			}
				
				/* Perform Danielson-Lanczos section of FFT */
			n = nn << 1;
			mmax = 2;
			while (n > mmax)  /* Loop executed log(2)nn times */
			{
				istep = mmax << 1;
				theta = isign * (TWOPI/mmax);  /* Initialize the trigonimetric recurrance */
				wtemp = sin(0.5*theta);
				wpr = -2.0*wtemp*wtemp;
				wpi = sin(theta);
				wr = 1.0;
				wi = 0.0;
				for (m=1; m<mmax; m+=2)
				{
					for (i=m; i<=n; i+=istep)
					{
							j = i+mmax;                       /* The Danielson-Lanczos formula */
						tempr = wr*Data[j]-wi*Data[j+1];
						tempi = wr*Data[j+1]+wi*Data[j];
						Data[j] = Data[i]-tempr;
						Data[j+1] = Data[i+1]-tempi;
						Data[i] += tempr;
						Data[i+1] += tempi;
					}
					wr = (wtemp=wr)*wpr-wi*wpi+wr;
					wi = wi*wpr+wtemp*wpi+wi;
				}
					mmax = istep;
				}
				/* Normalize if FT */
				if (isign == 1)
				for (i=1; i<=nn; i++)
				{
					Data[2*i-1] /= nn;
					Data[2*i] /= nn;
				}

		}  /* End of Four1() */

	/*
		* Calculate the Fourier Transform of a set of n real valued data points.
		* Replaces this data which is stored in array data[1..n]) by the positive
		* frequency half of its complex Fourier transform. The real valued first
		* and last components of the complex transform are returned as elements
		* data[1] and data[2] respectively. n must be a power of 2. This routine
		* also calculates the inverse transform of a complex data array if it is
		* the transform of real data.
		*
		* isign = 1 for FFT.
		* isign = -1 for IFFT.
		*/
		// hArray is a 16 bit selector for a 16:32 buffer
		int realft(int n, float *data,  int isign)
		{
			int        i,i1,i2,i3,i4,n2p3;
			float      c1=0.5,c2,hir,h1i,h2r,h2i;
			double     wpr,wpi,wi,wr,theta,wtemp;

			n = n/2;
			theta=3.141592653589793/(double) n;
			if(isign == 1)
			{
				c2 = -0.5;
				four(n,data,1);
			}
			else
			{
				c2 = 0.5;
				theta = -theta;
			}
			wtemp = sin(0.5*theta);
			wpr = -2.0*wtemp*wtemp;
			wpi = sin(theta);
			wr = 1.0 + wpr;
			wi = wpi;
			n2p3 = 2*n + 3;
			for(i=2;i<=n/2;i++)
			{
				i4 = 1+(i3=n2p3-(i2=1+(i1=i+i-1)));
				hir=c1*(data[i1-1]+data[i3-1]);
				h1i=c1*(data[i2-1]-data[i4-1]);
				h2r = -c2*(data[i2-1]+data[i4-1]);
				h2i = c2*(data[i1-1]-data[i3-1]);
				data[i1-1] = (float)(hir+wr*h2r-wi*h2i);
				data[i2-1] = (float)(h1i+wr*h2i+wi*h2r);
				data[i3-1] = (float)(hir-wr*h2r+wi*h2i);
				data[i4-1] = (float)(-h1i+wr*h2i+wi*h2r);
				wr=(wtemp=wr)*wpr-wi*wpi+wr;
				wi=wi*wpr+wtemp*wpi+wi;
			}
			if(isign == 1)
			{
				data[0] = (hir=data[0])+data[1];
				data[1] = hir-data[1];
		//		for(i=0;i<(n*2);i++) data[i] /= (n);  // GAA 50-30-00
			}
			else
			{
				data[0] = c1*((hir=data[0])+data[0]);
				data[1] = c1*(hir-data[1]);
				four(n,data,-1);
				for(i=0;i<(n*2);i++) data[i] /= n;
			}
			return 0 ;
		}
	}
}