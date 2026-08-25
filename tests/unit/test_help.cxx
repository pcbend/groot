#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <GHelp.h>

namespace {

void require(bool condition, const char* message) {
  if(!condition) {
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
  }
}

void requireContains(const std::string& text,
                     const std::string& needle,
                     const char* message) {
  if(text.find(needle) == std::string::npos) {
    std::cerr << "FAILED: " << message << "\nmissing: " << needle
              << "\noutput:\n" << text << '\n';
    std::exit(1);
  }
}

} // namespace

int main() {
  require(GHelp::IsHelpCommand("?"), "? should be a help command");
  require(GHelp::IsHelpCommand("?;"), "?; should be a help command");
  require(GHelp::IsHelpCommand("help"), "help should be a help command");
  require(GHelp::IsHelpCommand("#line 1 \"ROOT_prompt_0\"\nhelp"),
          "ROOT prompt wrapped help should be a help command");
  require(GHelp::IsHelpCommand("#line 1 \"ROOT_prompt_1\"\n?"),
          "ROOT prompt wrapped ? should be a help command");
  require(GHelp::IsHelpCommand("help commands"), "help commands should be a help command");
  require(!GHelp::IsHelpCommand("helpful"), "helpful should pass through to ROOT");

  std::ostringstream overview;
  GHelp::Print("?", overview);
  requireContains(overview.str(), "groot help", "overview title");
  requireContains(overview.str(), "help commands", "overview topic hints");

  std::ostringstream commands;
  GHelp::Print("help commands", commands);
  requireContains(commands.str(), "groot command summary", "commands title");
  requireContains(commands.str(), "x/y", "commands projections");

  std::ostringstream root;
  GHelp::Print("help root", root);
  requireContains(root.str(), "AddExec", "root help should mention TExec attachment");

  std::ostringstream roi;
  GHelp::Print("help roi", roi);
  requireContains(roi.str(), "not as a", "roi help should describe current status");
  requireContains(roi.str(), "GROI::CreateFromMarkers", "roi help should mention API");

  std::ostringstream journal;
  GHelp::Print("help journal", journal);
  requireContains(journal.str(), ".gui_history", "journal help should mention file");
  requireContains(journal.str(), "GGuiHistory::SetEnabled", "journal help should mention controls");
  return 0;
}
