#include "pch.h"
#include "MyVector.h"
#include "Node.h"
#include "Ray.h"

class MyVectorTest : public ::testing::Test
{
protected:
    std::shared_ptr<MyVector> vector1;
    std::shared_ptr<MyVector> vector2;
    std::shared_ptr<MyVector> vector3;
    std::shared_ptr<Node> node1;
    std::shared_ptr<Node> node2;

    void SetUp() override
    {
        vector1 = std::make_shared<MyVector>( nullptr, 0.0, 0.0 );
        vector2 = std::make_shared<MyVector>( nullptr, 0.0, 0.0 );
        vector3 = std::make_shared<MyVector>( nullptr, 0.0, 0.0 );
        node1 = std::make_shared<Node>( 0.0, 0.0 );
        node2 = std::make_shared<Node>( 0.0, 0.0 );

        vector1->origin = node1;
        vector2->origin = node1;
        vector3->origin = node2;

        vector1->x = 1.0;
        vector1->y = 2.0;
        vector2->x = 1.0;
        vector2->y = 2.0;
        vector3->x = 2.0;
        vector3->y = 6.0;
    }
};

TEST_F( MyVectorTest, Equals_SameVector_ReturnsTrue )
{
    EXPECT_TRUE( vector1->equals( vector2 ) );
}

TEST_F( MyVectorTest, Equals_DifferentVector_ReturnsFalse )
{
    EXPECT_FALSE( vector1->equals( vector3 ) );
}

TEST_F( MyVectorTest, Equals_NullVector_ReturnsFalse )
{
    EXPECT_FALSE( vector1->equals( nullptr ) );
}

TEST_F( MyVectorTest, Equals_DifferentType_ReturnsFalse )
{
    std::shared_ptr<Constants> constElem = std::make_shared<Node>( 0.0, 0.0 );
    EXPECT_FALSE( vector1->equals( constElem ) );
}

TEST( MyVectorTests, LengthTest )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 3.0, 4.0 );
    EXPECT_DOUBLE_EQ( v1.length(), 5.0 );

    MyVector v2( origin, 5.0, 12.0 );
    EXPECT_DOUBLE_EQ( v2.length(), 13.0 );

    MyVector v3( origin, 8.0, 15.0 );
    EXPECT_DOUBLE_EQ( v3.length(), 17.0 );

    MyVector v4( origin, 0.0, 0.0 );
    EXPECT_DOUBLE_EQ( v4.length(), 0.0 );
}

TEST( MyVectorTests, AngleTest_PositiveX )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, 1.0, 0.0 );
    EXPECT_DOUBLE_EQ( vector.angle(), 0.0 );
}

TEST( MyVectorTests, AngleTest_PositiveY )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, 0.0, 1.0 );
    EXPECT_DOUBLE_EQ( vector.angle(), std::numbers::pi / 2.0 );
}

TEST( MyVectorTests, AngleTest_NegativeX )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, -1.0, 0.0 );
    EXPECT_DOUBLE_EQ( vector.angle(), std::numbers::pi );
}

TEST( MyVectorTests, AngleTest_NegativeY )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, 0.0, -1.0 );
    EXPECT_DOUBLE_EQ( vector.angle(), -std::numbers::pi / 2.0 );
}

TEST( MyVectorTests, AngleTest_PositiveXY )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, 1.0, 1.0 );
    EXPECT_DOUBLE_EQ( vector.angle(), std::acos( 1.0 / std::sqrt( 2.0 ) ) );
}

TEST( MyVectorTests, AngleTest_NegativeXY )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, -1.0, -1.0 );
    EXPECT_DOUBLE_EQ( vector.angle(), -std::acos( 1.0 / std::sqrt( 2.0 ) )-std::numbers::pi/2 );
}

TEST( MyVectorTests, PosAngle_ZeroVector )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 0.0, 0.0 );
    EXPECT_DOUBLE_EQ( v.posAngle(), 0.0 );
}

TEST( MyVectorTests, PosAngle_PositiveX )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 1.0, 0.0 );
    EXPECT_DOUBLE_EQ( v.posAngle(), 0.0 );
}

TEST( MyVectorTests, PosAngle_PositiveY )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 0.0, 1.0 );
    EXPECT_DOUBLE_EQ( v.posAngle(), Constants::PIdiv2 );
}

TEST( MyVectorTests, PosAngle_NegativeX )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, -1.0, 0.0 );
    EXPECT_DOUBLE_EQ( v.posAngle(), std::numbers::pi );
}

TEST( MyVectorTests, PosAngle_NegativeY )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 0.0, -1.0 );
    EXPECT_DOUBLE_EQ( v.posAngle(), Constants::PIx3div2 );
}

TEST( MyVectorTests, PosAngle_PositiveXY )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 1.0, 1.0 );
    EXPECT_DOUBLE_EQ( v.posAngle(), std::acos( 1.0 / std::sqrt( 2.0 ) ) );
}

TEST( MyVectorTests, PosAngle_NegativeXY )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, -1.0, -1.0 );
    EXPECT_DOUBLE_EQ( v.posAngle(), std::numbers::pi + std::acos( 1.0 / std::sqrt( 2.0 ) ) );
}

TEST( MyVectorTests, SetLengthAndAngle )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, 1.0, 0.0 );

    vector.setLengthAndAngle( 2.0, std::numbers::pi / 2.0 );
    EXPECT_NEAR( vector.x, 0.0, 1e-9 );
    EXPECT_NEAR( vector.y, 2.0, 1e-9 );

    vector.setLengthAndAngle( 3.0, std::numbers::pi );
    EXPECT_NEAR( vector.x, -3.0, 1e-9 );
    EXPECT_NEAR( vector.y, 0.0, 1e-9 );

    vector.setLengthAndAngle( 4.0, 3 * std::numbers::pi / 2.0 );
    EXPECT_NEAR( vector.x, 0.0, 1e-9 );
    EXPECT_NEAR( vector.y, -4.0, 1e-9 );

    vector.setLengthAndAngle( 5.0, 0.0 );
    EXPECT_NEAR( vector.x, 5.0, 1e-9 );
    EXPECT_NEAR( vector.y, 0.0, 1e-9 );
}

TEST( MyVectorTests, PlusVectors )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 3.0, 4.0 );
    MyVector v2( origin, 1.0, 2.0 );

    MyVector result = v1.plus( v2 );

    EXPECT_DOUBLE_EQ( result.x, 4.0 );
    EXPECT_DOUBLE_EQ( result.y, 6.0 );
    EXPECT_TRUE( result.origin == origin );
}

TEST( MyVectorTests, PlusVectorsWithDifferentOrigins )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 1.0 );
    MyVector v1( origin1, 3.0, 4.0 );
    MyVector v2( origin2, 1.0, 2.0 );

    MyVector result = v1.plus( v2 );

    EXPECT_DOUBLE_EQ( result.x, 4.0 );
    EXPECT_DOUBLE_EQ( result.y, 6.0 );
    EXPECT_TRUE( result.origin == origin1 );
}

TEST( MyVectorTests, MinusVectors )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 5.0, 3.0 );
    MyVector v2( origin, 2.0, 1.0 );
    MyVector result = v1.minus( v2 );

    EXPECT_DOUBLE_EQ( result.x, 3.0 );
    EXPECT_DOUBLE_EQ( result.y, 2.0 );
}

TEST( MyVectorTests, MinusVectorsWithNegativeResult )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 2.0, 1.0 );
    MyVector v2( origin, 5.0, 3.0 );
    MyVector result = v1.minus( v2 );

    EXPECT_DOUBLE_EQ( result.x, -3.0 );
    EXPECT_DOUBLE_EQ( result.y, -2.0 );
}

TEST( MyVectorTests, MinusVectorsWithZeroResult )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 2.0, 1.0 );
    MyVector v2( origin, 2.0, 1.0 );
    MyVector result = v1.minus( v2 );

    EXPECT_DOUBLE_EQ( result.x, 0.0 );
    EXPECT_DOUBLE_EQ( result.y, 0.0 );
}

TEST( MyVectorTests, MulTest )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 3.0, 4.0 );
    double scalar = 2.0;

    MyVector result = v.mul( scalar );

    EXPECT_DOUBLE_EQ( result.x, 6.0 );
    EXPECT_DOUBLE_EQ( result.y, 8.0 );
    EXPECT_EQ( result.origin, origin );
}

TEST( MyVectorTests, MulTestWithNegativeScalar )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 3.0, 4.0 );
    double scalar = -1.0;

    MyVector result = v.mul( scalar );

    EXPECT_DOUBLE_EQ( result.x, -3.0 );
    EXPECT_DOUBLE_EQ( result.y, -4.0 );
    EXPECT_EQ( result.origin, origin );
}

TEST( MyVectorTests, MulTestWithZeroScalar )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 3.0, 4.0 );
    double scalar = 0.0;

    MyVector result = v.mul( scalar );

    EXPECT_DOUBLE_EQ( result.x, 0.0 );
    EXPECT_DOUBLE_EQ( result.y, 0.0 );
    EXPECT_EQ( result.origin, origin );
}

TEST( MyVectorTests, DivByPositiveNumber )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 10.0, 20.0 );
    MyVector result = v.div( 2.0 );

    EXPECT_DOUBLE_EQ( result.x, 5.0 );
    EXPECT_DOUBLE_EQ( result.y, 10.0 );
}

TEST( MyVectorTests, DivByNegativeNumber )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 10.0, 20.0 );
    MyVector result = v.div( -2.0 );

    EXPECT_DOUBLE_EQ( result.x, -5.0 );
    EXPECT_DOUBLE_EQ( result.y, -10.0 );
}

TEST( MyVectorTests, DivByZero )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 10.0, 20.0 );

    EXPECT_THROW( v.div( 0.0 ), std::invalid_argument );
}

TEST( MyVectorTests, DivByOne )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 10.0, 20.0 );
    MyVector result = v.div( 1.0 );

    EXPECT_DOUBLE_EQ( result.x, 10.0 );
    EXPECT_DOUBLE_EQ( result.y, 20.0 );
}

TEST( MyVectorTests, ComputePosAngle_ZeroAngle )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 1.0, 0.0 );
    EXPECT_NEAR( v1.computePosAngle( v2 ), 0.0, 1e-9 );
}

TEST( MyVectorTests, ComputePosAngle_90Degrees )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 0.0, 1.0 );
    EXPECT_NEAR( v1.computePosAngle( v2 ), Constants::PIdiv2, 1e-9 );
}

TEST( MyVectorTests, ComputePosAngle_180Degrees )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, -1.0, 0.0 );
    EXPECT_NEAR( v1.computePosAngle( v2 ), Constants::DEG_180, 1e-9 );
}

TEST( MyVectorTests, ComputePosAngle_270Degrees )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 0.0, -1.0 );
    EXPECT_NEAR( v1.computePosAngle( v2 ), Constants::PIx3div2, 1e-9 );
}

TEST( MyVectorTests, ComputePosAngle_45Degrees )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 1.0, 1.0 );
	EXPECT_NEAR( v1.computePosAngle( v2 ), Constants::PIdiv2/2, 1e-9 );
}

TEST( MyVectorTests, IsCWto_SameVector )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 1.0 );
    EXPECT_FALSE( v1.isCWto( v1 ) );
}

TEST( MyVectorTests, IsCWto_FirstQuadrant )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 1.0 );
    MyVector v2( origin, 1.0, 2.0 );
    EXPECT_TRUE( v1.isCWto( v2 ) );
}

TEST( MyVectorTests, IsCWto_SecondQuadrant )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, -1.0, 1.0 );
    MyVector v2( origin, -2.0, 1.0 );
    EXPECT_TRUE( v1.isCWto( v2 ) );
}

TEST( MyVectorTests, IsCWto_ThirdQuadrant )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, -1.0, -1.0 );
    MyVector v2( origin, -1.0, -2.0 );
    EXPECT_TRUE( v1.isCWto( v2 ) );
}

TEST( MyVectorTests, IsCWto_FourthQuadrant )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, -1.0 );
    MyVector v2( origin, 2.0, -1.0 );
    EXPECT_TRUE( v1.isCWto( v2 ) );
}

TEST( MyVectorTests, IsCWto_DifferentQuadrants )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 1.0 );
    MyVector v2( origin, -0.5, -1.0 );
    EXPECT_FALSE( v1.isCWto( v2 ) );
}

TEST( MyVectorTests, ComputeAngle_ZeroAngle )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 1.0, 0.0 );
    EXPECT_DOUBLE_EQ( v1.computeAngle( v2 ), 0.0 );
}

TEST( MyVectorTests, ComputeAngle_90Degrees )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 0.0, 1.0 );
    EXPECT_DOUBLE_EQ( v1.computeAngle( v2 ), std::numbers::pi / 2.0 );
}

TEST( MyVectorTests, ComputeAngle_180Degrees )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, -1.0, 0.0 );
    EXPECT_DOUBLE_EQ( v1.computeAngle( v2 ), std::numbers::pi );
}

TEST( MyVectorTests, ComputeAngle_270Degrees )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 0.0, -1.0 );
    EXPECT_DOUBLE_EQ( v1.computeAngle( v2 ), -std::numbers::pi / 2.0 );
}

TEST( MyVectorTests, ComputeAngle_45Degrees )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 1.0, 1.0 );
    EXPECT_DOUBLE_EQ( v1.computeAngle( v2 ), std::numbers::pi / 4.0 );
}

TEST( MyVectorTests, CrossProductTest )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 3.0, 4.0 );
    MyVector v2( origin, 4.0, 3.0 );

    double result = v1.cross( v2 );
    EXPECT_DOUBLE_EQ( result, -7.0 );

    MyVector v3( origin, 1.0, 0.0 );
    MyVector v4( origin, 0.0, 1.0 );

    result = v3.cross( v4 );
    EXPECT_DOUBLE_EQ( result, 1.0 );

    MyVector v5( origin, 0.0, 0.0 );
    MyVector v6( origin, 0.0, 0.0 );

    result = v5.cross( v6 );
    EXPECT_DOUBLE_EQ( result, 0.0 );
}

TEST( MyVectorTests, PointIntersectsAt_Intersection )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin1, 1.0, 1.0 );

    auto origin2 = std::make_shared<Node>( 1.0, 0.0 );
    MyVector v2( origin2, -1.0, 1.0 );

    auto intersection = v1.pointIntersectsAt( v2 );
    ASSERT_NE( intersection, nullptr );
    EXPECT_DOUBLE_EQ( intersection->x, 0.5 );
    EXPECT_DOUBLE_EQ( intersection->y, 0.5 );
}

TEST( MyVectorTests, PointIntersectsAt_NoIntersection )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin1, 1.0, 1.0 );

    auto origin2 = std::make_shared<Node>( 2.0, 0.0 );
    MyVector v2( origin2, 1.0, 1.0 );

    auto intersection = v1.pointIntersectsAt( v2 );
    ASSERT_EQ( intersection, nullptr );
}

TEST( MyVectorTests, PointIntersectsAt_ParallelVectors )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin1, 1.0, 1.0 );

    auto origin2 = std::make_shared<Node>( 1.0, 1.0 );
    MyVector v2( origin2, 1.0, 1.0 );

    auto intersection = v1.pointIntersectsAt( v2 );
    ASSERT_EQ( intersection, nullptr );
}

TEST( MyVectorTests, PointIntersectsAt_IntersectionAtOrigin )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin1, 1.0, 0.0 );

    auto origin2 = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v2( origin2, 0.0, 1.0 );

    auto intersection = v1.pointIntersectsAt( v2 );
    ASSERT_NE( intersection, nullptr );
    EXPECT_DOUBLE_EQ( intersection->x, 0.0 );
    EXPECT_DOUBLE_EQ( intersection->y, 0.0 );
}

TEST( MyVectorTests, DotProductWithItself )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v( origin, 3.0, 4.0 );
    double result = v.dot( v );
    EXPECT_DOUBLE_EQ( result, 25.0 ); // 3^2 + 4^2 = 9 + 16 = 25
}

TEST( MyVectorTests, DotProductWithOrthogonalVector )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 0.0 );
    MyVector v2( origin, 0.0, 1.0 );
    double result = v1.dot( v2 );
	EXPECT_TRUE( abs( result - 0.0 ) < 1e-12 ); // Orthogonal vectors have dot product 0
}

TEST( MyVectorTests, DotProductWithParallelVector )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 2.0, 3.0 );
    MyVector v2( origin, 4.0, 6.0 );
    double result = v1.dot( v2 );
    EXPECT_DOUBLE_EQ( result, 26.0 ); // 2*4 + 3*6 = 8 + 18 = 26
}

TEST( MyVectorTests, DotProductWithNegativeComponents )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, -1.0, -2.0 );
    MyVector v2( origin, 3.0, 4.0 );
    double result = v1.dot( v2 );
    EXPECT_DOUBLE_EQ( result, -11.0 ); // -1*3 + -2*4 = -3 + -8 = -11
}

TEST( MyVectorTests, DotProductWithZeroVector )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 0.0, 0.0 );
    MyVector v2( origin, 3.0, 4.0 );
    double result = v1.dot( v2 );
    EXPECT_DOUBLE_EQ( result, 0.0 ); // Zero vector has dot product 0 with any vector
}

TEST( MyVectorTests, DescrTest )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, 3.0, 4.0 );
    std::string expected = "(0.000000, 0.000000), (3.000000, 4.000000)";
    EXPECT_EQ( vector.descr(), expected );
}

TEST( MyVectorTests, DescrTestWithNegativeValues )
{
    auto origin = std::make_shared<Node>( -1.0, -1.0 );
    MyVector vector( origin, -2.0, -3.0 );
    std::string expected = "(-1.000000, -1.000000), (-3.000000, -4.000000)";
    EXPECT_EQ( vector.descr(), expected );
}

TEST( MyVectorTests, DescrTestWithZeroValues )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector vector( origin, 0.0, 0.0 );
    std::string expected = "(0.000000, 0.000000), (0.000000, 0.000000)";
    EXPECT_EQ( vector.descr(), expected );
}

TEST( MyVectorTests, Intersects_ParallelVectors )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 1.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, 1.0, 1.0 );
    EXPECT_FALSE( v1.intersects( v2 ) );
}

TEST( MyVectorTests, Intersects_IntersectingVectors )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 0.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, -1.0, 1.0 );
    EXPECT_TRUE( v1.intersects( v2 ) );
}

TEST( MyVectorTests, Intersects_NonIntersectingVectors )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 2.0, 2.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, 1.0, 1.0 );
    EXPECT_FALSE( v1.intersects( v2 ) );
}

TEST( MyVectorTests, Intersects_CollinearNonOverlappingVectors )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 2.0, 2.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, 1.0, 1.0 );
    EXPECT_FALSE( v1.intersects( v2 ) );
}

TEST( MyVectorTests, PointIntersects_True )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 1.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, -1.0, -1.0 );

    EXPECT_TRUE( v1.pointIntersects( v2 ) );
}

TEST( MyVectorTests, PointIntersects_False_Parallel )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 1.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, 1.0, 1.0 );

    EXPECT_FALSE( v1.pointIntersects( v2 ) );
}

TEST( MyVectorTests, PointIntersects_False_NoIntersection )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 2.0, 2.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, 1.0, 1.0 );

    EXPECT_FALSE( v1.pointIntersects( v2 ) );
}

TEST( MyVectorTests, PointIntersects_True_AtEdge )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 0.0 );
    MyVector v1( origin1, 1.0, 0.0 );
    MyVector v2( origin2, 0.0, 1.0 );

    EXPECT_TRUE( v1.pointIntersects( v2 ) );
}

TEST( MyVectorTests, InnerPointIntersects_SameOrigin )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 1.0, 1.0 );
    MyVector v2( origin, 2.0, 2.0 );
    EXPECT_FALSE( v1.innerpointIntersects( v2 ) );
}

TEST( MyVectorTests, InnerPointIntersects_ParallelVectors )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 1.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, 2.0, 2.0 );
    EXPECT_FALSE( v1.innerpointIntersects( v2 ) );
}

TEST( MyVectorTests, InnerPointIntersects_IntersectingAtEdge )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 1.0 );
    MyVector v1( origin1, 2.0, 2.0 );
    MyVector v2( origin2, 1.0, 1.0 );
    EXPECT_FALSE( v1.innerpointIntersects( v2 ) );
}

TEST( MyVectorTests, InnerPointIntersects_IntersectingAtInnerPoint )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 1.0, 0.0 );
    MyVector v1( origin1, 2.0, 2.0 );
    MyVector v2( origin2, 0.0, 2.0 );
    EXPECT_TRUE( v1.innerpointIntersects( v2 ) );
}

TEST( MyVectorTests, InnerPointIntersects_NonIntersecting )
{
    auto origin1 = std::make_shared<Node>( 0.0, 0.0 );
    auto origin2 = std::make_shared<Node>( 3.0, 3.0 );
    MyVector v1( origin1, 1.0, 1.0 );
    MyVector v2( origin2, 1.0, 1.0 );
    EXPECT_FALSE( v1.innerpointIntersects( v2 ) );
}

TEST( MyVectorTests, CrossProductWithVector )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 3.0, 4.0 );
    MyVector v2( origin, 1.0, 2.0 );
    double result = v1.cross( v2 );
    EXPECT_DOUBLE_EQ( result, 2.0 );
}

TEST( MyVectorTests, CrossProductWithRay )
{
    auto origin = std::make_shared<Node>( 0.0, 0.0 );
    MyVector v1( origin, 3.0, 4.0 );
    Ray r( origin, 0.0 );
    double result = v1.cross( r );
    EXPECT_DOUBLE_EQ( result, -4.0 );
}