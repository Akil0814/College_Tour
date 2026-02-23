#ifndef ADMIN_H
#define ADMIN_H

#include <iostream>

class Admin
{
public:
    Admin();
    ~Admin();
private:
    unsigned char _key;
};

bool id_verify(std::string i_user_name, std::string i_password);


#endif // ADMIN_H
