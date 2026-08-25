#ifndef G_GUI_HISTORY_H
#define G_GUI_HISTORY_H

#include <iosfwd>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

struct GInteractionInfo;

namespace GGuiHistory {

using Field = std::pair<std::string, std::string>;

bool IsEnabled();
void SetEnabled(bool enabled);

std::string Path();
void SetPath(const std::string& path);
void Clear();
std::vector<std::string> Recent(std::size_t count = 20,
                                const std::string& actionFilter = "");
std::vector<std::string> Summary(std::size_t count = 20,
                                 const std::string& actionFilter = "");
void Print(std::size_t count = 20,const std::string& actionFilter = "");
void Print(std::ostream& out,
           std::size_t count = 20,
           const std::string& actionFilter = "");

void Record(const std::string& action,
            const std::vector<Field>& fields = {});
void Record(const std::string& action,
            const GInteractionInfo& info,
            const std::vector<Field>& fields = {});

std::string FormatLine(const std::string& action,
                       const GInteractionInfo* info,
                       const std::vector<Field>& fields);

} // namespace GGuiHistory

void gui_history(int count = 20,const char* actionFilter = "");
void gui_history(const char* actionFilter);

#endif
