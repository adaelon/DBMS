#include"SQLanalysis.h"
#include<QDebug>


int sqlAnalysis(QString sql,vector<QString> &sqlkey)
{
    //存放正则表达式
    vector<QString> regVector;
    //对应的正则表达式的捕获数量
    vector<int> regSize;

    //匹配创建数据库的正则:0
    regVector.push_back(QString("create(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));
    regSize.push_back(1);
    //匹配重命名数据库的正则:1
    regVector.push_back(QString("rename(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)to(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));
    regSize.push_back(2);
     //匹配删除数据库的正则:2
    regVector.push_back(QString("drop(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));
    regSize.push_back(1);
     //匹配使用数据库的正则:3
    regVector.push_back(QString("use(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));
    regSize.push_back(1);
    //匹配展示数据库的正则:4
    regVector.push_back(QString("show(?:\\s*)databases(?:\\s*);"));
    regSize.push_back(0);
    //匹配初始化数据库的正则:5
    regVector.push_back(QString("initialize(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));
    regSize.push_back(1);


    //开始解析sql语句
    for (unsigned int i = 0; i < regVector.size(); i++)
    {
        QRegExp reg(regVector[i],Qt::CaseInsensitive);
        //传入的sql语句和每个正则表达式进行匹配
        //匹配的第一个字符的位置，从0开始，若为-1则不匹配
        int pos = reg.indexIn(sql);
        //若匹配
        if (pos >= 0)
        {
            for (int j = 0; j < regSize[i]; j++)
            {
                //将对应的值放入sqlkey
                sqlkey.push_back(reg.cap(j + 1));
            }
            //返回数据库操作对应的编号，从0开始
            return i;
        }
    }
    //没有对应的操作，返回-1，表示该类型操作暂未支持，或者sql语句错误
    return -1;
}


int sqlAnalysisTable(QString sql,vector<QString> &sqlkey,QString DBname)
{
    //存放正则表达式
    vector<QString> regVector;
    //匹配创建表的正则：0
    regVector.push_back(QString("create\\s+table\\s+(\\w+)\\s*\\((.*)\\)\\s*;"));
    //匹配向表中插入数据的正则：1
    regVector.push_back(QString("insert\\s+into\\s+([^\\s]+)\\s*\\(([^\\)]+)\\)\\s*values\\s*\\(([^\\)]+)\\)\\s*;"));
    //匹配删除表的正则：2
    regVector.push_back(QString("drop(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));
    //匹配添加字段的正则：3
    regVector.push_back(QString("alter\\s+table\\s+([^\\s]+)\\s+add\\s+column\\s+([^\\s]+)\\s+([^\\s]+)\\s*;"));
    //匹配修改字段的正则：4
    regVector.push_back(QString("alter\\s+table\\s+([^\\s]+)\\s+modify\\s+column\\s+([^\\s]+)\\s+([^\\s]+)\\s*;"));
    //匹配删除字段的正则：5
    regVector.push_back(QString("alter\\s+table\\s+([^\\s]+)\\s+drop\\s+column\\s+([^\\s]+)\\s+([^\\s]+)\\s*;"));
    //匹配创建查询所有记录的正则：6
    regVector.push_back(QString("select\\s+\\*\\s+from\\s+([^\\s]+)*;"));
    //匹配查询指定字段的所有记录的正则：7
    regVector.push_back(QString("select\\s*([^\\)]+)\\s*from\\s+([^\\s]+)\\s*;"));
    //匹配查询指定字段带where条件的记录的正则：8
    regVector.push_back(QString("select\\s*([^\\)]+)\\s*from\\s+([^\\s]+)\\s+where\\s+([^\\s]+)\\s+\\=\\s+([^\\s]+)\\s*;"));
    //匹配创建索引的正则：9
    regVector.push_back(QString("create\\s+index\\s+(\\w+)\\s+on\\s+(\\w+)\\s*\\(([^\\)]+)\\)\\s*;"));
    //匹配修改记录的正则：10
    regVector.push_back(QString("update\\s+([^\\s]+)\\s+set\\s*\\((.*)\\)\\s+where\\s+([^\\s]+)\\s+\\=\\s+([^\\s]+)\\s*;"));
    //匹配删除记录的正则：11
    regVector.push_back(QString("drop\\s+from\\s+([^\\s]+)\\s+where\\s+([^\\s]+)\\s+\\=\\s+([^\\s]+)\\s*;"));

    //开始解析sql语句

        //是否符合创建表的语法
        QRegularExpression re(regVector[0],QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(sql);
        if(match.hasMatch())
        {
            QString tableName = match.captured(1);
            QString columns = match.captured(2);

            sqlkey.push_back(tableName);

            QRegularExpression columnRe("\\s*(\\w+)\\s+(\\w+)\\s*");
            QRegularExpressionMatchIterator i = columnRe.globalMatch(columns);
            while (i.hasNext()) {
                QRegularExpressionMatch columnMatch = i.next();
                QString columnName = columnMatch.captured(1);
                QString columnType = columnMatch.captured(2);
                sqlkey.push_back(columnName);
                sqlkey.push_back(columnType);


            }
            return 0;
        }

        //是否符合插入表的语法

       QRegularExpression re1(regVector[1],QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match1 = re1.match(sql);

        if (match1.hasMatch()) {
            QString tableName = match1.captured(1);
               QStringList columns = match1.captured(2).split(",");
               QStringList values = match1.captured(3).split(",");
             //存储表名
            sqlkey.push_back(tableName);
            //存储列数量
            sqlkey.push_back(QString::number(columns.size()));
            //存储列名
            for(int i=0;i<columns.size();i++)
               sqlkey.push_back(columns.at(i).simplified());
            //存储数值
            for(int i=0;i<values.size();i++)
               sqlkey.push_back(values.at(i).simplified());

          return 1;
        }





        //是否符合添加字段的语法

       QRegularExpression re3(regVector[3],QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match3 = re3.match(sql);

        if (match3.hasMatch()) {
            QString tableName = match3.captured(1);
            QString column = match3.captured(2);

            QString field = match3.captured(3);

             //存储表名
            sqlkey.push_back(tableName);
            //存储列数量
            sqlkey.push_back(column);

            sqlkey.push_back(field);

          return 3;
        }

        //是否符合修改字段的语法

       QRegularExpression re4(regVector[4],QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match4 = re4.match(sql);

        if (match4.hasMatch()) {
            QString tableName = match4.captured(1);
            QString column = match4.captured(2);

            QString field = match4.captured(3);

             //存储表名
            sqlkey.push_back(tableName);
            //存储列数量
            sqlkey.push_back(column);

            sqlkey.push_back(field);

          return 4;
        }

        //是否符合删除字段的语法

       QRegularExpression re5(regVector[5],QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match5 = re5.match(sql);

        if (match5.hasMatch()) {
            QString tableName = match5.captured(1);
            QString column = match5.captured(2);

            QString field = match5.captured(3);

             //存储表名
            sqlkey.push_back(tableName);
            //存储列数量
            sqlkey.push_back(column);

            sqlkey.push_back(field);

          return 5;
        }


        //是否符合查询所有记录的语法
        QRegularExpression re6(regVector[6],QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match6 = re6.match(sql);
        if(match6.hasMatch())
        {
            QString tableName = match6.captured(1);


            sqlkey.push_back(tableName);


            return 6;
        }



        //是否符合查询指定字段所有记录的语法

       QRegularExpression re7(regVector[7],QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match7 = re7.match(sql);

        if (match7.hasMatch()) {
            QString tableName = match7.captured(2);
               QStringList columns = match7.captured(1).split(",");

             //存储表名
            sqlkey.push_back(tableName);
            //存储列数量
            sqlkey.push_back(QString::number(columns.size()));
            //存储列名
            for(int i=0;i<columns.size();i++)
               sqlkey.push_back(columns.at(i).simplified());


          return 7;
        }


        //是否符合查询指定字段带where条件的记录的语法

       QRegularExpression re8(regVector[8],QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match8 = re8.match(sql);

        if (match8.hasMatch()) {
            QString tableName = match8.captured(2);
               QStringList columns = match8.captured(1).split(",");
               QString columnName = match8.captured(3);
               QString columnJudge = match8.captured(4);

             //存储表名
            sqlkey.push_back(tableName);
            //存储列数量
            sqlkey.push_back(QString::number(columns.size()));
            //存储列名
            for(int i=0;i<columns.size();i++)
               sqlkey.push_back(columns.at(i).simplified());
            sqlkey.push_back(columnName);
            sqlkey.push_back(columnJudge);


          return 8;
        }

        //是否符合创建索引的语法
        QRegularExpression re9(regVector[9],QRegularExpression::CaseInsensitiveOption);

         QRegularExpressionMatch match9 = re9.match(sql);
         if (match9.hasMatch()) {
             // 获取索引名
             QString indexName = match9.captured(1);
             //获取表名
             QString tableName = match9.captured(2);
             // 获取列名
             QString columns = match9.captured(3);

             // 存取索引名称
             sqlkey.push_back(indexName);
             //存储表名
             sqlkey.push_back(tableName);
             //存储列数量
             sqlkey.push_back(columns);

           return 9;
         }


         //是否符合修改记录的语法

        QRegularExpression re10(regVector[10],QRegularExpression::CaseInsensitiveOption);

         QRegularExpressionMatch match10 = re10.match(sql);

         if (match10.hasMatch()) {
             QString tableName = match10.captured(1);
                QStringList columns = match10.captured(2).split(",");
                QStringList values = match10.captured(3).split(" ");
                QString columnName = match10.captured(4);
                QString columnJudge = match10.captured(5);

              //存储表名
             sqlkey.push_back(tableName);
             //存储列数量
             sqlkey.push_back(QString::number(columns.size()));
             //存储列名
             for(int i=0;i<columns.size();i++)
                sqlkey.push_back(columns.at(i).simplified());
             //存储数值
             for(int i=0;i<values.size();i++)
                sqlkey.push_back(values.at(i).simplified());

             sqlkey.push_back(columnName);
             sqlkey.push_back(columnJudge);

           return 10;
         }

         //删除记录的语法

        QRegularExpression re11(regVector[11],QRegularExpression::CaseInsensitiveOption);

         QRegularExpressionMatch match11 = re11.match(sql);

         if (match11.hasMatch()) {
             QString tableName = match11.captured(1);

                QString columnName = match11.captured(2);
                QString columnJudge = match11.captured(3);

              //存储表名
             sqlkey.push_back(tableName);

             sqlkey.push_back(columnName);
             sqlkey.push_back(columnJudge);


           return 11;
         }

        for (unsigned int i = 0; i < regVector.size(); i++)
        {
            QRegExp reg(regVector[i],Qt::CaseInsensitive);
            //传入的sql语句和每个正则表达式进行匹配
            //匹配的第一个字符的位置，从0开始，若为-1则不匹配
            int pos = reg.indexIn(sql);
            //若匹配
            if (pos >= 0)
            {
                for (int j = 0; j < 1; j++)
                {
                    //将对应的值放入sqlkey
                    sqlkey.push_back(reg.cap(j + 1));
                }
                //返回数据库操作对应的编号，从0开始
                return 2;
            }
        }

    return -1;

}


int sqlAnalysisQuery(QString sql,vector<QString> &sqlkey,QString DBname)
{
    //存放正则表达式
    vector<QString> regVector;
    //匹配创建查询所有记录的正则：0
    regVector.push_back(QString("select\\s+\\*\\s+from\\s+([^\\s]+)*;"));

    //开始解析sql语句

        //是否符合查询所有记录的语法
        QRegularExpression re(regVector[0],QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(sql);
        if(match.hasMatch())
        {
            QString tableName = match.captured(1);


            sqlkey.push_back(tableName);


            return 0;
        }


    return -1;

}
