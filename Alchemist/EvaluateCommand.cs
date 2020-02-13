using Element;
using CommandLine;

namespace Alchemist
{
	[Verb("evaluate", HelpText = "Compile and evaluate an element expression, printing the results to standard out.")]
	internal class EvaluateCommand : BaseCommand
	{
		[Option('e', "expression", Required = true, HelpText = "Expression to evaluate.")]
		public string Expression { get; set; }

		protected override (int ExitCode, string Result) CommandImplementation(in CompilationInput input) =>
			(0, string.Join(", ", new AtomicHost().Evaluate(input, Expression))
				.Replace("∞", "Infinity"));
		// Windows terminal replaces ∞ with 8 - see here https://stackoverflow.com/questions/40907417/why-is-infinity-printed-as-8-in-the-windows-10-console
	}
}