#pragma once

#include "cblock.h"
#include <algorithm>
#include <memory>
#include <cassert>

//impl shared_ptr

//前置声明
template<class T> class weak_ptr;

template <typename T>
class shared_ptr {
public:
    //默认构造函数
    shared_ptr() noexcept = default;

    //接收一个裸指针的带参构造函数
    //Todo:explicit表示禁止隐式构造，这里很关键，防止这种写法 shared_ptr<int> sp1 = new int(10)
    explicit shared_ptr(T* data)
        :data_(data)
        
    {
        if(data)
        {
            cblock_ = new cblock();
        }
    }

    //拷贝构造函数
    shared_ptr(const shared_ptr& sp) noexcept
        :data_(sp.data_)
        ,cblock_(sp.cblock_)
    {
        if(cblock_)
        {
            ++(cblock_->shared_count);
        }
    }

    //shared_ptr为另外一种类型的data
    template<class D>
    shared_ptr(const shared_ptr<D>& sp) noexcept
        :data_(sp.data_)//若T* data和D* 不能在这里隐式转换的话，则在编译阶段，这里模板展开后，会直接编译报错。
        ,cblock_(sp.cblock_)//下面必须得声明template<class Y> friend class shared_ptr; 否则访问不了另外一个类shared_ptr<D>& sp的私有成员。
    {
        if(cblock_)
        {
            ++(cblock_->shared_count);
        }
    }

    //移动构造函数
    shared_ptr(shared_ptr&& sp) noexcept
    {
        swap(sp);
        sp.reset();
    }

    //接收一个weak_ptr
    explicit shared_ptr(const weak_ptr<T>& wp) noexcept
        :data_(wp.data_)
        ,cblock_(wp.cblock_)
    {
        //增加强引用计数
        if(cblock_)
        {
            ++(cblock_->shared_count);
        }
    }

    ~shared_ptr() noexcept
    {
        dec_shared_count();
    }

    //拷贝赋值函数
    shared_ptr& operator=(const shared_ptr& sp) noexcept
    {
        if(this != &sp)
        {
            shared_ptr tmp(sp);
            swap(tmp);
            //无需reset
        }
        
        return *this;
    }

    //移动赋值函数
    shared_ptr& operator=(shared_ptr&& sp) noexcept
    {
        //Todo:移动赋值函数必须得处理自重复，否则可能会导致后面的reset把引用计数减1后为0，导致data_被delete掉。
        //Todo:如果移动赋值函数没有处理自重复,则在进行swap后,
        //下面的reset方法会把sp的引用计数-1,由于sp和this相同，此时引用计数为1，若还进行减1，则强引用计数会变为0，会导致data_被delete。
        if(this!=&sp)
        {
            swap(sp);
            sp.reset();//reset it
        }
        
        return *this;
    }

    //获取裸指针
    T* get() const noexcept
    {
        return data_;
    }

    //Todo:这个符号什么时候会被调用呢？
    //相当于调用 sp->getABC() 这时候其实是期望访问data_
    T* operator->() const noexcept
    {
        return data_;
    }

    //解引用
    T& operator*() const noexcept
    {
        //assert该shared_ptr对象一定没被释放掉。
        assert(*this);
        return *data_;
    }

    explicit operator bool() const noexcept
    {
        return data_ != nullptr;
    }

    void reset() noexcept
    {
        //减少当前对象的引用计数
        dec_shared_count();
        //重置成员变量
        data_ = nullptr;
        cblock_ = nullptr;
    }

    void reset(T* data)
    {
        //减少当前对象的引用计数
        dec_shared_count();
        data_= data;
        cblock_=new cblock();//it will be inited in the constructor
    }

private:
    void swap(shared_ptr& sp) noexcept
    {
        std::swap(data_,sp.data_);
        std::swap(cblock_,sp.cblock_);
    }

    void dec_shared_count() noexcept
    {
        if(!cblock_)
            return;

        --(cblock_->shared_count);

        if(cblock_->shared_count <= 0)
        {
            //释放对象内存
            delete data_;
            data_ = nullptr;
            if(cblock_->weak_count<=0)
            {
                //若没有弱引用，则整个cblock都可释放之
                delete cblock_;
                cblock_=nullptr;
            }
        }
    }

private:
    T* data_ = nullptr;
    cblock* cblock_ = nullptr;

    //声明另外一种shared_ptr<D>也是我们freind，方便上面的template<class D> shared_ptr(const shared_ptr<D>& sp) noexcept使用
    template<class Y> friend class shared_ptr;
    template<class Y> friend class weak_ptr;
};
