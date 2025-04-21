
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <filesystem>

using namespace std;

const int CF_SUCCEES = 0;
const int CF_ERROR_INVALID_ARGS = -1;
const int CF_ERROR_NOTEXISTS = -2;
const int CF_ERROR_OPEN = -3;
const int CF_INVALID_OFFSET = -4;
const int CF_INVALID_OFFSET_AROUND = -5;
const int CF_INVALID_BUFFER_SIZE_FOR_HTABLE = -6;

const char cNormal[]  = "\033[0m";
const char cRed[]     = "\x1b[1;31m";
const char cGreen[]   = "\x1b[1;32m";
const char cBlue[]    = "\x1b[1;34m";
const char cBoldOn[]  = "\e[1m";
const char cBoldOff[] = "\e[0m";

const long MAX_HTABLE_BUFFER_SIZE = 100000;
const int  MAX_HTABLE_COLUMN_COUNT = 60;

enum outFormat {
  RAW,
  HEX,
  HTABLE
};

struct inFileData {
  string path;
  long offsetPos;
  long offsetAround;
  string offsetColor;
  outFormat printFormat;
  long columnsCount;
  bool isComplete;
  inFileData() {
    offsetPos = -1;
    offsetAround = 0;
    offsetColor = "";
    printFormat = outFormat::RAW;
    columnsCount = 10;
    isComplete = false;
  }
};

inFileData readFileData(int argc, char*argv[]) {
  inFileData fileData;
  for (auto i = 1; i < argc; ++i) {
    string argument {argv[i]};
    if (argument.length() >= 3 && argument.substr(0,3) == "-o:")
      fileData.offsetPos = stol(argument.substr(3, argument.length()));
    else if (argument.length() >= 3 && argument.substr(0,3) == "-a:") {
      fileData.offsetAround = stol(argument.substr(3, argument.length()));
      if (fileData.offsetAround < 0)
	fileData.offsetAround *= -1;
    }
    else if (argument.length() >= 3 && argument.substr(0,3) == "-c:") 
      fileData.offsetColor = argument.substr(3, argument.length());
    else if (argument.length() >= 4 && argument.substr(0,4) == "-tc:")
      fileData.columnsCount = stol(argument.substr(4, argument.length()));
    else if (argument.length() >= 4 && argument.substr(0,4) == "-RAW")
      fileData.printFormat = outFormat::RAW;
    else if (argument.length() >= 4 && argument.substr(0,4) == "-HEX")
      fileData.printFormat = outFormat::HEX;
    else if (argument.length() >= 7 && argument.substr(0,7) == "-HTABLE")
      fileData.printFormat = outFormat::HTABLE;
    else
      fileData.path = argument;
  }
  fileData.isComplete = (fileData.offsetPos >= 0 && fileData.path.length() > 0);
  return fileData;
}

string getColorCode(const string& colorStr) {
  string sColor{colorStr};
  transform(sColor.begin(), sColor.end(), sColor.begin(),
	    [](unsigned char c){return tolower(c);});
  if (sColor == "bold")
    return cBoldOn;
  if (sColor == "red")
    return cRed;
  if (sColor == "green")
    return cGreen;
  if (sColor == "blue")
    return cBlue;
  return "";
}

void printHelp() {
  cout << "\nccat -o:<offset> -a:<around> -c:<bold,red,green,blue> -tc:<colCount> -<RAW,HEX,HTABLE> <file>\n\n";
  cout << "  -o:<offset>         : Zero based offset\n";
  cout << "  -a:<around>         : Max count char before and after offset print out too\n";
  cout << "  -c:<bold,red,..>    : Mark color for char at requested offset\n";
  cout << "  -tc:<tableColCount> : Hextable columncount\n";
  cout << "  -RAW                : Normal raw value printed out, thats the default\n";
  cout << "  -HEX                : Value printed outed as Hexvalue\n";
  cout << "  -HTABLE             : Value printed outed as Hexvalue as table\n";
  cout << endl;
}

string chexVal(const auto& c){
  stringstream ost;
  ost << setw(2);
  ost << setfill('0');
  ost << hex << int{c};
  string result{ost.str()};
  transform(result.begin(), result.end(), result.begin(),
	    [](unsigned char c){ return toupper(c); });
  return result;
}

void printOffsetBufferAsHexTable(const inFileData& fileData, long reqOffsetMark,
				 char *reqOffsetBuffer, long reqOffsetBufferLength) {
  auto rowCount = reqOffsetBufferLength / fileData.columnsCount;
  if (reqOffsetBufferLength % fileData.columnsCount)
    rowCount++;
  long startOffset {0};
  for (long r = 0; r < rowCount;  ++r) {
    if (!r) {
      cout << "      ";
      for (long ci = 0; ci < fileData.columnsCount; ++ci) {
	if (ci)
	  cout << " ";
	cout << setw(2) << setfill('0') << ci;
      }
      cout << endl;
    }
    cout << setw(5) << setfill('0') << startOffset << " ";
    auto outCount = r*fileData.columnsCount; 
    for (long c = 0; ((c < fileData.columnsCount) && ((outCount+c) < reqOffsetBufferLength)); ++c) {
      if (c)
	cout << " ";
      if (outCount+c != reqOffsetMark)
	cout << chexVal(reqOffsetBuffer[outCount+c]);
      else if ((outCount+c) == reqOffsetMark) {
	cout << getColorCode(fileData.offsetColor);
	cout << chexVal(reqOffsetBuffer[outCount+c]);
	cout << cBoldOff;
	cout << cNormal;
      }
    }
    startOffset += fileData.columnsCount;
    cout << endl;
  }
}

void printOffsetBuffer(const inFileData& fileData, long reqOffsetMark,
		       char *reqOffsetBuffer, long reqOffsetBufferLength) {
  if (fileData.printFormat == outFormat::RAW) {
    for (long i = 0; i < reqOffsetBufferLength; ++i) {
      if (i != reqOffsetMark)
	cout << reqOffsetBuffer[i];
      else if (i == reqOffsetMark) {
	cout << getColorCode(fileData.offsetColor);
	cout << reqOffsetBuffer[i];
	cout << cBoldOff;
	cout << cNormal;
      }
    }
    cout << endl;
  }
  else if (fileData.printFormat == outFormat::HEX) {
    for (long i = 0; i < reqOffsetBufferLength; ++i) {
      if (i)
	cout << " ";
      if (i != reqOffsetMark)
	cout << chexVal(reqOffsetBuffer[i]);
      else if (i == reqOffsetMark) {
	cout << getColorCode(fileData.offsetColor);
	cout << chexVal(reqOffsetBuffer[i]);
	cout << cBoldOff;
	cout << cNormal;
      }
    }
    cout << endl;
  }
  else if (fileData.printFormat == outFormat::HTABLE)
    printOffsetBufferAsHexTable(fileData, reqOffsetMark, reqOffsetBuffer, reqOffsetBufferLength);
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

  if (fileData.printFormat == outFormat::HTABLE) {
    if (readSize > 1 && readSize > MAX_HTABLE_BUFFER_SIZE) {
      cout << "Invalid readsize for HTABLE-format : \"";
      cout << readSize;
      cout << "\"\n";
      return CF_INVALID_BUFFER_SIZE_FOR_HTABLE;
    }
    if (fileData.columnsCount > MAX_HTABLE_COLUMN_COUNT) {
      cout << "Invalid columncount : \"";
      cout << fileData.columnsCount;
      cout << "\"\nMax. 60 columns are supported!\n";
      return CF_INVALID_BUFFER_SIZE_FOR_HTABLE;
    }
  }
  
  char buffer[readSize+1] = {0};
  fseek(filePtr, fileData.offsetPos-calcAroundLeft, SEEK_SET);
  fread(buffer, sizeof(char), readSize, filePtr);
  fclose(filePtr);
  filePtr = nullptr;
  printOffsetBuffer(fileData, calcAroundLeft, buffer, readSize);
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
