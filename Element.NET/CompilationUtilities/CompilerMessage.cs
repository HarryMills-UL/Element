using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Newtonsoft.Json;
using Tomlyn;
using Tomlyn.Model;

namespace Element
{
    public readonly struct CompilerMessage
    {
        private static readonly TomlTable MessageToml = Toml.Parse(File.ReadAllText("Messages.toml")).ToModel();

        private static TomlTable GetMessageToml(int messageCode) =>
            MessageToml[$"ELE{messageCode}"] is TomlTable messageTable
                ? messageTable
                : throw new InternalCompilerException($"ELE{messageCode} could not be found");

        public static string GetMessageName(int messageCode) => (string) GetMessageToml(messageCode)["name"];

        public static MessageLevel GetMessageLevel(int messageCode) =>
            Enum.TryParse((string) GetMessageToml(messageCode)["level"], out MessageLevel level)
                ? level
                : throw new InternalCompilerException($"\"{level}\" is not a valid message level");

        public CompilerMessage(string message, MessageLevel? messageLevel = null) : this(null, messageLevel, message, null){}

        [JsonConstructor]
        public CompilerMessage(int? messageCode, MessageLevel? messageLevel, string? context, IReadOnlyCollection<TraceSite>? callStack)
        {
            MessageCode = messageCode;
            MessageLevel = messageCode.HasValue ? GetMessageLevel(messageCode.Value) : messageLevel;
            Context = context;
            CallStack = callStack ?? Array.Empty<TraceSite>();

            var builder = new StringBuilder();
            if (messageCode.HasValue)
            {
                builder.Append("ELE").Append(MessageCode).Append(": ").Append(MessageLevel).Append(" - ").AppendLine(GetMessageName(messageCode.Value));
            }

            builder.Append(Context);
            if (CallStack.Count > 0)
            {
                builder.AppendLine();
                builder.AppendLine("Element source trace:");
                foreach (var site in CallStack)
                {
                    builder.Append("    ").AppendLine(site.ToString());
                }

                builder.AppendLine();
            }

            _message = builder.ToString();
        }
        private readonly string _message;

        public int? MessageCode { get; }
        public MessageLevel? MessageLevel { get; }
        public string? Context { get; }
        public IReadOnlyCollection<TraceSite> CallStack { get; }

        public override string ToString() => _message;
    }
}