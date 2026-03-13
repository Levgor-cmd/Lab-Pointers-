#include <stdexcept>
#include <iostream>
// part 1

template<class T>
class MyUnique
{
    T * p=nullptr;
public:
    MyUnique(T *p)
    {
        this->p = p;
    }

    ~MyUnique()
    {
        delete this->p;
    }

    MyUnique(const MyUnique& pointer) = delete;

    MyUnique(MyUnique&& other) noexcept
    {
        this->p = other.p;
        other.p = nullptr;
    }

    MyUnique& operator=(MyUnique&& other) noexcept
    {
        if (this != &other)
        {
            delete this->p;
            this->p = other.p;
            other.p = nullptr;
        }
        return *this;
    }

    MyUnique& operator=(const MyUnique&) = delete;

    T* get() const
    {
        return this->p;
    }

    T& operator*()
    {
        if (!p) throw std::runtime_error("Dereferencing null pointer");
        return *(this->p);
    }

    T* operator->()
    {
        return this->p;
    }
};

// part 2

template<class T>
class MyShared
{
    T * p=nullptr;
    int* ref_counter = nullptr;
public:
    MyShared(T *p)
    {
        this->p = p;
        this->ref_counter = new int(1);
    }

    ~MyShared()
    {
        if (--(*ref_counter) == 0)
        {
            delete this->p;
            delete this->ref_counter;
        }
    }

    MyShared(const MyShared& other)
    {
        this->p = other.p;
        this->ref_counter = other.ref_counter;
        (*ref_counter)++;
    }

    MyShared(MyShared&& other) noexcept
    {
        this->p = other.p;
        this->ref_counter = other.ref_counter;
        other.p = nullptr;
        other.ref_counter = nullptr;
    }

    MyShared& operator=(MyShared&& other) noexcept
    {
        if (this != &other)
        {
            if (ref_counter && --(*ref_counter) == 0)
            {
                delete p;
                delete ref_counter;
            }
            p = other.p;
            ref_counter = other.ref_counter;
            other.p = nullptr;
            other.ref_counter = nullptr;
        }
        return *this;
    }

    MyShared& operator=(const MyShared& other)
    {
        if (this != &other)
        {
            if (--(*ref_counter) == 0)
            {
                delete p;
                delete ref_counter;
            }
            p = other.p;
            ref_counter = other.ref_counter;
            (*ref_counter)++;
        }
        return *this;
    }

    T* get()
    {
        return this->p;
    }

    T& operator*()
    {
        if (!p) throw std::runtime_error("Dereferencing null pointer");
        return *(this->p);
    }

    T* operator->()
    {
        return this->p;
    }
};

int main() {
    // Конструктор от сырого указателя
    MyUnique<int> u1(new int(42));
    std::cout << "u1 value: " << *u1 << "\n";

    // Демонстрация operator-> с встроенным типом (через указатель)
    MyUnique<int*> u_ptr(new int*(new int(100)));
    std::cout << "u_ptr points to pointer, which points to: " << **u_ptr << "\n";
    // operator-> возвращает int**, разыменовываем один раз для получения int*

    // Конструктор перемещения
    MyUnique<int> u2 = std::move(u1);
    std::cout << "u2 value after move: " << *u2 << "\n";
    std::cout << "u1 is null after move: " << (u1.get() == nullptr ? "yes" : "no") << "\n";

    // Оператор присваивания перемещением
    MyUnique<int> u3(new int(100));
    std::cout << "u3 before move assignment: " << *u3 << "\n";
    u3 = std::move(u2);
    std::cout << "u3 after move assignment: " << *u3 << "\n";
    std::cout << "u2 is null after move assignment: " << (u2.get() == nullptr ? "yes" : "no") << "\n";

    // Конструктор от сырого указателя
    MyShared<int> s1(new int(10));
    std::cout << "s1: " << *s1 << "\n";

    // Конструктор копирования
    MyShared<int> s2 = s1;
    std::cout << "s2 (copy of s1): " << *s2 << "\n";

    // Оператор присваивания копированием
    MyShared<int> s3(new int(20));
    std::cout << "s3 before copy assignment: " << *s3 << "\n";
    s3 = s2;
    std::cout << "s3 after copy assignment (=s2): " << *s3 << "\n";

    // Проверка — все указывают на одно значение
    std::cout << "All point to same value: s1=" << *s1 << ", s2=" << *s2 << ", s3=" << *s3 << "\n";

    // Конструктор перемещения
    MyShared<int> s4 = std::move(s3);
    std::cout << "s4 after move from s3: " << *s4 << "\n";
    std::cout << "s3 is null after move: " << (s3.get() == nullptr ? "yes" : "no") << "\n";

    // Оператор присваивания перемещением
    MyShared<int> s5(new int(999));
    MyShared<int> s6(new int(111));
    std::cout << "\nBefore move assignment:\n";
    std::cout << "s5: " << *s5 << "\n";
    std::cout << "s6: " << *s6 << "\n";

    s6 = std::move(s5);
    std::cout << "\nAfter move assignment (s6 = std::move(s5)):\n";
    std::cout << "s6: " << *s6 << "\n";
    std::cout << "s5 is null: " << (s5.get() == nullptr ? "yes" : "no") << "\n";

    // operator-> с MyShared
    MyShared<int*> s_ptr(new int*(new int(555)));
    std::cout << "\ns_ptr operator-> demo: " << **s_ptr << "\n";

    return 0;
}
