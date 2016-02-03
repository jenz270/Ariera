/**
  CS3307 Team Banana

  summary.cpp shows the summary of the excel data to the user
**/

#include "summary.h"
#include "ui_summary.h"



/**
 * Summary constructor that runs through each file in the list and checks the filename for
 * keywords to determine the data type then loads the data into a tree that can be
 * displayed by the widget to the client as a summary. Also creates tabs to switch between the different
 * data types and initializes the graphing unit.
 *
 * @param qs            String to hold the filename from the list
 * @param listititem    list of filenames
 * @param dashb         Dashboard object that the summary uses
 * @param parent        The file manager window which is parent to the summary window
 */
Summary::Summary(QString qs, vector<string> listititem, DashBoard *dashb, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Summary),
    stringTransfered(qs),
    dashBoardData(dashb),
    listVectorTransfered(listititem),
    PresentationSummary(NULL),
    GrantSummary(NULL),
    PublicationSummary(NULL),
    TeachingSummary(NULL)
{
    ui->setupUi(this);
    loadSettings();
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(0);

    graph = ui->graphWidget;
    graphTitleInit();

    foreach (string s, listVectorTransfered)
    {
        if(QString::fromStdString(s).contains("Grant"))
        {
            GrantSummary = new TreeWidgetForGrant();
            GrantSummary->AddRoot("", "", "");
            GrantSummary->fillTreeWidgetForGrant(db, 1000, 3000); //shows all year, use filter to display desired time frame
            ui->tabWidget->addTab(GrantSummary->getCreateTreeWidget(), "Grants and Clinical");
        }
        else if(QString::fromStdString(s).contains("Presentation"))
        {
            PresentationSummary = new TreeWidgetForPresentation();
            PresentationSummary->AddRoot("", "");
            PresentationSummary->fillTreeWidgetForPresentation(db, 1000, 3000);;
            ui->tabWidget->addTab(PresentationSummary->getCreateTreeWidget(), "Presentations");
        }
        else if(QString::fromStdString(s).contains("Publication"))
        {
            PublicationSummary = new TreeWidgetForPublication();
            PublicationSummary->AddRoot("", "");
            PublicationSummary->fillTreeWidgetForPublication(db, 1000, 3000);;
            ui->tabWidget->addTab(PublicationSummary->getCreateTreeWidget(), "Publications");
        }
        else if(QString::fromStdString(s).contains("Teaching"))
        {
            TeachingSummary = new TreeWidgetForTeaching();
            int startYear = ui->lineEdit->text().toInt(), endYear = ui->lineEdit_2->text().toInt(), different = endYear-startYear;
            std::vector<std::vector<std::string>> PME_Name(different, std::vector<std::string>()), UME_Name(different, std::vector<std::string>()), CME_Name(different, std::vector<std::string>()), Other_Name(different, std::vector<std::string>());
            std::vector<std::vector<double>> PME_Time(different, std::vector<double>()), UME_Time(different, std::vector<double>()), CME_Time(different, std::vector<double>()), Other_Time(different, std::vector<double>());
            db.teachingSummary(PME_Name, UME_Name, CME_Name, Other_Name,PME_Time, UME_Time, CME_Time, Other_Time, startYear, endYear );
            double totalPME = TeachingSummary->calcTotal(PME_Name, PME_Time);
            TeachingSummary->fillTreeWidgetForTeaching("PME", totalPME, PME_Name, PME_Time,startYear);

            double totalUME = TeachingSummary->calcTotal(UME_Name, UME_Time);
            TeachingSummary->fillTreeWidgetForTeaching("UME", totalUME, UME_Name, UME_Time,startYear);

            double totalCME = TeachingSummary->calcTotal(CME_Name, CME_Time);
            TeachingSummary->fillTreeWidgetForTeaching("CME", totalCME, CME_Name, CME_Time,startYear);

            double totalOTHER = TeachingSummary->calcTotal(Other_Name, Other_Time);
            TeachingSummary->fillTreeWidgetForTeaching("OTHER", totalOTHER, Other_Name, Other_Time,startYear);

            ui->tabWidget->addTab(TeachingSummary->getCreateTreeWidget(), "Teaching");
        }
    }

}



/**
 *
 * summary deconstructor used when client returns to file manager. deletes all components of
 * the summary to free up space.
 */
Summary::~Summary()
{
    delete GrantSummary;
    delete PublicationSummary;
    delete PresentationSummary;
    delete ui;
}



/**
 * Generates a graph once the "Graph" button on the Summary page is clicked
 * NOTE: Currently generates its own data - not tied to Summary; should be refactored
 *
 * @brief Summary::on_pushButton_2_clicked create the graph form.
 */
void Summary::graphs()
{
    ui->tabWidget->currentIndex();
    vector< vector<string> > results;
    vector<grantsAndFundingField> fields;
    FieldMatchFilter nameMatch(MEMBER_NAME,"");

    //Define filter of start dates within 2000 and 2001
    vector<Filter*> filters;
    ui->lineEdit->setValidator(new QIntValidator(1000, 3000, this));
    ui->lineEdit_2->setValidator(new QIntValidator(1000, 3000, this));
    int earliest = ui->lineEdit->text().toInt();
    int latest = ui->lineEdit_2->text().toInt();

    if(earliest > latest) //throw error and do not create graph
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Invalid Range");
        messageBox.setFixedSize(600,200);
    }
    else
    {
        //if range left blank set earliest to 1970, and latest to current year
        if(latest == 0)
        {
            time_t t = std::time(0);
            struct tm * now = localtime(&t);
            earliest = 1970;
            latest = now->tm_year + 1900;
            ui->lineEdit->setText(QString::number(earliest));
            ui->lineEdit_2->setText(QString::number(latest));
        }
        //title of the selected tab
        QString tabSelected = ui->tabWidget->tabText(ui->tabWidget->currentIndex());

        //Apply date filters
        DateFilter df(earliest,latest);
        filters.push_back(&df);

        int dateChoice = ui->dateSelector->currentText().toInt();
        int dataType = 0;
        if(tabSelected.contains("Grant",Qt::CaseInsensitive) == true){
            filters = chosenName(filters,&nameMatch);
            fields.push_back(START_DATE);
            fields.push_back(TOTAL_AMOUNT);
            fields.push_back(ROLE);
            db.getGrantsAndFunding(results,filters,fields);
            dataType = GRANT;
        }
        else if(tabSelected.contains("Pub",Qt::CaseInsensitive) == true){
            filters = chosenName(filters,&nameMatch);
            fields.push_back(STATUS_DATE);
            fields.push_back(TYPE);
            fields.push_back(ROLE);
            db.getPublications(results,filters,fields);
            dataType = PUBLICATION;
        }
        else if(tabSelected.contains("Teach",Qt::CaseInsensitive) == true){
            filters = chosenName(filters,&nameMatch);
            fields.push_back(START_DATE);
            fields.push_back(PROGRAM);
            fields.push_back(TYPE_OF_COURSE);
            fields.push_back(HOURS_PER_SESSION_OR_WEEK);
            fields.push_back(NUMBER_OF_SESSIONS_OR_WEEKS);
            fields.push_back(TOTAL_HOURS);
            fields.push_back(MEMBER_NAME);
            db.getTeaching(results,filters,fields);
            dataType = TEACHING;
        }
        else if(tabSelected.contains("Pres",Qt::CaseInsensitive) == true){
            filters = chosenName(filters,&nameMatch);
            fields.push_back(DATES);
            fields.push_back(TYPE);
            db.getPresentations(results,filters,fields);
            dataType = PRESENTATION;
        }
        else{
            exit(EXIT_FAILURE);
        }

        graph->clearPlottables();

        GraphHelper graphHelp(ui->graphSelector->currentIndex(),
                              ui->graphChoice->currentIndex(),
                              dataType,dateChoice,earliest,latest);
        graphHelp.makeGraph(graph,results);
        QString titleName = "";
        if(QString::compare(ui->nameBox->currentText(),"All Names") != 0)
            titleName = " for " + ui->nameBox->currentText();
        graphHelp.setTitle(graphTitle,ui->graphChoice->currentText() + titleName);
        graph->replot();
    }
}



/**
 * When the filter button is clicked this function is called which applies the
 * current date filter to the data displayed. If date is invalid a message box
 * is produced to inform the client.
 */
void Summary::on_pushButton_clicked()
{
    int tabindex = ui->tabWidget->currentIndex();
    QString currentTabText = ui->tabWidget->tabText(tabindex);

    ui->lineEdit->setValidator(new QIntValidator(1000, 3000, this));
    ui->lineEdit_2->setValidator(new QIntValidator(1000, 3000, this));
    int earliest = ui->lineEdit->text().toInt();
    int latest = ui->lineEdit_2->text().toInt();
    if(earliest > latest)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Invalid input !");
        messageBox.setFixedSize(600,200);
    }
    else
    {
        qDebug() << ui->tabWidget->tabText(tabindex);
        if(/*(GrantSummary !=NULL) &&*/ (ui->tabWidget->tabText(tabindex).contains("Grant")) && currentTabText.contains("Grant"))
        {
            GrantSummary->getCreateTreeWidget()->clear();
            GrantSummary->AddRoot("", "", "");
            GrantSummary->fillTreeWidgetForGrant(db, earliest, latest);
        }
        if(/*(PublicationSummary != NULL) &&*/ (ui->tabWidget->tabText(tabindex).contains("Publication")) && currentTabText.contains("Publication"))
        {
            PublicationSummary->getCreateTreeWidget()->clear();
            PublicationSummary->AddRoot("", "");
            PublicationSummary->fillTreeWidgetForPublication(db, earliest, latest);
        }
        if(/*(PresentationSummary != NULL) && */(ui->tabWidget->tabText(tabindex).contains("Presentation")) && currentTabText.contains("Presentation"))
        {
            PresentationSummary->getCreateTreeWidget()->clear();
            PresentationSummary->AddRoot("", "");
            PresentationSummary->fillTreeWidgetForPresentation(db, earliest, latest);
        }
        if(/*(PresentationSummary != NULL) && */(ui->tabWidget->tabText(tabindex).contains("Teaching")) && currentTabText.contains("Teaching"))
        {
            TeachingSummary->getCreateTreeWidget()->clear();
            int startYear = ui->lineEdit->text().toInt(), endYear = ui->lineEdit_2->text().toInt(), different = endYear-startYear;
            std::vector<std::vector<std::string>> PME_Name(different, std::vector<std::string>()), UME_Name(different, std::vector<std::string>()), CME_Name(different, std::vector<std::string>()), Other_Name(different, std::vector<std::string>());
            std::vector<std::vector<double>> PME_Time(different, std::vector<double>()), UME_Time(different, std::vector<double>()), CME_Time(different, std::vector<double>()), Other_Time(different, std::vector<double>());
            db.teachingSummary(PME_Name, UME_Name, CME_Name, Other_Name,PME_Time, UME_Time, CME_Time, Other_Time, startYear, endYear );
            double totalPME = TeachingSummary->calcTotal(PME_Name, PME_Time);
            TeachingSummary->fillTreeWidgetForTeaching("PME", totalPME, PME_Name, PME_Time,startYear);

            double totalUME = TeachingSummary->calcTotal(UME_Name, UME_Time);
            TeachingSummary->fillTreeWidgetForTeaching("UME", totalUME, UME_Name, UME_Time,startYear);

            double totalCME = TeachingSummary->calcTotal(CME_Name, CME_Time);
            TeachingSummary->fillTreeWidgetForTeaching("CME", totalCME, CME_Name, CME_Time,startYear);

            double totalOTHER = TeachingSummary->calcTotal(Other_Name, Other_Time);
            TeachingSummary->fillTreeWidgetForTeaching("OTHER", totalOTHER, Other_Name, Other_Time,startYear);
        }
    }
    graphs();
    //ui->tabWidget->setCurrentIndex(tabindex);
}

/**
 * Exports the summary table as a pdf
 */
void Summary::on_pushButton_2_clicked()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    QString filename = QFileDialog::getSaveFileName(0, "Export Summary as PDF", "", ".pdf");
    filename.append(".pdf");
    printer.setOutputFileName(filename);

    QPainter painter;
    painter.begin(&printer);

    double xscale = printer.pageRect().width()/ui->tabWidget->width();
    double yscale = printer.pageRect().height()/ui->tabWidget->height();
    double scale = qMin(xscale, yscale);
    painter.scale(scale, scale);

    ui->tabWidget->currentWidget()->render(&painter);
    painter.end();
}

/**
 * Closes the summary window and returns to the file manager
 */
void Summary::on_pushButton_3_clicked()
{
    saveSettings();
    this->close();
    dashBoardData->show();
}


/**
 * Creates a message box with options yes and no which confirms the
 * user's desire to leave the summary.
 */
void Summary::on_pushButton_4_clicked()
{
    QMessageBox msgBox;
    msgBox.setInformativeText("Are you sure you would like to quit?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret)
    {
        case QMessageBox::Yes:
            this->close();
            break;
        case QMessageBox::No:
            break;
    }
}


/**
 * Clears the filter data that has been entered and returns to the
 * default settings where all data is shown.
 */
void Summary::on_pushButton_5_clicked()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit->setText("1975");
    ui->lineEdit_2->setText("2015");

    if(GrantSummary != NULL)
    {
        GrantSummary->getCreateTreeWidget()->clear();
        GrantSummary->AddRoot("", "", "");
        GrantSummary->fillTreeWidgetForGrant(db, 1000, 3000);
    }

    if(PublicationSummary != NULL)
    {
        PublicationSummary->getCreateTreeWidget()->clear();
        PublicationSummary->AddRoot("", "");
        PublicationSummary->fillTreeWidgetForPublication(db,1000, 3000);
    }

    if(PresentationSummary != NULL)
    {
        PresentationSummary->getCreateTreeWidget()->clear();
        PresentationSummary->AddRoot("", "");
        PresentationSummary->fillTreeWidgetForPresentation(db,1000, 3000);
    }
    if(TeachingSummary != NULL)
    {
        TeachingSummary->getCreateTreeWidget()->clear();
        int startYear = ui->lineEdit->text().toInt(), endYear = ui->lineEdit_2->text().toInt(), different = endYear-startYear;
        std::vector<std::vector<std::string>> PME_Name(different, std::vector<std::string>()), UME_Name(different, std::vector<std::string>()), CME_Name(different, std::vector<std::string>()), Other_Name(different, std::vector<std::string>());
        std::vector<std::vector<double>> PME_Time(different, std::vector<double>()), UME_Time(different, std::vector<double>()), CME_Time(different, std::vector<double>()), Other_Time(different, std::vector<double>());
        db.teachingSummary(PME_Name, UME_Name, CME_Name, Other_Name,PME_Time, UME_Time, CME_Time, Other_Time, startYear, endYear );
        double totalPME = TeachingSummary->calcTotal(PME_Name, PME_Time);
        TeachingSummary->fillTreeWidgetForTeaching("PME", totalPME, PME_Name, PME_Time,startYear);

        double totalUME = TeachingSummary->calcTotal(UME_Name, UME_Time);
        TeachingSummary->fillTreeWidgetForTeaching("UME", totalUME, UME_Name, UME_Time,startYear);

        double totalCME = TeachingSummary->calcTotal(CME_Name, CME_Time);
        TeachingSummary->fillTreeWidgetForTeaching("CME", totalCME, CME_Name, CME_Time,startYear);

        double totalOTHER = TeachingSummary->calcTotal(Other_Name, Other_Time);
        TeachingSummary->fillTreeWidgetForTeaching("OTHER", totalOTHER, Other_Name, Other_Time,startYear);
    }
}


/**
 * Creates a pdf of the current graph then asked the user for the
 * filename to save it as.
 */
void Summary::on_pushButton_6_clicked()
{
    // Option 1: Export to PDF
    QString filename = QFileDialog::getSaveFileName(0,"Export Graph as PDF","",".pdf");
    filename.append(".pdf");
    graph->savePdf(filename);

}

/**
 * Initializes the graph with a blank title title
 */
void Summary::graphTitleInit()
{
   graph->plotLayout()->insertRow(0);
   graphTitle = new QCPPlotTitle(graph,"");
   graph->plotLayout()->addElement(0, 0, graphTitle);
}

/**
 * Controls what happens when a new tab is selected in the summary window.
 * Depending on the tab selected, changes the displayed summary data to the
 * appropriate type. Also changes the labels and data in the graphing section
 * to match.
 */
void Summary::on_tabWidget_currentChanged(int index)
{
    ui->graphChoice->clear();
    ui->dateSelector->setEnabled(false);
    vector<grantsAndFundingField> nameFields;
    nameFields.push_back(MEMBER_NAME);
    vector<vector<string>> nameResults;
    vector<Filter*> filters;
    int earliest = ui->lineEdit->text().toInt();
    int latest = ui->lineEdit_2->text().toInt();
    if(latest == 0){
        time_t t = std::time(0);
        struct tm * now = localtime(&t);
        earliest = 1970;
        latest = now->tm_year + 1900;
        ui->lineEdit->setText(QString::number(earliest));
        ui->lineEdit_2->setText(QString::number(latest));
    }
    DateFilter df(earliest,latest);
    filters.push_back(&df);

    if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Pub",Qt::CaseInsensitive) == true){
        ui->graphChoice->addItem("Number of Publications by Year");
        ui->graphChoice->addItem("Number of Publications by Type");
        ui->graphChoice->addItem("Number of each Role by Type for Single Year");
        nameFields.push_back(STATUS_DATE);
        db.getPublications(nameResults,filters,nameFields);
        processChoices(nameResults);
        graphs();
    }
    else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Grant",Qt::CaseInsensitive) == true){
        ui->graphChoice->addItem("Amount of Grant Funding by Year");
        ui->graphChoice->addItem("Number of Grants per Year");
        ui->graphChoice->addItem("Roles taken in Grants for Single year");
        nameFields.push_back(START_DATE);
        db.getGrantsAndFunding(nameResults,filters,nameFields);
        processChoices(nameResults);
        graphs();

    }
    else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Pres",Qt::CaseInsensitive) == true){
        ui->graphChoice->addItem("Number of Presentations by Year");
        ui->graphChoice->addItem("Number of Presentations by Type");
        nameFields.push_back(DATES);
        db.getPresentations(nameResults,filters,nameFields);
        processChoices(nameResults);
        graphs();

    }
    else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Teach",Qt::CaseInsensitive) == true){
        ui->graphChoice->addItem("Number of Teaching Programs by Year");
        ui->graphChoice->addItem("Number of Teaching Programs by Type");
        ui->graphChoice->addItem("Number of Teaching Courses by Type");
        ui->graphChoice->addItem("Number of Teaching Courses by Year");
        ui->graphChoice->addItem("Number of Hours per Teaching Session by Year");
        ui->graphChoice->addItem("Number of Teaching Sessions by Year");
        ui->graphChoice->addItem("Total Number of Hours by Year");
        ui->graphChoice->addItem("Number of Hours by Member Name");
        nameFields.push_back(START_DATE);
        db.getTeaching(nameResults,filters,nameFields);
        processChoices(nameResults);
        graphs();
    }

}

/**
 * Goes through all of the different names in the current data type to populate
 * a selection list to be used for graphing.
 */
void Summary::processChoices(std::vector<std::vector<std::string> > &results){
    std::vector< std::vector<std::string> >::iterator resultIter;
    std::vector<std::string>::iterator fieldIter;
    QVector<QString> names;
    QVector<QString> years;
    resultIter = results.begin();
    ui->nameBox->clear();
    ui->nameBox->addItem("All Names");
    ui->dateSelector->clear();
    while(resultIter != results.end())
    {
      fieldIter = resultIter->begin();
      QString name = QString::fromStdString(*(fieldIter));
      QString year = QString::fromStdString(*(fieldIter + 1));

      if(QString::compare(name,"") != 0 && names.contains(name) == false)
          names.push_back(name);

      if(QString::compare(year,"") != 0)
          years.push_back(year);

      resultIter++;
    }

    names = sortNames(names);
    for(int i=0;i<names.size();i++){
        ui->nameBox->addItem(names[i]);
    }

    years = cleanDates(years);
    for(int i=0;i<years.size();i++){
        ui->dateSelector->addItem(years[i]);
    }
}

/**
 * Gets all dates and populates a list of them so the client can chose which
 * they want for the graph.
 */
void Summary::processDate(std::vector<std::vector<std::string> > &results){
    std::vector< std::vector<std::string> >::iterator resultIter;
    std::vector<std::string>::iterator fieldIter;
    QVector<QString> years;
    resultIter = results.begin();
    ui->dateSelector->clear();
    while(resultIter != results.end())
    {
      fieldIter = resultIter->begin();
      QString year = QString::fromStdString(*(fieldIter));

      if(QString::compare(year,"") != 0)
          years.push_back(year);

      resultIter++;
    }

    years = cleanDates(years);
    for(int i=0;i<years.size();i++){
        ui->dateSelector->addItem(years[i]);
    }
}

/**
 * Filters the data shown based on the name given. if All Names is given then
 * it diplays all data.
 *
 * @param nameMatch     name of person to be filtered
 */
vector<Filter*> Summary::chosenName(vector<Filter*> filters,FieldMatchFilter* nameMatch){
    QString selected = ui->nameBox->currentText();

    if(QString::compare(selected,"All Names",Qt::CaseInsensitive) != 0){
      std::string name = selected.toStdString();
      nameMatch->setValue(name);
      filters.push_back(nameMatch);
    }
    return filters;
}


/**
 * Retreives the stored previous location of the summary window and starts it at
 * that position. Also initialized the size of the window.
 */
void Summary::loadSettings()
{
    QDesktopWidget *desktop =  QApplication::desktop();
    int screenwidth = (desktop->width()-this->width())/2;
    int screenheight = (desktop->height()-this->height())/2;

    QSettings appSetting("TeamBananaSetting", "TeamBanana");
    appSetting.beginGroup("SummaryWindows");
    QRect summarySize = appSetting.value("position", QRect(QPoint(screenwidth, screenheight), QSize(this->width(), this->height()))).toRect();
    setGeometry(summarySize);
    appSetting.endGroup();
}

/**
 * Saves the window position so that is can be used next time the summary window is opened
 */
void Summary::saveSettings()
{
    QSettings appSetting("TeamBananaSetting", "TeamBanana");
    appSetting.beginGroup("SummaryWindows");
    appSetting.setValue("position", this->geometry());
    appSetting.endGroup();
}

/*
 *
 */
QVector<QString> Summary::cleanDates(QVector<QString> years){
    QString temp;
    QVector<QString> orderedYear;
    for(int i =0;i<years.size();i++){
        temp = years[i];
        if(temp.size() > 4){
            temp = temp.left(4);
        }
        if(orderedYear.contains(temp)== false)
            orderedYear.push_back(temp);
    }

    for(int i =0;i<orderedYear.size()-1;i++){
        if(orderedYear[i].toInt() < orderedYear[i+1].toInt()){
            qSwap(orderedYear[i],orderedYear[i+1]);
            i =-1;
        }
    }
    return orderedYear;
}

/**
 * Sorts the names in the list so that the name selection for the graphing is in
 * allphabetical order.
 *
 * @param names     list of names to be sorted
 */
QVector<QString> Summary::sortNames(QVector<QString> names){
    QVector<QString> ordered = names;

    for(int i =0;i<ordered.size()-1;i++){
            if(ordered[i] > ordered[i+1]){
            qSwap(ordered[i],ordered[i+1]);
            i =-1;
        }
    }
    return ordered;
}


/**
 * Gets called when the name box is selected. Displays a list of names for the
 * client to choose in order to filter the graphing.
 *
 */
void Summary::on_nameBox_activated(int index)
{
        FieldMatchFilter nameMatch(MEMBER_NAME, "");
        vector<grantsAndFundingField> nameFields;;
        vector<vector<string>> nameResults;
        vector<Filter*> filters;

        int earliest = ui->lineEdit->text().toInt();
        int latest = ui->lineEdit_2->text().toInt();
        if(latest == 0){
            time_t t = std::time(0);
            struct tm * now = localtime(&t);
            earliest = 1970;
            latest = now->tm_year + 1900;
            ui->lineEdit->setText(QString::number(earliest));
            ui->lineEdit_2->setText(QString::number(latest));
        }
        DateFilter df(earliest,latest);
        filters.push_back(&df);
        filters = chosenName(filters, &nameMatch);

        if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Pub",Qt::CaseInsensitive) == true){
            nameFields.push_back(STATUS_DATE);
            db.getPublications(nameResults,filters,nameFields);
        }
        else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Grant",Qt::CaseInsensitive) == true){
            nameFields.push_back(START_DATE);
            db.getGrantsAndFunding(nameResults,filters,nameFields);

        }
        else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Pres",Qt::CaseInsensitive) == true){
            nameFields.push_back(DATES);
            db.getPresentations(nameResults,filters,nameFields);
        }
        else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Teach",Qt::CaseInsensitive) == true){
            nameFields.push_back(START_DATE);
            db.getTeaching(nameResults,filters,nameFields);
        }

        processDate(nameResults);
        graphs();
    graphs();
}

/**
 * checks the current information displayed in the summary and makes the graph match that data.
 */
void Summary::on_graphChoice_activated(int index)
{
    if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Grant",Qt::CaseInsensitive) == true){
        if(index == 2)
            ui->dateSelector->setEnabled(true);
        else
            ui->dateSelector->setEnabled(false);
    }
    else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Pub",Qt::CaseInsensitive) == true){
        if(index == 1 || index == 2)
            ui->dateSelector->setEnabled(true);
        else
            ui->dateSelector->setEnabled(false);
    }
    else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Pres",Qt::CaseInsensitive) == true){
        if(index == 1)
            ui->dateSelector->setEnabled(true);
        else
            ui->dateSelector->setEnabled(false);
    }
    else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).contains("Teach",Qt::CaseInsensitive) == true){
        if(index == 1 || index == 2 || index == 7)
            ui->dateSelector->setEnabled(true);
        else
            ui->dateSelector->setEnabled(false);
    }
    graphs();
}

/**
 * Recreates the graph when the type of graph is changed.
 */
void Summary::on_graphSelector_activated(int index)
{
    graphs();
}

/**
 * recreates the graph when the date range is changed
 */
void Summary::on_dateSelector_activated(int index)
{
    graphs();
}
