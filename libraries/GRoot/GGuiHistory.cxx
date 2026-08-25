#include <GGuiHistory.h>

#include <GCommands.h>
#include <GH1D.h>

#include <TDirectory.h>
#include <TEnv.h>
#include <TFile.h>
#include <TH1.h>
#include <TObject.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

bool gEnabled = true;
bool gInitialized = false;
std::string gPath;

struct ObjectInfo {
  std::string name;
  std::string className;
  std::string file;
  std::string rootPath;
};

struct EventInfo {
  int rootEvent = 0;
  int keyCode = 0;
  std::string key;
  std::string selected;
};

struct RangeInfo {
  bool hasXLow = false;
  bool hasXHigh = false;
  bool hasYLow = false;
  bool hasYHigh = false;
  double xlow = 0.0;
  double xhigh = 0.0;
  double ylow = 0.0;
  double yhigh = 0.0;
};

struct ProjectionInfo {
  std::string axis;
  std::string scope;
  std::string output;
  bool hasGateLow = false;
  bool hasGateHigh = false;
  bool hasBgLow = false;
  bool hasBgHigh = false;
  double gateLow = 0.0;
  double gateHigh = 0.0;
  double bgLow = 0.0;
  double bgHigh = 0.0;
};

struct FitInfo {
  std::string model;
  std::string function;
  bool hasCentroid = false;
  bool hasArea = false;
  bool hasFwhm = false;
  bool hasChi2Ndf = false;
  double centroid = 0.0;
  double area = 0.0;
  double fwhm = 0.0;
  double chi2Ndf = 0.0;
};

struct MarkerInfo {
  std::string type;
  bool hasX = false;
  bool hasY = false;
  double x = 0.0;
  double y = 0.0;
};

struct DisplayInfo {
  bool hasLogX = false;
  bool hasLogY = false;
  bool hasLogZ = false;
  bool hasResiduals = false;
  bool logX = false;
  bool logY = false;
  bool logZ = false;
  bool residuals = false;
};

struct HistoryRecord {
  std::string time;
  std::string action;
  EventInfo event;
  ObjectInfo object;
  ObjectInfo parent;
  RangeInfo range;
  ProjectionInfo projection;
  FitInfo fit;
  MarkerInfo marker;
  DisplayInfo display;
  std::map<std::string,std::string> extra;
};

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
  return gEnv ? gEnv->GetValue("Gui.HistSize", 10000) : 10000;
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

std::string KeyName(int keyCode) {
  if(keyCode == 0)
    return "";
  if(keyCode >= 32 && keyCode <= 126)
    return std::string(1, static_cast<char>(keyCode));
  return "";
}

bool ToDouble(const std::string& value,double& output) {
  char* end = nullptr;
  output = std::strtod(value.c_str(),&end);
  return end && *end == '\0' && std::isfinite(output);
}

bool ToBool(const std::string& value,bool& output) {
  std::string lower = value;
  std::transform(lower.begin(),lower.end(),lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if(lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
    output = true;
    return true;
  }
  if(lower == "false" || lower == "0" || lower == "no" || lower == "off") {
    output = false;
    return true;
  }
  return false;
}

std::string JsonEscape(const std::string& value) {
  std::ostringstream out;
  for(char ch : value) {
    switch(ch) {
      case '\\': out << "\\\\"; break;
      case '"':  out << "\\\""; break;
      case '\b': out << "\\b"; break;
      case '\f': out << "\\f"; break;
      case '\n': out << "\\n"; break;
      case '\r': out << "\\r"; break;
      case '\t': out << "\\t"; break;
      default:
        if(static_cast<unsigned char>(ch) < 0x20) {
          out << "\\u"
              << std::hex << std::setw(4) << std::setfill('0')
              << static_cast<int>(static_cast<unsigned char>(ch))
              << std::dec << std::setfill(' ');
        } else {
          out << ch;
        }
        break;
    }
  }
  return out.str();
}

void StringValue(std::ostringstream& out,const std::string& value) {
  if(value.empty())
    out << "null";
  else
    out << '"' << JsonEscape(value) << '"';
}

void NumberValue(std::ostringstream& out,bool hasValue,double value) {
  if(!hasValue || !std::isfinite(value)) {
    out << "null";
    return;
  }
  out << std::setprecision(16) << value;
}

void BoolValue(std::ostringstream& out,bool hasValue,bool value) {
  if(!hasValue)
    out << "null";
  else
    out << (value ? "true" : "false");
}

void AppendStringField(std::ostringstream& out,const char* key,const std::string& value,bool comma = true) {
  if(comma) out << ',';
  out << '"' << key << "\":";
  StringValue(out,value);
}

void AppendNumberField(std::ostringstream& out,const char* key,bool hasValue,double value,bool comma = true) {
  if(comma) out << ',';
  out << '"' << key << "\":";
  NumberValue(out,hasValue,value);
}

void AppendIntField(std::ostringstream& out,const char* key,int value,bool comma = true) {
  if(comma) out << ',';
  out << '"' << key << "\":";
  if(value)
    out << value;
  else
    out << "null";
}

void AppendBoolField(std::ostringstream& out,const char* key,bool hasValue,bool value,bool comma = true) {
  if(comma) out << ',';
  out << '"' << key << "\":";
  BoolValue(out,hasValue,value);
}

ObjectInfo BuildObjectInfo(TObject* object,const std::string& fallbackName = "") {
  ObjectInfo info;
  if(!object && fallbackName.empty())
    return info;

  info.name = object ? object->GetName() : fallbackName;
  info.className = object ? object->ClassName() : "";

  auto* hist = dynamic_cast<TH1*>(object);
  TDirectory* directory = hist ? hist->GetDirectory() : nullptr;
  if(directory) {
    if(TFile* file = directory->GetFile())
      info.file = file->GetName();
    info.rootPath = directory->GetPath();
    if(!info.rootPath.empty() && !info.name.empty()) {
      if(info.rootPath.back() != '/')
        info.rootPath += '/';
      info.rootPath += info.name;
    }
  }

  return info;
}

void FillActionDefaults(HistoryRecord& record) {
  if(record.action == "fit.gaus")
    record.fit.model = "gaus";
  else if(record.action == "fit.photopeak")
    record.fit.model = "photopeak";
  else if(record.action == "fit.double_gaus")
    record.fit.model = "double_gaus";
}

void ApplyField(HistoryRecord& record,const std::string& key,const std::string& value) {
  double number = 0.0;
  bool boolean = false;

  if(key == "hist") {
    if(record.object.name.empty())
      record.object.name = value;
  } else if(key == "x" && ToDouble(value,number)) {
    record.marker.hasX = true;
    record.marker.x = number;
  } else if(key == "y" && ToDouble(value,number)) {
    record.marker.hasY = true;
    record.marker.y = number;
  } else if(key == "type") {
    record.marker.type = value;
  } else if(key == "xlow" && ToDouble(value,number)) {
    record.range.hasXLow = true;
    record.range.xlow = number;
  } else if(key == "xhigh" && ToDouble(value,number)) {
    record.range.hasXHigh = true;
    record.range.xhigh = number;
  } else if(key == "ylow" && ToDouble(value,number)) {
    record.range.hasYLow = true;
    record.range.ylow = number;
  } else if(key == "yhigh" && ToDouble(value,number)) {
    record.range.hasYHigh = true;
    record.range.yhigh = number;
  } else if(key == "gate_low" && ToDouble(value,number)) {
    record.projection.hasGateLow = true;
    record.projection.gateLow = number;
  } else if(key == "gate_high" && ToDouble(value,number)) {
    record.projection.hasGateHigh = true;
    record.projection.gateHigh = number;
  } else if(key == "bg_low" && ToDouble(value,number)) {
    record.projection.hasBgLow = true;
    record.projection.bgLow = number;
  } else if(key == "bg_high" && ToDouble(value,number)) {
    record.projection.hasBgHigh = true;
    record.projection.bgHigh = number;
  } else if(key == "axis") {
    record.projection.axis = value;
  } else if(key == "scope") {
    record.projection.scope = value;
  } else if(key == "projection") {
    record.projection.output = value;
  } else if(key == "parent") {
    if(record.parent.name.empty())
      record.parent.name = value;
  } else if(key == "model") {
    record.fit.model = value;
  } else if(key == "function") {
    record.fit.function = value;
  } else if(key == "centroid" && ToDouble(value,number)) {
    record.fit.hasCentroid = true;
    record.fit.centroid = number;
  } else if(key == "area" && ToDouble(value,number)) {
    record.fit.hasArea = true;
    record.fit.area = number;
  } else if(key == "fwhm" && ToDouble(value,number)) {
    record.fit.hasFwhm = true;
    record.fit.fwhm = number;
  } else if(key == "chi2_ndf" && ToDouble(value,number)) {
    record.fit.hasChi2Ndf = true;
    record.fit.chi2Ndf = number;
  } else if(key == "logx" && ToBool(value,boolean)) {
    record.display.hasLogX = true;
    record.display.logX = boolean;
  } else if(key == "logy" && ToBool(value,boolean)) {
    record.display.hasLogY = true;
    record.display.logY = boolean;
  } else if(key == "logz" && ToBool(value,boolean)) {
    record.display.hasLogZ = true;
    record.display.logZ = boolean;
  } else if(key == "enabled" && record.action == "residuals.toggle" && ToBool(value,boolean)) {
    record.display.hasResiduals = true;
    record.display.residuals = boolean;
  } else {
    record.extra[key] = value;
  }
}

HistoryRecord BuildRecord(const std::string& action,
                          const GInteractionInfo* info,
                          const std::vector<GGuiHistory::Field>& fields) {
  HistoryRecord record;
  record.time = Timestamp();
  record.action = action;

  if(info) {
    record.event.rootEvent = info->event;
    record.event.keyCode = info->py;
    record.event.key = KeyName(info->py);
    record.event.selected = info->selectedName;
    record.object = BuildObjectInfo(info->target,info->targetName);

    if(auto* gh1 = dynamic_cast<GH1D*>(info->target))
      record.parent = BuildObjectInfo(gh1->GetParent());
  }

  FillActionDefaults(record);
  for(const auto& field : fields)
    ApplyField(record,field.first,field.second);

  return record;
}

void WriteObject(std::ostringstream& out,const ObjectInfo& object) {
  out << '{';
  AppendStringField(out,"name",object.name,false);
  AppendStringField(out,"class",object.className);
  AppendStringField(out,"file",object.file);
  AppendStringField(out,"root_path",object.rootPath);
  out << '}';
}

void WriteEvent(std::ostringstream& out,const EventInfo& event) {
  out << '{';
  AppendIntField(out,"root_event",event.rootEvent,false);
  AppendIntField(out,"key_code",event.keyCode);
  AppendStringField(out,"key",event.key);
  AppendStringField(out,"selected",event.selected);
  out << '}';
}

void WriteRange(std::ostringstream& out,const RangeInfo& range) {
  out << '{';
  AppendNumberField(out,"xlow",range.hasXLow,range.xlow,false);
  AppendNumberField(out,"xhigh",range.hasXHigh,range.xhigh);
  AppendNumberField(out,"ylow",range.hasYLow,range.ylow);
  AppendNumberField(out,"yhigh",range.hasYHigh,range.yhigh);
  out << '}';
}

void WriteProjection(std::ostringstream& out,const ProjectionInfo& projection) {
  out << '{';
  AppendStringField(out,"axis",projection.axis,false);
  AppendStringField(out,"scope",projection.scope);
  AppendStringField(out,"output",projection.output);
  AppendNumberField(out,"gate_low",projection.hasGateLow,projection.gateLow);
  AppendNumberField(out,"gate_high",projection.hasGateHigh,projection.gateHigh);
  AppendNumberField(out,"bg_low",projection.hasBgLow,projection.bgLow);
  AppendNumberField(out,"bg_high",projection.hasBgHigh,projection.bgHigh);
  out << '}';
}

void WriteFit(std::ostringstream& out,const FitInfo& fit) {
  out << '{';
  AppendStringField(out,"model",fit.model,false);
  AppendStringField(out,"function",fit.function);
  AppendNumberField(out,"centroid",fit.hasCentroid,fit.centroid);
  AppendNumberField(out,"area",fit.hasArea,fit.area);
  AppendNumberField(out,"fwhm",fit.hasFwhm,fit.fwhm);
  AppendNumberField(out,"chi2_ndf",fit.hasChi2Ndf,fit.chi2Ndf);
  out << '}';
}

void WriteMarker(std::ostringstream& out,const MarkerInfo& marker) {
  out << '{';
  AppendStringField(out,"type",marker.type,false);
  AppendNumberField(out,"x",marker.hasX,marker.x);
  AppendNumberField(out,"y",marker.hasY,marker.y);
  out << '}';
}

void WriteDisplay(std::ostringstream& out,const DisplayInfo& display) {
  out << '{';
  AppendBoolField(out,"logx",display.hasLogX,display.logX,false);
  AppendBoolField(out,"logy",display.hasLogY,display.logY);
  AppendBoolField(out,"logz",display.hasLogZ,display.logZ);
  AppendBoolField(out,"residuals",display.hasResiduals,display.residuals);
  out << '}';
}

void WriteExtra(std::ostringstream& out,const std::map<std::string,std::string>& extra) {
  out << '{';
  bool first = true;
  for(const auto& field : extra) {
    if(!first)
      out << ',';
    first = false;
    out << '"' << JsonEscape(field.first) << "\":";
    StringValue(out,field.second);
  }
  out << '}';
}

std::string ToJsonLine(const HistoryRecord& record) {
  std::ostringstream out;
  out << '{';
  out << "\"time\":";
  StringValue(out,record.time);
  AppendStringField(out,"action",record.action);

  out << ",\"event\":";
  WriteEvent(out,record.event);
  out << ",\"object\":";
  WriteObject(out,record.object);
  out << ",\"parent\":";
  WriteObject(out,record.parent);
  out << ",\"range\":";
  WriteRange(out,record.range);
  out << ",\"projection\":";
  WriteProjection(out,record.projection);
  out << ",\"fit\":";
  WriteFit(out,record.fit);
  out << ",\"marker\":";
  WriteMarker(out,record.marker);
  out << ",\"display\":";
  WriteDisplay(out,record.display);
  out << ",\"extra\":";
  WriteExtra(out,record.extra);
  out << "}\n";
  return out.str();
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

void AppendLine(const std::string& line) {
  const std::string path = GGuiHistory::Path();
  std::ofstream output(path,std::ios::app);
  if(!output)
    return;
  output << line;
  output.close();
  TrimHistoryFile(path);
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
  std::ofstream output(Path(),std::ios::trunc);
}

std::string FormatLine(const std::string& action,
                       const GInteractionInfo* info,
                       const std::vector<Field>& fields) {
  return ToJsonLine(BuildRecord(action,info,fields));
}

void Record(const std::string& action,const std::vector<Field>& fields) {
  if(!IsEnabled())
    return;
  AppendLine(FormatLine(action,nullptr,fields));
}

void Record(const std::string& action,
            const GInteractionInfo& info,
            const std::vector<Field>& fields) {
  if(!IsEnabled())
    return;
  AppendLine(FormatLine(action,&info,fields));
}

} // namespace GGuiHistory
