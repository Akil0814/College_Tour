#ifndef MANAGER_H
#define MANAGER_H

template<typename T>
class Manager
{
public:
    // Get the singleton instance (create it on first call).
    static T* instance()
    {
        if (!manager) manager = new T(); // lazy init, never deleted here
        return manager;
    }

private:
    static T* manager; // per-type static pointer

protected:
    Manager() = default;
    ~Manager() = default;
    Manager(const Manager&) = delete;            // no copy
    Manager& operator=(const Manager&) = delete; // no assign
};

// Static member definition (starts as null).
template<typename T>
T* Manager<T>::manager = nullptr;

#endif // MANAGER_H

