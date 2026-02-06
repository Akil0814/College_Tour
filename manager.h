#ifndef MANAGER_H
#define MANAGER_H

// Generic singleton helper: gives you one shared instance of T.
/*
    Manager<T> is a generic "singleton helper" base class.

    Goal:
    - Make exactly ONE shared instance of some class T
    - Let everyone access it through T::instance()

    How it's meant to be used:
    class SomethingManager : public Manager<SomethingManager>
    {
        friend class Manager<SomethingManager>; // allows Manager to call SomethingManager's constructor
    protected:
        SomethingManager();
        ~SomethingManager();
    public:
        void SomeFunc()
        .....
    };

    Then:
        auto manager_ptr=SomethingManager::instance();
        manager_ptr->SomeFunc();
     or:
        SomethingManager::instance()->SomeFunc();
*/

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

