#ifndef DATA_TYPES_H
#define DATA_TYPES_H
#include <QString>

struct college
{
    int college_id = -1;
    QString name;
};

struct souvenir
{
    int souvenir_id= -1;
    QString souvenir_name;

    int college_belong_id = -1;
    QString college_belong_name;

    double price = 0.0;
};

struct neighbor_distance
{
    int to_college_id = -1;
    QString to_college_name;
    double miles_to_college = 0.0;
};


#endif // DATA_TYPES_H
