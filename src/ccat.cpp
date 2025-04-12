
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

const int CF_SUCCEES = 0;
const int CF_ERROR_INVALID_ARGS = -1;
const int CF_ERROR_OPEN = -2;

enum outFormat {
  RAW,
  HEX,
  BASE64
};

struct inFileData {
  string path;
  long offsetPos;
  long offsetAround;
  bool isComplete;
};

inFileData readFileData(int argc, char*argv[]) {
  inFileData fileData;
  fileData.offsetPos = 0;
  fileData.offsetAround = 0;
  fileData.isComplete = false;
  for (auto i = 1; i < argc; ++i) {
    string argument {argv[i]};
    if (argument.substr(0,3) == "-o:")
      fileData.offsetPos = stol(argument.substr(3, argument.length()));
    else if (argument.substr(0,3) == "-a:")
      fileData.offsetAround = stol(argument.substr(3, argument.length()));
    else
      fileData.path = argument;
  }
  fileData.isComplete = (fileData.offsetPos > 0 && fileData.path.length() > 0);
  return fileData;
}

void printHelp() {
  cout << "ccat -o:<offset> -a:<around> <file> \n";
}

int main(int argc, char *argv[])
{
  inFileData fileData = readFileData(argc, argv);
  if (fileData.isComplete == false) {
    printHelp();
    return CF_ERROR_INVALID_ARGS;
  }
  auto cFile = fopen(fileData.path.c_str(), "rb");
  if (cFile == nullptr) {
    cout << "Cannot open file : \"" << fileData.path << "\"\n";
    return CF_ERROR_OPEN;
  }
  fclose(cFile);
  cFile = nullptr;
  return CF_SUCCEES;
}
