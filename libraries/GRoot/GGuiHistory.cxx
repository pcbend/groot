#include <GGuiHistory.h>

#include <GCommands.h>

#include <TEnv.h>

#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace {

bool gEnabled = true;
bool gInitialized = false;
std::string gPath;

void InitializeFromEnv() {
  if(gInitialized)
    return;
  gInitialized = true;
  if(gEnv)
    gEnabled = gEnv->GetValue("GGuiHistory.Enabled", 1) != 0;
}

std::string DefaultPath() {
  const char* configured = gEnv ? gEnv->GetValue("GGuiHistory.File", ".gui_history")
                                : ".gui_history";
  return configured && configured[0] ? configured : ".gui_history";
}

int MaxLines() {
  const int maxLines = gEnv ? gEnv->GetValue("Gui.HistSize", 10000) : 10000;
  return maxLines;
}

void TrimHistoryFile(const std::string& path) {
  const int maxLines = MaxLines();
  if(maxLines <= 0)
    return;

  std::ifstream input(path);
  if(!input)
    return;

  std::vector<std::string> lines;
  std::string line;
  while(std::getline(input,line))
    lines.push_back(line);

  if(static_cast<int>(lines.size()) <= maxLines)
    return;

  std::ofstream output(path,std::ios::trunc);
  if(!output)
    return;

  const auto first = lines.end() - maxLines;
  for(auto iter = first; iter != lines.end(); ++iter)
    output << *iter << '\n';
}

std::string Timestamp() {
  std::time_t now = std::time(nullptr);
  std::tm localTime;
#if defined(_WIN32)
  localtime_s(&localTime, &now);
#else
  localtime_r(&now, &localTime);
#endif

  std::ostringstream out;
  out << std::put_time(&localTime, "%Y-%m-%dT%H:%M:%S");
  return out.str();
}

std::string Quote(const std::string& value) {
  std::string quoted = "\"";
  for(char ch : value) {
    if(ch == '\\' || ch == '"')
      quoted.push_back('\\');
    quoted.push_back(ch);
  }
  quoted.push_back('"');
  return quoted;
}

std::string Value(const std::string& value) {
  if(value.empty())
    return "\"\"";
  for(char ch : value) {
    if(std::isspace(static_cast<unsigned char>(ch)) || ch == '"' || ch == '\\')
      return Quote(value);
  }
  return value;
}

void AppendField(std::ostringstream& out, const std::string& key, const std::string& value) {
  out << ' ' << key << '=' << Value(value);
}

} // namespace

namespace GGuiHistory {

bool IsEnabled() {
  InitializeFromEnv();
  return gEnabled;
}

void SetEnabled(bool enabled) {
  gInitialized = true;
  gEnabled = enabled;
}

std::string Path() {
  InitializeFromEnv();
  if(gPath.empty())
    gPath = DefaultPath();
  return gPath;
}

void SetPath(const std::string& path) {
  InitializeFromEnv();
  gPath = path.empty() ? DefaultPath() : path;
}

void Clear() {
  std::ofstream output(Path(), std::ios::trunc);
}

std::string FormatLine(const std::string& action,
                       const GInteractionInfo* info,
                       const std::vector<Field>& fields) {
  std::ostringstream out;
  out << Timestamp();
  AppendField(out, "action", action);

  if(info) {
    if(!info->targetName.empty())
      AppendField(out, "target", info->targetName);
    if(!info->selectedName.empty())
      AppendField(out, "selected", info->selectedName);
    if(info->event)
      AppendField(out, "event", std::to_string(info->event));
    if(info->py)
      AppendField(out, "key", std::to_string(info->py));
  }

  for(const auto& field : fields)
    AppendField(out, field.first, field.second);

  out << '\n';
  return out.str();
}

void Record(const std::string& action, const std::vector<Field>& fields) {
  if(!IsEnabled())
    return;

  const std::string path = Path();
  std::ofstream output(path, std::ios::app);
  if(!output)
    return;
  output << FormatLine(action, nullptr, fields);
  output.close();
  TrimHistoryFile(path);
}

void Record(const std::string& action,
            const GInteractionInfo& info,
            const std::vector<Field>& fields) {
  if(!IsEnabled())
    return;

  const std::string path = Path();
  std::ofstream output(path, std::ios::app);
  if(!output)
    return;
  output << FormatLine(action, &info, fields);
  output.close();
  TrimHistoryFile(path);
}

} // namespace GGuiHistory
