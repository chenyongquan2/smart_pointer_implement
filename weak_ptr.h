#pragma once

#include "cblock.h"
#include <algorithm>
#include <memory>
#include "shared_ptr.h"


//impl weak_ptr
template <typename T>
class weak_ptr {
public:
    //默认构造
    weak_ptr() noexcept = default;
    //拷贝构造
    weak_ptr(const weak_ptr& wp) noexcept
        :cblock_(wp.cblock_)
        ,data_(wp.data_)
    {
        if(cblock_)
        {
            ++(cblock_->weak_count);
        }
    }

    //绑定一个shared_ptr,注意这里的shared_ptr可以接左值，也可接右值
    weak_ptr(const shared_ptr<T>& sp) noexcept
        :data_(sp.data_)
        ,cblock_(sp.cblock_)
    {
        if(cblock_)
        {
            ++(cblock_->weak_count);
        }
    }

    //移动构造
    weak_ptr(weak_ptr&& wp) noexcept
    {
        swap(wp);//交换到当前对象
        wp.reset();//重置原本对象wp
    }

    //析构
    ~weak_ptr()
    {
        dec_weak_count();
    }

    //拷贝赋值函数
    weak_ptr& operator=(const weak_ptr& wp) noexcept 
    {
        //Todo:void swap(weak_ptr& wp) noexcept 要求是一个左值，但是这里wp是一个const weak_ptr& wp ，很可能是一个右值
        weak_ptr tmp(wp);
        swap(tmp);
        //wp.reset();//无需reset
        return *this;
    }

    //移动赋值函数
    weak_ptr& operator=(weak_ptr&& wp) noexcept
    {
        swap(wp);
        wp.reset();
        return *this;
    }

    //返回shared_ptr
    shared_ptr<T> lock() const
    {
        if(expired())
        {
            return shared_ptr<T>(nullptr);
        }
        else
        {
            //todo:
            return shared_ptr<T>(*this);
        }
    }

    //判断是否过期/有效
    bool expired() const
    {
        //cblock_结构被释放了或者强引用计数小于等于0了.
        return !cblock_ || cblock_->shared_count <=0 ;
    }

    //重置
    void reset()
    {
        dec_weak_count();
        data_ = nullptr;
        cblock_= nullptr;
    }

private:
    void swap(weak_ptr& wp) noexcept
    {
        //Todo:若当前data_和cblock_为空，则swap后会把wp.data_和wp.cblock_置为空。
        std::swap(data_, wp.data_);
        std::swap(cblock_, wp.cblock_);
    }

    //减少弱引用计数
    void dec_weak_count() noexcept
    {
        if(!cblock_)
            return;

        //dec weak_count
        --(cblock_->weak_count);

        //judge whether delete cblock_
        if(cblock_->shared_count <=0 && cblock_->weak_count<=0)
        {
            delete cblock_;
            cblock_=nullptr;
        }
    }

private:
    T* data_ = nullptr;
    cblock* cblock_ = nullptr;


    //友元声明
    //Todo:这是什么写法
    template<class Y> friend class shared_ptr;
};
