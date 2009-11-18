using System;
using System.IO;

namespace IonMobility
{
	/// <summary>
	/// Provide an interface for reading processed Ion Mobility files. A reader is initialized with an
	/// experiment object that encompasses an experiment folder.
	/// </summary>
	public class CReader
	{
		public CReader(CExperiment experiment)
		{
			_experiment = experiment;
		}

		// Get entire frame data, add a calibrator at some point
		public void ReadFrame(int frameNumber, out CFrame frame)
		{
			int [] spectraStartByte;
			int [] spectraStartIndex;
			int [] spectraNumRecs;
			int [] spectraTIC;
			int	[] TOFValues;
			short [] intensities;
			int dataStartByte;

			FileStream fs = new FileStream(_experiment.AccumulationFile(frameNumber), FileMode.Open, FileAccess.Read);
			BinaryReader r = new BinaryReader(fs);

			ReadIMFHeader(r, out dataStartByte, out spectraStartByte, out spectraNumRecs, out spectraTIC);
			ReadFrameData(r, spectraStartByte, spectraNumRecs, spectraTIC, out spectraStartIndex, out TOFValues, out intensities);

			frame = new CFrame(frameNumber, spectraStartIndex, spectraNumRecs, spectraTIC, TOFValues, intensities, _experiment);

			r.Close();
			fs.Close();
		}

		/// <summary>
		/// Open the ScanTime file and get the mean tof scan time for this frame.
		/// </summary>
		/// <param name="frameNumber"></param>
		/// <returns>The mean length of the individual TOF scans, as found by the TDC</returns>
		public double ReadMeanTOFScan(int frameNumber)
		{
			try 
			{
				string line;
				using (StreamReader sr = new StreamReader(this._experiment.ScanTimeFile(frameNumber)) )
				{
					line = sr.ReadLine();
				}
				string[] tokens = line.Split(new char[]{' '});
				return Convert.ToDouble(tokens[0]);
			}
			catch (Exception e) 
			{
				// Let the user know what went wrong.
				Console.WriteLine("The file could not be read:");
				Console.WriteLine(e.Message);
			}

			return -1.0;
		}

		#region Member Functions

		protected virtual void ReadIMFHeader( BinaryReader r, 
									out int dataStartByte,
									out int [] spectraStartByte, 
									out int [] spectraNumRecs,
									out int [] spectraTIC
								  )
		{
			// Read past the ESC position
			dataStartByte = GetESCPosition(r) + 1;
			
			// Because of GetESCPosition(), stream should be at loc of the IMF header start
			int numSpectra = r.ReadInt32();
			int [] spectraLens = new int[numSpectra];
			spectraStartByte = new int[numSpectra];
			spectraNumRecs = new int[numSpectra];
			spectraTIC = new int[numSpectra];

			for(int i=0; i<numSpectra; i++)
			{
				spectraTIC[i] = r.ReadInt32();
				spectraNumRecs[i] = r.ReadInt32() / RECORD_SIZE; 
			}
			
			spectraStartByte[0] = 0;
			for(int i=1; i<numSpectra; i++)
				spectraStartByte[i] = spectraStartByte[i-1] + (spectraNumRecs[i-1] * RECORD_SIZE);
		}

		// Read all of the frame data.
		protected virtual void ReadFrameData( BinaryReader r, 
									int [] spectraByteLoc, 
									int [] spectraNumRecs,
									int [] spectraTIC,
									out int [] spectraStartIndex,
									out int [] TOFValues,
									out short [] intensities
								   )
		{			
			int bytesToRead;
			int numRecs;
			byte [] binData;

			bytesToRead = spectraByteLoc[spectraByteLoc.Length-1] + 
						(spectraNumRecs[spectraNumRecs.Length-1]* RECORD_SIZE) -
						spectraByteLoc[0];
			numRecs = bytesToRead / RECORD_SIZE;

			// Should already be in the correct position since ReadIMFHeader was prev. called.
			binData = r.ReadBytes(bytesToRead);
			
			MemoryStream ms = new MemoryStream(binData,0,binData.Length);

			// New binary reader to read the memory stream
			BinaryReader br = new BinaryReader(ms);

			// Prepare arrays for data
			TOFValues = new int[numRecs];
			intensities = new short[numRecs];

			for(int i=0; i<numRecs; i++)
			{
				TOFValues[i] = br.ReadInt32();
				intensities[i] = br.ReadInt16();
			}

			spectraStartIndex = new int[spectraByteLoc.Length];
			for(int i=0; i<spectraByteLoc.Length; i++)
				spectraStartIndex[i] = (spectraByteLoc[i] - spectraByteLoc[0]) / RECORD_SIZE;

			br.Close();
			ms.Close();
		}
		// Return the byte position of the ESC char, which delineates the ICR-2LS header
		// from the binary data
		protected int GetESCPosition(BinaryReader r)
		{
			byte b;
			int pos = 0;
			while((b = r.ReadByte()) != 0x1b) // Looking for ESC byte
				pos++;
			return pos;
		}

		#endregion

		#region Data Members
		protected CExperiment _experiment;
		
		#endregion

		#region CONSTANT
		protected const int RECORD_SIZE = 6;
		#endregion
	}
}













