
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

const int CF_SUCCEES = 0;
const int CF_ERROR_OPEN = -1;

enum outFormat {
  RAW,
  HEX,
  BASE64
};

struct inFileData {
  string filePath;
  long startPos;
  long preCount;
  long postCount;
};

int main(int argc, char *argv[])
{
  inFileData fileData;
  auto cFile = fopen(fileData.filePath.c_str(), "rb");
  if (cFile == nullptr) {
    cout << "Cannot open file : \"" << fileData.filePath << "\" !";
    return CF_ERROR_OPEN;
  }
  fclose(cFile);
  cFile = nullptr;
  return CF_SUCCEES;
}
