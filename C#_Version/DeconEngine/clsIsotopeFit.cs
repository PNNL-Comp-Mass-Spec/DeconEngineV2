#if Enable_Obsolete
using System;
using System.Collections.Generic;
using System.Linq;
using DeconToolsV2.Peaks;
using Engine.PeakProcessing;
using Engine.TheoreticalProfile;

namespace DeconToolsV2
{
    [Obsolete("Only used by Decon2LS.UI", false)]
    public class clsIsotopeFit
    {
        private enmIsotopeFitType menmFitType;
        private Engine.HornTransform.IsotopicProfileFitScorer mobj_fit;

        public void SetOptions(string averagine_mf, string tag_mf,
            double cc_mass, bool thrash_or_not, bool complete_fit)
        {
            string averagine_formula = "";
            string tag_formula = "";

            if (averagine_mf != null)
            {
                averagine_formula = averagine_mf;
            }
            if (tag_mf != null)
            {
                tag_formula = tag_mf;
            }
            mobj_fit.SetOptions(averagine_formula, tag_formula, cc_mass, thrash_or_not, complete_fit);
        }

        public enmIsotopeFitType IsotopeFitType
        {
            get { return menmFitType; }
            set
            {
                menmFitType = value;
                Engine.HornTransform.IsotopicProfileFitScorer new_fit;
                switch (menmFitType)
                {
                    case enmIsotopeFitType.PEAK:
                        new_fit = new Engine.HornTransform.PeakFitScorer();
                        break;
                    case enmIsotopeFitType.AREA:
                        new_fit = new Engine.HornTransform.AreaFitScorer();
                        break;
                    case enmIsotopeFitType.CHISQ:
                        new_fit = new Engine.HornTransform.ChiSqFitScorer();
                        break;
                    default:
                        return;
                        break;
                }

                if (mobj_fit != null)
                {
                    new_fit.CloneSettings(mobj_fit);
                }
                mobj_fit = new_fit;
            }
        }

        public clsIsotopeFit()
        {
            IsotopeFitType = enmIsotopeFitType.AREA;
        }

        public double GetFitScore(ref float[] mzs, ref float[] intensities, ref DeconToolsV2.Peaks.clsPeak[] peaks,
            short charge, int peak_index, double delete_intensity_threshold, double min_intensity_for_score,
            System.Collections.Hashtable elementCounts)
        {
            List<double> vectMzs = new List<double>();
            List<double> vectIntensities = new List<double>();
            int numPoints = mzs.Length;

            if (mzs.Length == 0)
            {
                throw new System.Exception("No data provided for the observed spectrum to match to");
            }

            // mzs should be in sorted order
            double minMZ = mzs[0];
            double maxMZ = mzs[numPoints - 1];
            vectMzs = new List<double>(mzs.Select(x => (double) x));
            vectIntensities = new List<double>(intensities.Select(x => (double) x));

            Engine.PeakProcessing.PeakData peakData = new PeakData();
            clsPeak currentPeak;

            peakData.SetPeaks(peaks);
            peakData.MzList = vectMzs;
            peakData.IntensityList = vectIntensities;
            peakData.GetPeak(peak_index, out currentPeak);

            MolecularFormula formula = new MolecularFormula();
            DeconEngine.Utils.ConvertElementTableToFormula(
                ref mobj_fit.IsotopeDistribution.ElementalIsotopeComposition, elementCounts, out formula);

            return mobj_fit.GetFitScore(peakData, charge, currentPeak, formula, delete_intensity_threshold,
                min_intensity_for_score, false);
        }
    }
}
#endif