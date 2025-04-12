
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

using namespace std;

const int CF_SUCCEES = 0;
const int CF_ERROR_INVALID_ARGS = -1;
const int CF_ERROR_NOTEXISTS = -2;
const int CF_ERROR_OPEN = -3;
const int CF_INVALID_OFFSET = -4;
const int CF_INVALID_OFFSET_AROUND = -5;

enum outFormat {
  RAW,
  HEX
};

struct inFileData {
  string path;
  long offsetPos;
  long offsetAround;
  outFormat printFormat;
  bool isComplete;
};

inFileData readFileData(int argc, char*argv[]) {
  inFileData fileData;
  fileData.offsetPos = -1;
  fileData.offsetAround = 0;
  fileData.printFormat = outFormat::RAW;
  fileData.isComplete = false;
  for (auto i = 1; i < argc; ++i) {
    string argument {argv[i]};
    if (argument.substr(0,3) == "-o:")
      fileData.offsetPos = stol(argument.substr(3, argument.length()));
    else if (argument.substr(0,3) == "-a:") {
      fileData.offsetAround = stol(argument.substr(3, argument.length()));
      if (fileData.offsetAround < 0)
	fileData.offsetAround *= -1;
    }
    else if (argument.substr(0,3) == "-RAW")
      fileData.printFormat = outFormat::RAW;
    else if (argument.substr(0,3) == "-HEX")
      fileData.printFormat = outFormat::HEX;
    else
      fileData.path = argument;
  }
  fileData.isComplete = (fileData.offsetPos >= 0 && fileData.path.length() > 0);
  return fileData;
}

void printHelp() {
  cout << "ccat -o:<offset> -a:<around> -<RAW,HEX> <file> \n";
}

int printFileOffset(const inFileData& fileData) {
  auto filePtr = fopen(fileData.path.c_str(), "rb");
  if (filePtr == nullptr) {
    cout << "Cannot open file : \"" << fileData.path << "\"\n";
    return CF_ERROR_OPEN;
  }
  fseek(filePtr, 0, SEEK_END);
  auto fileSize = ftell(filePtr);
  if (fileData.offsetPos >= fileSize) {
    cout << "Invalid given offset : \"" << fileData.offsetPos << "\" at filesize of \"" << fileSize << "\"\n";
    return CF_INVALID_OFFSET;
  }
  long readSize = (2*fileData.offsetAround)+1;
  if (readSize > 1 && readSize > fileSize) {
    cout << "Invalid given offset around : \"" << fileData.offsetAround << "\" at filesize of \"" << fileSize << "\"\n";
    return CF_INVALID_OFFSET_AROUND;
  }
  char buffer[readSize+1] = {0};
  fseek(filePtr, fileData.offsetPos-fileData.offsetAround, SEEK_SET);
  fread(buffer, sizeof(char), readSize, filePtr);
  cout << buffer << endl;
  fclose(filePtr);
  filePtr = nullptr;
  return CF_SUCCEES;
}

int main(int argc, char *argv[])
{
  inFileData fileData = readFileData(argc, argv);
  if (fileData.isComplete == false) {
    printHelp();
    return CF_ERROR_INVALID_ARGS;
  }
  if (!filesystem::exists(fileData.path)) {
    cout << "File \"" << fileData.path << "\" doesnt exists\n";
    return CF_ERROR_NOTEXISTS;
  }
  return printFileOffset(fileData);
}
