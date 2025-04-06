#include "pch.h"

#include "Constants.h"

#include "Node.h"
#include "Edge.h"


TEST( NodeTest, Equals_SameCoordinates_ReturnsTrue )
{
    auto node1 = std::make_shared<Node>( 1.0, 2.0 );
    auto node2 = std::make_shared<Node>( 1.0, 2.0 );

    EXPECT_TRUE( node1->equals( node2 ) );
}

TEST( NodeTest, Equals_DifferentXCoordinate_ReturnsFalse )
{
    auto node1 = std::make_shared<Node>( 1.0, 2.0 );
    auto node2 = std::make_shared<Node>( 2.0, 2.0 );

    EXPECT_FALSE( node1->equals( node2 ) );
}

TEST( NodeTest, Equals_DifferentYCoordinate_ReturnsFalse )
{
    auto node1 = std::make_shared<Node>( 1.0, 2.0 );
    auto node2 = std::make_shared<Node>( 1.0, 3.0 );

    EXPECT_FALSE( node1->equals( node2 ) );
}

TEST( NodeTest, Equals_DifferentCoordinates_ReturnsFalse )
{
    auto node1 = std::make_shared<Node>( 1.0, 2.0 );
    auto node2 = std::make_shared<Node>( 3.0, 4.0 );

    EXPECT_FALSE( node1->equals( node2 ) );
}

TEST( NodeTest, Equals_SameObject_ReturnsTrue )
{
    auto node1 = std::make_shared<Node>( 1.0, 2.0 );

    EXPECT_TRUE( node1->equals( node1 ) );
}

TEST( NodeTest, CopyNode )
{
    // Create a node
    auto node1 = std::make_shared<Node>( 1.0, 2.0 );
    auto node2 = std::make_shared<Node>( 2.0, 3.0 );
	node1->edgeList.add( std::make_shared<Edge>( node1, node2 ) );

    // Copy the node
    auto copiedNode = node1->copy();

    // Check if the copied node has the same coordinates
    EXPECT_EQ( copiedNode->x, 1.0 );
    EXPECT_EQ( copiedNode->y, 2.0 );

    // Check if the edgeList is copied
    EXPECT_EQ( copiedNode->edgeList.size(), 1 );
    EXPECT_EQ( copiedNode->edgeList.get(0), node1->edgeList.get(0) );
}

TEST( NodeTest, CopyXY )
{
    Node node( 1.0, 2.0 );
    auto copy = node.copyXY();
    EXPECT_EQ( copy->x, 1.0 );
    EXPECT_EQ( copy->y, 2.0 );
}

TEST( NodeTests, SetXY_UpdatesCoordinates )
{
    // Arrange
    Node node1( 1.0, 2.0 );
    Node node2( 3.0, 4.0 );

    // Act
    node1.setXY( node2 );

    // Assert
    EXPECT_DOUBLE_EQ( node1.x, 3.0 );
    EXPECT_DOUBLE_EQ( node1.y, 4.0 );
}

TEST( NodeTests, SetXY_DoesNotAffectOtherProperties )
{
    // Arrange
    auto node1 = std::make_shared<Node>( 1.0, 2.0 );
    auto node2 = std::make_shared<Node>( 3.0, 4.0 );
    node1->edgeList.add( std::make_shared<Edge>(node1, node2) );

    // Act
    node1->setXY( *node2 );

    // Assert
    EXPECT_EQ( node1->edgeList.size(), 1 );
    EXPECT_EQ( node1->edgeList.get(0), node1->edgeList.get(0) );
}

TEST( NodeTest, CrossProduct )
{
    Node node1( 3.0, 4.0 );
    Node node2( 1.0, 2.0 );

    double result = node1.cross( node2 );
    EXPECT_DOUBLE_EQ( result, 2.0 );

    Node node3( 0.0, 0.0 );
    Node node4( 5.0, 5.0 );

    result = node3.cross( node4 );
    EXPECT_DOUBLE_EQ( result, 0.0 );

    Node node5( -1.0, -1.0 );
    Node node6( 1.0, 1.0 );

    result = node5.cross( node6 );
    EXPECT_DOUBLE_EQ( result, 0.0 );
}

TEST( NodeTests, ConnectToEdge_AddsEdgeToList )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto edge = std::make_shared<Edge>(node1 , node2);

    node1->connectToEdge( edge );

    ASSERT_TRUE( node1->edgeList.contains( edge ) );
}

TEST( NodeTests, ConnectToEdge_DoesNotAddDuplicateEdge )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto edge = std::make_shared<Edge>(node1, node2);

    node1->connectToEdge( edge );
    node1->connectToEdge( edge );

    auto count = std::count( node1->edgeList.begin(), node1->edgeList.end(), edge );
    ASSERT_EQ( count, 1 );
}

TEST( NodeTests, LengthTest )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 3.0, 4.0 );

    double length = node1->length( node2 );

    EXPECT_DOUBLE_EQ( length, 5.0 );
}

TEST( NodeTests, LengthTestSameNode )
{
    auto node1 = std::make_shared<Node>( 1.0, 1.0 );

    double length = node1->length( node1 );

    EXPECT_DOUBLE_EQ( length, 0.0 );
}

TEST( NodeTests, LengthTestNegativeCoordinates )
{
    auto node1 = std::make_shared<Node>( -1.0, -1.0 );
    auto node2 = std::make_shared<Node>( -4.0, -5.0 );

    double length = node1->length( node2 );

    EXPECT_DOUBLE_EQ( length, 5.0 );
}

TEST( NodeTests, DescrReturnsCorrectString )
{
    Node node( 3.5, 7.2 );
    EXPECT_EQ( node.descr(), "(3.500000, 7.200000)" );
}

TEST( NodeTests, DescrHandlesNegativeCoordinates )
{
    Node node( -1.0, -2.5 );
    EXPECT_EQ( node.descr(), "(-1.000000, -2.500000)" );
}

TEST( NodeTests, DescrHandlesZeroCoordinates )
{
    Node node( 0.0, 0.0 );
    EXPECT_EQ( node.descr(), "(0.000000, 0.000000)" );
}

TEST( NodeTest, ValDescrTest )
{
    // Test case 1: pattern with multiple elements
    Node node1( 0.0, 0.0 );
    node1.pattern = { 5, 3, 4, 2, 1 };
    EXPECT_EQ( node1.valDescr(), "3-421" );

    // Test case 2: pattern with single element
    Node node2( 0.0, 0.0 );
    node2.pattern = { 3, 2, 1 };
    EXPECT_EQ( node2.valDescr(), "2-1" );

    // Test case 3: pattern with no elements
    Node node3( 0.0, 0.0 );
    node3.pattern = { 2, 0 };
    EXPECT_EQ( node3.valDescr(), "0-" );
}