using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib
{
    public class Logger
    {
        public string Type { get; set; }
        public ConsoleColor Color { get; set; }
        public bool Close { get; set; }

        public Logger(string Type, ConsoleColor Color, bool Close = false)
        {
            this.Type = Type;
            this.Color = Color;
            this.Close = Close;
        }

        public void Log(string Text, bool NewLine = true)
        {
            PrintPrefix(NewLine);

            Console.Write(Text);

            if (NewLine)
                Console.WriteLine();
            if (Close)
            {
                Console.WriteLine("Press any key to exit...");
                if (!CLI.IsArgumentsOnly)
                {
                    Console.ReadKey();
                }
            }
        }

        private void PrintPrefix(bool NewLine)
        {
            ConsoleColor prev = Console.ForegroundColor;
            if (NewLine)
                Console.WriteLine();
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("[");
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.Write("gtree");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.Write(":");
            Console.ForegroundColor = Color;
            Console.Write(Type);
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("]");
            Console.ForegroundColor = prev;
            Console.Write(" ");
        }
    }
}
