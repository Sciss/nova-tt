//  semaphore class, win32 wrapper
//  based on API proposed in N2043
//
//  Copyright (C) 2008, 2009 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
//  Boston, MA 02110-1301, USA.

#ifndef NOVA_TT_SEMAPHORE_WIN32_HPP
#define NOVA_TT_SEMAPHORE_WIN32_HPP

#include <cassert>
#include <limits>

#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>

#include <windows.h>

namespace nova {
namespace nova_tt {

/** semaphore class */
template <bool has_timed_wait = false>
class semaphore:
    boost::noncopyable
{
public:
    semaphore(unsigned int i=0)
    {
        ghSemaphore = CreateSemaphore(
            NULL,           // default security attributes
            i,  // initial count
            (std::numeric_limits<LONG>::max)(),  // maximum count
            NULL);          // unnamed semaphore
        assert(ghSemaphore != NULL);
        //TODO: take care of NULL return -> exception
        //printf("CreateSemaphore error: %d\n", GetLastError());
    }

    ~semaphore(void)
    {
        CloseHandle(ghSemaphore);
    }

    /** signal semaphore */
    void post(void)
    {
        int status = ReleaseSemaphore(
                        ghSemaphore,  // handle to semaphore
                        1,            // increase count by one
                        NULL);      // not interested in previous count
        assert(status != 0);
        //TODO report:
        //printf("ReleaseSemaphore error: %d\n", GetLastError());
    }

    /** wait until this semaphore is signaled */
    void wait(void)
    {
        int status = WaitForSingleObject(
            ghSemaphore,
            INFINITE);           // wait forever
        assert(status == 0);
        //TODO: report error status
    }

    /** try to wait for the semaphore
     *
     * \return true, if the value can be decremented
     *         false, otherweise
     */
    bool try_wait(void)
    {
        int status = WaitForSingleObject(
            ghSemaphore,
            0L);           // zero-second time-out interval
        return status == 0;
    }

    /** try to wait for the semaphore until timeout
     *
     * \return true, if the value can be decremented
     *         false, otherweise
     */
    bool timed_wait(struct timespec const & absolute_timeout)
    {
        long milisecons = 0 ;//TODO absolute_timeout - now
        BOOST_STATIC_ASSERT(has_timed_wait);
        int status = WaitForSingleObject(
            ghSemaphore,
            milisecons);   // zero-second time-out interval
        return status == 0;
    }

    int value(void)
    {
        int ret;
        bool status = ReleaseSemaphore( ghSemaphore, 0, (long*)&ret );
        assert(status == 0);
        if (ret < 0)
            return 0;
        else
            return ret;
    }

private:
    HANDLE ghSemaphore;
};

} // namespace nova_tt
} // namespace nova

#endif /* NOVA_TT_SEMAPHORE_WIN32_HPP */
