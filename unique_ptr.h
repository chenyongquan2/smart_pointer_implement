#pragma once

#include <cassert>
#include <algorithm>

template<class T>
class unique_ptr
{
public:
    //默认构造函数
    unique_ptr() noexcept = default;

    //带参构造函数
    unique_ptr(T *data)
        :data_(data)
    {

    }

    //禁止拷贝构造函数
    unique_ptr(const unique_ptr& up) = delete;

    //移动构造函数
    unique_ptr(unique_ptr&& up) noexcept
        :data_(up.data_)
    {
        //reset the orginal up data
        up.data_ = nullptr;
    }

    ~unique_ptr() noexcept
    {
        delete data_;
        data_ = nullptr;
    }

    //禁止拷贝赋值
    unique_ptr& operator=(const unique_ptr& up) = delete;

    //移动赋值函数
    unique_ptr& operator=(unique_ptr&& up) noexcept
    {
        //Todo:下面必须得处理自重复的情况，否则由于up和this一样，导致下面的reset方法先把原有的data_空间给delete掉了,
        //但是up和this的data_空间是同一块空间，所以就会导致两个指向的data_空间被释放掉了。。。
        if(this != &up)
        {
            // data_ = up.data_;
            //这里做了两件事:1释放原有的空间，把up.data_的空间给转义给this->data_; 2把up.data_给置null
            reset(up.data_);
            up.data_ = nullptr;
        }

        
        return *this;
    }

    T* get() const noexcept 
    {
        return data_;
    }

    T* operator->() const noexcept
    {
        return data_;
    }

    T& operator*() const noexcept
    {
        assert(*this);
        return *data_;
    }

    //it will be called like this usage "assert(!up1);"
    explicit operator bool() const noexcept
    {
        return data_ != nullptr;
    }

    //返回裸指针，并且解除智能指针和裸指针的关系
    T* release() noexcept
    {
        T* tmp = data_;
        data_ = nullptr;
        return tmp;
    }

    void reset(T *data = nullptr)
    {
        //释放原有的空间
        delete data_;
        //重新设置到新的空间上。
        data_ = data;
    }

private:
    T* data_ = nullptr;


};
