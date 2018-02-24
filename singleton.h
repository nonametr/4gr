#ifndef SINGLETON_H
#define SINGLETON_H

#include <assert.h>

template <class type>
class Singleton
{
public:
    Singleton(Singleton const&) = delete; // Copy construct
    Singleton(Singleton&&) = delete; // Move construct
    Singleton& operator=(Singleton const&) = delete; // Copy assign
    Singleton& operator=(Singleton&&) = delete; // Move assign

    virtual ~Singleton()
    {
        this->_singleton_ptr = 0;
    }

    inline static type& getSingleton()
    {
        assert(_singleton_ptr);
        return *_singleton_ptr;
    }
    inline static type* getSingletonPtr()
    {
        assert(_singleton_ptr);
        return _singleton_ptr;
    }
    inline static type* create()
    {
        /// If you hit this assert, this singleton already exists -- you can't create another one!
        assert(_singleton_ptr == 0);

        new type();
        return _singleton_ptr;
    }

protected:
    /// Constructor
    Singleton() noexcept
    {
        static type singleton;

        /// If you hit this assert, this singleton already exists -- you can't create another one!
        assert(_singleton_ptr == 0);

        _singleton_ptr = &singleton;
        this->_singleton_ptr = static_cast<type*>(this);
    }

private:
    /// Singleton pointer, must be set to 0 prior to creating the object
    static type* _singleton_ptr;
};

/// Should be placed in the appropriate .cpp file somewhere
#define initialiseSingleton(type) \
    template <>                   \
    type* Singleton<type>::_singleton_ptr;
/// Should be placed in the appropriate .cpp file somewhere for tamplate singleton class
#define initialiseTemplateSingleton(temp, type) \
    template <>                                 \
    temp<type>* Singleton<temp<type>>::_singleton_ptr = nullptr

#endif // SINGLETON_H
