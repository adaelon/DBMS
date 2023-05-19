#include "table.h"
#include<QDir>
#include<QTextStream>


//属性判断：char 返回1，number 返回2，都不是返回0
int judge(QString attribute)
{
    if(attribute.compare("char")==0 )
    {
            return 1;
    }else if(attribute.compare("number")==0)
    {
        return 2;
    }else
    {
        return 0;
    }
}

//输入表名和属性名，返回属性类型和位置
column judgeColumn(QString user,QString DBname,QString tableName,QString attributeName)
{
    column col;
    //打开文件
     QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+tableName+"/column.txt";
      QFile file(folder);
     if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
         qDebug()<<"fail to open file!";
     }
     else
     {

         //遍历文件，按行读取
           QTextStream in1(&file);
           int number = -1;
           QString line;
           QStringList temp;
           while (!in1.atEnd())
           {
            number++;
            line = in1.readLine();
            temp = line.split("#");
            //属性名匹配上，返回该属性的类型和位置
            if(temp[0].compare(attributeName)==0)
            {
               col.attributeType=judge(temp[1]);
               col.attributeLocation=number;
               return col;
            }

           }

     }
     qDebug()<<"fail to insert!";
     col.attributeLocation=-1;
     col.attributeType=-1;
     return col;


}

int judgeNumber(QString user,QString DBname,QString tableName)
{
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+tableName+"/column.txt";
     QFile file(folder);
     if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
         qDebug()<<"fail to open file!";
     }
     else
     {
         //遍历文件，按行读取
           QTextStream in1(&file);
           QString line;
           int number = 0;
           while (!in1.atEnd())
           {
            number++;
            line = in1.readLine();
           }
           return number;
     }
}

//创建表
bool createTable(QString user,QString DBname,vector<QString> sqlkey)
{
    //在对应的数据库文件夹下创建表文件夹
    sqlkey.at(0) = sqlkey.at(0).toUpper();
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0);
    QDir *temp = new QDir;
    bool exist = temp->exists(folder);
    if(!exist)
    {
        if(!temp->mkdir(folder))
            return false;
    }
    //在表文件夹下创建column.txt，保存表的列名和属性
    folder += "/column.txt";
    QFile file(folder);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        return false;
    }
    else
    {
         QTextStream qts1(&file);
         //获取属性的类别
         int attributeType;
         for(unsigned int i = 1;i<sqlkey.size();i = i+2)
         {
             attributeType = judge(sqlkey.at(i+1));
             if(attributeType > 0)
             {
              qts1<<sqlkey.at(i)+"#"+sqlkey.at(i+1)<<Qt::endl;
             }else
             {
                  return false;
             }

         }

    }
    //database.txt中对应数据库里的表数量加一
    folder = "D:/download/DBMS1.0/DBMS/"+user+"/database.txt";
    QFile file1(folder);
    QStringList list;

    //读取文件
    if(!file1.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return false;
    }
    QTextStream in1(&file1);
    vector<QString> lines;
     int databaseNum = in1.readLine().toInt();
     int number;
     for(int i=0;i<databaseNum;i++)
     {
         QString temp;
         temp = in1.readLine();
         list = temp.split("#");
         //找到对应数据库
         if(list.at(0)==DBname)
         {
          number = list.at(2).toInt();
          number++;
          lines.push_back(list.at(0)+"#"+list.at(1)+"#"+QString::number(number,10));

         }else
         {
          lines.push_back(temp);
         }
     }
     file1.close();

     file1.open(QIODevice::WriteOnly|QIODevice::Text);
     //十进制数字转为QString
     QString num = QString::number(databaseNum,10);
     QTextStream qts(&file1);
     //写入该用户拥有的数据库数量
     qts<<num<<Qt::endl;
     //写入其他的数据
     for (vector<QString>::iterator iter=lines.begin();iter!=lines.end();iter++)
     {
         qts<<(*iter)<<Qt::endl;
     }


     //创建记录文件，第一行写上当前记录数：0
     QString folder_temp = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/record.txt";
     QFile file_temp(folder_temp);
     if(!file_temp.open(QIODevice::WriteOnly|QIODevice::Text))
     {
         return false;
     }
     else
     {
          QTextStream qts_temp(&file_temp);
           qts_temp<<QString::number(0);
     }

     return true;




}

//插入数值
bool insertIntoTable(QString user,QString DBname,vector<QString> sqlkey)
{
    sqlkey.at(0) = sqlkey.at(0).toUpper();
    //打开记录文件
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/record.txt";
    QFile file(folder);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        return false;
    }
    //记录数+1
    QString firstLine = file.readLine();
        file.seek(0);
        int temp = firstLine.toInt();
        temp++;
        firstLine = QString::number(temp);
        file.write(firstLine.toUtf8());
        file.close();
    //追加
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        return false;
    }
    else
    {
         QTextStream qts1(&file);
         //获取列的数量
         int number = sqlkey.at(1).toInt();
         qts1<<Qt::endl;

         //将列的名称转为对应的属性和位置
         vector<column> col;
         for(int i=0;i<number;i++)
         {
             col.push_back(judgeColumn(user,DBname,sqlkey.at(0),sqlkey.at(2+i)));
         }
         //获取总的列的数量
         int numberOfAttribute = judgeNumber(user,DBname,sqlkey.at(0));

         //处理每一行数据
         QStringList row;
         for(unsigned int i = 2+number;i<sqlkey.size();i = i+number)
         {
             for(int k=0;k<numberOfAttribute;k++)
             {
                 row <<" ";
             }

             for( int j=0;j<number;j++)
             {
                 //获得插入值的位置和数据类型
                 int location = col[j].attributeLocation;
                 int type = col[j].attributeType;
                 //检查数据类型
                 if(type==2)
                 {
                     bool isNum;
                     sqlkey.at(i+j).toDouble(&isNum);
                     if(!isNum)
                     {
                         return false;
                     }
                 }
                 row[location]=sqlkey.at(i+j);
             }
             QString tempString="";
             for(int k=0;k<numberOfAttribute;k++)
             {
                   tempString += row[k];
                   tempString +="#";
             }
             tempString = tempString.left(tempString.size() - 1);
             qts1<<tempString;
         }


    }
    file.close();
    return true;

}
//删除表
bool dropTable(QString user,QString DBname,vector<QString> sqlkey){
    //删除表的文件夹
    sqlkey.at(0) = sqlkey.at(0).toUpper();
    QString folderTable = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0);
    DeleteFileOrFolder(folderTable);

    //database.txt中对应数据库里的表数量减一
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/database.txt";
    QFile file1(folder);
    QStringList list;

    //读取文件
    if(!file1.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return false;
    }
    QTextStream in1(&file1);
    vector<QString> lines;
     int databaseNum = in1.readLine().toInt();
     int number;
     for(int i=0;i<databaseNum;i++)
     {
         QString temp;
         temp = in1.readLine();
         list = temp.split("#");
         //找到对应数据库
         if(list.at(0)==DBname)
         {
          number = list.at(2).toInt();
          number--;
          lines.push_back(list.at(0)+"#"+list.at(1)+"#"+QString::number(number,10));

         }else
         {
          lines.push_back(temp);
         }
     }
     file1.close();

     file1.open(QIODevice::WriteOnly|QIODevice::Text);
     //十进制数字转为QString
     QString num = QString::number(databaseNum,10);
     QTextStream qts(&file1);
     //写入该用户拥有的数据库数量
     qts<<num<<Qt::endl;
     //写入其他的数据
     for (vector<QString>::iterator iter=lines.begin();iter!=lines.end();iter++)
     {
         qts<<(*iter)<<Qt::endl;
     }

     return true;

}
//添加字段
bool addField(QString user,QString DBname,vector<QString> sqlkey){

    sqlkey.at(0) = sqlkey.at(0).toUpper();
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/column.txt";
    QFile file(folder);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        return false;
    }
    else
    {
         QTextStream qts1(&file);
         qts1<<sqlkey.at(1)+"#"+sqlkey.at(2)<<Qt::endl;

    }

    file.close();

    return true;
}

//修改字段
bool modifyField(QString user,QString DBname,vector<QString> sqlkey){

    QString strAll;
    QStringList strList;
    sqlkey.at(0) = sqlkey.at(0).toUpper();
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/column.txt";
    QFile file(folder);
    if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        //把文件所有信息读出来
        QTextStream stream(&file);
        strAll=stream.readAll();
    }
    file.close();


    QFile writeFile(folder);
     if(writeFile.open(QIODevice::WriteOnly|QIODevice::Text))
     {
             QTextStream stream(&writeFile);
             strList=strAll.split("\n");           //以换行符为基准分割文本
             for(int i=0;i<strList.count();i++)    //遍历每一行
             {

                 if(strList.at(i).indexOf(sqlkey.at(1))!=-1)
                 {

                     QString tempStr=strList.at(i);
                     tempStr.replace(0,tempStr.length(),sqlkey.at(1)+"#"+sqlkey.at(2));
                     stream<<tempStr<<'\n';
                 }
                 //如果没有找到要替换的内容，照常写入
                 else
                 {
                     if(i==strList.count()-1)
                      {
                         stream<<strList.at(i);
                      }
                      else
                      {
                         stream<<strList.at(i)<<'\n';
                      }
                 }
             }
     }
     writeFile.close();

    return true;
}


//删除字段
bool dropField(QString user,QString DBname,vector<QString> sqlkey){

    QString strAll;
    QStringList strList;
    sqlkey.at(0) = sqlkey.at(0).toUpper();
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/column.txt";
    QFile file(folder);
    if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        //把文件所有信息读出来
        QTextStream stream(&file);
        strAll=stream.readAll();
    }
    file.close();


    QFile writeFile(folder);
     if(writeFile.open(QIODevice::WriteOnly|QIODevice::Text))
     {
             QTextStream stream(&writeFile);
             strList=strAll.split("\n");           //以换行符为基准分割文本
             for(int i=0;i<strList.count();i++)    //遍历每一行
             {

                 if(strList.at(i).indexOf(sqlkey.at(1)+"#"+sqlkey.at(2))!=-1)
                 {
                     continue;
                 }
                 //如果没有找到要删除的内容，照常写入
                 else
                 {
                     if(i==strList.count()-1)
                      {
                         stream<<strList.at(i);
                      }
                      else
                      {
                         stream<<strList.at(i)<<'\n';
                      }
                 }
             }
     }
     writeFile.close();

    return true;
}

//查询所有记录
QStringList selectAll(QString user,QString DBname,vector<QString> sqlkey){

    QString strAll;
    QString strAllColumn;
    QStringList strList;
    QStringList strListTemp;
    QStringList strListColumn;
    QString field = "";
    sqlkey.at(0) = sqlkey.at(0).toUpper();
    //打开字段文件
    QString folderColumn = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/column.txt";
    QFile file1(folderColumn);
    if(file1.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        //把文件所有信息读出来
        QTextStream stream(&file1);
        strAllColumn=stream.readAll();
    }

    strListTemp = strAllColumn.split("\n");
    for(int i=0;i<strListTemp.count();i++){
        QStringList temp = strListTemp.at(i).split("#");
        if(temp.at(0)!="") strListColumn.push_back(temp.at(0));
    }

    for(int i=0;i<strListColumn.count();i++){
        if(i==strListColumn.count()-1){
            field+=strListColumn.at(i);
        }else{
            field+=strListColumn.at(i)+"#";
        }
    }






    //打开记录文件
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/record.txt";
    QFile file(folder);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {

        QStringList failReturn;
        QString fail="fail to open file";
        failReturn.push_back(fail);
        return failReturn;
    }
    file.close();

    if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        //把文件所有信息读出来
        QTextStream stream(&file);
        strAll=stream.readAll();
    }

    strList=strAll.split("\n");
    strList.push_front(field);


    file.close();


    return strList;

}

//查询指定字段的所有记录
QStringList selectFieldAll(QString user,QString DBname,vector<QString> sqlkey){
    QString strAll;
    QString strAllColumn;
    QStringList strList;
    QStringList strListTemp;
    QStringList strListColumn;
    QString field = "";
    sqlkey.at(0) = sqlkey.at(0).toUpper();







    //打开记录文件
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/record.txt";
    QFile file(folder);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {

        QStringList failReturn;
        QString fail="fail to open file";
        failReturn.push_back(fail);
        return failReturn;
    }
    file.close();






    if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        //把文件所有信息读出来
        QTextStream stream(&file);
        strAllColumn=stream.readAll();


    }
    //获取列的数量
    int number = sqlkey.at(1).toInt();


    //将列的名称转为对应的属性和位置
    vector<column> col;
    for(int i=0;i<number;i++)
    {
        col.push_back(judgeColumn(user,DBname,sqlkey.at(0),sqlkey.at(2+i)));
    }
    //获取总的列的数量
    int numberOfAttribute = judgeNumber(user,DBname,sqlkey.at(0));


    strListTemp = strAllColumn.split("\n");


    for(int i=1;i<strListTemp.count();i++){
        QStringList temp = strListTemp.at(i).split("#");
        QString temp1="";
        for(int j =0 ;j<number;j++){
            if(temp.at(0)!="") temp1+=temp.at(col[j].attributeLocation)+"#";

        }
        if(i!=strListTemp.count()-1 )temp1+="\n";
        strListColumn.push_back(temp1);
    }

    for(int j =0 ;j<number;j++){
        field+=sqlkey.at(2+j);
        field+="#";
    }

    strListColumn.push_front(field);

    file.close();


    return strListColumn;
}


//查询指定字段和where条件的记录
QStringList selectWhere(QString user,QString DBname,vector<QString> sqlkey){
    QString strAll;
    QString strAllColumn;
    QStringList strList;
    QStringList strListTemp;
    QStringList strListColumn;
    QString field = "";
    sqlkey.at(0) = sqlkey.at(0).toUpper();







    //打开记录文件
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/record.txt";
    QFile file(folder);

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {

        QStringList failReturn;
        QString fail="fail to open file";
        failReturn.push_back(fail);
        return failReturn;
    }
    file.close();






    if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        //把文件所有信息读出来
        QTextStream stream(&file);
        strAllColumn=stream.readAll();


    }
    //获取列的数量
    int number = sqlkey.at(1).toInt();


    //将列的名称转为对应的属性和位置
    vector<column> col;
    for(int i=0;i<number;i++)
    {
        col.push_back(judgeColumn(user,DBname,sqlkey.at(0),sqlkey.at(2+i)));
    }


    QString colmnName =sqlkey.at(number+2);
    QString colmnJudge =sqlkey.at(number+3);

    column target = judgeColumn(user,DBname,sqlkey.at(0),colmnName);

    //获取总的列的数量
    int numberOfAttribute = judgeNumber(user,DBname,sqlkey.at(0));


    strListTemp = strAllColumn.split("\n");


    for(int i=1;i<strListTemp.count();i++){
        QStringList temp = strListTemp.at(i).split("#");
        QString temp1="";
        int sign=0;
        for(int j =0 ;j<number;j++){
            if(temp.at(0)!=""&&temp.at(target.attributeLocation)==colmnJudge){
                temp1+=temp.at(col[j].attributeLocation)+"#";
                sign=1;
            }

        }
        if(i!=strListTemp.count()-1 && sign==1)temp1+="\n";
        if(sign ==1) strListColumn.push_back(temp1);
    }

    for(int j =0 ;j<number;j++){
        field+=sqlkey.at(2+j);
        field+="#";
    }

    strListColumn.push_front(field);

    file.close();


    return strListColumn;
}


bool createIndex(QString user,QString DBname,vector<QString> sqlkey)
{
        //获取相应列的位置
        column col = judgeColumn(user, DBname,sqlkey.at(1),sqlkey.at(2));
        int number = col.attributeLocation;
        //打开文件，获取相应数据，构建B树
        QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(1)+"/record.txt";
        QFile file(folder);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug()<<"fail to open file!";
            return false;
        }
        else
        {
            BTree bTree(3);
            QMap<QString, int> keyValuePairs;
            //遍历文件，按行读取
              QTextStream in1(&file);
              //跳过第一行
              in1.readLine();
              int lineLocation = 1;
              QString line;
              QStringList temp;
              while (!in1.atEnd())
              {
               lineLocation++;
               line = in1.readLine();
               temp = line.split("#");
               //相应数据存入B树
               bTree.insert(temp[number]);
               keyValuePairs.insert(temp[number],lineLocation);
              }
              //写入文件
              bTree.writeToFile("D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(1)+"/"+sqlkey.at(0)+".txt");
              bTree.writeValue("D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(1)+"/"+sqlkey.at(0)+"value.txt",keyValuePairs);

        }
        return true;

}

bool updateTable(QString user,QString DBname,vector<QString> sqlkey){
    QString strAll;
    QString strAllColumn;
    QStringList strList;
    QStringList strListTemp;
    QStringList strListColumn;
    QString field = "";
    sqlkey.at(0) = sqlkey.at(0).toUpper();







    //打开记录文件
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/record.txt";
    QFile file(folder);

    if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        //把文件所有信息读出来
        QTextStream stream(&file);
        strAllColumn=stream.readAll();


    }else{
        return false;
    }

    file.close();

    //获取列的数量
    int number = sqlkey.at(1).toInt();
    //将列的名称转为对应的属性和位置
    vector<column> col;
    for(int i=0;i<number;i++)
    {
        qDebug()<<sqlkey.at(2+i);
        col.push_back(judgeColumn(user,DBname,sqlkey.at(0),sqlkey.at(2+i)));
    }

    vector<QString> afterValue;
    for(int i=number;i<2*number;i++)
    {
        afterValue.push_back(sqlkey.at(2+i));
    }


    QString colmnName =sqlkey.at(2*number+2);
    QString colmnJudge =sqlkey.at(2*number+3);


    column target = judgeColumn(user,DBname,sqlkey.at(0),colmnName);

    //获取总的列的数量
    int numberOfAttribute = judgeNumber(user,DBname,sqlkey.at(0));


    strListTemp = strAllColumn.split("\n");



    for(int i=1;i<strListTemp.count();i++){
        QStringList temp = strListTemp.at(i).split("#");
        QString temp1="";
        int sign=0;
        for(int j =0 ;j<numberOfAttribute;j++){


            if(temp.at(0)!=""&&temp.at(target.attributeLocation)==colmnJudge){


                    if(j==col[j].attributeLocation&&j!=numberOfAttribute-1){
                        temp1+=afterValue[j]+"#";
                    }else{
                        temp1+=afterValue[j];
                    }



            }else{
                if(j!=numberOfAttribute-1) temp1+=temp.at(j)+"#";
                else temp1+=temp.at(j);
            }

        }
        if(i!=strListTemp.count()-1) temp1+="\n";
        strListColumn.push_back(temp1);
    }
    QString temo = strAllColumn.at(0);
    int tempInt = temo.toInt();
    strListColumn.push_front(QString::number(tempInt));


    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {

        return false;

    }else{

        QTextStream stream(&file);


        for(int i=0;i<strListColumn.count();i++){
             stream<<strListColumn.at(i)<<Qt::endl;
        }




    }

    file.close();




    return true;
}
bool dropRecord(QString user,QString DBname,vector<QString> sqlkey){
    QString strAll;
    QString strAllColumn;
    QStringList strList;
    QStringList strListTemp;
    QStringList strListColumn;
    QString field = "";
    sqlkey.at(0) = sqlkey.at(0).toUpper();







    //打开记录文件
    QString folder = "D:/download/DBMS1.0/DBMS/"+user+"/"+DBname+"/"+sqlkey.at(0)+"/record.txt";
    QFile file(folder);

    if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        //把文件所有信息读出来
        QTextStream stream(&file);
        strAllColumn=stream.readAll();


    }else{
        return false;
    }

    file.close();


    QString colmnName =sqlkey.at(1);
    QString colmnJudge =sqlkey.at(2);

    column target = judgeColumn(user,DBname,sqlkey.at(0),colmnName);

    //获取总的列的数量
    int numberOfAttribute = judgeNumber(user,DBname,sqlkey.at(0));


    strListTemp = strAllColumn.split("\n");



    for(int i=1;i<strListTemp.count();i++){
        QStringList temp = strListTemp.at(i).split("#");
        QString temp1="";
        int sign=0;
        for(int j =0 ;j<numberOfAttribute;j++){

            if(j!=numberOfAttribute-1) temp1+=temp.at(j)+"#";
            else temp1+=temp.at(j);

            if(temp.at(0)!=""&&temp.at(target.attributeLocation)==colmnJudge){

                sign=1;

            }

        }
        if(i!=strListTemp.count()-1)temp1+="\n";
        if(sign ==0) strListColumn.push_back(temp1);
    }

    QString temo = strAllColumn.at(0);
    int tempInt = temo.toInt()-1;
    strListColumn.push_front(QString::number(tempInt));


    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {

        return false;

    }else{

        QTextStream stream(&file);


        for(int i=0;i<strListColumn.count();i++){
             stream<<strListColumn.at(i)<<Qt::endl;
        }




    }

    file.close();




    return true;
}
