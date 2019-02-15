#include <iostream>
#include <sstream>
#include <fstream>
#include "logger.h"

logger::logger() {
  ofstream = NULL;
}

void logger::set_filename(std::string filename) {
  logfilename = filename;
  ofstream = new std::ofstream();
  ofstream->open(filename, std::ofstream::out | std::ofstream::app);
  if (ofstream->fail()) {
    std::cerr << "Failed to open " << filename << " for writing" << std::endl;
    ofstream = NULL;
  }
}

std::ostringstream& logger::_stream() {
  return os;
}

void logger::_flush() {
  std::string message = os.str();
  std::cout << message << std::flush;
  if (ofstream != NULL) {
    *ofstream << message << std::flush;
  }
  os.str("");
  os.clear();
}

logger& logger::get_instance()
{
  static logger instance;
  return instance;
}

void logger::filename(std::string filename) {
  get_instance().set_filename(filename);
}

std::ostringstream& logger::stream() {
  return get_instance()._stream();
}

void logger::flush() {
  get_instance()._flush();
}

logger::~logger() {
  _flush();
  if (ofstream != NULL) {
    ofstream->close();
  }
}

/*
int main(int argc, char* argv[]) {
  logger::stream() << "Hello, world!" << std::endl;
  logger::flush();
  logger::filename("output.log");
  logger::stream() << "Foo, bar!" << std::endl;
}
*/
