using System;
using System.Runtime.CompilerServices;

namespace Sora
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    }
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main constructor!");
            NativeLog("Test log");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World from C#!");
        }

        public void PrintInt(int value)
        {
            Console.WriteLine($"C# says int: {value}");
        }

        public void PrintInts(int value1, int value2)
        {
            Console.WriteLine($"C# says int1: {value1} and int2: {value2}");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void NativeLog(string message);

    }

}