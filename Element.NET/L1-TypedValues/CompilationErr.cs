using Element.AST;

namespace Element
{
    /// <summary>
    /// A value that results from a failure during compilation. It will be accepted everywhere and generate no further
    /// errors, returning itself from each operation (the error is non-recoverable).
    /// </summary>
    public sealed class CompilationErr : IValue, IFunction
    {
        public static CompilationErr Instance { get; } = new CompilationErr();
        private CompilationErr() { }
        public override string ToString() => "<error>";
        public bool CanBeCached => true;

        PortInfo[] IFunction.Inputs => null;
        PortInfo[] IFunction.Outputs => null;
        IFunction IFunction.CallInternal(IFunction[] arguments, string output, CompilationContext context) => this;
    }
}