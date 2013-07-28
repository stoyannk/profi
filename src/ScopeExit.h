// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once

namespace profi {

template<typename T>
class scope_exit
{
public:
    explicit scope_exit(T&& s)
        : m_Scope(std::forward<T>(s))
    	, m_IsDismissed(false)
    {}

    scope_exit(scope_exit&& rhs)
        : m_Scope(std::move(rhs.m_Scope))
    	, m_IsDismissed(std::move(rhs.m_IsDismissed))
    {}

    ~scope_exit()
    {
        if(!m_IsDismissed)
        {
        	m_Scope();
        }
    }

    void dismiss()
    {
    	m_IsDismissed = true;
    }

private:
    scope_exit(scope_exit&);
    scope_exit& operator=(scope_exit&);

    T m_Scope;
    bool m_IsDismissed;
};

template<typename T>
scope_exit<T> make_scope_exit(T&& s)
{
    return scope_exit<T>(std::forward<T>(s));
}

}