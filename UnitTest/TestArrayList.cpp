#include "pch.h"
#include "ArrayList.h"

class TestObject
{
public:
    TestObject( int value ) : value( value ) {}
    bool equals( const std::shared_ptr<TestObject>& other ) const
    {
		return value == other->value;
    }
private:
    int value;
};

TEST( ArrayListTest, AddAndSize )
{
    ArrayList<std::shared_ptr<TestObject>> list;
    list.add( std::make_shared<TestObject>( 1 ) );
    list.add( std::make_shared<TestObject>( 2 ) );
    EXPECT_EQ( list.size(), 2 );
}

TEST( ArrayListTest, Get )
{
    ArrayList<std::shared_ptr<TestObject>> list;
    auto obj1 = std::make_shared<TestObject>( 1 );
    auto obj2 = std::make_shared<TestObject>( 2 );
    list.add( obj1 );
    list.add( obj2 );
    EXPECT_EQ( list.get( 0 ), obj1 );
    EXPECT_EQ( list.get( 1 ), obj2 );
}

TEST( ArrayListTest, Contains )
{
    ArrayList<std::shared_ptr<TestObject>> list;
    auto obj1 = std::make_shared<TestObject>( 1 );
    auto obj2 = std::make_shared<TestObject>( 2 );
    list.add( obj1 );
    list.add( obj2 );
    EXPECT_TRUE( list.contains( obj1 ) );
    EXPECT_TRUE( list.contains( obj2 ) );
    EXPECT_TRUE( list.contains( std::make_shared<TestObject>( 2 ) ) );
    EXPECT_FALSE( list.contains( std::make_shared<TestObject>( 3 ) ) );
}

TEST( ArrayListTest, IndexOf )
{
    ArrayList<std::shared_ptr<TestObject>> list;
    auto obj1 = std::make_shared<TestObject>( 1 );
    auto obj2 = std::make_shared<TestObject>( 2 );
    list.add( obj1 );
    list.add( obj2 );
    EXPECT_EQ( list.indexOf( obj1 ), 0 );
    EXPECT_EQ( list.indexOf( obj2 ), 1 );
    EXPECT_EQ( list.indexOf( std::make_shared<TestObject>( 3 ) ), -1 );
}

TEST( ArrayListTest, IndexOfRange )
{
    ArrayList<std::shared_ptr<TestObject>> list;
    auto obj1 = std::make_shared<TestObject>( 1 );
    auto obj2 = std::make_shared<TestObject>( 2 );
    auto obj3 = std::make_shared<TestObject>( 3 );
    list.add( obj1 );
    list.add( obj2 );
    list.add( obj3 );
    EXPECT_EQ( list.indexOfRange( obj2, 1, 3 ), 1 );
    EXPECT_EQ( list.indexOfRange( obj3, 1, 3 ), 2 );
    EXPECT_EQ( list.indexOfRange( obj1, 1, 3 ), -1 );
}