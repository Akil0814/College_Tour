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

    int owner_college_id = -1;

    double price = 0.0;
};

struct distance_to
{
    int to_college_id = -1;
    double miles = 0.0;
};


#endif // DATA_TYPES_H
