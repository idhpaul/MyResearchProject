//
// Aspia Project
// Copyright (C) 2020 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef BASE__WIN__SCOPED_GDI_OBJECT_H
#define BASE__WIN__SCOPED_GDI_OBJECT_H

#include <Windows.h>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) 

// Like ScopedHandle but for GDI objects.
template<class T, class Traits>
class ScopedGDIObject
{
public:
    ScopedGDIObject() = default;

    explicit ScopedGDIObject(T object)
        : object_(object)
    {
        // Nothing
    }

    ~ScopedGDIObject() { Traits::close(object_); }

    T get() { return object_; }

    void reset(T object = nullptr)
    {
        if (object_ && object != object_)
            Traits::close(object_);
        object_ = object;
    }

    ScopedGDIObject& operator=(T object)
    {
        reset(object);
        return *this;
    }

    T release()
    {
        T object = object_;
        object_ = nullptr;
        return object;
    }

    operator T() { return object_; }

private:
    T object_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(ScopedGDIObject);
};

// The traits class that uses DeleteObject() to close a handle.
template <typename T>
class DeleteObjectTraits
{
public:
    // Closes the handle.
    static void close(T handle)
    {
        if (handle)
            DeleteObject(handle);
    }
};

// Typedefs for some common use cases.
using ScopedHBITMAP = ScopedGDIObject<HBITMAP, DeleteObjectTraits<HBITMAP>>;
using ScopedHRGN = ScopedGDIObject<HRGN, DeleteObjectTraits<HRGN>>;
using ScopedHFONT = ScopedGDIObject<HFONT, DeleteObjectTraits<HFONT>>;
using ScopedHBRUSH = ScopedGDIObject<HBRUSH, DeleteObjectTraits<HBRUSH>>;

#endif // BASE__WIN__SCOPED_GDI_OBJECT_H

