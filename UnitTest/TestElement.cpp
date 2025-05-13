#include "pch.h"
#include "Element.h"
#include "Node.h"

//All tests are working.

class ElementTest : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }

    void TearDown() override
    {

    }
};


TEST( ElementTest, CrossProductTest )
{
    auto o1 = std::make_shared<Node>( 0.0, 0.0 );
    auto p1 = std::make_shared<Node>( 1.0, 0.0 );
    auto o2 = std::make_shared<Node>( 0.0, 0.0 );
    auto p2 = std::make_shared<Node>( 0.0, 1.0 );

    double result = Element::cross( o1, p1, o2, p2 );
    EXPECT_DOUBLE_EQ( result, 1.0 );

    p1 = std::make_shared<Node>( 0.0, 1.0 );
    p2 = std::make_shared<Node>( 1.0, 0.0 );

    result = Element::cross( o1, p1, o2, p2 );
    EXPECT_DOUBLE_EQ( result, -1.0 );

    p1 = std::make_shared<Node>( 1.0, 1.0 );
    p2 = std::make_shared<Node>( 1.0, 1.0 );

    result = Element::cross( o1, p1, o2, p2 );
    EXPECT_DOUBLE_EQ( result, 0.0 );
}

