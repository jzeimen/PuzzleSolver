#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <fstream>

class logger
{
private:
  std::ostringstream os;
  std::string logfilename;
  std::ofstream* ofstream;
  logger();
  void set_filename(std::string filename);
  std::ostringstream& _stream();
  void _flush();
public:
  static logger& get_instance();
  static void filename(std::string filename);
  static std::ostringstream& stream();
  static void flush();
  logger(logger const&) = delete;
  void operator=(logger const&)  = delete;
  virtual ~logger();
};

#endif /* LOGGER_H */
