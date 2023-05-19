#include "widget.h"
#include "ui_widget.h"
#include"SQLanalysis.h"
#include"database.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

//设置用户名
 void Widget:: SetUser(QString username)
 {
     user.username = username;
 }


void Widget::on_Execute_clicked()
{

    ui->textBrowser->clear();
    QString curuser = user.username;

    QString statement = ui->Qstatements->toPlainText();

    QString folder = "D:/download/DBMS1.0/DBMS/log.txt";
    QFile file(folder);



    QRegExp reg("use(?:\\s*)bjtuSQL(?:\\s*);",Qt::CaseInsensitive);
    if(reg.indexIn(statement) >=0)
    {
        DBname = QString();
        ui->textBrowser->setText("RETURN TO databases!");
        return ;
    }

    if(DBname.isNull())
    {
        vector<QString> sqlkey;
        int sqlType = sqlAnalysis(statement,sqlkey);


            switch(sqlType)
             {
             //创建数据库
              case 0:
                if(createDBEntity(curuser,sqlkey.at(0).toUpper()))
                {
                    ui->textBrowser->setText("Create database successfully!");

                    QDateTime current_date_time =QDateTime::currentDateTime();
                    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                    if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                    {
                        QTextStream qts1(&file);
                        qts1<<statement+"#"+current_date<<Qt::endl;
                    }
                    file.close();
                }

                 else
                 {
                    ui->textBrowser->setText("Failed to create!");
                 }
                 break;
                //重命名数据库
            case 1:
                if(renameDBEntity(curuser,sqlkey.at(0).toUpper(),sqlkey.at(1).toUpper()))
                {
                    ui->textBrowser->setText("Rename database successfully!");
                    QDateTime current_date_time =QDateTime::currentDateTime();
                    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                    if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                    {
                        QTextStream qts1(&file);
                        qts1<<statement+"#"+current_date<<Qt::endl;
                    }
                    file.close();
                }
                else
                {
                    ui->textBrowser->setText("Failed to rename databse!");

                }
                break;
                //删除数据库
            case 2:
                if(dropDBEntity(curuser,sqlkey.at(0).toUpper()))
                {
                    ui->textBrowser->setText("Drop database successfully!");
                    QDateTime current_date_time =QDateTime::currentDateTime();
                    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                    if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                    {
                        QTextStream qts1(&file);
                        qts1<<statement+"#"+current_date<<Qt::endl;
                    }
                    file.close();
                }
                else
                {
                     ui->textBrowser->setText("Failed to drop database!");
                }
                break;
            case 3://使用数据库
                if(useDBEntity(curuser,sqlkey.at(0).toUpper()))
                {
                    DBname = sqlkey.at(0).toUpper();
                    ui->textBrowser->setText("Database changed!");


                }
                else
                {
                   ui->textBrowser->setText("Failed to use this database!");
                }
                break;
              //展示数据库
            case 4:
            {
                QString databases;
                if(showdatabases(curuser,databases))
                {
                    ui->textBrowser->setText(databases);
                }
                else
               {
                    ui->textBrowser->setText("Failed to show databases!");

               }
                break;
            }
                //初始化数据库
            case 5:
                if(initDBEntity(curuser,sqlkey.at(0).toUpper()))
                {
                    ui->textBrowser->setText("Initialize database successfully!");
                    QDateTime current_date_time =QDateTime::currentDateTime();
                    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                    if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                    {
                        QTextStream qts1(&file);
                        qts1<<statement+"#"+current_date<<Qt::endl;
                    }
                    file.close();
                }
                else
                {
                     ui->textBrowser->setText("Failed to initialize database!");
                }
                break;

            case -1:
                 ui->textBrowser->setText("syntax error!");
                break;
            default:
                break;


             }
    }
    else
    {
        vector<QString> sqlkey;
        int sqlType = sqlAnalysisTable(statement,sqlkey,DBname);



        switch(sqlType)
        {
            case 0:
            if(createTable(curuser,DBname,sqlkey))
            {
                ui->textBrowser->setText("create table successfully!");
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    QTextStream qts1(&file);
                    qts1<<statement+"#"+current_date<<Qt::endl;
                }
                file.close();
            }
            else
            {
                ui->textBrowser->setText("Failed to create table!");
            }
                 break;

            case 1:
            if(insertIntoTable(curuser,DBname,sqlkey))
            {

                  ui->textBrowser->append("insert into table successfully!");
                  QDateTime current_date_time =QDateTime::currentDateTime();
                  QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                  if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                  {
                      QTextStream qts1(&file);
                      qts1<<statement+"#"+current_date<<Qt::endl;
                  }
                  file.close();
            }
            else
            {
               ui->textBrowser->setText("Failed to insert into table!");
            }


           break;

        case 2:
            if(dropTable(curuser,DBname,sqlkey)){
                ui->textBrowser->setText("drop table successfully!");
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    QTextStream qts1(&file);
                    qts1<<statement+"#"+current_date<<Qt::endl;
                }
                file.close();
            }else{
                ui->textBrowser->setText("Failed to drop table!");
            }

            break;

        case 3:
            if(addField(curuser,DBname,sqlkey)){
                ui->textBrowser->setText("add field successfully!");
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    QTextStream qts1(&file);
                    qts1<<statement+"#"+current_date<<Qt::endl;
                }
                file.close();
            }else{
                ui->textBrowser->setText("Failed to add field!");
            }

            break;
        case 4:
            if(modifyField(curuser,DBname,sqlkey)){
                ui->textBrowser->setText("modify field successfully!");
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    QTextStream qts1(&file);
                    qts1<<statement+"#"+current_date<<Qt::endl;
                }
                file.close();
            }else{
                ui->textBrowser->setText("Failed to modify field!");
            }

            break;

        case 5:
            if(dropField(curuser,DBname,sqlkey)){
                ui->textBrowser->setText("drop field successfully!");
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    QTextStream qts1(&file);
                    qts1<<statement+"#"+current_date<<Qt::endl;
                }
                file.close();
            }else{
                ui->textBrowser->setText("Failed to drop field!");
            }

            break;

        case 6:

            if(selectAll(curuser,DBname,sqlkey).at(0).indexOf("fail to open file")!=-1){
                ui->textBrowser->setText("table does not exist!");
            }else{
                QStringList strList = selectAll(curuser,DBname,sqlkey);
                ui->textBrowser->append(strList.at(0));
                for(int i=2;i<2+strList.at(1).toInt();i++){
                    ui->textBrowser->append(strList.at(i));
                }
            }
            break;

        case 7:

            if(selectFieldAll(curuser,DBname,sqlkey).at(0).indexOf("fail to open file")!=-1){
                ui->textBrowser->setText("table does not exist!");
            }else{
                QStringList strListColumn = selectFieldAll(curuser,DBname,sqlkey);
                ui->textBrowser->append(strListColumn.at(0));
                for(int i=1;i<strListColumn.count();i++){
                    ui->textBrowser->append(strListColumn.at(i));
                }
            }
            break;

        case 8:

            if(selectWhere(curuser,DBname,sqlkey).at(0).indexOf("fail to open file")!=-1){
                ui->textBrowser->setText("table does not exist!");
            }else{
                QStringList strListColumn = selectWhere(curuser,DBname,sqlkey);
                ui->textBrowser->append(strListColumn.at(0));
                for(int i=1;i<strListColumn.count();i++){
                    ui->textBrowser->append(strListColumn.at(i));
                }
            }
            break;

        case 9:
           if(createIndex(curuser,DBname,sqlkey)){
               ui->textBrowser->setText("create index successfully!");
               QDateTime current_date_time =QDateTime::currentDateTime();
               QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
               if(file.open(QIODevice::ReadWrite | QIODevice::Append))
               {
                   QTextStream qts1(&file);
                   qts1<<statement+"#"+current_date<<Qt::endl;
               }
               file.close();
           }else{
               ui->textBrowser->setText("Failed to create index!");
           }
           break;

        case 10:
            if(updateTable(curuser,DBname,sqlkey)){
                ui->textBrowser->setText("update record successfully!");
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    QTextStream qts1(&file);
                    qts1<<statement+"#"+current_date<<Qt::endl;
                }
                file.close();
            }else{
                ui->textBrowser->setText("update to drop record!");
            }

            break;

        case 11:
            if(dropRecord(curuser,DBname,sqlkey)){
                ui->textBrowser->setText("drop record successfully!");
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
                if(file.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    QTextStream qts1(&file);
                    qts1<<statement+"#"+current_date<<Qt::endl;
                }
                file.close();
            }else{
                ui->textBrowser->setText("Failed to drop record!");
            }

            break;
        case -1:
             ui->textBrowser->setText("syntax error!");
            break;

           default:
            break;


        }

    }



}


void Widget::on_pushButton_clicked()
{
    if(backupDBEntity(user.username))
    {
        cout<<"Back up database successfully!"<<endl;
    }else
    {
        cout<<"Fail to back up database!"<<endl;
    }
}





void Widget::on_sql_clicked()
{
    QString fullPath = QFileDialog::getOpenFileName();
        QString filename, filePath;
        QFileInfo fileInfo = QFileInfo(fullPath);
        //文件名
        filename = fileInfo.fileName();

        //绝对路径l
        filePath = fileInfo.absolutePath();

        QString strAll;
        QString strAllColumn;
        QStringList strList;
        QStringList strListTemp;
        QStringList strListColumn;
        QString field = "";

        //打开记录文件
        QString folder = filePath;
        QFile file(folder);

        if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
        {
            //把文件所有信息读出来
            QTextStream stream(&file);
            strAllColumn=stream.readAll();


        }else{
            return;
        }




        file.close();
}
