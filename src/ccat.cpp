
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

const char cNormal[]  = "\033[0m";
const char cBoldOn[]  = "\e[1m";
const char cBoldOff[] = "\e[0m";

enum outFormat {
  RAW,
  HEX
};

struct inFileData {
  string path;
  long offsetPos;
  long offsetAround;
  string offsetColor;
  outFormat printFormat;
  bool isComplete;
};

inFileData readFileData(int argc, char*argv[]) {
  inFileData fileData;
  fileData.offsetPos = -1;
  fileData.offsetAround = 0;
  fileData.offsetColor = cBoldOn;
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
    else if (argument.substr(0,3) == "-c:") 
      fileData.offsetColor = argument.substr(3, argument.length());
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
  cout << "ccat -o:<offset> -a:<around> -c:<color> -<RAW,HEX> <file>\n";
}

void printOffset(const inFileData& fileData, long reqOffsetMark, char *reqOffsetBuffer, long reqOffsetBufferLength) {
  if (fileData.printFormat == outFormat::RAW) {
    for (long i = 0; i < reqOffsetBufferLength; ++i) {
      if (i != reqOffsetMark)
	cout << reqOffsetBuffer[i];
      else if (i == reqOffsetMark) {
	cout << fileData.offsetColor;
	cout << reqOffsetBuffer[i];
	cout << cBoldOff;
	cout << cNormal;
      }
    }
    cout << endl;
  }
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
  long calcAroundLeft {fileData.offsetAround};
  long calcAroundRight {fileData.offsetAround};
  if (fileData.offsetPos-calcAroundLeft < 0) {
    calcAroundLeft -= ( (fileData.offsetPos-calcAroundLeft)  * -1 ); 
  }
  if (fileData.offsetPos+calcAroundRight >= fileSize) {
    calcAroundRight -= ( ((fileData.offsetPos+1)+calcAroundRight) - fileSize ); 
  }
  long readSize = calcAroundLeft + calcAroundRight + 1;
  if (readSize > 1 && readSize > fileSize) {
    cout << "Invalid given offset around : \"";
    cout << fileData.offsetAround;
    cout << "\" at filesize of \"";
    cout << fileSize;
    cout << "\"\n";
    return CF_INVALID_OFFSET_AROUND;
  }
  char buffer[readSize+1] = {0};
  fseek(filePtr, fileData.offsetPos-calcAroundLeft, SEEK_SET);
  fread(buffer, sizeof(char), readSize, filePtr);
  fclose(filePtr);
  filePtr = nullptr;
  printOffset(fileData, calcAroundLeft, buffer, readSize);
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
    cout << "File \"";
    cout << fileData.path;
    cout << "\" doesnt exists\n";
    return CF_ERROR_NOTEXISTS;
  }
  return printFileOffset(fileData);
}
