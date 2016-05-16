#if !Disable_Obsolete
using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using DeconEngine;
using Engine.TheoreticalProfile;

namespace DeconToolsV2
{
    [Obsolete("Usage unknown - no path to usage exists within the DeconTools solution")]
    public class clsMercuryIsotopeDistribution
    {
        private int mChargeState;
        private readonly MercuryIsotopeDistribution mMercuryIsotopeDistribution;

        public clsMercuryIsotopeDistribution()
        {
            mMercuryIsotopeDistribution = new MercuryIsotopeDistribution();
            MercurySize = 8192;
            ChargeCarrierMass = 1.007824659;
            ApodizationType = ApodizationType.Gaussian;
            Resolution = 100000;
            ChargeState = 1;
        }

        /// <summary>
        ///     Gets or Sets the mercury size. Only accepts multiples of 2.
        /// </summary>
        public int MercurySize
        {
            get { return mMercuryIsotopeDistribution.MercurySize; }
            set
            {
                if (value <= 0)
                {
                    throw new ArgumentOutOfRangeException("value", value,
                        "Mercury size must be greater than 0");
                }

                var goodSize = false;
                for (var i = 1; i <= 536870912; i *= 2)
                {
                    if (value == i)
                    {
                        goodSize = true;
                        break;
                    }
                }
                if (!goodSize)
                {
                    throw new ArgumentException("MercurySize must be a power of two.", "value");
                }

                mMercuryIsotopeDistribution.MercurySize = value;
            }
        }

        public double ChargeCarrierMass
        {
            get { return mMercuryIsotopeDistribution.ChargeCarrierMass; }
            set { mMercuryIsotopeDistribution.ChargeCarrierMass = value; }
        }

        public ApodizationType ApodizationType
        {
            get { return mMercuryIsotopeDistribution.ApType; }
            set { mMercuryIsotopeDistribution.ApType = value; }
        }

        public double Resolution { get; set; }

        /// <summary>
        ///     Gets and sets charge state property
        /// </summary>
        public short ChargeState
        {
            get { return (short) mChargeState; }

            set { mChargeState = value; }
        }

        /// <summary>
        ///     Gets average molecular weight of the last call to CalculateDistribution
        /// </summary>
        public double AverageMolecularMass
        {
            get { return mMercuryIsotopeDistribution.AverageMw; }
        }

        /// <summary>
        ///     Gets the monoisotopic molecular weight of the last call to CalculateDistribution
        /// </summary>
        public double MonoMolecularMass
        {
            get { return mMercuryIsotopeDistribution.MonoMw; }
        }

        /// <summary>
        ///     Gets the most abundant MZ of the last call to CalculateDistribution
        /// </summary>
        public double MostAbundantMZ
        {
            get { return mMercuryIsotopeDistribution.MaxPeakMz; }
        }

        /// <summary>
        ///     Gets the mass variance of the last call to CalculateDistribution
        /// </summary>
        public double MassVariance
        {
            get { return mMercuryIsotopeDistribution.MassVariance; }
        }

        public clsElementIsotopes ElementIsotopes
        {
            set
            {
                mMercuryIsotopeDistribution.SetElementalIsotopeComposition(
                    value);
            }
        }

        /// <summary>
        ///     Call the mMercuryIsotopeDistribution method, passing in the currently set resolution and other settings.  Copy the
        ///     result into an array of PointF.
        /// </summary>
        /// <param name="elementCounts"></param>
        /// <returns></returns>
        public PointF[] CalculateDistribution(Hashtable elementCounts)
        {
            List<double> x;
            List<double> y;
            List<double> xIsotope;
            List<double> yIsotope;
            PointF[] points;

            MolecularFormula formula;
            Utils.ConvertElementTableToFormula(
                ref mMercuryIsotopeDistribution.ElementalIsotopeComposition, elementCounts, out formula);

            mMercuryIsotopeDistribution.CalculateDistribution((short) mChargeState, Resolution, formula, out x, out y, 0,
                out xIsotope, out yIsotope, false);

            var numPts = x.Count;
            points = new PointF[numPts];

            for (var i = 0; i < numPts; i++)
            {
                points[i] = new PointF((float) x[i], (float) y[i]);
            }

            return points;
        }

        public void CalculateMasses(Hashtable elementCounts)
        {
            MolecularFormula formula;
            Utils.ConvertElementTableToFormula(
                ref mMercuryIsotopeDistribution.ElementalIsotopeComposition, elementCounts, out formula);

            mMercuryIsotopeDistribution.CalculateMasses(formula);
        }
    }
}

#endif