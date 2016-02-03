/**
  Class implemtation for editing window
  Coded by Team Banana
  **/

#include "edit.h"
#include "ui_edit.h"

/**
 * @brief edit::edit initializer for the editing window
 * @param dboard pointer to the customized window
 * @param fileselected the filename of the file
 * @param parent parent window
 */
edit::edit(DashBoard *dboard, QString fileselected, QWidget *parent) :
    dashb(dboard),
    filename(fileselected),
    QDialog(parent),
    ui(new Ui::edit)
{
    ui->setupUi(this);
    loadSettings();
    fillQtableWiget();
}

/**
 * @brief edit::~edit method to take care of memory when out of scope
 */
edit::~edit()
{
    delete ui;
}

/**
 * @brief edit::loadSettings method to load saved setting, so the reopened program is as same as before
 */
void edit::loadSettings()
{
    QDesktopWidget *desktop =  QApplication::desktop();
    int screenwidth = (desktop->width()-this->width())/2;
    int screenheight = (desktop->height()-this->height())/2;
    QSettings appSetting("TeamBananaSetting", "TeamBanana");
    appSetting.beginGroup("SummaryWindows");
    // potion as the old saved position
    QRect summarySize = appSetting.value("position", QRect(QPoint(screenwidth, screenheight), QSize(this->width(), this->height()))).toRect();
    setGeometry(summarySize);
    appSetting.endGroup();
}

/**
 * @brief edit::saveSettings method to save setting when close the program
 */
void edit::saveSettings()
{
    QSettings appSetting("TeamBananaSetting", "TeamBanana");
    appSetting.beginGroup("SummaryWindows");
    // save the current posiont and size of the window
    appSetting.setValue("position", this->geometry());
    appSetting.endGroup();
}

/**
 * @brief edit::fillQtableWiget method to read the csv file to the editing window
 */
void edit::fillQtableWiget()
{
    QString dataRead;
    //QString headers;
    QFile opencsv(filename);
    QStringList rowofdata;
    QStringList rowdata;

    rowofdata.clear();
    rowdata.clear();
    // try read the file
    if(opencsv.open(QFile::ReadOnly))
    {
        headerReadLine = opencsv.readLine();                 //read the first line as the column header
        dataRead = opencsv.readAll();                       // read all the other lines
        rowofdata = dataRead.split("\n");                   // split the lines into a string list
        opencsv.close();
    }
    // set the number of the column and row the same as in the csv file
    ui->tableWidget->setRowCount(rowofdata.size());
    int columnsize = rowofdata.at(1).split("\",\"").size();
    ui->tableWidget->setColumnCount(columnsize);
    ui->tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableWidget->setHorizontalHeaderLabels(headerReadLine.split(",").replaceInStrings("\r",""));

    // loop for each line
    for(int i=0; i<rowofdata.size(); i++)
    {
        // get the position of the last quotation mark
        int lastIndexQuote = rowofdata.at(i).lastIndexOf("\"");
        // delete the first and the last quotation marks
        // split the fields using "," as delimiter
        rowdata = rowofdata.at(i).mid(1,lastIndexQuote-1).split("\",\"");
        // for csv double quote between quotation marks is single quote
        rowdata.replaceInStrings("\"\"", "\"");
        // for each fields
        for(int j=0; j<rowdata.size(); j++)
        {
            // set the value to each cell in the table
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(rowdata[j]));
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}

/**
 * @brief edit::on_pushButton_2_clicked handler for save button
 */
void edit::on_pushButton_2_clicked()
{
    saveToCSVFile(filename, headerReadLine);
}

/**
 * @brief edit::saveToCSVFile the method to save the edited file to csv
 * @param filename the filename to write in
 * @param headerstring useless, need update to delete
 */
void edit::saveToCSVFile(QString filename, QString headerstring)
{
    // create the file
    QFile file(filename.remove(".csv") + ".csv");
    // if the file is correctly opened
    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        // open the stream
        QTextStream data( &file );
        QStringList strList;
        // print the column title line
        for( int c = 0; c < ui->tableWidget->columnCount(); c++ )
        {
            strList << ui->tableWidget->horizontalHeaderItem(c)->data(Qt::DisplayRole).toString();
        }
        data << strList.join(",");
        // save each line
        for( int r = 0; r < ui->tableWidget->rowCount(); r++ )
        {
            strList.clear();
            // save each fields
            for( int c = 0; c < ui->tableWidget->columnCount(); c++ )
            {
                QTableWidgetItem* item = ui->tableWidget->item(r,c);
                if(!item){
                    ui->tableWidget->setItem(r,c,new QTableWidgetItem(""));
                }
                // make the quotation mark in the fields to double quotation marks
                strList << "\"" + ui->tableWidget->item(r,c)->text().replace("\"","\"\"")+ "\"";
            }
            // write the line to the csv file
            data << strList.join( "," )+"\n";
        }
        ui->label->setText(tr("File saved successfully."));
        file.close();
    }
}

/**
 * @brief edit::on_pushButton_clicked handler for save button
 */
void edit::on_pushButton_clicked()
{
    saveSettings();
    this->close();
    dashb->show();
}
