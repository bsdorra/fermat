/*
 * cugar
 * Copyright (c) 2011-2018, NVIDIA CORPORATION. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the NVIDIA CORPORATION nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

namespace cugar {

// constructor
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE priority_queue<Key,Container,Compare>::priority_queue(const Container cont, const Compare cmp)
    : m_size(0), m_queue(cont), m_cmp(cmp)
{
    // TODO: heapify if not empty!
    assert( m_queue.empty() == true );
}

// is queue empty?
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE bool priority_queue<Key,Container,Compare>::empty() const
{
    return m_size == 0;
}

// return queue size
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE uint32 priority_queue<Key,Container,Compare>::size() const
{
    return m_size;
}

// push an element
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE void priority_queue<Key,Container,Compare>::push(const Key key)
{
    // check whether the queue is full
    m_size++;
    m_queue.resize( m_size+1 ); // we need one more entry for things to work out

	uint32 r = m_size;
	while (r > 1) // sift up new item
	{		
		const uint32 p = r/2;
        if (! m_cmp( m_queue[p], key )) // in proper order
			break;

        m_queue[r] = m_queue[p]; // else swap with parent
		r = p;
	}
    m_queue[r] = key; // insert new item at final location
}

// pop an element
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE void priority_queue<Key,Container,Compare>::pop()
{
	Key dn = m_queue[m_size--];  // last item in queue
    m_queue.resize( m_size+1 );  // we need one more entry for things to work out

    uint32 p = 1;                // p points to item out of position
	uint32 r = p << 1;           // left child of p

	while (r <= m_size) // while r is still within the heap
	{
		// set r to smaller child of p
		if (r < m_size  && m_cmp( m_queue[r], m_queue[r+1] )) r++;
		if (! m_cmp( dn, m_queue[r] ))	// in proper order
			break;

		m_queue[p] = m_queue[r];    // else swap with child
		p = r;                      // advance pointers
		r = p<<1;
	}
    m_queue[p] = m_queue[m_size+1]; // insert last item in proper place
}

// top of the queue
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE Key& priority_queue<Key,Container,Compare>::top()
{
    return m_queue[1];
}

// top of the queue
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE Key priority_queue<Key,Container,Compare>::top() const
{
    return m_queue[1];
}

// return the i-th element in the heap
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE const Key& priority_queue<Key,Container,Compare>::operator[] (const uint32 i) const
{
    return m_queue[1+i];
}

// clear the queue
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE void priority_queue<Key,Container,Compare>::clear()
{
    m_size = 0;
    m_queue.resize(0);
}

// locate the largest element v such that v <= x; return end() if no
// such element exists
//
template <typename Key, typename Container, typename Compare>
CUGAR_FORCEINLINE CUGAR_HOST_DEVICE typename priority_queue<Key,Container,Compare>::iterator
priority_queue<Key,Container,Compare>::upper_bound(const Key& x)
{
    uint32 max_i = 0;
    Key    max;

    for (uint32 j = 1; j < size()+1; ++j)
    {
        if (!m_cmp( x, m_queue[j] )) // m_queue[j] <= x
        {
            if (max_i == 0 || !m_cmp( m_queue[j], max )) // m_queue[j] >= max
            {
                // found a new maximum
                max = m_queue[j];
                max_i = j;
            }
        }
    }

    if (max_i == 0)
        return end();

    return begin() + max_i-1;
}

} // namespace cugar
