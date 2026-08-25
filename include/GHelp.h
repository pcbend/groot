#ifndef G_HELP_H
#define G_HELP_H

#include <iosfwd>
#include <string>

namespace GHelp {

bool IsHelpCommand(const std::string& line);
void Print(const std::string& line, std::ostream& out);
void Print(const std::string& line);

} // namespace GHelp

#endif
