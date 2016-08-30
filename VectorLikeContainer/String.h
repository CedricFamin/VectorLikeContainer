#pragma once

#include <cassert>

namespace CContainers
{
    class StringBaseContainers
    {

    };

    class SharedString
    {
    public:
        SharedString(char const * str)
        {

        }
    private:
    };

    class String
    {
    public:
        String(char const * str)
            : _sharedString(str)
        {

        }
    private:
        SharedString _sharedString;
    };
}