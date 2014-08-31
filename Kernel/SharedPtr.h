//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef UTILITIES_SHAREDPTR_H
#define UTILITIES_SHAREDPTR_H

//******************************************************************************
// This class implements smart pointer that automatically calls AddRef and Release.
//******************************************************************************
template <typename T>
class RefPtr
{
public:
    RefPtr();
    RefPtr(T* p_pPtr);
    RefPtr(const RefPtr& p_Ptr);
    
    RefPtr& operator=(T* p_pPtr);
    RefPtr& operator=(const RefPtr& p_Ptr);
    
    T& operator*();
    const T& operator*() const;

private:
    T*      m_pPtr;     // The pointer we hold.

    void Connect(T* p_pPtr);
    void Disconnect();
}

#define IMPLEMENT_REFPTR \
    private int _m_RefCount;

//******************************************************************************
// Constructor.
//******************************************************************************
template <typename T>
inline RefPtr<T>::RefPtr()
:   m_pPtr(0)
{
}

//******************************************************************************
// Constructor.
//******************************************************************************
template <typename T>
inline RefPtr::RefPtr(T* p_pPtr)
:   m_pPtr(p_pPtr)
{
    Connect();
}

//******************************************************************************
// Constructor.
//******************************************************************************
template <typename T>
inline RefPtr<T>::RefPtr(const RefPtr<T>& p_pPtr)
:   m_pPtr(p_Ptr.m_pPtr)
{
    Connect();
}

//******************************************************************************
// Assignment operator.
// p_pPtr - The pointer to assign.
//******************************************************************************
template <typename T>
inline RefPtr<T>& RefPtr::operator=(T* p_pPtr)
{
    Disconnect();
    m_pPtr = p_pPtr;
    Connect();
    return *this;
}

//******************************************************************************
// Assignment operator.
// p_Ptr - The pointer to assign.
//******************************************************************************
template <typename T>
inline RefPtr<T>& RefPtr<T>::operator=(const RefPtr<T>& p_Ptr)
{
    Disconnect();
    Connect(p_Ptr.m_pPtr);
    return *this;
}

//******************************************************************************
// Comparison operator.
// p_pPtr - The pointer to compare to.
//******************************************************************************
template <typename T>
inline bool RefPtr<T>::operator==(const T* p_pPtr)
{
    return p_pPtr == m_pPtr;
}

//******************************************************************************
// Comparison operator.
// p_Ptr - The pointer to compare to.
//******************************************************************************
template <typename T>
inline bool RefPtr<T>::operator==(const RefPtr<T>& p_Ptr)
{
    return p_Ptr.m_pPtr == m_pPtr;
}

//******************************************************************************
// Dereference operator.
// Returns a pointer on the object we hold.
//******************************************************************************
template <typename T>
inline T& RefPtr<T>::operator*()
{
    assert(m_pPtr != 0);
    return *m_pPtr;
}

//******************************************************************************
// Dereference operator.
// Returns a pointer on the object we hold.
//******************************************************************************
template <typename T>
inline const T& RefPtr<T>::operator*() const
{
    assert(m_pPtr != 0);
    return *m_pPtr;
}

//******************************************************************************
// Connects the object to a pointer.
//******************************************************************************
template <typename T>
inline void RefPtr<T>::Connect()
{
    if (m_pPtr != 0) {
        m_pPtr->AddRef();
    }
}

//******************************************************************************
// Disconnects the object from any pointer we may hold.
//******************************************************************************
template <typename T>
inline void RefPtr<T>::Disconnects()
{
    if (m_pPtr != 0) {
        m_pPtr->Release();
    }
    m_pPtr = 0;
}


#endif // UTILITIES_SHAREDPTR_H

