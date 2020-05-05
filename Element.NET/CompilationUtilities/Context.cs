using Element.AST;

namespace Element
{
    public abstract class Context
    {
        protected Context(CompilationInput compilationInput) => CompilationInput = compilationInput;

        protected internal CompilationInput CompilationInput { get; }
        public MessageLevel Verbosity => CompilationInput.Verbosity;
        public bool Debug => CompilationInput.Debug;
        public bool SkipValidation => CompilationInput.SkipValidation;

        public CompilationErr LogError(int? messageCode, string context = default)
        {
            
            var msg = MakeMessage(messageCode, context);
            if (!msg.MessageLevel.HasValue || msg.MessageLevel.Value >= CompilationInput.Verbosity)
            {
                CompilationInput.LogCallback?.Invoke(msg);
            }

            return CompilationErr.Instance;
        }

        public void Log(string message)
        {
            var msg = MakeMessage(null, message);
            if (!msg.MessageLevel.HasValue || msg.MessageLevel.Value >= CompilationInput.Verbosity)
            {
                CompilationInput.LogCallback?.Invoke(msg);
            }
        }
        
        protected abstract CompilerMessage MakeMessage(int? messageCode, string context = default);
    }
}