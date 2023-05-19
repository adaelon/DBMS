#ifndef TABLE_H
#define TABLE_H
#include <vector>
#include <set>
#include <QCoreApplication>
#include<QDebug>
#include"btree.h"
using namespace std;
typedef struct
{
    //字段名
    QString name;
    //字段类型
    QString type;
}Field;

//定义属性的结构
struct column{
    int attributeType;
    int attributeLocation;
};

typedef struct
{
    //约束条件名
    QString name;
    //约束条件类型，0代表主键，1代表外键，2代表unique，3代表not null，4代表check，5代表default
    int type;
    //施加约束的列，主键允许加于多列

    vector<QString> colnos;
    //外键需要
    //参考的表名
    QString outertable;
    //参考的列名
    QString outerprino;

    //check需要
    //0代表是连续型范围，存在最大和最小值；1代表是离散型范围，用集合表示，值从集合的元素中选择
    int checktype;
    //最大值
    QString max;
    //最小值
    QString min;
    //取值的集合
    set<QString> valueset;

    //defalut需要
    //默认值
    QString defaultvalue;
}Constraint;


//创建表
bool createTable(QString user,QString DBname,vector<QString> sqlkey);
//插入数值
bool insertIntoTable(QString user,QString DBname,vector<QString> sqlkey);
//删除表
bool dropTable(QString user,QString DBname,vector<QString> sqlkey);
//添加字段
bool addField(QString user,QString DBname,vector<QString> sqlkey);
//修改字段
bool modifyField(QString user,QString DBname,vector<QString> sqlkey);
//删除字段
bool dropField(QString user,QString DBname,vector<QString> sqlkey);
//查询所有记录
QStringList selectAll(QString user,QString DBname,vector<QString> sqlkey);
//查询指定字段的所有记录
QStringList selectFieldAll(QString user,QString DBname,vector<QString> sqlkey);
//查询指定字段和where条件的记录
QStringList selectWhere(QString user,QString DBname,vector<QString> sqlkey);
//创建索引
bool createIndex(QString user,QString DBname,vector<QString> sqlkey);
//修改记录
bool updateTable(QString user,QString DBname,vector<QString> sqlkey);
//删除记录
bool dropRecord(QString user,QString DBname,vector<QString> sqlkey);
//删除文件或文件夹
bool DeleteFileOrFolder(QString &strPath);
//属性判断：char 返回1，number 返回2，都不是返回0
int judge(QString attribute);
//输入表名和属性名，返回属性类型和位置
column judgeColumn(QString user,QString DBname,QString tableName,QString attributeName);

int judgeNumber(QString user,QString DBname,QString tableName);




#endif // TABLE_H
