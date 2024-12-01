#ifndef WSTL_ALLOCATOR_H
#define WSTL_ALLOCATOR_H

// 这个头文件包含模版类 allocator，用于管理内存分配、释放、对象构造和析构

#include "construct.h"
#include "util.h"

namespace wstl {

    // 模版类 allocator
    template<class T>
    class allocator {
    public:
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        static T *allocate();
        static T *allocate(size_type n);

        static void deallocate(T *ptr);
        static void deallocate(T *ptr, size_type);

        static void construct(T *ptr);
        static void construct(T *ptr, const T &value);
        static void construct(T *ptr, T &&value);

        template<class... Args>
        static void construct(T *ptr, Args &&... args);

        static void destroy(T *ptr);
        static void destroy(T *first, T *last);
    };

    // allocate 分配内存

    template<class T>
    T *allocator<T>::allocate() {
        return static_cast<T *>(::operator new(sizeof(T)));
    }

    template<class T>
    T *allocator<T>::allocate(size_type n) {
        if (n == 0) {
            return nullptr;
        }
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    // deallocate 释放内存

    template<class T>
    void allocator<T>::deallocate(T *ptr) {
        if (ptr != nullptr) {
            ::operator delete(ptr);
        }
    }

    template<class T>
    void allocator<T>::deallocate(T *ptr, size_type) {
        if (ptr != nullptr) {
            ::operator delete(ptr);
        }
    }

    // construct 构造对象

    template<class T>
    void allocator<T>::construct(T *ptr) {
        wstl::construct(ptr);
    }

    template<class T>
    void allocator<T>::construct(T *ptr, const T &value) {
        wstl::construct(ptr, value);
    }

    template<class T>
    void allocator<T>::construct(T *ptr, T &&value) {
        wstl::construct(ptr, wstl::move(value));
    }

    template<class T>
    template<class... Args>
    void allocator<T>::construct(T *ptr, Args &&... args) {
        wstl::construct(ptr, wstl::forward<Args>(args)...);
    }

    // destroy 析构对象

    template<class T>
    void allocator<T>::destroy(T *ptr) {
        wstl::destroy(ptr);
    }

    template<class T>
    void allocator<T>::destroy(T *first, T *last) {
        wstl::destroy(first, last);
    }
}

#endif // WSTL_ALLOCATOR_H