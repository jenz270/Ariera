/**
 * CS3307 Team Banana
 *
 * DateFilter is a tool used when filtering data by date. It handles comparing data to the date range
 * and returning whether or not the data point should be included in the filtered summary
 */

#include "DateFilter.h"



/**
 * Constructs the datefilter object. Fills the variables earliestDate and latestDate with the
 * values inputed into the constructor
 *
 * @param earliestYear  Value to be sotred in the objects variable earliestDate
 * @param latestYear  Value to be sotred in the objects variable latestDate
 */
DateFilter::DateFilter(int earliestYear, int latestYear): earliestDate(earliestYear), latestDate(latestYear) {
  ;
}


/**
 * Takes a Grants and Funding record and returns whether or not it is within the date filter
 *
 * @param record    The record that the function is checking
 *
 * @return          Returns 0 if record is withing filter and 1 otherwise
 */
int DateFilter::filter(ProcessedGrantsAndFundingRecord& record){
  std::string date = record.getStartDate();
  if (date.compare("") == 0)
    return 1;
  std::istringstream yearStream(date.substr(0,4));
  int year;
  if (!(yearStream >> year))
    return 1;
  if (year >= earliestDate)
    if (year <= latestDate)
      return 0;
  return 1;
}


/**
 * Takes a Publication record and returns whether or not it is within the date filter
 *
 * @param record    The record that the function is checking
 *
 * @return          Returns 0 if record is withing filter and 1 otherwise
 */
int DateFilter::filter(ProcessedPublicationRecord& record){
  std::string date = record.getStatusDate();
  if (date.compare("") == 0)
    return 1;
  std::istringstream yearStream(date.substr(0,4));
  int year;
  if (!(yearStream >> year))
    return 1;
  if (year >= earliestDate)
    if (year <= latestDate)
      return 0;
  return 1;
}


/**
 * Takes a Presentation record and returns whether or not it is within the date filter
 *
 * @param record    The record that the function is checking
 *
 * @return          Returns 0 if record is withing filter and 1 otherwise
 */
int DateFilter::filter(ProcessedPresentationRecord& record){
  std::string date = record.getDate();
  if (date.compare("") == 0)
    return 1;
  std::istringstream yearStream(date.substr(0,4));
  int year;
  if (!(yearStream >> year))
    return 1;
  if (year >= earliestDate)
    if (year <= latestDate)
      return 0;
  return 1;
}
int DateFilter::filter(ProcessedTeachingRecord& record){
  std::string date = record.getStartDate();
  if (date.compare("") == 0)
    return 1;
  std::istringstream yearStream(date.substr(0,4));
  int year;
  if (!(yearStream >> year))
    return 1;
  if (year >= earliestDate)
    if (year <= latestDate)
      return 0;
  return 1;
}
