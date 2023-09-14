#include <cassert>
#include <iostream>

#include "unique_ptr.h"
#include "shared_ptr.h"
#include "weak_ptr.h"

void test_shared_ptr()
{
    // {
    //     //compile error because of shared_ptr constuctor is explicit
    //     shared_ptr<int> sp1 = new int(10);
    // }

    {
        //test default constructor
        shared_ptr<int> sp1;
        assert(sp1.get() == nullptr);
        assert(!sp1);
    }

    {
        //test constructor with one param that is a pointer.
        shared_ptr<int> sp1(new int(10));
        assert(*sp1 == 10);
        assert(*(sp1.get()) == 10);
    }

    {
        //test copy constructor
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(sp1);
        assert(*sp1 == 1);
        assert(*sp2 == 1);
    }

    {
        //test move constructor
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(std::move(sp1));
        assert(!sp1);
        assert(sp1.get() == nullptr);
        assert(*sp2 == 1);
    }

    {
        //test assign operator
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));
        shared_ptr<int> sp3;

        sp3=sp1;
        assert(*sp3==1);
        assert(*sp1==1);

        sp3=sp2;
        assert(*sp3==2);
        assert(*sp2==2);
    }

    {
        //test move-assign operator
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));
        shared_ptr<int> sp3;

        sp3=std::move(sp1);
        assert(sp1.get() == nullptr);
        assert(!sp1);
        assert(*sp3==1);

        sp3=std::move(sp2);
        assert(sp2.get() == nullptr);
        assert(!sp2);
        assert(*sp3==2);
    }

    {
        //test reset function
        shared_ptr<int> sp1 (new int(1));
        sp1.reset(new int(2));
        assert(*sp1==2);

        sp1.reset();
        assert(sp1.get()==nullptr);
        assert(!sp1);
    }

    {
        //test T is the class type instead of the basic type
        class Base{};
        class Derived :public Base{};
        shared_ptr<Derived> sp1(new Derived);
        shared_ptr<Base> sp2(sp1);
    }

    // {
    //     //测试这里编译shared_ptr<T> 和shared_ptr<D> 当T*data_ 和D* data_不能进行类型转换时，编译阶段进行《模板实例化》后，这里会直接编译报错。
    //     class Base{};
    //     class Derived{};
    //     shared_ptr<Derived> sp1(new Derived);
    //     shared_ptr<Base> sp2(sp1);
    // }

}

void test_weak_ptr()
{
    {
        //test function lock and expired
        weak_ptr<int> wp1;
        assert(!wp1.lock());
        assert(wp1.expired());
    }

    {
        //test use weak_ptr to assist shared_ptr
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        assert(wp1.lock());
        assert(!wp1.expired());

        //test reset and expired after reset
        wp1.reset();
        assert(wp1.expired());
    }

    {
        //test weak_ptr to bind same shared_ptr
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(sp1);
        assert(wp1.lock());
        assert(wp2.lock());
    }

    {
        //test move constructor
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(std::move(wp1));
        assert(wp1.expired());
        assert(wp2.lock());

        weak_ptr<int> wp3(std::move(sp1));//Todo:注意这里只是对sp1生成其对应的右值元素，并没有将其sp1本身给清空掉。
        assert(wp3.lock());
    }

    {
        //test assign operator
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));
        
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(sp2);
        weak_ptr<int> wp3;

        wp3=wp1;
        assert(wp1.lock());
        assert(wp2.lock());
        assert(wp3.lock());

        wp3=wp2;
        assert(wp1.lock());
        assert(wp2.lock());
        assert(wp3.lock());

    }

    {
        //test move-assign operator
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));

        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(sp1);
        weak_ptr<int> wp3;

        //移动赋值运算符。
        wp3 = std::move(wp1);//wp1 will be reseted.
        assert(wp1.expired());
        assert(wp3.lock());

        wp3=std::move(wp2);
        assert(wp2.expired());
        assert(wp3.lock());
    }

    {
        //test after the shared_ptr be reset, the weak_ptr will be expired and call lock()  will return empty shared_ptr
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        {
            auto sp2=wp1.lock();
            assert(sp2);
        }

        {
            shared_ptr<int> sp2(sp1);
            //the shared_ptr's strong count is still valided.
            sp2.reset();
            assert(wp1.lock());
            assert(!wp1.expired());
        }

        sp1.reset();
        assert(!wp1.lock());
        assert(wp1.expired());


    }
}

void test_unique_ptr()
{
    {
        //test unique_ptr(T *data) constructor and funtion named * and get()
        unique_ptr<int> up1(new int(1));
        assert(*up1==1);
        assert(*up1.get() == 1);
    }

    {
        //test move constructor
        unique_ptr<int> up1(new int(1));
        unique_ptr<int> up2(std::move(up1));
        assert(!up1);//it will call "explicit operator bool() const noexcept"
        assert(!up1.get());

        assert(up2);
        assert(up2.get());

        //copy constructor and assign constructor are delete, so that compile failed.
        // unique_ptr<int> up3(up1);
        // unique_ptr<int> up4 = up1;
    }

    {
        //test move-assign 
        unique_ptr<int> up1(new int(1));
        unique_ptr<int> up2;
        up2=std::move(up1);

        assert(!up1);//it will call "explicit operator bool() const noexcept"
        assert(!up1.get());

        assert(up2);
        assert(up2.get());
    }

    {
        //test move assign to it-self
        unique_ptr<int> up1(new int(1));
        up1=std::move(up1);
        assert(up1);
        assert(up1.get());

        //对比标准库，下面assert是不会报错的。所以上面我们得处理一下自重复的情况。
        {
            //test move assign to it-self
            std::unique_ptr<int> up1(new int(1));
            up1=std::move(up1);
            assert(up1);
            assert(up1.get());
        }
    }

    {
        //test reset to another pointer
        unique_ptr<int> up1(new int(1));
        up1.reset(new int(2));
        assert(*up1==2);
        assert(up1);
        assert(up1.get());

        up1.reset();
        assert(!up1);
        assert(!up1.get());
    }

    {
        //test release
        unique_ptr<int> up1(new int(1));
        int* data = up1.release();
        assert(!up1);
        assert(!up1.get());
        delete data;
    }

}

int main(int, char**){
    test_shared_ptr();
    test_weak_ptr();
    test_unique_ptr();

    

    std::cout << "Hello, from run!\n";
}
