// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#include <vector> 
#include <new>
// regular deque implementation in VS.net does not allocate blocks of memory,
// but by blocks growing by an increasing size of 50%. This can be a problem
// by allocating (a) too many blocks. (b) getting to a situation where the last
// block allocated is a little too big and memory allocation fails. 
// Instead we have here subset functionality with 64K blocks. Works faster than
// VS implementation and is able to allocate up to 2Gb no problem. 
namespace Engine
{
	namespace Utilities
	{
		template <typename T> class BlockDeque
		{
			int mint_num_stored ; 
			int mint_num_left_in_current_block ; 

			int CLS_SIZE ; 
			int NUM_ELEMENTS_IN_BLOCK ; 
			int BLOCK_SIZE ; 
			//! Stores all the data found in memory blocks of size BLOCK_SIZE. This allows 
			// large amount of data to be kept in memory without necessarily requiring them 
			// to be in contiguous memory spaces which can often result in memory allocation 
			// being unsuccessful. A personal version of deque.
			std::vector<T*> mvect_blocks_ptr ; 
			inline void CopyPts(std::vector<T> &vect_data, T *dest, int start_index, int num_pts)
			{
				int to_index = start_index + num_pts ; 
				int num_copied = 0 ;
				// using the actual memory location in the vector. 
				// Lets hope no one performs an insert in the meanwhile. 
				memcpy(dest, &vect_data.at(start_index), num_pts * CLS_SIZE) ; 
			}
		protected:
			inline T GetPoint(int block_num, int offset) 
			{
				return mvect_blocks_ptr.at(block_num)[offset] ; 
			}  

		public:
			friend class iterator ; 
			class iterator
			{
			public:
				const T& operator*() const
				{	// return designated object
					return *mptr_data ; 
				}
				iterator()
				{
					_Mycont = NULL ; 
					mptr_data = NULL ; 
					mint_block_offset = 0 ; 
					mint_current_block = 0 ; 
				}
				iterator(int pos, const BlockDeque<T> *pdeque)
				{
					_Mycont = pdeque ; 
					int block = pos/_Mycont->NUM_ELEMENTS_IN_BLOCK ; 
					int offset = pos% _Mycont->NUM_ELEMENTS_IN_BLOCK ; 
					if (block < (int)_Mycont->mvect_blocks_ptr.size())
						mptr_data = _Mycont->mvect_blocks_ptr.at(block) + offset ; 
					else
						mptr_data = NULL ;
					mint_block_offset = offset ; 
					mint_current_block = block ; 
				}
				iterator(int block, int offset, const BlockDeque<T> *pdeque)
				{
					_Mycont = pdeque ; 
					if (block < (int) _Mycont->mvect_blocks_ptr.size())
						mptr_data = _Mycont->mvect_blocks_ptr.at(block) + offset ; 
					else
						mptr_data = NULL ;
					mint_block_offset = offset ; 
					mint_current_block = block ; 
				}
				iterator& operator++()
				{	// preincrement
					++mint_block_offset ; 
					if (mint_block_offset == _Mycont->NUM_ELEMENTS_IN_BLOCK)
					{
						mint_current_block++ ; 
						mint_block_offset = 0 ; 
						if (mint_current_block < (int) _Mycont->mvect_blocks_ptr.size())
						{
							mptr_data = _Mycont->mvect_blocks_ptr[mint_current_block] ; 
						}
						else
						{
							mptr_data = NULL ; 
						}
					}
					else
					{
						mptr_data++ ; 
					}
					return (*this);
				}

				iterator operator++(int)
				{	// postincrement
					iterator _Tmp = *this;
					++*this;
					return (_Tmp);
				}

				iterator& operator--()
				{	
					// predecrement
					--mint_block_offset ; 
					if (mint_block_offset == -1)
					{
						mint_current_block-- ; 
						mint_block_offset = _Mycont->NUM_ELEMENTS_IN_BLOCK-1 ; 
						if (mint_current_block >= 0)
						{
							mptr_data = _Mycont->mvect_blocks_ptr[mint_current_block] + mint_block_offset ; 
						}
						else
						{
							mptr_data = NULL ; 
						}
					}
					else
					{
						--mptr_data ; 
					}
					return (*this);
				}

				iterator operator--(int)
				{	// postdecrement
					iterator _Tmp = *this;
					--*this;
					return (_Tmp);
				}

				bool operator!=(const iterator& _Right) const
				{	// test for iterator equality
				return (_Mycont != _Right._Mycont 
					|| mint_current_block != _Right.mint_current_block 
					|| mint_block_offset != _Right.mint_block_offset );
				}

				bool operator==(const iterator& _Right) const
				{	// test for iterator equality
				return (_Mycont == _Right._Mycont 
					&& mint_current_block == _Right.mint_current_block 
					&& mint_block_offset == _Right.mint_block_offset );
				}
				bool operator<=(const iterator& _Right) const
				{	// test for iterator equality
					if (mint_current_block != _Right.mint_current_block)
						return mint_current_block <= _Right.mint_current_block ; 
					return mint_block_offset <= _Right.mint_block_offset; 
				}
				bool operator<(const iterator& _Right) const
				{	// test for iterator equality
					if (mint_current_block != _Right.mint_current_block)
						return mint_current_block < _Right.mint_current_block ; 
					return mint_block_offset < _Right.mint_block_offset; 
				}

				bool operator>=(const iterator& _Right) const
				{	// test for iterator equality
					if (mint_current_block != _Right.mint_current_block)
						return mint_current_block >= _Right.mint_current_block ; 
					return mint_block_offset >= _Right.mint_block_offset; 
				}
				bool operator>(const iterator& _Right) const
				{	// test for iterator equality
					if (mint_current_block != _Right.mint_current_block)
						return mint_current_block > _Right.mint_current_block ; 
					return mint_block_offset > _Right.mint_block_offset; 
				}

				protected:
					const BlockDeque<T> *_Mycont;	// pointer to deque
					const T *mptr_data ; 
					int mint_block_offset ;	// offset of element in current block. 
					int mint_current_block ; 
			} ; 

			inline iterator begin()
			{
				return iterator(0,0, this) ;
			}
			iterator end()
			{
				if (mvect_blocks_ptr.size() != 0)
					return iterator(mvect_blocks_ptr.size()-1,NUM_ELEMENTS_IN_BLOCK-mint_num_left_in_current_block, this) ;
				return iterator(0,0, this) ;
			}

			inline iterator get_iter(int pos)
			{
				if (pos < mint_num_stored)
					return iterator(pos, this) ;
				return end() ; 
			}

			int size()
			{
				return mint_num_stored ; 
			}
			inline T GetPoint(int pt_num) 
			{
				int block_num = pt_num/NUM_ELEMENTS_IN_BLOCK ; 
				int index = pt_num % NUM_ELEMENTS_IN_BLOCK ; 
				return mvect_blocks_ptr.at(block_num)[index] ; 
			}  

			void SetBlockSize(int block_size)
			{
				BLOCK_SIZE = block_size ; 
				if (BLOCK_SIZE < CLS_SIZE)
					BLOCK_SIZE = CLS_SIZE ; 
				NUM_ELEMENTS_IN_BLOCK = BLOCK_SIZE/CLS_SIZE ; 
				Clear() ; 
			}

			BlockDeque<T>()
			{
				CLS_SIZE = sizeof(T) ;
				BLOCK_SIZE = 1024 * 64 ; 
				if (BLOCK_SIZE < CLS_SIZE)
					BLOCK_SIZE = CLS_SIZE ; 
				NUM_ELEMENTS_IN_BLOCK = BLOCK_SIZE/CLS_SIZE ; 
				Clear() ; 
			}

			BlockDeque<T>(int block_size)
			{
				CLS_SIZE = sizeof(T) ;
				BLOCK_SIZE = block_size ; 
				if (BLOCK_SIZE < CLS_SIZE)
					throw "Block size cannot be smaller than class size" ; 

				NUM_ELEMENTS_IN_BLOCK = BLOCK_SIZE/CLS_SIZE ; 
				Clear() ; 
			}

			~BlockDeque<T>(void)
			{
			}

			void Clear()
			{
				int num_blocks = mvect_blocks_ptr.size() ; 
				for (int block = 0 ; block < num_blocks ; block++)
				{
					T* block_ptr = mvect_blocks_ptr[block] ; 
					delete [] block_ptr ; 
				}

				mvect_blocks_ptr.clear() ; 
				mint_num_stored = 0 ;
				mint_num_left_in_current_block = 0 ; 
			}

			void Append(std::vector<T> &vect_data)
			{
				Append(vect_data, 0) ; 
			}

			void Append(std::vector<T> &vect_data, int start_index)
			{
				Append(vect_data, start_index, vect_data.size() - start_index) ; 
			}

			void Append(std::vector<T> &vect_data, int start_index, int num_pts)
			{
				T *destPtr = NULL ;  
				if (mint_num_stored != 0 && mint_num_left_in_current_block != 0)
				{
					destPtr = mvect_blocks_ptr.at(mvect_blocks_ptr.size()-1) + (NUM_ELEMENTS_IN_BLOCK-mint_num_left_in_current_block) ; 
				}

				mint_num_stored += num_pts ; 

				while (start_index <  num_pts)
				{
					if (num_pts > start_index + mint_num_left_in_current_block)
					{
						if (mint_num_left_in_current_block != 0)
						{
							CopyPts(vect_data, destPtr, start_index, mint_num_left_in_current_block) ; 
							start_index += mint_num_left_in_current_block ; 
							mint_num_left_in_current_block = 0 ; 
						}
						else
						{
							destPtr = new T [NUM_ELEMENTS_IN_BLOCK] ;  
							if (destPtr == NULL)
							{
								throw std::bad_alloc( );
							}
							mint_num_left_in_current_block = NUM_ELEMENTS_IN_BLOCK ; 
							mvect_blocks_ptr.push_back(destPtr) ; 
						}
					}
					else
					{
						CopyPts(vect_data, destPtr, start_index, num_pts - start_index) ; 
						mint_num_left_in_current_block -= (num_pts - start_index) ; 
						start_index = num_pts ; 
					}
				}
			}

			void Get(std::vector<T> &vect_data, int start_index, int num_to_get)
			{
				int current_block = start_index/NUM_ELEMENTS_IN_BLOCK ; 
				int in_block_index = start_index % NUM_ELEMENTS_IN_BLOCK ; 
				T *ptPtr = mvect_blocks_ptr[current_block] ; 
				ptPtr += in_block_index ; 

				int num_blocks = mvect_blocks_ptr.size() ; 
				int stop_index = start_index + num_to_get ; 
				for (int pt_index = start_index ; pt_index < stop_index ; pt_index++)
				{
					vect_data.push_back(*ptPtr) ; 
					in_block_index++ ; 
					if (in_block_index == NUM_ELEMENTS_IN_BLOCK)
					{
						current_block++ ; 
						if (current_block == num_blocks)
							break ; 
						ptPtr = mvect_blocks_ptr[current_block] ; 
						in_block_index = 0 ; 
					}
					else
					{
						ptPtr++ ; 
					}
				}
			}
		};
}
}