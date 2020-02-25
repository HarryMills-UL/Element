using System;
using System.Collections.Generic;

namespace Element.AST
{
    public abstract class DeclaredScope : ScopeBase
    {
        protected abstract IEnumerable<DeclaredItem> ItemsToCacheOnValidate { get; }
        public DeclaredItem Declarer { get; private set; }
        public override string Location => Declarer.Location;

        public void Initialize(DeclaredScope parent, DeclaredItem declarer)
        {
            if (!(this is GlobalScope)) // Global scope has no parent or declarer!
            {
                Parent = parent ?? throw new ArgumentNullException(nameof(parent));
                Declarer = declarer ?? throw new ArgumentNullException(nameof(declarer));
            }

            foreach (var item in ItemsToCacheOnValidate)
            {
                item.Initialize(this);
            }
        }

        public bool ValidateScope(CompilationContext compilationContext, List<Identifier> identifierWhitelist = null)
        {
            if (compilationContext.Input.SkipValidation) return true;
            
            var success = true;

            foreach (var item in ItemsToCacheOnValidate)
            {
                if (Contains(item.Identifier))
                {
                    compilationContext.LogError(2, $"Cannot add duplicate identifier '{item.Identifier}'");
                    success = false;
                }
                else
                {
                    Set(item.Identifier, item);
                }

                if (!compilationContext.ValidateIdentifier(item.Identifier, identifierWhitelist))
                {
                    success = false;
                }

                if (!item.Validate(compilationContext))
                {
                    success = false;
                }
            }

            return success;
        }
    }
}