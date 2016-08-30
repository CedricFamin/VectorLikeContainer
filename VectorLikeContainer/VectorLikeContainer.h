#pragma once

#include <cassert>

namespace CContainers
{
    typedef unsigned int u32;

    template <typename T>
    class VectorLikeContainers
    {
    public:
        typedef T value_type;
        typedef VectorLikeContainers<value_type> self_type;
        typedef value_type* pointer;
        typedef pointer const const_pointer;
        typedef value_type & reference;
        typedef reference const const_reference;

        // handle reverse iterator, const iterator
        // base iterator
        // const vs normal iterator
        // reverse vs normal iterator (-- au lieu de ++)
        // base_iterator interface d'un const iterator dans les operateur ++ et --
        // 
        class iterator : public std::iterator<std::input_iterator_tag, value_type, size_t, pointer, reference>
        {
        public:
            iterator(pointer ptr) : _ptr(ptr) {}
            reference       operator*()         { return *_ptr; }
            const_reference operator*()   const { return *_ptr; }
            pointer         operator->()        { return _ptr; }
            pointer         getPtr()      const { return _ptr; }
            const_pointer   getConstPtr() const { return _ptr; }
            iterator&       operator++()        { ++_ptr; return *this; }
            iterator&       operator--()        { --_ptr; return *this; }
            
            bool            operator==(const iterator & other) const
            {
                if (&other == this)
                    return true;
                return _ptr == other._ptr;
            }

            bool operator!=(const iterator & other) const
            {
                return !(*this == other);
            }
        private:
            pointer _ptr;
        };

        /*class iterator : public base_iterator
        {

        };*/

        typedef iterator const const_iterator;


        VectorLikeContainers(pointer mem, size_t size)
            : _data(mem)
            , _max_size(size)
            , _current_size(0)
        {
        }

        template<typename InputIterator>
        void assign(InputIterator first, InputIterator last)
        {
            clear();
            for (;first != last; ++first)
            {
                push_back(*first);
            }
        }
        void assign(size_t const & n, value_type const & val)
        {
            clear();
            assert(n <= capacity());
            _current_size = n;
            memcpy(data(), &val, n * sizeof(value_type));
        }

        void clear()
        {
            while (!empty())
                pop_back();
        }

        bool empty() const
        {
            return _current_size == 0;
        }

        iterator erase(iterator position)
        {
            assert(iterator >= begin());
            assert(iterator < end());

            size_t idx = position.getPtr() - data();
            _data[idx].~value_type();
            memcpy(_data + idx, _data + idx + 1, (_current_size - idx) * sizeof(value_type));
            --_current_size;
            return position; // never reallocate - stl compliant
        }

        /*iterator erase(const_iterator position)
        {
            assert(iterator >= cbegin());
            assert(iterator < cend());

            size_t idx = position.getPtr() - data();
            _data[idx].~value_type();
            memcpy(_data + idx, _data + idx + 1, (_current_size - idx) * sizeof(value_type));
            --_current_size;
            return position; // never reallocate - stl compliant
        }*/

        iterator erase(iterator first, iterator last)
        {
            assert(first <= last);
            assert(first >= begin());
            assert(last <= end());
            size_t beginIdx = first.getPtr() - data();
            size_t range = last.getPtr() - first.getPtr();

            for (first != last; ++first)
            {
                first.~value_type();
            }

            memcpy(_data + beginIdx, _data + beginIdx + range, (_current_size - range) * sizeof(value_type));
            _current_size -= range;
            return first;
        }
        //iterator erase(const_iterator first, const_iterator last);

        //iterator insert(iterator position, const value_type& val);
        //void insert(iterator position, size_t n, const value_type& val);
        //template <class InputIterator>
        //void insert(iterator position, InputIterator first, InputIterator last);

        template <class... Args>
        void emplace_back(Args&&... args)
        {
            _current_size += 1;
            assert(_current_size <= _max_size);
            new (&back()) value_type(args...);
        }

        void pop_back()
        {
            at(_current_size - 1).~value_type();
            assert(!empty());
            --_current_size;

        }

        void push_back(const value_type& val)
        {
            _current_size += 1;
            assert(_current_size <= _max_size);
            new (&back()) value_type(val);
        }

        
        void push_back(value_type&& val)
        {
            _current_size += 1;
            assert(_current_size <= _max_size);
            new (&back()) value_type(std::forward<value_type>(val));
        }
        

        size_t max_size() const { return _max_size; }
        void resize(size_t n)
        {
            assert(_max_size >= n);
            while (size() < n)
            {
                new (_data + _current_size) T();
                ++_current_size;
            }

            while (size() > n)
            {
                pop_back();
            }
            _current_size = n;
        }
        void resize(size_t n, const value_type& val)
        {
            size_t prevMaxIdx = _current_size;
            resize(n);
            if (!empty())
            {
                pointer ptr = &at(prevMaxIdx);
                for (; prevMaxIdx < _current_size; ++prevMaxIdx, ++ptr)
                {
                    *ptr = val;
                }
            }
        }
        size_t size() const { return _current_size; }
        //void swap(self_type& x);

        size_t capacity() const { return _max_size;  }

        // accessors
        reference at(size_t const & n)
        {
            assert(n < _current_size);
            return _data[n];
        }
        const_reference at(size_t const & n) const
        {
            assert(n < _current_size);
            return _data[n];
        }

        reference back()
        {
            assert(!empty());
            return at(_current_size - 1);
        }
        const_reference back() const
        {
            assert(!empty());
            return at(_current_size - 1);
        }
        reference front()
        {
            assert(!empty());
            return (*this)[0];
        }
        const_reference front() const
        {
            assert(!empty());
            return (*this)[0];
        }

        /*
        const_iterator cbegin();
        reverse_iterator rbegin() const;
        const_iterator cend();
        reverse_iterator cend() const;
        */
        iterator begin() { return iterator(data()); }
        const_iterator begin() const { return const_iterator(data()); }
        iterator end() { return iterator(data() + _current_size); }
        const_iterator end() const { return iterator(data() + _current_size); }

        pointer data() { return _data; }
        const_pointer data() const { return _data; }
        reference operator[] (size_t n) { return at(n); }
        const_reference operator[] (size_t n) const { return at(n); }
    private:
        pointer _data;
        size_t _max_size;
        size_t _current_size;
    };

    template<typename T, size_t n>
    class VectorStackLocal : public VectorLikeContainers<T>
    {
    public:
        VectorStackLocal() : VectorLikeContainers(reinterpret_cast<T*>(_data), n) {}
    private:
        char _data[n * sizeof(T)];
    };

    template<typename T, size_t n>
    class MaxSizedVector : public VectorLikeContainers<T>
    {
    public:
        MaxSizedVector() : VectorLikeContainers(reinterpret_cast<T*>(new char[n * sizeof(T)]), n)
        {
        }
        ~MaxSizedVector()
        {
            delete reinterpret_cast<char*>(data());
        }
    };
}