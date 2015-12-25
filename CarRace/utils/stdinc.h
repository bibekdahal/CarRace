#pragma once

#define GLEW_STATIC
#include "../glew/glew.h"

#include "../GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_inverse.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <functional>
#include <sstream>
#include <stdint.h>
#include <typeindex>
#include <algorithm>

class Exception
{
protected:
    int m_errorType;
    std::string m_errorString;

public:
    Exception(std::string errorString, int errorType) : m_errorString(errorString), m_errorType(errorType) {}
    virtual std::string GetErrorString() { return m_errorString; }
    virtual int GetType() { return m_errorType; }
};

template<class T>
class Manager
{
public:
    unsigned int GetSize() const { return m_objects.size(); }
    unsigned int Add(T* object) { m_objects.push_back(object); return m_objects.size() - 1; }

    void Destroy(T* object)
    {
        auto it = std::find(m_objects.begin(), m_objects.end(), object);
        delete *it;
        *it = NULL;
    }
    /*WARNING INSECURE: USE DESTROY INSTEAD*/
    void Remove(T* object) 
    {
        auto it = std::find(m_objects.begin(), m_objects.end(), object);
        *it = *m_objects.end();
        m_objects.pop_back();
    }
    T* GetObject(unsigned int handle) { return m_objects[handle]; }
    T* operator [] (unsigned int handle) { return GetObject(handle); }

    int Find(T* object)
    {
        for (unsigned i = 0; i < m_objects.size(); ++i)
        if (m_objects[i] == object) return i;
        return -1;
    }
    
    void CleanUp()
    {
        for (unsigned i = 0; i < m_objects.size(); ++i)
            if (m_objects[i]) delete m_objects[i];
        m_objects.clear();
    }

private:
    std::vector<T*> m_objects;
};

class Manageable
{
public:
    virtual ~Manageable() {};
};



// Bit-wise type conversion
template <class Dest, class Source>
inline Dest bit_cast(Source const &source) {
    static_assert(sizeof(Dest) == sizeof(Source), "size of destination and source objects must be equal");
    static_assert(std::is_trivially_copyable<Dest>::value, "destination type must be trivially copyable.");
    static_assert(std::is_trivially_copyable<Source>::value, "source type must be trivially copyable");

    Dest dest;
    std::memcpy(&dest, &source, sizeof(dest));
    return dest;
}