/**
 *   CS3307 Team Banana
 *
 *  Database.cpp handles the filtering and summarization of all 4 types of data into a structure that can
 *  be read by the gui and displayed to the user.
*/

#include "Database.h"

#include <algorithm>

Database::Database():grantsAndFundingRecord(), teachingRecord(), presentationRecord(), publicationRecord(){

    QDir dir(".");
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo info, list)
    {
        //QRegExp re("^Processed.*v$");
        if(info.fileName().contains("ProcessedGrantsClinicalFunding.csv"))
        {
            IOUtility::readDatabase("ProcessedGrantsClinicalFunding.csv",
                                     this->grantsAndFundingRecord,
                                     this->teachingRecord,
                                     this->presentationRecord,
                                     this->publicationRecord);
        }
        else if(info.fileName().contains("ProcessedPresentation.csv"))
        {
            IOUtility::readDatabase("ProcessedPresentation.csv",
                                     this->grantsAndFundingRecord,
                                     this->teachingRecord,
                                     this->presentationRecord,
                                     this->publicationRecord);
        }
        else if(info.fileName().contains("ProcessedPublication.csv"))
        {
            IOUtility::readDatabase("ProcessedPublication.csv",
                                     this->grantsAndFundingRecord,
                                     this->teachingRecord,
                                     this->presentationRecord,
                                     this->publicationRecord);
        }
        else if(info.fileName().contains("ProcessedTeaching.csv"))
        {
            IOUtility::readDatabase("ProcessedTeaching.csv",
                                     this->grantsAndFundingRecord,
                                     this->teachingRecord,
                                     this->presentationRecord,
                                     this->publicationRecord);
        }
    }

}

/*
*	Following code was to be implemented as part of the singleton design pattern
*	However, it was commented and removed from project due to time and functionality purposes
*/
//Database *database::instance() {
//	if (!inst)
//		inst = new Database();
//	return inst;
//}


/**
 * Filters grants data. load the filtered data into a new structure that can be used to make a new summary.
 *
 */
void Database::getGrantsAndFunding(vector< vector<string> >& results,
                                    vector<Filter*>& filterList,
                                    vector<grantsAndFundingField>& fieldList){

  vector<ProcessedGrantsAndFundingRecord>::iterator recordIter = grantsAndFundingRecord.begin(); 
  vector<grantsAndFundingField>::iterator fieldIter;
  vector< vector<string> >::iterator resultIter;
  vector<Filter*>::iterator filterIter;

  /*
  resultIter = results.begin();
  while(resultIter != results.end()){
    free( &(*resultIter) );
    resultIter++;
  }
  */
  results.clear();

  bool filterFlag;
  vector<string>* resultFields;
  while (recordIter != grantsAndFundingRecord.end()){
    /*
     * Code to filter results
     */
    filterIter = filterList.begin();
    filterFlag = false;
    while(filterIter != filterList.end()){
      if ((**filterIter).filter(*recordIter)){
        filterFlag = true;
      }
      filterIter++;
    }

    //if (recordIter->getPeerReviewed().compare("False") == 0)
    //  filterFlag = false;
    /*
     * Code to filter results
     */
    if (!filterFlag){
      resultFields = new vector<string>;
      fieldIter = fieldList.begin(); 
      while (fieldIter != fieldList.end()){
        switch (*fieldIter){
          case MEMBER_NAME:             resultFields->push_back(recordIter->getMemberName());
            break;
          case PRIMARY_DOMAIN:          resultFields->push_back(recordIter->getPrimaryDomain());
            break;
          case START_DATE:              resultFields->push_back(recordIter->getStartDate());
            break;
          case END_DATE:                resultFields->push_back(recordIter->getEndDate());
            break;
          case FUNDING_TYPE:            resultFields->push_back(recordIter->getFundingType());
            break;
          case STATUS:                  resultFields->push_back(recordIter->getStatus());
            break;
          case PEER_REVIEWED:           resultFields->push_back(recordIter->getPeerReviewed());
            break;
          case INDUSTRY_GRANT:          resultFields->push_back(recordIter->getIndustryGrant());
            break;
          case ROLE:                    resultFields->push_back(recordIter->getRole());
            break;
          case TITLE:                   resultFields->push_back(recordIter->getTitle());
            break;
          case PRINCIPAL_INVESTIGATOR:  resultFields->push_back(recordIter->getPrincipalInvestigator());
            break;
          case CO_INVESTIGATORS:        resultFields->push_back(recordIter->getCoInvestigators());
            break;
          case TOTAL_AMOUNT:            resultFields->push_back(recordIter->getTotalAmount());
            break;
        } 
        fieldIter++;
      } 
      results.push_back(*resultFields);
    }
    recordIter++;
  }
}

/**
 * Used to sort data by comparing two vectors
 *
 * @return  result of comparison
 */
bool sortSummary(vector<string> vect1, vector<string> vect2){
  if (vect1[0].compare(vect2[0]) < 0)
    return true;
  else
    return false;
}

/**
 * Changes the summary of the grants data to reflect only the date range selected by the filter.
 * Sorts the informaton and sums values that need to be summed for the summary.
 *
 * @param sum           a struct that holds all the presentation information
 * @param earliestDate  The beginning date in the filter
 * @param latestDate    The ending date in the filter
 *
 */
void Database::grantsAndFundingSummary(struct grantsAndFundingSummary& sum, int earliestDate, int latestDate){
  //Define fields vector and iterator
  vector<grantsAndFundingField> fields;
  vector<string>::iterator fieldIter;
  //select fields to retrieve (look in GrantsAndFundingFields.h for a list of fields)
  fields.push_back(MEMBER_NAME);
  fields.push_back(START_DATE);
  fields.push_back(TOTAL_AMOUNT);

// ---------- Choose which records to retrieve by defining filters -----------
  
  //vector has list of filters (pointers) used to query database
  vector<Filter*> filters;
  //define date range (inclusive)
  //Filter by date range (inclusive)
  DateFilter df(earliestDate,latestDate);
  filters.push_back(&df);

  //Select Grant records
  FieldMatchFilter isGrant(FUNDING_TYPE, "Grants");
  filters.push_back(&isGrant);
  //Select Peer Reviewed records
  FieldMatchFilter isPeerReviewed(PEER_REVIEWED, "True");
  filters.push_back(&isPeerReviewed);

  //Query Database
  this->getGrantsAndFunding(sum.grantsPeerReviewed, filters, fields);
  std::sort(sum.grantsPeerReviewed.begin(), sum.grantsPeerReviewed.end(), sortSummary);

  //remove the peer reviewed filter
  filters.pop_back();
  //add an industry sponsored filter
  FieldMatchFilter isIndustrySponsored(INDUSTRY_GRANT, "True");
  filters.push_back(&isPeerReviewed);
  //Query Database
  this->getGrantsAndFunding(sum.grantsIndustrySponsored, filters, fields);
  std::sort(sum.grantsIndustrySponsored.begin(), sum.grantsIndustrySponsored.end(), sortSummary);

  //remove the industry sponsored filter
  filters.pop_back();
  //remove the grants filter
  filters.pop_back();
  //add an Clinical Trials sponsored filter
  FieldMatchFilter isClinicalTrials(FUNDING_TYPE, "Clinical Trials");
  filters.push_back(&isClinicalTrials);
  //add the peer reviewed filter
  filters.push_back(&isPeerReviewed);
  //Query Database
  this->getGrantsAndFunding(sum.clinicalPeerReviewed, filters, fields);
  std::sort(sum.clinicalPeerReviewed.begin(), sum.clinicalPeerReviewed.end(), sortSummary);
  
  //remove the peer reviewed filter
  filters.pop_back();
  //add industry sponsored filter
  filters.push_back(&isIndustrySponsored);
  //Query Database
  this->getGrantsAndFunding(sum.clinicalIndustrySponsored, filters, fields);
  std::sort(sum.clinicalIndustrySponsored.begin(), sum.clinicalIndustrySponsored.end(), sortSummary);
}

// =================================================================================================================
// =================================================================================================================
// =================================================================================================================



/**
 * Filters publication data. load the filtered data into a new structure that can be used to make a new summary.
 *
 */
void Database::getPublications(vector< vector<string> >& results,
                                    vector<Filter*>& filterList,
                                    vector<grantsAndFundingField>& fieldList){
  vector<ProcessedPublicationRecord>::iterator recordIter = publicationRecord.begin(); 
  vector<grantsAndFundingField>::iterator fieldIter;
  vector< vector<string> >::iterator resultIter;
  vector<Filter*>::iterator filterIter;

  results.clear();

  bool filterFlag;
  vector<string>* resultFields;
  while (recordIter != publicationRecord.end()){
    /*
     * Code to filter results
     */
    filterIter = filterList.begin();
    filterFlag = false;
    while(filterIter != filterList.end()){
      if ((**filterIter).filter(*recordIter)){
        filterFlag = true;
      }
      filterIter++;
    }

    /*
     * Code to filter results
     */
    if (!filterFlag){
      resultFields = new vector<string>;
      fieldIter = fieldList.begin(); 
      while (fieldIter != fieldList.end()){
        switch (*fieldIter){
          case MEMBER_NAME:             resultFields->push_back(recordIter->getMemberName());
            break;
          case PRIMARY_DOMAIN:          resultFields->push_back(recordIter->getPrimaryDomain());
            break;
          case PUBLICATION_STATUS:      resultFields->push_back(recordIter->getPublicationStatus());
            break;
          case TYPE:                    resultFields->push_back(recordIter->getType());
            break;
          case STATUS_DATE:             resultFields->push_back(recordIter->getStatusDate());
            break;
          case ROLE:                    resultFields->push_back(recordIter->getRole());
            break;
          case JOURNAL_NAME:            resultFields->push_back(recordIter->getJournalName());
            break;
          case AUTHOR:                  resultFields->push_back(recordIter->getAuthor());
            break;
          case TITLE:                   resultFields->push_back(recordIter->getTitle());
            break;
        } 
        fieldIter++;
      } 
      results.push_back(*resultFields);
    }
    recordIter++;
  }
}

/**
 * Changes the summary of the publication data to reflect only the date range selected by the filter.
 * Sorts the informaton and sums values that need to be summed for the summary.
 *
 * @param sum           a struct that holds all the presentation information
 * @param earliestDate  The beginning date in the filter
 * @param latestDate    The ending date in the filter
 *
 */
void Database::publicationSummary(struct publicationSummary& sum, int earliestDate, int latestDate){
  //Define fields vector and iterator
  vector<grantsAndFundingField> fields;
  vector<string>::iterator fieldIter;
  //select fields to retrieve (look in GrantsAndFundingFields.h for a list of fields)
  fields.push_back(MEMBER_NAME);
  fields.push_back(STATUS_DATE);

// ---------- Choose which records to retrieve by defining filters -----------
  
  //vector has list of filters (pointers) used to query database
  vector<Filter*> filters;
  //define date range (inclusive)
  //Filter by date range (inclusive)
  DateFilter df(earliestDate,latestDate);
  filters.push_back(&df);

  FieldMatchFilter fieldFilter(TYPE, "Books");
  filters.push_back(&fieldFilter);
  this->getPublications(sum.books, filters, fields);
  std::sort(sum.books.begin(), sum.books.end(), sortSummary);

  fieldFilter.setValue("Book Chapters");
  this->getPublications(sum.bookChapters, filters, fields);
  std::sort(sum.bookChapters.begin(), sum.bookChapters.end(), sortSummary);

  fieldFilter.setValue("Books Edited");
  this->getPublications(sum.booksEdited, filters, fields);
  std::sort(sum.booksEdited.begin(), sum.booksEdited.end(), sortSummary);

  fieldFilter.setValue("Case Reports");
  this->getPublications(sum.caseReports, filters, fields);
  std::sort(sum.caseReports.begin(), sum.caseReports.end(), sortSummary);

  fieldFilter.setValue("Published Abstract");
  this->getPublications(sum.publishedAbstract, filters, fields);
  std::sort(sum.publishedAbstract.begin(), sum.publishedAbstract.end(), sortSummary);

  fieldFilter.setValue("Journal Article");
  this->getPublications(sum.journalArticle, filters, fields);
  std::sort(sum.journalArticle.begin(), sum.journalArticle.end(), sortSummary);

  fieldFilter.setValue("Letters to Editor");
  this->getPublications(sum.lettersToEditor, filters, fields);
  std::sort(sum.lettersToEditor.begin(), sum.lettersToEditor.end(), sortSummary);

  fieldFilter.setValue("Clinical Care Guidelines");
  this->getPublications(sum.lettersToEditor, filters, fields);
  std::sort(sum.lettersToEditor.begin(), sum.lettersToEditor.end(), sortSummary);

  fieldFilter.setValue("Commentaries");
  this->getPublications(sum.commentaries, filters, fields);
  std::sort(sum.commentaries.begin(), sum.commentaries.end(), sortSummary);

  fieldFilter.setValue("Conference Proceedings");
  this->getPublications(sum.conferenceProceedings, filters, fields);
  std::sort(sum.conferenceProceedings.begin(), sum.conferenceProceedings.end(), sortSummary);

  fieldFilter.setValue("Editorials");
  this->getPublications(sum.editorials, filters, fields);
  std::sort(sum.editorials.begin(), sum.editorials.end(), sortSummary);

  fieldFilter.setValue("Invited Articles");
  this->getPublications(sum.invitedArticles, filters, fields);
  std::sort(sum.invitedArticles.begin(), sum.invitedArticles.end(), sortSummary);

  fieldFilter.setValue("Magazine Entries");
  this->getPublications(sum.magazineEntries, filters, fields);
  std::sort(sum.magazineEntries.begin(), sum.magazineEntries.end(), sortSummary);

  fieldFilter.setValue("Manuals");
  this->getPublications(sum.manuals, filters, fields);
  std::sort(sum.manuals.begin(), sum.manuals.end(), sortSummary);

  fieldFilter.setValue("Monographs");
  this->getPublications(sum.monographs, filters, fields);
  std::sort(sum.monographs.begin(), sum.monographs.end(), sortSummary);

  fieldFilter.setValue("Multimedia");
  this->getPublications(sum.multimedia, filters, fields);
  std::sort(sum.multimedia.begin(), sum.multimedia.end(), sortSummary);

  fieldFilter.setValue("Newspaper Articles");
  this->getPublications(sum.newspaperArticles, filters, fields);
  std::sort(sum.newspaperArticles.begin(), sum.newspaperArticles.end(), sortSummary);

  fieldFilter.setValue("Supervised Student Publications");
  this->getPublications(sum.supervisedStudentPublications, filters, fields);
  std::sort(sum.supervisedStudentPublications.begin(), sum.supervisedStudentPublications.end(), sortSummary);

  fieldFilter.setValue("Websites / Videos");
  this->getPublications(sum.websiteOrVideos, filters, fields);
  std::sort(sum.websiteOrVideos.begin(), sum.websiteOrVideos.end(), sortSummary);

  fieldFilter.setValue("Working Papers");
  this->getPublications(sum.workingPapers, filters, fields);
  std::sort(sum.workingPapers.begin(), sum.workingPapers.end(), sortSummary);
}
// =================================================================================================================
// =================================================================================================================
// =================================================================================================================

/**
 * Filters presentation data. load the filtered data into a new structure that can be used to make a new summary.
 *
 */
void Database::getPresentations(vector< vector<string> >& results,
                                    vector<Filter*>& filterList,
                                    vector<grantsAndFundingField>& fieldList){
  vector<ProcessedPresentationRecord>::iterator recordIter = presentationRecord.begin(); 
  vector<grantsAndFundingField>::iterator fieldIter;
  vector< vector<string> >::iterator resultIter;
  vector<Filter*>::iterator filterIter;

  results.clear();

  bool filterFlag;
  vector<string>* resultFields;
  while (recordIter != presentationRecord.end()){
    /*
     * Code to filter results
     */
    filterIter = filterList.begin();
    filterFlag = false;
    while(filterIter != filterList.end()){
      if ((**filterIter).filter(*recordIter)){
        filterFlag = true;
      }
      filterIter++;
    }

    /*
     * Code to filter results
     */
    if (!filterFlag){
      resultFields = new vector<string>;
      fieldIter = fieldList.begin(); 
      while (fieldIter != fieldList.end()){
        switch (*fieldIter){
          case MEMBER_NAME:             resultFields->push_back(recordIter->getMemberName());
            break;
          case PRIMARY_DOMAIN:          resultFields->push_back(recordIter->getPrimaryDomain());
            break;
          case DATES:                    resultFields->push_back(recordIter->getDate());
            break;
          case TYPE:                    resultFields->push_back(recordIter->getType());
            break;
          case ROLE:                    resultFields->push_back(recordIter->getRole());
            break;
          case TITLE:                   resultFields->push_back(recordIter->getTitle());
            break;
        } 
        fieldIter++;
      } 
      results.push_back(*resultFields);
    }
    recordIter++;
  }
}


/**
 * Changes the summary of the presentation data to reflect only the date range selected by the filter.
 * Sorts the informaton and sums values that need to be summed.
 *
 * @param sum           a struct that holds all the presentation information
 * @param earliestDate  The beginning date in the filter
 * @param latestDate    The ending date in the filter
 *
 */
void Database::presentationSummary(struct presentationSummary& sum, int earliestDate, int latestDate){
  //Define fields vector and iterator
  vector<grantsAndFundingField> fields;
  vector<string>::iterator fieldIter;
  //select fields to retrieve (look in GrantsAndFundingFields.h for a list of fields)
  fields.push_back(MEMBER_NAME);
  fields.push_back(DATES);

// ---------- Choose which records to retrieve by defining filters -----------
  
  //vector has list of filters (pointers) used to query database
  vector<Filter*> filters;
  //define date range (inclusive)
  //Filter by date range (inclusive)
  DateFilter df(earliestDate,latestDate);
  filters.push_back(&df);

  FieldMatchFilter fieldFilter(TYPE, "Invited Lectures");
  filters.push_back(&fieldFilter);
  this->getPresentations(sum.invitedLectures, filters, fields);
  std::sort(sum.invitedLectures.begin(), sum.invitedLectures.end(), sortSummary);

  fieldFilter.setValue("Abstract Presented");
  this->getPresentations(sum.abstractPresented, filters, fields);
  std::sort(sum.abstractPresented.begin(), sum.abstractPresented.end(), sortSummary);

  fieldFilter.setValue("Visiting Professorship");
  this->getPresentations(sum.visitingProfessorship, filters, fields);
  std::sort(sum.visitingProfessorship.begin(), sum.visitingProfessorship.end(), sortSummary);

  fieldFilter.setValue("Plenary Presentation");
  this->getPresentations(sum.plenaryPresentation, filters, fields);
  std::sort(sum.plenaryPresentation.begin(), sum.plenaryPresentation.end(), sortSummary);

  fieldFilter.setValue("Poster Presentation");
  this->getPresentations(sum.posterPresentation, filters, fields);
  std::sort(sum.posterPresentation.begin(), sum.posterPresentation.end(), sortSummary);

  fieldFilter.setValue("Student Presentation");
  this->getPresentations(sum.studentPresentation, filters, fields);
  std::sort(sum.studentPresentation.begin(), sum.studentPresentation.end(), sortSummary);

  /*
  fieldFilter.setValue("Other");
  this->getPresentations(sum.bookChapters, filters, fields);
  std::sort(sum.bookChapters.begin(), sum.bookChapters.end(), sortSummary);
  */

}
void Database::getTeaching(vector< vector<string> >& results,
                                    vector<Filter*>& filterList,
                                    vector<grantsAndFundingField>& fieldList){

  vector<ProcessedTeachingRecord>::iterator recordIter = teachingRecord.begin(); 
  vector<grantsAndFundingField>::iterator fieldIter;
  vector< vector<string> >::iterator resultIter;
  vector<Filter*>::iterator filterIter;

  results.clear();

  bool filterFlag;
  vector<string>* resultFields;
  while (recordIter != teachingRecord.end()){
    /*
     * Code to filter results
     */
    filterIter = filterList.begin();
    filterFlag = false;
    while(filterIter != filterList.end()){
      if ((**filterIter).filter(*recordIter)){
        filterFlag = true;
      }
      filterIter++;
    }

    //if (recordIter->getPeerReviewed().compare("False") == 0)
    //  filterFlag = false;
    /*
     * Code to filter results
     */
    if (!filterFlag){
      resultFields = new vector<string>;
      fieldIter = fieldList.begin(); 
      while (fieldIter != fieldList.end()){
        switch (*fieldIter){
          case MEMBER_NAME:                 resultFields->push_back(recordIter->getMemberName());
            break;
          case PRIMARY_DOMAIN:              resultFields->push_back(recordIter->getPrimaryDomain());
            break;
          case START_DATE:                  resultFields->push_back(recordIter->getStartDate());
            break;
          case END_DATE:                    resultFields->push_back(recordIter->getEndDate());
            break;
          case PROGRAM:                     resultFields->push_back(recordIter->getProgram());
            break;
          case TYPE_OF_COURSE:              resultFields->push_back(recordIter->getTypeOfCourse());
            break;
          case GEOGRAPHICAL_SCOPE:          resultFields->push_back(recordIter->getGeographicalScope());
            break;
          case HOURS_PER_SESSION_OR_WEEK:   resultFields->push_back(recordIter->getHoursPerTeachingSessionOrWeek());
            break;
          case NUMBER_OF_SESSIONS_OR_WEEKS: resultFields->push_back(recordIter->getNumberOfTeachingSessionsOrWeek());
            break;
          case TOTAL_HOURS:                 resultFields->push_back(recordIter->getTotalHours());
        } 
        fieldIter++;
      } 
      results.push_back(*resultFields);
    }
    recordIter++;
  }
}
int Database::getNamePosition(std::vector<std::string>& nameVector, const std::string& name){
    for(int i = 0; i < nameVector.size(); i++){
        if(nameVector[i].compare(name) == 0) return i;
    }
    return -1;
}

std::string Database::addString(std::string& str1, std::string& str2){
    double result = stringToDouble(str1) + stringToDouble(str2);
    std::ostringstream s;
    s << result;
    return s.str();
}

double Database::stringToDouble(std::string& str){
  std::istringstream instream(str);
  double result;
  if (!(instream >> result))
  {
    return 0;
  }
  return result;
}
void Database::teachingSummary(std::vector<std::vector<std::string>>& PME_Name, std::vector<std::vector<std::string>>& UME_Name,std::vector<std::vector<std::string>>& CME_Name,std::vector<std::vector<std::string>>& Other_Name,std::vector<std::vector<double>>& PME_Hour, std::vector<std::vector<double>>& UME_Hour, std::vector<std::vector<double>>& CME_Hour, std::vector<std::vector<double>>& Other_Hour, int earliestDate, int latestDate){
    for(int i = 0; i < this->teachingRecord.size(); i++){
        // if the record is complete
        if(teachingRecord[i].checkRecord()){
            //PME
            if(teachingRecord[i].getProgram().find("Postgraduate") != std::string::npos){
                for(int year = earliestDate; year < latestDate; year++){
                    if(teachingRecord[i].getStartYear() <= year && teachingRecord[i].getEndYear() >= year){
                        std::string name = teachingRecord[i].getMemberName();
                        int namePos = -1;
                        for(int j = 0; j < PME_Name[year-earliestDate].size(); j++){
                            if(PME_Name[year - earliestDate][j].compare(name) == 0) {namePos = j; break;}
                        }
                        if(namePos > 0){
                            std::string hourStr = teachingRecord[i].getTotalHours();
                            double hour2 = stringToDouble(hourStr);
                            PME_Hour[year - earliestDate][namePos] +=  hour2;
                            //std::cout <<  PME_Hour[year - earliestDate][namePos] << "," << PME_Name[year - earliestDate][namePos];
                        }
                        else{
                            std::string hourStr = teachingRecord[i].getTotalHours();
                            PME_Hour[year - earliestDate].push_back(stringToDouble(hourStr));
                            PME_Name[year - earliestDate].push_back(name);
                            //std::cout <<  summary[0][year - earliestDate][1][namePos] << "," << summary[0][year - earliestDate][0][namePos];
                        }

                    }
                }
            }
            //UME
            else if(teachingRecord[i].getProgram().find("Undergraduate") != std::string::npos){
                for(int year = earliestDate; year < latestDate; year++){
                    if(teachingRecord[i].getStartYear() <= year && teachingRecord[i].getEndYear() >= year){
                        std::string name = teachingRecord[i].getMemberName();
                        int namePos = -1;
                        for(int j = 0; j < UME_Name[year-earliestDate].size(); j++){
                            if(UME_Name[year - earliestDate][j].compare(name) == 0) {namePos = j; break;}
                        }
                        if(namePos > 0){
                            std::string hourStr = teachingRecord[i].getTotalHours();
                            double hour2 = stringToDouble(hourStr);
                            UME_Hour[year - earliestDate][namePos] +=  hour2;
                            //std::cout <<  UME_Hour[year - earliestDate][namePos] << "," << UME_Name[year - earliestDate][namePos];
                        }

                        else{
                            std::string hourStr = teachingRecord[i].getTotalHours();
                            UME_Hour[year - earliestDate].push_back(stringToDouble(hourStr));
                            UME_Name[year - earliestDate].push_back(name);
                            //std::cout <<  summary[0][year - earliestDate][1][namePos] << "," << summary[0][year - earliestDate][0][namePos];
                        }

                    }
                }
            }
            //CME
            else if(teachingRecord[i].getProgram().find("Continuing") != std::string::npos){
                for(int year = earliestDate; year < latestDate; year++){
                    if(teachingRecord[i].getStartYear() <= year && teachingRecord[i].getEndYear() >= year){
                        std::string name = teachingRecord[i].getMemberName();
                        int namePos = -1;
                        for(int j = 0; j < CME_Name[year-earliestDate].size(); j++){
                            if(CME_Name[year - earliestDate][j].compare(name) == 0) {namePos = j; break;}
                        }
                        if(namePos > 0){
                            std::string hourStr = teachingRecord[i].getTotalHours();
                            double hour2 = stringToDouble(hourStr);
                            CME_Hour[year - earliestDate][namePos] +=  hour2;
                            //std::cout <<  CME_Hour[year - earliestDate][namePos] << "," << CME_Name[year - earliestDate][namePos];
                        }

                        else{
                            std::string hourStr = teachingRecord[i].getTotalHours();
                            CME_Hour[year - earliestDate].push_back(stringToDouble(hourStr));
                            CME_Name[year - earliestDate].push_back(name);
                            //std::cout <<  summary[0][year - earliestDate][1][namePos] << "," << summary[0][year - earliestDate][0][namePos];
                        }

                    }
                }
            }
            //Other
            else if(teachingRecord[i].getProgram().find("Other") != std::string::npos){
                for(int year = earliestDate; year < latestDate; year++){
                    if(teachingRecord[i].getStartYear() <= year && teachingRecord[i].getEndYear() >= year){
                        std::string name = teachingRecord[i].getMemberName();
                        int namePos = -1;
                        for(int j = 0; j < Other_Name[year-earliestDate].size(); j++){
                            if(Other_Name[year - earliestDate][j].compare(name) == 0) {namePos = j; break;}
                        }
                        if(namePos > 0){
                            std::string hourStr = teachingRecord[i].getTotalHours();
                            double hour2 = stringToDouble(hourStr);
                            Other_Hour[year - earliestDate][namePos] +=  hour2;
                            //std::cout <<  Other_Hour[year - earliestDate][namePos] << "," << Other_Name[year - earliestDate][namePos];
                        }

                        else{
                            std::string hourStr = teachingRecord[i].getTotalHours();
                            Other_Hour[year - earliestDate].push_back(stringToDouble(hourStr));
                            Other_Name[year - earliestDate].push_back(name);
                            //std::cout <<  summary[0][year - earliestDate][1][namePos] << "," << summary[0][year - earliestDate][0][namePos];
                        }


                    }
                }
            }
            else{;}

        }
    }
}
