//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef THREADING_GUARDS_H
#define THREADING_GUARDS_H

namespace Nutshell {
namespace Threading {

//******************************************************************************
// This class allows exception safe scoped locking of an object.
//******************************************************************************
template<typename TYPE>
class Locker : boost::noncopyable {
private:
    
    TYPE&   m_rObject;  // The object we hold a lock on.

public:

    //**************************************************************************
    // Constructor.
    //
    // Parameters:
    //  p_rObject - The object to lock.
    //**************************************************************************
    Locker(TYPE& p_rObject)
    :   m_rObject(p_rObject)
    {
        assert(this != 0);

        // Lock the object
        m_rObject.Lock();
    }

    //**************************************************************************
    // Destructor.
    //**************************************************************************
    ~Locker()
    {
        assert(this != 0);

        // Unlock the object
        m_rObject.Unlock();
    }
};

//******************************************************************************
// This class allows exception safe scoped try-locking of an object.
//******************************************************************************
template<typename TYPE>
class TryLocker : boost::noncopyable {
private:
    
    TYPE&   m_rObject;  // The object we hold a lock on.
    bool    m_Locked;   // Whether we got the lock on the object.

public:

    //**************************************************************************
    // Constructor.
    //
    // Parameters:
    //  p_rObject - The object to lock.
    //**************************************************************************
    TryLocker(TYPE& p_rObject)
    :   m_rObject(p_rObject),
        m_Locked()
    {
        assert(this != 0);

        // Lock the object
        m_Locked = m_rObject.TryLock();
    }

    //**************************************************************************
    // Destructor.
    //**************************************************************************
    ~TryLocker()
    {
        assert(this != 0);

        // Unlock the object if needed
        if (m_Locked) m_rObject.Unlock();
    }

    //**************************************************************************
    // Returns whether we locked the object.
    //**************************************************************************
    bool Locked() const
    {
        assert(this != 0);

        return m_Locked;
    }
};

//******************************************************************************
// This class allows exception safe scoped unlocking of an object.
//******************************************************************************
template<typename TYPE>
class Unlocker {
private:
    
    TYPE&   m_rObject;  // The object we unlock.

public:

    //**************************************************************************
    // Constructor.
    //
    // Parameters:
    //  p_rObject - The object to unlock.
    //**************************************************************************
    Unlocker(TYPE& p_rObject)
    :   m_rObject(p_rObject)
    {
        assert(this != 0);

        // Unlock the object
        m_rObject.Unlock();
    }

    //**************************************************************************
    // Destructor.
    //**************************************************************************
    ~Unlocker()
    {
        assert(this != 0);

        // Lock the object
        m_rObject.Lock();
    }
};

} // namespace Threading
} // namespace Nutshell

#endif // !GUARDS_GUARDS_H
