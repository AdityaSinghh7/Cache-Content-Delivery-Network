#include "cacheCDN.h"

#include <algorithm>
#include <climits>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
using namespace ICS45C::CDN;
Cache::Cache(unsigned int freshnessCount) {
  freshnessCount = freshnessCount;
  diskreadtimes = 0;
}
Cache::~Cache() {
  for (auto entry : BinaryFileMap) {
    delete[] entry.second;
  }
  BinaryFileMap.clear();
  textFileFreshness.clear();
  TextFile_map.clear();
  BinaryFileFreshness.clear();
  topfilecounter.clear();
}
string Cache::readtextfile(string filepath) {
  diskreadtimes++;
  ifstream infile;
  string temp;
  string text;
  infile.open(filepath);
  while (getline(infile, temp)) {
    text += temp + "\n";
  }
  infile.close();
  return text;
}
string ICS45C::CDN::Cache::getText(string filepath) {
  if (TextFile_map.size() == 0) {
    textFileFreshness[filepath] = freshnessCount - 1;
    TextFile_map[filepath] = readtextfile(filepath);
    if (topfilecounter.find(filepath) != topfilecounter.end()) {
      topfilecounter[filepath] += 1;
    } else {
      topfilecounter[filepath] = 1;
    }
  } else {
    if (freshnessCount == 0 || freshnessCount == 1) {
      textFileFreshness[filepath] = freshnessCount - 1;
      TextFile_map[filepath] = readtextfile(filepath);
      auto s = topfilecounter.find(filepath);
      if (s != topfilecounter.end()) {
        s->second++;
      } else {
        topfilecounter[filepath] = 1;
      }
    } else {
      auto s = textFileFreshness.find(filepath);
      if (s != textFileFreshness.end()) {
        unsigned int count;
        count = s->second;
        if (count > 0) {
          count -= 1;
          textFileFreshness[filepath] = count;
          topfilecounter[filepath]++;
        } else {
          textFileFreshness[filepath] = freshnessCount - 1;
          TextFile_map[filepath] = readtextfile(filepath);
          topfilecounter[filepath]++;
        }
      } else {
        textFileFreshness[filepath] = freshnessCount - 1;
        TextFile_map[filepath] = readtextfile(filepath);
        topfilecounter[filepath] = 1;
      }
    }
  }
  auto retsearch = TextFile_map.find(filepath);
  string ret = retsearch->second;
  return ret;
}
bool Cache::isCached(string filepath) {
  auto s = TextFile_map.find(filepath);
  auto x = BinaryFileMap.find(filepath);
  if (s != TextFile_map.end()) {
    return true;
  } else if (x != BinaryFileMap.end()) {
    return true;
  } else {
    return false;
  }
}
unsigned int Cache::getFreshness(string filepath) {
  unsigned int ret = 0;
  auto s = textFileFreshness.find(filepath);
  auto x = BinaryFileFreshness.find(filepath);
  if (s != textFileFreshness.end()) {
    ret = s->second;
  } else if (x != BinaryFileFreshness.end()) {
    ret = x->second;
  }
  if (freshnessCount == 0) {
    return 0;
  } else {
    return ret;
  }
}
void Cache::markFileFresh(string filepath) {
  auto s = textFileFreshness.find(filepath);
  auto x = BinaryFileFreshness.find(filepath);
  if (s != textFileFreshness.end()) {
    textFileFreshness[filepath] = freshnessCount;
  } else if (x != BinaryFileFreshness.end()) {
    BinaryFileFreshness[filepath] = freshnessCount;
  } else {
    return;
  }
}
void Cache::purgeCache() {
  for (auto entry : BinaryFileMap) {
    delete[] entry.second;
  }
  TextFile_map.clear();
  textFileFreshness.clear();
  BinaryFileFreshness.clear();
  BinaryFileMap.clear();
}
string Cache::topFile() {
  int maxValue = INT_MIN;
  pair<string, int> maxpair;

  for (const auto &entry : topfilecounter) {
    if (maxValue < entry.second) {
      maxValue = entry.second;
      maxpair = entry;
    }
  }
  string ret = maxpair.first;
  return ret;
}
string Cache::getStats() {
  float requests = 0;
  float filecounter = 0;
  float averagerequests;
  for (auto x : topfilecounter) {
    requests += x.second;
    filecounter++;
  }

  if (filecounter == 0) {
    averagerequests = 0;
  } else {
    averagerequests = (requests) / (filecounter);
  }

  stringstream x;
  x << fixed << setprecision(2) << averagerequests;
  string reduced = x.str();

  int maxValue = 0;
  string topfilepath;

  for (const auto &entry : topfilecounter) {
    if (maxValue < entry.second) {
      maxValue = entry.second;
      topfilepath = entry.first;
    }
  }
  string ret = "";
  ret += "Cache Stats\n";
  ret += "----------\n";
  ret += "Total requests: " + to_string(int(requests)) + "\n";
  ret += "Total files requested: " + to_string(int(filecounter)) + "\n" +
         "Average requests per file: " + reduced + "\n" +
         "Top file: " + topfilepath + " (" + to_string(maxValue) +
         " requests)\n" +
         "Total times read from disk: " + to_string(diskreadtimes) + "\n" +
         "----------\n";
  return ret;
}
char *Cache::readbinaryfile(string filepath) {
  char *memblock;
  diskreadtimes++;
  streampos size;
  ifstream inBinfile(filepath, ios::in | ios::binary | ios::ate);
  if (inBinfile.is_open()) {
    size = inBinfile.tellg();
    memblock = new char[size];
    inBinfile.seekg(0, ios::beg);
    inBinfile.read(memblock, size);
    inBinfile.close();
  }
  return memblock;
}
char *Cache::getBinary(string filepath) {
  if (BinaryFileMap.empty()) {
    BinaryFileMap[filepath] = readbinaryfile(filepath);
    BinaryFileFreshness[filepath] = freshnessCount - 1;
    if (topfilecounter.find(filepath) != topfilecounter.end()) {
      topfilecounter[filepath] += 1;
    } else {
      topfilecounter[filepath] = 1;
    }
  } else {
    if (freshnessCount == 0 || freshnessCount == 1) {
      auto searchfor = BinaryFileMap.find(filepath);
      if (searchfor != BinaryFileMap.end()) {
        delete[] BinaryFileMap[filepath];
      }
      BinaryFileMap[filepath] = readbinaryfile(filepath);
      BinaryFileFreshness[filepath] = freshnessCount;
      auto s = topfilecounter.find(filepath);
      if (s != topfilecounter.end()) {
        s->second++;
      } else {
        topfilecounter[filepath] = 1;
      }
    } else {
      auto s = BinaryFileFreshness.find(filepath);
      if (s != BinaryFileFreshness.end()) {
        unsigned int count = s->second;
        if (count > 0) {
          count -= 1;
          BinaryFileFreshness[filepath] = count;
          topfilecounter[filepath]++;
        } else {
          delete[] BinaryFileMap[filepath];
          BinaryFileMap[filepath] = readbinaryfile(filepath);
          BinaryFileFreshness[filepath] = freshnessCount - 1;
          topfilecounter[filepath]++;
        }
      } else {
        delete[] BinaryFileMap[filepath];
        BinaryFileMap[filepath] = readbinaryfile(filepath);
        BinaryFileFreshness[filepath] = freshnessCount - 1;
        topfilecounter[filepath] = 1;
      }
    }
  }
  auto x = BinaryFileMap.find(filepath);
  return x->second;
}