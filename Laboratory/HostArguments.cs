using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Element;
using Newtonsoft.Json;
using NUnit.Framework;

namespace Laboratory
{
    /// <summary>
    /// Implements hosts types and enumerates hosts for host fixtures
    /// </summary>
    internal class HostArguments
    {
        public static IHost MakeHost() => _processHostInfo != null
                                            ? (IHost)new ProcessHost(_processHostInfo.Value)
                                            : new AtomicHost();
        
        static HostArguments()
        {
            static TValue Get<TValue>(string key) => TestContext.Parameters.Get<TValue>(key, default);
            var name = Get<string>("name");
            
            _processHostInfo = string.IsNullOrEmpty(name)
                                   ? (ProcessHostInfo?)null
                                   : new ProcessHostInfo(name,
                                                         Get<string>("build-command"),
                                                         Path.Combine(_elementRootDirectory.Value, Get<string>("executable-path")));
        }

        private static readonly Lazy<string> _elementRootDirectory = new Lazy<string>(() =>
        {
            var success = TryGetParent(new DirectoryInfo(Directory.GetCurrentDirectory()), "Element", out var rootDir);
            if (!success) throw new DirectoryNotFoundException("Couldn't find Element root directory");
            return rootDir.FullName;
        });

        private static bool TryGetParent(DirectoryInfo directory, string parentName, out DirectoryInfo parent)
        {
            parent = null;
            while (true)
            {
                if (directory.Parent == null) return false;
                if (string.Equals(directory.Parent.Name, parentName, StringComparison.OrdinalIgnoreCase))
                {
                    parent = directory.Parent;
                    return true;
                }

                directory = directory.Parent;
            }
        }

        private readonly struct ProcessHostInfo
        {
            public ProcessHostInfo(string name, string buildCommand, string executablePath)
            {
                Name = name;
                BuildCommand = buildCommand;
                ExecutablePath = executablePath;
            }

            public string Name { get; }
            public string BuildCommand { get; }
            public string ExecutablePath { get; }
        }

        private static readonly ProcessHostInfo? _processHostInfo;

        /// <summary>
        /// Implements commands by calling external process defined using a command string.
        /// </summary>
        private readonly struct ProcessHost : IHost
        {
            private static List<string> Run(Process process)
            {
                var messages = new List<string>();
                async Task ReadStream(Process proc, StreamReader streamReader)
                {
                    while (!proc.HasExited || !streamReader.EndOfStream)
                    {
                        var msg = await streamReader.ReadLineAsync();
                        if(!string.IsNullOrEmpty(msg))
                            messages.Add(msg);
                    }
                }

                process.StartInfo.RedirectStandardOutput = true;
                process.StartInfo.RedirectStandardError = true;
                process.Start();
                var readingStdOut = ReadStream(process, process.StandardOutput);
                var readingStdErr = ReadStream(process, process.StandardError);
                process.WaitForExit();
                Task.WhenAll(readingStdOut, readingStdErr).Wait();
                return messages;
            }

            static ProcessHost()
            {
                if (!_processHostInfo.HasValue) return;

                // Perform build command - within static constructor so it's only performed once per test run
                var info = _processHostInfo.Value;
                try
                {
                    var splitCommand = info.BuildCommand.Split(' ', 2);
                    var process = new Process
                    {
                        StartInfo = new ProcessStartInfo
                        {
                            FileName = splitCommand[0],
                            Arguments = splitCommand[1],
                            WorkingDirectory = _elementRootDirectory.Value
                        }
                    };

                    process.StartInfo.RedirectStandardError = true;
                    process.Start();
                    process.WaitForExit();

                    if (process.ExitCode != 0)
                    {
                        _hostBuildErrors.Add(info, process.StandardError.ReadToEnd());
                    }
                }
                catch (Exception e)
                {
                    _hostBuildErrors.Add(info, e.ToString());
                }
            }

            private static bool TryParseJson<T>(string json, out T result)
            {
                var success = true;
                var settings = new JsonSerializerSettings
                {
                    Error = (sender, args) => { success = false; args.ErrorContext.Handled = true; },
                    MissingMemberHandling = MissingMemberHandling.Error
                };

                try
                {
                    result = JsonConvert.DeserializeObject<T>(json, settings);
                }
                catch
                {
                    result = default;
                    success = false;
                }

                return success;
            }

            private static readonly Dictionary<ProcessHostInfo, string> _hostBuildErrors = new Dictionary<ProcessHostInfo, string>();

            public ProcessHost(ProcessHostInfo info) => _info = info;

            public override string ToString() => _info.Name;

            private readonly ProcessHostInfo _info;

            private string RunHostProcess(CompilationInput input, string arguments)
            {
                if (_hostBuildErrors.TryGetValue(_info, out var buildError))
                {
                    Assert.Fail(buildError);
                }

                var process = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = _info.ExecutablePath,
                        Arguments = arguments
                    }
                };

                var messages = Run(process);

                var compilerMessages = messages.Select(msg => TryParseJson(msg, out CompilerMessage compilerMessage)
                    ? compilerMessage
                    : new CompilerMessage(msg)).ToArray();

                if (process.ExitCode != 0)
                {
                    Assert.Fail(compilerMessages
                        .Aggregate(new StringBuilder($"{_info.Name} process quit with exit code '{process.ExitCode}'.").AppendLine().AppendLine($"Beginning of {_info.Name} output"),
                            (builder, s) => builder.Append("    ").AppendLine(s.ToString())).AppendLine($"End of {_info.Name} output").ToString());
                }
                else
                {
                    foreach (var msg in compilerMessages)
                    {
                        input.LogCallback?.Invoke(msg);
                    }
                }

                process.Close();

                return compilerMessages.Last().ToString();
            }

            private static StringBuilder BeginCommand(CompilationInput input, string command)
            {
                var processArgs = new StringBuilder();
                processArgs.Append($"{command} --logjson");
                if (input.ExcludePrelude) processArgs.Append(" --no-prelude ");
                if (input.Packages.Count > 0) processArgs.Append(" --packages ").AppendJoin(' ', input.Packages);
                if (input.ExtraSourceFiles.Count > 0) processArgs.Append(" --source-files ").AppendJoin(' ', input.ExtraSourceFiles);
                if (input.Debug) processArgs.Append(" --debug ");
                if (input.SkipValidation) processArgs.Append(" --no-validation ");
                return processArgs;
            }

            bool IHost.Parse(CompilationInput input) => bool.Parse(RunHostProcess(input, BeginCommand(input, "parse").ToString()));

            (bool Success, float[] Result) IHost.Evaluate(CompilationInput input, string expression)
            {
                var success = true;
                var result = RunHostProcess(input, BeginCommand(input, "evaluate").Append($" -e \"{expression}\"").ToString());
                var values = result.Split(' ', StringSplitOptions.RemoveEmptyEntries)
                    .Select(s =>
                    {
                        success &= float.TryParse(s, out var value);
                        return value;
                    })
                    .ToArray();
                return (success, values);
            }

            (bool Success, string Result) IHost.Typeof(CompilationInput input, string expression) =>
                (true, RunHostProcess(input, BeginCommand(input, "typeof").Append($" -e \"{expression}\"").ToString()));
        }
    }
}