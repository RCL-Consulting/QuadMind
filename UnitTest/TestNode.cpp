#include "pch.h"

#include "Constants.h"

#include "Node.h"
#include "Edge.h"

class NodeTest : public ::testing::Test
{
protected:
    std::shared_ptr<Node> node_ptr = std::make_shared<Node>(1.0, 2.0);
    std::shared_ptr<Node> node_ptr1 = std::make_shared<Node>(0.0, 0.0);
    Node node;

    void SetUp() override
    {
        node_ptr->setXY(1.0, 2.0);
        node_ptr1->setXY(0.0, 0.0);
        node.setXY(1.0, 2.0);
    }

    void TearDown() override
    {

    }
};


TEST_F( NodeTest, Equals_SameCoordinates_ReturnsTrue )
{
    EXPECT_TRUE(node_ptr->equals(node_ptr) );
}

TEST_F( NodeTest, Equals_DifferentXCoordinate_ReturnsFalse )
{
    auto node2 = std::make_shared<Node>( 2.0, 2.0 );

    EXPECT_FALSE(node_ptr->equals( node2 ) );
}

TEST_F( NodeTest, Equals_DifferentYCoordinate_ReturnsFalse )
{
    auto node2 = std::make_shared<Node>( 1.0, 3.0 );

    EXPECT_FALSE(node_ptr->equals( node2 ) );
}

TEST_F( NodeTest, Equals_DifferentCoordinates_ReturnsFalse )
{
    auto node2 = std::make_shared<Node>( 3.0, 4.0 );

    EXPECT_FALSE(node_ptr->equals( node2 ) );
}

TEST_F( NodeTest, Equals_SameObject_ReturnsTrue )
{
    EXPECT_TRUE(node_ptr->equals(node_ptr) );
}

TEST_F( NodeTest, CopyNode )
{
    // Create a node
    auto node2 = std::make_shared<Node>( 2.0, 3.0 );
    node_ptr->edgeList.add( std::make_shared<Edge>(node_ptr, node2 ) );

    // Copy the node
    auto copiedNode = node_ptr->copy();

    // Check if the copied node has the same coordinates
    EXPECT_EQ( copiedNode->x, 1.0 );
    EXPECT_EQ( copiedNode->y, 2.0 );

    // Check if the edgeList is copied
    EXPECT_EQ( copiedNode->edgeList.size(), 1 );
    EXPECT_EQ( copiedNode->edgeList.get(0), node_ptr->edgeList.get(0) );
}

TEST_F( NodeTest, CopyXY )
{
    auto copy = node.copyXY();
    EXPECT_EQ( copy->x, 1.0 );
    EXPECT_EQ( copy->y, 2.0 );
}

TEST_F( NodeTest, SetXY_UpdatesCoordinates )
{
    // Arrange
    Node node2( 3.0, 4.0 );

    // Act
    node.setXY( node2 );

    // Assert
    EXPECT_DOUBLE_EQ( node.x, 3.0 );
    EXPECT_DOUBLE_EQ( node.y, 4.0 );
}

TEST_F( NodeTest, SetXY_DoesNotAffectOtherProperties )
{
    // Arrange
    auto node2 = std::make_shared<Node>( 3.0, 4.0 );
    node_ptr->edgeList.add( std::make_shared<Edge>(node_ptr, node2) );

    // Act
    node_ptr->setXY( *node2 );

    // Assert
    EXPECT_EQ(node_ptr->edgeList.size(), 1 );
    EXPECT_EQ(node_ptr->edgeList.get(0), node_ptr->edgeList.get(0) );
}

TEST_F( NodeTest, CrossProduct )
{
    Node node1( 3.0, 4.0 );

    double result = node1.cross( node );
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

TEST_F( NodeTest, ConnectToEdge_AddsEdgeToList )
{
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto edge = std::make_shared<Edge>(node_ptr1, node2);

    node_ptr1->connectToEdge( edge );

    ASSERT_TRUE(node_ptr1->edgeList.contains( edge ) );
}

TEST_F( NodeTest, ConnectToEdge_DoesNotAddDuplicateEdge )
{
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto edge = std::make_shared<Edge>(node_ptr1, node2);

    node_ptr1->connectToEdge( edge );
    node_ptr1->connectToEdge( edge );

    auto count = std::count(node_ptr1->edgeList.begin(), node_ptr1->edgeList.end(), edge );
    ASSERT_EQ( count, 1 );
}

TEST_F( NodeTest, LengthTest )
{
    auto node2 = std::make_shared<Node>( 3.0, 4.0 );

    double length = node_ptr1->length( node2 );

    EXPECT_DOUBLE_EQ( length, 5.0 );
}

TEST_F( NodeTest, LengthTestSameNode )
{
    auto node1 = std::make_shared<Node>( 1.0, 1.0 );

    double length = node1->length( node1 );

    EXPECT_DOUBLE_EQ( length, 0.0 );
}

TEST_F( NodeTest, LengthTestNegativeCoordinates )
{
    auto node1 = std::make_shared<Node>( -1.0, -1.0 );
    auto node2 = std::make_shared<Node>( -4.0, -5.0 );

    double length = node1->length( node2 );

    EXPECT_DOUBLE_EQ( length, 5.0 );
}

TEST_F( NodeTest, DescrReturnsCorrectString )
{
    Node node( 3.5, 7.2 );
    EXPECT_EQ( node.descr(), "52" );
}

TEST_F( NodeTest, DescrHandlesNegativeCoordinates )
{
    Node node( -1.0, -2.5 );
    EXPECT_EQ( node.descr(), "55" );
}

TEST_F( NodeTest, DescrHandlesZeroCoordinates )
{
    Node node( 0.0, 0.0 );
    EXPECT_EQ( node.descr(), "58" );
}

TEST_F( NodeTest, ValDescrTest )
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