using System.Collections.Generic;

namespace Element.AST
{
    public class CustomFunctionDeclaration : FunctionDeclaration
    {
        protected override string IntrinsicQualifier => string.Empty;
        protected override Identifier[] ScopeIdentifierWhitelist { get; } = {Parser.ReturnIdentifier};


        public override IReadOnlyList<Port> Inputs => DeclaredInputs;
        public override Port Output => DeclaredOutput;
        protected override void AdditionalValidation(ResultBuilder builder)
        {
            if (Body is Terminal)
            {
                builder.Append(MessageCode.MissingFunctionBody, $"Non intrinsic function '{Location}' must have a body");
            }
        }
        
        public override Result<IValue> Call(IReadOnlyList<IValue> arguments, CompilationContext context)
        {
            return FunctionHelpers.ApplyFunction(this, arguments, Child!.Declarer.Parent, , false, context);
        }
    }
}