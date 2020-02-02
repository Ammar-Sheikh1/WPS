/* 

Script used to dump data into levelDB database 

*/
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <regex>
#include <vector>
#include <cassert>

#include "includes/XMLParser.h"
#include "leveldb/db.h"


int main(int argc, char** argv){

  if (argc < 2) {
    std::cout << "./db_script <netxml file>";
    return 0;
  }

  const char* fileName = argv[1];
  ammar::XMLParser xml(fileName);
  xml.build();

  // Parse the .netxml document for Networks, BSSID & latitude and longitude

  std::unordered_map<std::string, std::string> map;

  for (const auto& network : xml.iterate("detection-run/wireless-network")) {

    std::string BSSID = "";
    std::string latitude = "";
    std::string longitude = "";

    for (const auto& bssid: xml.iterate("wireless-network/BSSID", network)) {
      BSSID = bssid->text;
    }
    for (const auto& lat : xml.iterate("wireless-network/gps-info/avg-lat", network)) {
      latitude = lat->text;
    }

    for (const auto& lon : xml.iterate("wireless-network/gps-info/avg-lon", network)) {
      longitude = lon->text;
    }
    if (BSSID.size() &&  latitude.size() && longitude.size()) {
     map[BSSID] = latitude + "/" + longitude;
    }
  } 
  
  // Open Database
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = false;
  leveldb::Status status = leveldb::DB::Open(options, "../db/APData", &db);
  assert(status.ok());
  leveldb::WriteOptions writeopts = leveldb::WriteOptions();

  //Dump Access Point Data in DB
  int i = 1;
  for (auto iter = map.begin(), end = map.end(); iter != end; iter++, ++i) {
    db->Put(writeopts, iter->first, iter->second);
  }

  // close db
  delete db;
  std::cout << i << " APs added \n";
  return 0;
}
