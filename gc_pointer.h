#ifndef GC_POINTER
#define GC_POINTER

#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <class T, int size = 0>
class Pointer{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise. 
        but how to know a pointer is pointing to an array or not  --- from the declare
    */
    bool isArray; 
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    unsigned arraySize; // size of the array
    static bool first; // true when first Pointer is created
    // Return an iterator to pointer details in refContainer.
    //what does typename mean,this seems delcare a function
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    // Define an iterator type for Pointer<T>.
    //why defined GCiterator, but not used
    typedef Iter<T> GCiterator;
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
    Pointer(){
        Pointer(NULL);
    }
    Pointer(T*);
    // Copy constructor.
    Pointer(const Pointer &);
    // Destructor for Pointer.
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*(){
        return *addr;
    }
    // Return the address being pointed to.
    T *operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    //shoud we check the addr is a array or not?
    T &operator[](int i){ return addr[i];}
    // Conversion function to T *.
    operator T *() { return addr; }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t){                             //ok, not verified
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;
    //this is a constructer, addr must be nullptr
    //case 0: t == nullptr 
    //case 1: t != nullptr
    // TODO: Implement Pointer constructor
    if (t == nullptr) {
        return;
    } 
	//t != nullptr
    //1. construct a PtrDetails<T> pd;
    //if this pointer is an array    
    if (size > 0) {
        isArray = true;
        arraySize = size;
    } else {
        isArray = false;
        arraySize = 0;
    }    
    auto itertor = findPtrInfo(t);  //possible?
    if (itertor != refContainer.end()) { // t is in the refcontainer already
        itertor->refcount++;
    } else {
    	//t is not in the refcontainer
        PtrDetails<T> pd(t, size);
        pd.refcount++;
        refContainer.push_back(pd);
    }
    addr = t;
    arraySize = size;
    if (size > 0) {
        isArray = true;
    } else {
        isArray = false;
    }
    // Lab: Smart Pointer Project Lab

}
// Copy constructor.
template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &ob){            //ok, not verified

    //this Pointer is definitely not the "first" Pointer
    // TODO: Implement Pointer constructor
    if (ob.addr == nullptr) {
        return;
    } 
    //ob.addr !=  nullptr
    addr = ob.addr;
    auto itertor = findPtrInfo(addr);
    if (itertor != refContainer.end()) {
        itertor->refcount++;
    } else {
        PtrDetails<T> pd(addr, size);
        pd.refcount++;
        refContainer.push_back(pd);
    }
    arraySize = ob.arraySize;
    isArray = ob.isArray;
    if (size > 0) {
        isArray = true;
    } else {
        isArray = false;
    }
    // Lab: Smart Pointer Project Lab

}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer(){
    
    // TODO: Implement Pointer destructor
    if (addr && (findPtrInfo(addr) != refContainer.end()))
    	findPtrInfo(addr)->refcount--;
    // Lab: New and Delete Project Lab
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){

    // TODO: Implement collect function
    //iterator the refContainer, if the refcounter is 0, delete the pointer
    int i = 0;
    for (auto itertor = refContainer.begin(); itertor != refContainer.end(); itertor++) {
		if (itertor->refcount == 0) {            
			if (itertor->isArray) {
                delete [] itertor->memPtr;                       
	        } else {
                delete itertor->memPtr;        
	        }
            refContainer.erase(itertor);  
            i++;  
		}        
    }
    if (i > 0) {
        return true;
    }
    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor
    return false;//why return a value??????
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t){   //ok, not verified

    // TODO: Implement operator==
    //t is nullptr? addr is nullptr?
    addr = t;
	arraySize = size;
	if (size > 0) {
		isArray = true;
	}
    if (t == nullptr && addr == nullptr) {
        return addr;
    } else if (t == nullptr && addr != nullptr) {
        auto itertor = findPtrInfo(addr); 
        if (itertor != refContainer.end()) {
            itertor->refcount--;
        }
		addr = t;
		arraySize = size;
		if (size > 0) {
			isArray = true;
		}
		return addr;
    }else if(t != nullptr && addr == nullptr) {
    	//1. find t is in the refcontainer or not
    	auto itertor = findPtrInfo(t);
		if (itertor != refContainer.end()) {
			itertor->refcount++;
		} else {
			PtrDetails<T> pd(t, size);
			pd.refcount++;
			refContainer.push_back(pd);
		}    
		addr = t;
		arraySize = size;
		if (size > 0) {
			isArray = true;
		}
		return addr;
    } else if(t != nullptr && addr != nullptr) {
        auto itertor = findPtrInfo(addr);
        if (itertor != refContainer.end()) {
            itertor->refcount--;
        }

		auto itertor_t = findPtrInfo(t);
		if (itertor_t != refContainer.end()) {
			itertor_t->refcount++;
			return addr;
		} else {
			PtrDetails<T> pd(t, size);
	        pd.refcount++;
	        refContainer.push_back(pd);
		} 
		addr = t;
		arraySize = size;
		if (size > 0) {
			isArray = true;
		}
		return addr;
    }
    // LAB: Smart Pointer Project Lab

}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){

    // TODO: Implement operator==
    //1. what if rv.addr == nullptr
    //1.1 if addr ==  nullptr, then, addr = nullptr, and then do nothing
    //1.2 if addr !=  nullptr, then, minius PtrDetail(addr).refcount, and addr = nullptr
    //2. what if rv.addr !=  nullptr
    //2.1 what if addr == nullptr, just assign addr = rv.addr, and push_back
    //2.2 what if addr !=  nullptr, PtrDetail(addr).refcount--, then push_back

    if (rv.addr == nullptr) {
        if (addr == nullptr) {
            return *this;
        } else {
            //addr != nullptr
            auto itertor = findPtrInfo(addr);
            if (itertor != refContainer.end()) {
                itertor->refcount--;
            }
            addr = rv.addr;
            arraySize = rv.arraySize;
            isArray = rv.isArray;
			return *this;
        }
    } else {
    //rv.addr != nullptr
        if (addr == nullptr) {
            addr = rv.addr;
            arraySize = rv.arraySize;
            isArray = rv.isArray;
            auto itertor = findPtrInfo(addr);
            if (itertor != refContainer.end()) {
                itertor->refcount++;
            } else {
                PtrDetails<T> pd(rv.addr, rv.arraySize);
                pd.refcount++;
                refContainer.pop_back(pd);
            }
			return *this;
        } else {
            auto itertor = findPtrInfo(addr);
            if (itertor != refContainer.end()) {
                itertor->refcount--;
            }
			addr = rv.addr;
            arraySize = rv.arraySize;
            isArray = rv.isArray;

			auto itertor_rv = findPtrInfo(rv->addr);
			if (itertor_rv != refContainer.end()) {
				itertor_rv->refcounter++;
				return *this;
			}
            PtrDetails<T> pd(rv.addr, rv.arraySize);
            pd.refcount++;
            refContainer.pop_back(pd);
        }
    }
    // LAB: Smart Pointer Project Lab

}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}

#endif