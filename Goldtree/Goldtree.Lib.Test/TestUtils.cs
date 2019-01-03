using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Goldtree.Lib.Test
{
    [TestClass]
    public static class TestUtils
    {
        public static string GetResource(string name)
        {
            return Path.GetFullPath(Path.Combine("Resources", name));
        }
    }
}
