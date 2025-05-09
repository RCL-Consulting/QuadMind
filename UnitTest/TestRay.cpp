#include "pch.h"

#include "Ray.h"
#include "Node.h"
#include "Edge.h"
#include "MyVector.h"

#include <numbers>

class RayTest : public ::testing::Test
{
protected:
    std::shared_ptr<Node> origin = std::make_shared<Node>(0.0, 0.0);
    std::shared_ptr<Node> origin1 = std::make_shared<Node>(1.0, 1.0);

    void SetUp() override
    {
        Node::mLastNumber = 0;
        origin->setXY(0.0, 0.0);
        origin1->setXY(1.0, 1.0);
    }

    void TearDown() override
    {

    }
};


TEST_F( RayTest, ConstructorWithOriginAndAngle )
{
    double angle = Constants::PIdiv2; // 90 degrees

    Ray ray( origin, angle );

    EXPECT_EQ( ray.origin, origin );
    EXPECT_NEAR( ray.x, 0.0, 1e-9 );
    EXPECT_NEAR( ray.y, 1.0, 1e-9 );
}

TEST_F( RayTest, ConstructorWithOriginAndPassThrough )
{
    origin->x = 0.0;
    origin->y = 0.0;

    auto passThrough = std::make_shared<Node>( 0.0, 0.0 );
    passThrough->x = 1.0;
    passThrough->y = 1.0;

    Ray ray( origin, passThrough );

    EXPECT_EQ( ray.origin, origin );
    EXPECT_NEAR( ray.x, std::sqrt( 2 ) / 2, 1e-9 );
    EXPECT_NEAR( ray.y, std::sqrt( 2 ) / 2, 1e-9 );
}

//TODO: Implement this test
//TEST_F(RayTest, ConstructorWithOriginRelEdgeAndAngle)
//{
//    auto origin = std::make_shared<Node>();
//    auto relEdge = std::make_shared<Edge>();
//    double angle = Constants::PIdiv2; // 90 degrees
//
//    // Mock the angleAt method of Edge
//    EXPECT_CALL( *relEdge, angleAt( origin ) ).WillOnce( Return( 0.0 ) );
//
//    Ray ray( origin, relEdge, angle );
//
//    EXPECT_EQ( ray.origin, origin );
//    EXPECT_NEAR( ray.x, 0.0, 1e-9 );
//    EXPECT_NEAR( ray.y, 1.0, 1e-9 );
//}

TEST_F( RayTest, CrossProductWithVector )
{
    Ray ray( origin, 0.0 ); // Ray along the x-axis
    MyVector vector( origin, 0.0, 1.0 ); // Vector along the y-axis

    double result = ray.cross( vector );
    EXPECT_DOUBLE_EQ( result, 1.0 ); // Cross product should be 1
}

TEST_F( RayTest, CrossProductWithVectorAtAngle )
{
    Ray ray( origin, std::numbers::pi / 4 ); // Ray at 45 degrees
    MyVector vector( origin, 1.0, 0.0 ); // Vector along the x-axis

    double result = ray.cross( vector );
    EXPECT_DOUBLE_EQ( result, -std::sin( std::numbers::pi / 4 ) ); // Cross product should be -sin(45 degrees)
}

TEST_F( RayTest, CrossProductWithVectorNegative )
{
    Ray ray( origin, std::numbers::pi / 2 ); // Ray along the y-axis
    MyVector vector( origin, 1.0, 0.0 ); // Vector along the x-axis

    double result = ray.cross( vector );
    EXPECT_DOUBLE_EQ( result, -1.0 ); // Cross product should be -1
}

TEST_F( RayTest, PointIntersectsAt_ParallelRays )
{
    Ray ray1( origin, 0.0 );
    MyVector vector2( origin1, 1.0, 0.0 );

    auto result = ray1.pointIntersectsAt( vector2 );
    EXPECT_EQ( result, nullptr );
}

TEST_F( RayTest, PointIntersectsAt_IntersectingRays )
{
    Ray ray1( origin, 0.0 );
    MyVector vector2( origin1, -1.0, -1.0 );

    auto result = ray1.pointIntersectsAt( vector2 );
    ASSERT_NE( result, nullptr );
    EXPECT_DOUBLE_EQ( result->x, 0.0 );
    EXPECT_DOUBLE_EQ( result->y, 0.0 );
}

TEST_F( RayTest, PointIntersectsAt_NoIntersection )
{
    Ray ray1( origin, 0.0 );
    MyVector vector2( origin1, 0.0, 1.0 );

    auto result = ray1.pointIntersectsAt( vector2 );
    EXPECT_EQ( result, nullptr );
}

TEST_F( RayTest, PointIntersectsAt_IntersectionAtOrigin )
{
    Ray ray1( origin, 0.0 );
    MyVector vector2( origin, 1.0, 0.0 );

    auto result = ray1.pointIntersectsAt( vector2 );
    ASSERT_NE( result, nullptr );
    EXPECT_DOUBLE_EQ( result->x, 0.0 );
    EXPECT_DOUBLE_EQ( result->y, 0.0 );
}

TEST_F( RayTest, ValuesTest )
{
    Ray ray1( origin, 0.0 );
    EXPECT_EQ( ray1.values(), "1, x= 1.000000, y= 0.000000" );

    Ray ray2( origin, std::numbers::pi / 2 );
    EXPECT_EQ( ray2.values(), "1, x= 0.000000, y= 1.000000" );

    auto passThrough = std::make_shared<Node>( 1.0, 1.0 );
    Ray ray3( origin, passThrough );
    EXPECT_EQ( ray3.values(), "1, x= 0.707107, y= 0.707107" );
}

TEST_F( RayTest, DescrWithAngle )
{
    Ray ray( origin, 45.0 * Constants::toRadians );
    std::string expected = "2, (0.707107, 0.707107)";
    EXPECT_EQ( ray.descr(), expected );
}

TEST_F( RayTest, DescrWithPassThrough )
{
    auto passThrough = std::make_shared<Node>( 1.0, 1.0 );
    Ray ray( origin, passThrough );
    std::string expected = "1, (0.707107, 0.707107)";

    std::string temp = ray.descr();

    EXPECT_EQ( ray.descr(), expected );
}

//TEST_F( RayTest, DescrWithRelEdge )
//{
//    auto relEdge = std::make_shared<Edge>();
//    Ray ray( origin, relEdge, 45.0 * Constants::toRadians );
//    std::string expected = "(0.000000, 0.000000), (0.707107, 0.707107)";
//    EXPECT_EQ( ray.descr(), expected );
//}