#if Enable_Obsolete && false
using System;

namespace Engine.Exception
{
    [Obsolete("Not used anywhere", true)]
    internal class InterpolationException : Exception
    {
        public InterpolationException(string mesg)
            : base(mesg)
        {
        }
    }
}
#endif