#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER
#include <utility>
#include <cassert>

class ControlBlockBase {
public:
    ControlBlockBase(): refcount(1){} // TODO: implement the default constructor.

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase() // TODO: implement the destructor.
    {

    }

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        // TODO: increment refcount by 1 and return result.
        assert(refcount>0);
        refcount++;
        return refcount;
    }

    long decrement()
    {
        // TODO: decrement refcount by 1 and return result.
        assert(refcount>0);
        refcount--;
        return refcount;
    }

    long refCount() const
    {
        // TODO: just return the refcount.
        return refcount;
    }

private:
    // TODO: add field(s) which both control block types need to have
    long refcount;
};

template <typename T>
class ControlBlock: public ControlBlockBase
{

    public:
        ControlBlock(T* resource): ctrl_ptr(resource){}
        ~ControlBlock() override
        {
            delete ctrl_ptr;
        }

        void* managedAddress() override
        {
            return ctrl_ptr;
        }

    private:
        T* ctrl_ptr;
};

template <typename T>
class SharedPtr
{
    public:
        SharedPtr(): ctrl_ptr(nullptr), stored_ptr(nullptr){}
        SharedPtr(T* ptr): ctrl_ptr(new ControlBlock<T>(ptr)), stored_ptr(ptr){}

        ~SharedPtr()
        {
            if (ctrl_ptr!= nullptr)
            {
                if (ctrl_ptr->decrement() == 0)
                {
                    delete ctrl_ptr; //triggers control block's destructor
                }
            }

            
        }

        SharedPtr(const SharedPtr& other)
        {
            stored_ptr = other.stored_ptr;
            ctrl_ptr = other.ctrl_ptr;

            if (ctrl_ptr!=nullptr)
            {
                ctrl_ptr->increment();
            }

        }
        
        
        SharedPtr& operator = (const SharedPtr& other)
        {
            SharedPtr<T> interim = other;
            swap(interim);
            return *this;
        }

         SharedPtr(SharedPtr&& other)
        {
            stored_ptr = other.stored_ptr;
            ctrl_ptr = other.ctrl_ptr;
            other.stored_ptr = nullptr; //shouldnt point at same pointer anymore;
            other.ctrl_ptr = nullptr;
        }

        SharedPtr& operator = (SharedPtr&& other)
        {
            swap(other);
            return *this;
        }
        
        T& operator*() const
        {
            assert(stored_ptr != nullptr);
            return *stored_ptr;
        }

        T* operator->() const
        {
            assert(stored_ptr != nullptr);
            return stored_ptr;
        }

        T* get() const
        {
            return stored_ptr;
        }

        bool operator==(const SharedPtr<T>& other) const
        {
            return stored_ptr == other.stored_ptr;
        }

        operator bool() const
        {
            return stored_ptr!= nullptr;
        }
        
        void swap(SharedPtr<T>& other)
        {
            std::swap(stored_ptr, other.stored_ptr);
            std::swap(ctrl_ptr, other.ctrl_ptr);
        }

        void reset(T* newPtr = nullptr)
        {
            if (newPtr == stored_ptr)
            {
                return;
            }
            SharedPtr<T> interim = newPtr;
            swap(interim);
        }

        long useCount() const
        {
            if (ctrl_ptr== nullptr)
            {
                return 0;
            }
            return ctrl_ptr->refCount();
        }
    
    private:
        ControlBlockBase* ctrl_ptr;
        T* stored_ptr;


};

template <typename T, typename... Args>
SharedPtr<T> makeSharedBasic(Args&&... args)
{
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}
#endif
