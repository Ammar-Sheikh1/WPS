/*Script used to dump data into levelDB database*/
#include <iostream>
#include "leveldb/db.h"

int main(int argc, char** argv){
  //quick test
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
  assert(status.ok());

  leveldb::WriteOptions writeopts = leveldb::WriteOptions();
  db->Put(writeopts, "Ammar", "972-533-9402");
  delete db;
  std::cout << "Done! \n";
  return 0;
}