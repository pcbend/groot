#ifndef G_GUI_HISTORY_H
#define G_GUI_HISTORY_H

#include <iosfwd>
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

void Record(const std::string& action,
            const std::vector<Field>& fields = {});
void Record(const std::string& action,
            const GInteractionInfo& info,
            const std::vector<Field>& fields = {});

std::string FormatLine(const std::string& action,
                       const GInteractionInfo* info,
                       const std::vector<Field>& fields);

} // namespace GGuiHistory

#endif
