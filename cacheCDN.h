#ifndef __CACHECDN_H__
#define __CACHECDN_H__

/* Add any includes here! */
#include <algorithm>
#include <climits>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;
namespace ICS45C {
namespace CDN {

class Cache {
 private:
  unsigned int freshnessCount;

 public:
  Cache(unsigned int freshnessCount);
  ~Cache();
  map<string, char*> BinaryFileMap;
  map<string, unsigned int> BinaryFileFreshness;
  map<string, int> topfilecounter;
  map<string, string> TextFile_map;
  map<string, unsigned int> textFileFreshness;
  int diskreadtimes;
  string readtextfile(string filepath);
  char* readbinaryfile(string filepath);
  string getText(string filepath);
  char* getBinary(string filepath);
  bool isCached(string filepath);
  unsigned int getFreshness(string filepath);
  void markFileFresh(string filepath);
  void purgeCache();
  string topFile();
  string getStats();
};

}  // namespace CDN
}  // namespace ICS45C

#endif  // __CACHECDN_H__