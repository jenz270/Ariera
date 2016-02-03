/**
 *  Team Banana 2015
 *
 * ProcessedPresentationRecord is an object to hold each individual record stored in the program for presentation csv files
 *
 */

#include "ProcessedPresentationRecord.h"


/**
 * Creates a ProcessedPresentationRecord object from a vector filled with the data
 *
 * @param fields    a vector which contains all of the differnt data needed for ProcessedPresentationRecord
 */

ProcessedPresentationRecord::ProcessedPresentationRecord(std::vector<std::string>& fields){
  if(fields.size() != 6){
    std::cout << "In the constructor of ProcessedPresentationRecord, the input field vector has wrong size.\n";
    exit(1);
  }
  this->memberName = fields[0];
  this->primaryDomain = fields[1];
  this->date = fields[2];
  this->type = fields[3];
  this->role = fields[4];
  this->title = fields[5];
}



/*
 * All of theses functions are pretty straightforward. They all return a variable
 * belonging to the object when called or set a varibale of the object.
 *
 */
std::string ProcessedPresentationRecord::getMemberName(){
  return this->memberName;
}
std::string ProcessedPresentationRecord::getPrimaryDomain(){
  return this->primaryDomain;
}
std::string ProcessedPresentationRecord::getDate(){
  return this->date;
}
std::string ProcessedPresentationRecord::getType(){
  return this->type;
}
std::string ProcessedPresentationRecord::getRole(){
  return this->role;
}
std::string ProcessedPresentationRecord::getTitle(){
  return this->title;
}
void ProcessedPresentationRecord::setMemberName(std::string& newValue){
  this->memberName = newValue;
}
void ProcessedPresentationRecord::setPrimaryDomain(std::string& newValue){
  this->primaryDomain = newValue;
}
void ProcessedPresentationRecord::setDate(std::string& newValue){
  this->date = newValue;
}
void ProcessedPresentationRecord::setType(std::string& newValue){
  this->type = newValue;
}
void ProcessedPresentationRecord::setRole(std::string& newValue){
  this->role = newValue;
}
void ProcessedPresentationRecord::setTitle(std::string& newValue){
  this->title = newValue;
}
