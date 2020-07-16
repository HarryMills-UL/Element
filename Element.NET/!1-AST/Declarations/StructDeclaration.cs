using System;
using System.Linq;

namespace Element.AST
{
    public class IntrinsicStructDeclaration : Declaration
    {
        protected override string IntrinsicQualifier { get; } = "intrinsic";
        protected override string Qualifier { get; } = "struct";
        protected override Type[] BodyAlternatives { get; } = {typeof(StructBlock), typeof(Terminal)};
        protected override Result<IValue> ResolveImpl(IScope scope, CompilationContext context) =>
            IntrinsicImplementationCache.Get<IIntrinsicStructImplementation>(Identifier, context)
                          .Accumulate(() => PortList.ResolveInputConstraints(scope, context, true, true))
                          .Bind(t =>
                          {
                              var (structImpl, inputPorts) = t;
                              
                              Result<IValue> ToIntrinsicStructResult(ResolvedBlock? associatedBlock) => new Result<IValue>(new IntrinsicStruct(structImpl, inputPorts, associatedBlock, scope));
                              
                              return Body is StructBlock b
                                         ? b.ResolveBlock(scope, context).Bind(ToIntrinsicStructResult)
                                         : ToIntrinsicStructResult(null);
                          });
        
        protected override void ValidateDeclaration(ResultBuilder builder, CompilationContext context)
        {
            builder.Append(IntrinsicImplementationCache.Get<IIntrinsicStructImplementation>(Identifier, builder.Trace));
            if (ReturnConstraint != null)
            {
                builder.Append(MessageCode.StructCannotHaveReturnType, $"Struct '{context.CurrentDeclarationLocation}' cannot have declared return type");
            }
            
            PortList?.Validate(builder, context);
            if (Body is StructBlock block)
            {
                block.Validate(builder, context);
                block.ValidateIdentifiers(builder, Identifier);
            }

            if (PortList?.Ports.List.Any(port => !port.Identifier.HasValue) ?? false)
            {
                builder.Append(MessageCode.PortListCannotContainDiscards, $"Struct '{context.CurrentDeclarationLocation}' contains discards");
            }
        }
    }
    
    public class CustomStructDeclaration : Declaration
    {
        protected override string IntrinsicQualifier { get; } = string.Empty;
        protected override string Qualifier { get; } = "struct";
        protected override Type[] BodyAlternatives { get; } = {typeof(StructBlock), typeof(Terminal)};
        protected override Result<IValue> ResolveImpl(IScope scope, CompilationContext context) =>
            PortList.ResolveInputConstraints(scope, context, true, false)
                    .Bind(inputPorts =>
                    {
                        Result<IValue> ToCustomStruct(ResolvedBlock? associatedBlock) => new Result<IValue>(new CustomStruct(Identifier, inputPorts, associatedBlock, scope));
                              
                        return Body is StructBlock b
                                   ? b.ResolveBlock(scope, context).Bind(ToCustomStruct)
                                   : ToCustomStruct(null);
                    });
        
        protected override void ValidateDeclaration(ResultBuilder builder, CompilationContext context)
        {
            if (!(PortList?.Ports.List.Count > 0))
            {
                builder.Append(MessageCode.MissingPorts, $"Non intrinsic '{context.CurrentDeclarationLocation}' must have ports");
            }
            
            if (ReturnConstraint != null)
            {
                builder.Append(MessageCode.StructCannotHaveReturnType, $"Struct '{context.CurrentDeclarationLocation}' cannot have declared return type");
            }
            
            PortList?.Validate(builder, context);
            if (Body is StructBlock block)
            {
                block.Validate(builder, context);
                block.ValidateIdentifiers(builder, Identifier);
            }

            if (PortList?.Ports.List.Any(port => !port.Identifier.HasValue) ?? false)
            {
                builder.Append(MessageCode.PortListCannotContainDiscards, $"Struct '{context.CurrentDeclarationLocation}' contains discards");
            }
        }
    }
    
    public class StructBlock : DeclarationBlock
    {
        public void ValidateIdentifiers(ResultBuilder builder, Identifier structIdentifier)
        {
            foreach (var decl in Items ?? Enumerable.Empty<Declaration>())
            {
                decl.Identifier.Validate(builder,new[] {structIdentifier}, Array.Empty<Identifier>());
            }
        }
    }
}