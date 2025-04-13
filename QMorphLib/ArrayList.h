#pragma once

#include <vector>

template< typename T >
class ArrayList 
{
public:

	void set( size_t Index, const T& Item )
	{
		mArray[Index] = Item;
	}

	void remove( size_t Index )
	{
		if ( Index >= mArray.size() )
			throw std::out_of_range( "Index out of range in remove" );
		mArray.erase( mArray.begin() + Index );
	}

	bool isEmpty() const
	{
		return mArray.empty();
	}

	void clear()
	{
		mArray.clear();
	}

	void add( const T& item )
	{
		mArray.push_back( item );
	}

	auto size() const
	{
		return mArray.size();
	}

	const T& get( size_t index ) const
	{
		return mArray.at( index );
	}

	T& get( size_t index )
	{
		return mArray.at( index );
	}

	auto begin() 
	{ 
		return mArray.begin();
	}
	
	auto end()
	{ 
		return mArray.end();
	}

	auto begin() const 
	{ 
		return mArray.begin();
	}

	auto end() const 
	{
		return mArray.end();
	}

	bool contains( const T& item ) const
	{
		return indexOf( item ) != -1;
	}

	std::ptrdiff_t indexOf( const T& value ) const
	{
		return indexOfRange( value, 0, mArray.size() );
	}

	std::ptrdiff_t indexOfRange( const T& value, 
								 size_t start, 
								 size_t end ) const
	{
		if ( start > end || end > mArray.size() )
			throw std::out_of_range( "Invalid range in index_of_range" );

		for ( size_t i = start; i < end; ++i )
		{
			if ( mArray[i]->equals( value ) )
			{
				return static_cast<std::ptrdiff_t>( i );
			}
		}
		return -1; // not found
	}

private:
	std::vector<T> mArray;
};