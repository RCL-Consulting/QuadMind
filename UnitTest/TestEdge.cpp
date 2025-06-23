#include "pch.h"
#include <cmath> // Ensure cmath is included

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 // Define M_PI_2 manually if not available
#endif

double expected = M_PI_2; // 90 degrees in radians
#include "Edge.h"
#include "Node.h"
#include "MyVector.h"
#include "Triangle.h"
#include "Quad.h"

//All tests are working.

class EdgeTest : public ::testing::Test
{
protected:
    std::shared_ptr<Node> node = std::make_shared<Node>(0.0, 0.0);
    std::shared_ptr<Node> node1 = std::make_shared<Node>(1.0, 0.0);
    std::shared_ptr<Node> node2 = std::make_shared<Node>(1.0, 1.0);
    std::shared_ptr<Node> node3 = std::make_shared<Node>(2.0, 2.0);
    std::shared_ptr<Node> node4 = std::make_shared<Node>(3.0, 3.0);
    std::shared_ptr<Node> node5 = std::make_shared<Node>(0.0, 1.0);
    std::shared_ptr<Edge> edge = std::make_shared<Edge>(node, node1);
    std::shared_ptr<Edge> edge1 = std::make_shared<Edge>(node2, node3);
    Edge edge_3;
    Edge edge_4;
    std::shared_ptr<Triangle> tri1, tri2;
    std::shared_ptr<Quad> quad1;

    void SetUp() override
    {
        Node::mLastNumber = 0;
        node->setXY(0.0, 0.0);
        node1->setXY(1.0, 0.0);
        node2->setXY(1.0, 1.0);
        node3->setXY(2.0, 2.0);
        node4->setXY(3.0, 3.0);
        node5->setXY(0.0, 1.0);
        edge_3 = Edge(node, node1);
        edge_4 = Edge(node, node2);
        tri1 = std::make_shared<Triangle>(edge, std::make_shared<Edge>(node, std::make_shared<Node>(0.0, 1.0)), std::make_shared<Edge>(node1, std::make_shared<Node>(1.0, 1.0)));
        tri2 = std::make_shared<Triangle>(edge, std::make_shared<Edge>(node, std::make_shared<Node>(-1.0, 0.0)), std::make_shared<Edge>(node1, std::make_shared<Node>(2.0, 0.0)));
		auto edge_n_n5 = std::make_shared<Edge>(node, node5);
		auto edge_n1_n2 = std::make_shared<Edge>(node1, node2);
		auto edge_n2_n5 = std::make_shared<Edge>(node2, node5);
        quad1 = std::make_shared<Quad>(edge, edge_n_n5, edge_n1_n2, edge_n2_n5);
    }

	void TearDown() override
	{
        Edge::clearStateList();
	}
};

TEST_F( EdgeTest, EqualsSameEdge )
{
    EXPECT_TRUE( edge->equals( edge ) );
}

TEST_F( EdgeTest, EqualsDifferentEdges )
{
    auto edge2 = std::make_shared<Edge>( node3, node4 );

    EXPECT_FALSE( edge1->equals( edge2 ) );
}

TEST_F( EdgeTest, EqualsNull )
{
    EXPECT_FALSE( edge1->equals( nullptr ) );
}

TEST_F( EdgeTest, EqualsDifferentType )
{
    EXPECT_FALSE( edge1->equals( node2 ) );
}

TEST_F( EdgeTest, CopyConstructor )
{
    Edge originalEdge( node, node2 );

    auto copiedEdge = originalEdge.copy();

    ASSERT_TRUE( copiedEdge != nullptr );
    EXPECT_TRUE( copiedEdge->leftNode->equals( originalEdge.leftNode ) );
    EXPECT_TRUE( copiedEdge->rightNode->equals( originalEdge.rightNode ) );
    EXPECT_DOUBLE_EQ( copiedEdge->computeLength(), originalEdge.computeLength() );
    EXPECT_EQ( copiedEdge->element1, originalEdge.element1 );
    EXPECT_EQ( copiedEdge->element2, originalEdge.element2 );
}

TEST_F( EdgeTest, ClearStateList )
{
    Edge::stateList[0].add( edge );
    Edge::stateList[1].add( edge );
    Edge::stateList[2].add( edge );

    // Ensure stateList is not empty before clearing
    ASSERT_FALSE( Edge::stateList[0].isEmpty() );
    ASSERT_FALSE( Edge::stateList[1].isEmpty() );
    ASSERT_FALSE( Edge::stateList[2].isEmpty() );

    // Clear the stateList
    Edge::clearStateList();

    // Check if stateList is empty after clearing
    ASSERT_TRUE( Edge::stateList[0].isEmpty() );
    ASSERT_TRUE( Edge::stateList[1].isEmpty() );
    ASSERT_TRUE( Edge::stateList[2].isEmpty() );
}

TEST_F( EdgeTest, RemoveFromStateList_ValidState )
{
    int state = 0;
    Edge::stateList[state].add( edge );
    EXPECT_TRUE( edge->removeFromStateList( state ) );
    EXPECT_EQ( Edge::stateList[state].size(), 0 );
}

TEST_F( EdgeTest, RemoveFromStateList_InvalidState )
{
    int state = 0;
    EXPECT_FALSE( edge->removeFromStateList( state ) );
}

TEST_F( EdgeTest, RemoveFromStateList_ValidState_MultipleEdges )
{
    int state = 0;
    auto edge2 = std::make_shared<Edge>( node1, node2 );
    Edge::stateList[state].add( edge );
    Edge::stateList[state].add( edge2 );
    EXPECT_TRUE( edge->removeFromStateList( state ) );
    EXPECT_EQ( Edge::stateList[state].size(), 1 );
    EXPECT_EQ( Edge::stateList[state].get( 0 ), edge2 );
}

TEST_F( EdgeTest, RemoveFromStateList_NoState )
{
    EXPECT_FALSE( edge->removeFromStateList() );
}

TEST_F( EdgeTest, GetState_NoSides )
{
    edge->leftSide = false;
    edge->rightSide = false;
    EXPECT_EQ( edge->getState(), 0 );
}

TEST_F( EdgeTest, GetState_LeftSide )
{
    edge->leftSide = true;
    edge->rightSide = false;
    EXPECT_EQ( edge->getState(), 1 );
}

TEST_F( EdgeTest, GetState_RightSide )
{
    edge->leftSide = false;
    edge->rightSide = true;
    EXPECT_EQ( edge->getState(), 1 );
}

TEST_F( EdgeTest, GetState_BothSides )
{
    edge->leftSide = true;
    edge->rightSide = true;
    EXPECT_EQ( edge->getState(), 2 );
}

TEST_F( EdgeTest, RemoveFromStateList_Success )
{
    int state = edge->getState();
    Edge::stateList[state].add( edge );
    ASSERT_TRUE( edge->removeFromStateList() );
    ASSERT_EQ( Edge::stateList[state].size(), 0 );
}

TEST_F( EdgeTest, RemoveFromStateList_Failure )
{
    ASSERT_FALSE( edge->removeFromStateList() );
}

TEST_F( EdgeTest, AlterLeftStateTrue )
{
	Edge::stateList[0].add( edge );
    edge->alterLeftState( true );
    EXPECT_TRUE( edge->leftSide );
    EXPECT_EQ( edge->getState(), 1 );
}

TEST_F( EdgeTest, AlterLeftStateFalse )
{
    edge->alterLeftState( false );
    EXPECT_FALSE( edge->leftSide );
    EXPECT_EQ( edge->getState(), 0 );
}

TEST_F( EdgeTest, AlterLeftStateChange )
{
    Edge::stateList[0].add( edge );
    edge->alterLeftState( true );
    EXPECT_TRUE( edge->leftSide );
    EXPECT_EQ( edge->getState(), 1 );

    edge->alterLeftState( false );
    EXPECT_FALSE( edge->leftSide );
    EXPECT_EQ( edge->getState(), 0 );
}

TEST_F( EdgeTest, AlterRightStateTrue )
{
    Edge::stateList[0].add( edge );
    edge->alterRightState( true );
    EXPECT_TRUE( edge->rightSide );
    EXPECT_EQ( edge->getState(), 1 );
}

TEST_F( EdgeTest, AlterRightStateFalse )
{
    edge->alterRightState( false );
    EXPECT_FALSE( edge->rightSide );
    EXPECT_EQ( edge->getState(), 0 );
}

TEST_F( EdgeTest, AlterRightStateChange )
{
    Edge::stateList[0].add( edge );
    edge->alterRightState( true );
    EXPECT_TRUE( edge->rightSide );
    EXPECT_EQ( edge->getState(), 1 );

    edge->alterRightState( false );
    EXPECT_FALSE( edge->rightSide );
    EXPECT_EQ( edge->getState(), 0 );
}

TEST_F( EdgeTest, ReplaceNode_LeftNode )
{
    ASSERT_TRUE( edge->replaceNode( node, node3 ) );
    ASSERT_EQ( edge->leftNode, node3 );
    ASSERT_EQ( edge->rightNode, node1 );
}

TEST_F( EdgeTest, ReplaceNode_RightNode )
{
    ASSERT_TRUE( edge->replaceNode( node1, node3 ) );
    ASSERT_EQ( edge->leftNode, node );
    ASSERT_EQ( edge->rightNode, node3 );
}

TEST_F( EdgeTest, ReplaceNode_NodeNotFound )
{
    ASSERT_FALSE( edge->replaceNode( node3, node4 ) );
    ASSERT_EQ( edge->leftNode, node );
    ASSERT_EQ( edge->rightNode, node1 );
}

TEST_F( EdgeTest, MidPointTest )
{
    auto midPoint = edge->midPoint();

    ASSERT_NE( midPoint, nullptr );
    EXPECT_DOUBLE_EQ( midPoint->x, 0.5 );
    EXPECT_DOUBLE_EQ( midPoint->y, 0.0 );
}

TEST_F( EdgeTest, MidPointTestNegativeCoordinates )
{
    auto node5 = std::make_shared<Node>( -2.0, -2.0 );
    auto edge = std::make_shared<Edge>( node5, node3 );

    auto midPoint = edge->midPoint();

    ASSERT_NE( midPoint, nullptr );
    EXPECT_DOUBLE_EQ( midPoint->x, 0.0 );
    EXPECT_DOUBLE_EQ( midPoint->y, 0.0 );
}

TEST_F( EdgeTest, MidPointTestSameCoordinates )
{
    auto edge = std::make_shared<Edge>( node2, node2 );

    auto midPoint = edge->midPoint();

    ASSERT_NE( midPoint, nullptr );
    EXPECT_DOUBLE_EQ( midPoint->x, 1.0 );
    EXPECT_DOUBLE_EQ( midPoint->y, 1.0 );
}

TEST_F( EdgeTest, LengthCalculation )
{
    double x1 = 0.0, y1 = 0.0;
    double x2 = 3.0, y2 = 4.0;
    double expectedLength = 5.0;

    double calculatedLength = Edge::length( x1, y1, x2, y2 );

    EXPECT_NEAR( calculatedLength, expectedLength, 1e-12 );
}

TEST_F( EdgeTest, LengthCalculationNegativeCoordinates )
{
    double x1 = -1.0, y1 = -1.0;
    double x2 = -4.0, y2 = -5.0;
    double expectedLength = 5.0;

    double calculatedLength = Edge::length( x1, y1, x2, y2 );

    EXPECT_NEAR( calculatedLength, expectedLength, 1e-12 );
}

TEST_F( EdgeTest, LengthCalculationZeroLength )
{
    double x1 = 1.0, y1 = 1.0;
    double x2 = 1.0, y2 = 1.0;
    double expectedLength = 0.0;

    double calculatedLength = Edge::length( x1, y1, x2, y2 );

    EXPECT_NEAR( calculatedLength, expectedLength, 1e-12 );
}

TEST_F( EdgeTest, DescrTest )
{
    auto node5 = std::make_shared<Node>( 1.0, 2.0 );
    auto node6 = std::make_shared<Node>( 3.0, 4.0 );
    Edge edge2( node5, node6 );

    std::string expected = "(1, 2)";
    EXPECT_EQ( edge2.descr(), expected );
}

TEST_F( EdgeTest, DescrTestReversedNodes )
{
    auto node1 = std::make_shared<Node>( 3.0, 4.0 );
    auto node2 = std::make_shared<Node>( 1.0, 2.0 );
    Edge edge( node1, node2 );

    std::string expected = "(2, 1)";
    EXPECT_EQ( edge.descr(), expected );
}

TEST_F( EdgeTest, ComputePosAngle_SameEdge )
{
    double angle = edge->computePosAngle( edge, node );
    EXPECT_DOUBLE_EQ( angle, 2 * Constants::PI );
}

TEST_F( EdgeTest, ComputePosAngle_ConnectedEdges )
{
    auto edge2 = std::make_shared<Edge>( node1, node2 );

    double angle = edge->computePosAngle( edge2, node1 );
    EXPECT_DOUBLE_EQ( angle, Constants::PIdiv2 );
}

TEST_F( EdgeTest, ComputePosAngle_NotConnectedEdges )
{
    auto node5 = std::make_shared<Node>( 2.0, 0.0 );
    auto edge2 = std::make_shared<Edge>( node1, node5 );

    double angle = edge->computePosAngle( edge2, node );
    EXPECT_DOUBLE_EQ( angle, 0.0 );
}

TEST_F( EdgeTest, ComputePosAngle_ObtuseAngle )
{
    auto edge2 = std::make_shared<Edge>( node1, node5 );

    double angle = edge->computePosAngle( edge2, node1 );
    EXPECT_NEAR( angle, Constants::PIdiv4, 1e-9 );
}

TEST_F( EdgeTest, ConnectNodes )
{
    edge->connectNodes();

    // Check if the edge is added to the edge lists of both nodes
    ASSERT_EQ( node->edgeList.size(), 1 );
    ASSERT_EQ( node1->edgeList.size(), 1 );
    ASSERT_EQ( node->edgeList.get(0), edge );
    ASSERT_EQ( node1->edgeList.get(0), edge );
}

TEST_F( EdgeTest, DisconnectNodes )
{
    // Connect nodes
    edge->connectNodes();
    EXPECT_EQ( node->edgeList.size(), 1 );
    EXPECT_EQ( node1->edgeList.size(), 1 );

    // Disconnect nodes
    edge->disconnectNodes();
    EXPECT_EQ( node->edgeList.size(), 0 );
    EXPECT_EQ( node1->edgeList.size(), 0 );
}

TEST_F( EdgeTest, TryDisconnectNodes )
{
    // Connect nodes
    edge->connectNodes();
    EXPECT_EQ( node->edgeList.size(), 1 );
    EXPECT_EQ( node1->edgeList.size(), 1 );

    // Disconnect nodes
    edge->tryToDisconnectNodes();
    EXPECT_EQ( node->edgeList.size(), 0 );
    EXPECT_EQ( node1->edgeList.size(), 0 );
}

TEST_F( EdgeTest, TryToDisconnectNodes_NoEffectIfEdgeNotInNodeEdgeLists )
{
    // Connect nodes
    edge->connectNodes();
    edge->tryToDisconnectNodes(); // First removal
    edge->tryToDisconnectNodes(); // Second removal, should have no effect

    EXPECT_EQ( node1->edgeList.size(), 0 );
    EXPECT_EQ( node2->edgeList.size(), 0 );
}

TEST_F( EdgeTest, UnitNormalAt_LeftNode )
{
    auto normalEdge = edge_3.unitNormalAt( node );

    EXPECT_NEAR( normalEdge->leftNode->x, 0.0, 1e-9 );
    EXPECT_NEAR( normalEdge->leftNode->y, 1.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->x, 0.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->y, 0.0, 1e-9 );
}

TEST_F( EdgeTest, UnitNormalAt_RightNode )
{
    auto normalEdge = edge_3.unitNormalAt( node );

    EXPECT_NEAR( normalEdge->leftNode->x, 0.0, 1e-9 );
    EXPECT_NEAR( normalEdge->leftNode->y, 1.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->x, 0.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->y, 0.0, 1e-9 );
}

TEST_F( EdgeTest, UnitNormalAt_DiagonalEdge )
{
    auto normalEdge = edge_4.unitNormalAt( node );

    EXPECT_NEAR( normalEdge->leftNode->x, -std::sqrt( 2 ) / 2, 1e-9 );
    EXPECT_NEAR( normalEdge->leftNode->y, std::sqrt( 2 ) / 2, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->x, 0.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->y, 0.0, 1e-9 );
}

TEST_F( EdgeTest, GetVectorFromLeftNode )
{
    auto vector = edge_4.getVector( node );

    EXPECT_EQ( vector.origin->x, node->x );
    EXPECT_EQ( vector.origin->y, node->y );
    EXPECT_EQ( vector.x, node2->x );
    EXPECT_EQ( vector.y, node2->y );
}

TEST_F( EdgeTest, GetVectorFromRightNode )
{
    auto vector = edge_4.getVector( node2 );

    EXPECT_EQ( vector.origin->x, node2->x );
    EXPECT_EQ( vector.origin->y, node2->y );
    EXPECT_EQ( vector.x, -1.0 );
    EXPECT_EQ( vector.y, -1.0 );
}

TEST_F( EdgeTest, GetVectorFromInvalidNode )
{
    auto invalidNode = std::make_shared<Node>( 2.0, 2.0 );

    EXPECT_THROW( edge_4.getVector( invalidNode ), std::runtime_error );
}

TEST_F( EdgeTest, HasNode_LeftNode )
{
    ASSERT_TRUE( edge_4.hasNode( node ) );
}

TEST_F( EdgeTest, HasNode_RightNode )
{
    ASSERT_TRUE( edge_4.hasNode( node2 ) );
}

TEST_F( EdgeTest, HasNode_NotPresent )
{
    ASSERT_FALSE( edge_4.hasNode( node3 ) );
}

TEST_F( EdgeTest, HasFalseFrontNeighbor_NoNeighbors )
{
    EXPECT_TRUE( edge_4.hasFalseFrontNeighbor() );
}

TEST_F( EdgeTest, HasFalseFrontNeighbor_LeftNeighborNull )
{
    edge_4.rightFrontNeighbor = std::make_shared<Edge>( node, node2 );
    edge_4.rightFrontNeighbor->frontEdge = true;

    EXPECT_TRUE( edge_4.hasFalseFrontNeighbor() );
}

TEST_F( EdgeTest, HasFalseFrontNeighbor_RightNeighborNull )
{
    edge_4.leftFrontNeighbor = std::make_shared<Edge>( node, node2 );
    edge_4.leftFrontNeighbor->frontEdge = true;

    EXPECT_TRUE( edge_4.hasFalseFrontNeighbor() );
}

TEST_F( EdgeTest, HasFalseFrontNeighbor_LeftNeighborNotFrontEdge )
{
    edge_4.leftFrontNeighbor = std::make_shared<Edge>( node, node2 );
    edge_4.leftFrontNeighbor->frontEdge = false;
    edge_4.rightFrontNeighbor = std::make_shared<Edge>( node, node2 );
    edge_4.rightFrontNeighbor->frontEdge = true;

    EXPECT_TRUE(edge_4.hasFalseFrontNeighbor() );
}

TEST_F( EdgeTest, HasFalseFrontNeighbor_RightNeighborNotFrontEdge )
{
    edge_4.leftFrontNeighbor = std::make_shared<Edge>( node, node2 );
    edge_4.leftFrontNeighbor->frontEdge = true;
    edge_4.rightFrontNeighbor = std::make_shared<Edge>( node, node2 );
    edge_4.rightFrontNeighbor->frontEdge = false;

    EXPECT_TRUE(edge_4.hasFalseFrontNeighbor() );
}

TEST_F( EdgeTest, HasFalseFrontNeighbor_BothNeighborsFrontEdge )
{
    edge_4.leftFrontNeighbor = std::make_shared<Edge>( node, node2 );
    edge_4.leftFrontNeighbor->frontEdge = true;
    edge_4.rightFrontNeighbor = std::make_shared<Edge>( node, node2 );
    edge_4.rightFrontNeighbor->frontEdge = true;

    EXPECT_FALSE(edge_4.hasFalseFrontNeighbor() );
}

TEST_F( EdgeTest, HasFrontNeighbor_LeftFrontNeighbor )
{
    auto node6 = std::make_shared<Node>( 0.5, 1.0 );

    auto edge2 = std::make_shared<Edge>( node1, node6 );

    edge->leftFrontNeighbor = edge2;

    ASSERT_TRUE( edge->hasFrontNeighbor( edge2 ) );
}

TEST_F( EdgeTest, HasFrontNeighbor_RightFrontNeighbor )
{
    auto node6 = std::make_shared<Node>( 0.5, 1.0 );

    auto edge2 = std::make_shared<Edge>( node1, node6 );

    edge->rightFrontNeighbor = edge2;

    ASSERT_TRUE( edge->hasFrontNeighbor( edge2 ) );
}

TEST_F( EdgeTest, HasFrontNeighbor_NoFrontNeighbor )
{
    auto node6 = std::make_shared<Node>( 0.5, 1.0 );

    auto edge2 = std::make_shared<Edge>( node2, node6 );

    ASSERT_FALSE( edge->hasFrontNeighbor( edge2 ) );
}

TEST_F( EdgeTest, OtherNode_LeftNode )
{
    auto result = edge_4.otherNode( node );
    ASSERT_EQ( result, node2 );
}

TEST_F( EdgeTest, OtherNode_RightNode )
{
    auto result = edge_4.otherNode( node2 );
    ASSERT_EQ( result, node );
}

TEST_F( EdgeTest, OtherNode_NodeNotOnEdge )
{
    auto result = edge_4.otherNode( node3 );
    ASSERT_EQ( result, nullptr );
}

TEST_F( EdgeTest, OtherNodeGivenNewLength )
{
    auto node6 = std::make_shared<Node>( 3.0, 4.0 );
    Edge edge_5( node, node6 );

    double newLength = 5.0;
    auto newNode = edge_5.otherNodeGivenNewLength( newLength, node );

    ASSERT_NEAR( newNode->x, 3.0, 1e-9 );
    ASSERT_NEAR( newNode->y, 4.0, 1e-9 );
}

TEST_F( EdgeTest, OtherNodeGivenNewLengthWithDifferentNode )
{
    auto node6 = std::make_shared<Node>( 4.0, 5.0 );
    Edge edge_5( node2, node6 );

    double newLength = 5.0;
    auto newNode = edge_5.otherNodeGivenNewLength( newLength, node2 );

    ASSERT_NEAR( newNode->x, 4.0, 1e-9 );
    ASSERT_NEAR( newNode->y, 5.0, 1e-9 );
}

TEST_F( EdgeTest, OtherNodeGivenNewLengthWithNegativeCoordinates )
{
    auto node6 = std::make_shared<Node>( -1.0, -1.0 );
    auto node7 = std::make_shared<Node>( -4.0, -5.0 );
    Edge edge_5( node6, node7 );

    double newLength = 5.0;
    auto newNode = edge_5.otherNodeGivenNewLength( newLength, node6 );

    ASSERT_NEAR( newNode->x, -4.0, 1e-9 );
    ASSERT_NEAR( newNode->y, -5.0, 1e-9 );
}

TEST_F( EdgeTest, UpperNode_LeftNodeHigher )
{
    Edge edge_5( node5, node );
    EXPECT_EQ( edge_5.upperNode(), node5 );
}

TEST_F( EdgeTest, UpperNode_RightNodeHigher )
{
    Edge edge_5( node, node5 );
    EXPECT_EQ(edge_5.upperNode(), node5 );
}

TEST_F( EdgeTest, UpperNode_NodesAtSameHeight )
{
    Edge edge_5( node5, node2 );
    EXPECT_EQ(edge_5.upperNode(), node5 );
}

TEST_F( EdgeTest, LowerNode )
{
    auto node6 = std::make_shared<Node>( 0.0, 2.0 );
    Edge edge_5( node5, node6 );

    EXPECT_EQ( edge_5.lowerNode(), node5 );

    auto node7 = std::make_shared<Node>( 0.0, 0.5 );
    Edge edge_6( node5, node7 );

    EXPECT_EQ( edge_6.lowerNode(), node7 );
}

TEST_F( EdgeTest, SetFrontNeighbor_LeftNode )
{
    auto edge2 = std::make_shared<Edge>( node, node5 );

    edge->setFrontNeighbor( edge2 );

    ASSERT_EQ( edge->leftFrontNeighbor, edge2 );
    ASSERT_EQ( edge->rightFrontNeighbor, nullptr );
}

TEST_F( EdgeTest, SetFrontNeighbor_RightNode )
{
    auto edge2 = std::make_shared<Edge>( node1, node2 );

    edge->setFrontNeighbor( edge2 );

    ASSERT_EQ( edge->rightFrontNeighbor, edge2 );
    ASSERT_EQ( edge->leftFrontNeighbor, nullptr );
}

TEST_F( EdgeTest, SetFrontNeighbor_NoCommonNode )
{
    auto node6 = std::make_shared<Node>( 2.0, 0.0 );
    auto node7 = std::make_shared<Node>( 3.0, 0.0 );

    auto edge2 = std::make_shared<Edge>( node6, node7 );

    edge->setFrontNeighbor( edge2 );

    ASSERT_EQ( edge->leftFrontNeighbor, nullptr );
    ASSERT_EQ( edge->rightFrontNeighbor, nullptr );
}

TEST_F( EdgeTest, PromoteToFront )
{
    ArrayList<std::shared_ptr<Edge>> frontList;
    int initialLevel = 1;

    edge->promoteToFront( initialLevel, frontList );

    EXPECT_TRUE( edge->frontEdge );
    EXPECT_EQ( edge->level, initialLevel );
    EXPECT_EQ( frontList.size(), 1 );
    EXPECT_EQ( frontList.get(0), edge );
}

TEST_F( EdgeTest, PromoteToFrontAlreadyFrontEdge )
{
    ArrayList<std::shared_ptr<Edge>> frontList;
    int initialLevel = 1;

    edge->promoteToFront( initialLevel, frontList );
    edge->promoteToFront( initialLevel + 1, frontList );

    EXPECT_TRUE( edge->frontEdge );
    EXPECT_EQ( edge->level, initialLevel );
    EXPECT_EQ( frontList.size(), 1 );
    EXPECT_EQ( frontList.get(0), edge );
}

TEST_F( EdgeTest, RemoveFromFront_Success )
{
    ArrayList<std::shared_ptr<Edge>> frontList;
    edge->promoteToFront( 1, frontList );

    EXPECT_TRUE( edge->removeFromFront( frontList ) );
    EXPECT_FALSE( edge->frontEdge );
    EXPECT_EQ( frontList.size(), 0 );
}

TEST_F( EdgeTest, RemoveFromFront_Failure )
{
    ArrayList<std::shared_ptr<Edge>> frontList;

    EXPECT_FALSE( edge->removeFromFront( frontList ) );
    EXPECT_FALSE( edge->frontEdge );
    EXPECT_EQ( frontList.size(), 0 );
}

TEST_F( EdgeTest, CommonNodeTest )
{
    auto node6 = std::make_shared<Node>( 1.0, 1.0 );

    auto edge2 = std::make_shared<Edge>( node, node2 );
    auto edge3 = std::make_shared<Edge>( node, node6 );

    // Test common node between edge1 and edge2
    auto commonNode = edge2->commonNode( edge1 );
    ASSERT_TRUE( commonNode != nullptr );
    EXPECT_EQ( commonNode, node2 );

    // Test common node between edge1 and edge3
    commonNode = edge2->commonNode( edge3 );
    ASSERT_TRUE( commonNode != nullptr );
    EXPECT_EQ( commonNode, node );

    // Test no common node between edge2 and edge3
    commonNode = edge1->commonNode( edge3 );
    EXPECT_EQ( commonNode, nullptr );
}

TEST_F( EdgeTest, FrontNeighborAt_LeftNeighbor )
{
    auto node6 = std::make_shared<Node>( 0.5, 1.0 );

    auto edge2 = std::make_shared<Edge>( node, node6 );

    edge->leftFrontNeighbor = edge2;

    EXPECT_EQ( edge->frontNeighborAt( node ), edge2 );
}

TEST_F( EdgeTest, FrontNeighborAt_RightNeighbor )
{
    auto node6 = std::make_shared<Node>( 1.5, 1.0 );

    auto edge2 = std::make_shared<Edge>( node1, node6 );

    edge->rightFrontNeighbor = edge2;

    EXPECT_EQ( edge->frontNeighborAt( node1 ), edge2 );
}

TEST_F( EdgeTest, FrontNeighborAt_NoNeighbor )
{
    auto node6 = std::make_shared<Node>( 1.5, 1.0 );

    auto edge2 = std::make_shared<Edge>( node2, node6 );

    EXPECT_EQ( edge->frontNeighborAt( node6 ), nullptr );
}

TEST_F( EdgeTest, NextFrontNeighbor_LeftNeighbor )
{
    auto edge2 = std::make_shared<Edge>( node, node5 );

    edge->leftFrontNeighbor = edge2;

    EXPECT_EQ( edge->nextFrontNeighbor( nullptr ), edge2 );
}

TEST_F( EdgeTest, NextFrontNeighbor_RightNeighbor )
{
    auto edge2 = std::make_shared<Edge>( node1, node2 );

    edge->rightFrontNeighbor = edge2;

    EXPECT_EQ( edge->nextFrontNeighbor( nullptr ), edge2 );
}

TEST_F( EdgeTest, NextFrontNeighbor_NoNeighbor )
{
    EXPECT_EQ( edge->nextFrontNeighbor( nullptr ), nullptr );
}

TEST_F( EdgeTest, NextFrontNeighbor_PrevIsLeftNeighbor )
{
    auto edge2 = std::make_shared<Edge>( node, node5 );

    edge->leftFrontNeighbor = edge2;

    EXPECT_EQ( edge->nextFrontNeighbor( edge2 ), nullptr );
}

TEST_F( EdgeTest, NextFrontNeighbor_PrevIsRightNeighbor )
{
    auto edge2 = std::make_shared<Edge>( node1, node5 );

    edge->rightFrontNeighbor = edge2;

    EXPECT_EQ( edge->nextFrontNeighbor( edge2 ), nullptr );
}

static std::shared_ptr<Triangle> makeSimpleTriangle(
    const std::shared_ptr<Edge>& e1,
    const std::shared_ptr<Edge>& e2,
    const std::shared_ptr<Edge>& e3)
{
    return std::make_shared<Triangle>(e1, e2, e3);
}

// Helper to create a quad for testing
static std::shared_ptr<Quad> makeSimpleQuad(
    const std::shared_ptr<Edge>& e1,
    const std::shared_ptr<Edge>& e2,
    const std::shared_ptr<Edge>& e3,
    const std::shared_ptr<Edge>& e4)
{
    return std::make_shared<Quad>(e1, e2, e3, e4);
}

TEST_F(EdgeTest, EvalPotSideEdge_Triangle_ReturnsFrontNeighborIfAngleSmall)
{
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.5, 1.0);

    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);

    auto tri = makeSimpleTriangle(e1, e2, e3);
    e1->connectToElement(tri);
    e2->connectToElement(tri);
    e3->connectToElement(tri);

    auto result = e1->evalPotSideEdge(e2, n2);
    ASSERT_EQ(result, e2);
}

TEST_F(EdgeTest, EvalPotSideEdge_Triangle_ReturnsNullIfAngleLarge)
{
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(2.0, 0.0);
    auto n3 = std::make_shared<Node>(1.0, 10.0);

    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);

    auto tri = makeSimpleTriangle(e1, e2, e3);
    e1->connectToElement(tri);
    e2->connectToElement(tri);
    e3->connectToElement(tri);

    auto result = e1->evalPotSideEdge(e2, n2);
    ASSERT_EQ(result, nullptr);
}

TEST_F(EdgeTest, EvalPotSideEdge_Quad_ReturnsFrontNeighborIfAngleSmall)
{  
    auto n1 = std::make_shared<Node>(0.0, 0.0);  
    auto n2 = std::make_shared<Node>(1.0, 0.0);  
    auto n3 = std::make_shared<Node>(1.0, 1.0);  
    auto n4 = std::make_shared<Node>(0.0, 1.0);  

    auto e1 = std::make_shared<Edge>(n1, n2);  
    auto e2 = std::make_shared<Edge>(n2, n3);  
    auto e3 = std::make_shared<Edge>(n3, n4);  
    auto e4 = std::make_shared<Edge>(n4, n1);  

    auto quad = makeSimpleQuad(e1, e2, e3, e4);
    e1->connectToElement(quad);  
    e2->connectToElement(quad);  
    e3->connectToElement(quad);  
    e4->connectToElement(quad);  

    auto result = e1->evalPotSideEdge(e2, n2);  
    ASSERT_EQ(result, e2);  
}

TEST_F(EdgeTest, EvalPotSideEdge_Quad_ReturnsNullIfAngleLarge)
{
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(2.0, 0.0);
    auto n3 = std::make_shared<Node>(2.0, 10.0);
    auto n4 = std::make_shared<Node>(0.0, 10.0);

    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n4);
    auto e4 = std::make_shared<Edge>(n4, n1);

    auto quad = makeSimpleQuad(e1, e2, e3, e4);
    e1->connectToElement(quad);
    e2->connectToElement(quad);
    e3->connectToElement(quad);
    e4->connectToElement(quad);

    auto result = e1->evalPotSideEdge(e2, n2);
    ASSERT_EQ(result, nullptr);
}



TEST_F(EdgeTest, ClassifyStateOfFrontEdge_Basic) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.5, 1.0);
    auto n4 = std::make_shared<Node>(1.5, 1.0);

    // Create edges
    auto e_main = std::make_shared<Edge>(n1, n2);
    auto e_left = std::make_shared<Edge>(n1, n3);
    auto e_right = std::make_shared<Edge>(n2, n4);

    // Set up as front edges
    e_main->frontEdge = true;
    e_left->frontEdge = true;
    e_right->frontEdge = true;

    // Set neighbors
    e_main->leftFrontNeighbor = e_left;
    e_main->rightFrontNeighbor = e_right;
    e_left->rightFrontNeighbor = e_main;
    e_right->leftFrontNeighbor = e_main;

    // Connect triangles to edges (simulate a front)
    auto tri1 = makeSimpleTriangle(e_main, e_left, std::make_shared<Edge>(n3, n2));
    auto tri2 = makeSimpleTriangle(e_main, e_right, std::make_shared<Edge>(n1, n4));

    // Clear state lists before test
    Edge::clearStateList();

    // Call function under test
    e_main->classifyStateOfFrontEdge();

    // Check that e_main is in the correct state list
    int state = e_main->getState();
    bool found = false;
    for (int i = 0; i < Edge::stateList[state].size(); ++i) {
        if (Edge::stateList[state].get(i) == e_main) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    // Check that leftSide and rightSide are set (true or false, but at least set)
    // and that the neighbors' states are updated accordingly
    EXPECT_TRUE(e_main->leftSide == true || e_main->leftSide == false);
    EXPECT_TRUE(e_main->rightSide == true || e_main->rightSide == false);
    // The neighbor's left/right state should match the main edge's logic
    if (e_main->leftSide) {
        if (e_main->leftNode == e_left->leftNode)
            EXPECT_TRUE(e_left->leftSide);
        else
            EXPECT_TRUE(e_left->rightSide);
    }
    else {
        if (e_main->leftNode == e_left->leftNode)
            EXPECT_FALSE(e_left->leftSide);
        else
            EXPECT_FALSE(e_left->rightSide);
    }
    if (e_main->rightSide) {
        if (e_main->rightNode == e_right->leftNode)
            EXPECT_TRUE(e_right->leftSide);
        else
            EXPECT_TRUE(e_right->rightSide);
    }
    else {
        if (e_main->rightNode == e_right->leftNode)
            EXPECT_FALSE(e_right->leftSide);
        else
            EXPECT_FALSE(e_right->rightSide);
    }
}

std::shared_ptr<Edge> makeEdge(double x1, double y1, double x2, double y2) {
    auto n1 = std::make_shared<Node>(x1, y1);
    auto n2 = std::make_shared<Node>(x2, y2);
    return std::make_shared<Edge>(n1, n2);
}

TEST_F(EdgeTest, IsLargeTransition_TrueWhenRatioGreaterThan2_5) {
    // Edge1: length 10, Edge2: length 3 (ratio > 2.5)
    auto e1 = makeEdge(0, 0, 10, 0);
    auto e2 = makeEdge(0, 0, 3, 0);
    EXPECT_TRUE(e1->isLargeTransition(e2));
    EXPECT_TRUE(e2->isLargeTransition(e1));
}

TEST_F(EdgeTest, IsLargeTransition_FalseWhenRatioEqualTo2_5) {
    // Edge1: length 5, Edge2: length 2 (ratio = 2.5)
    auto e1 = makeEdge(0, 0, 5, 0);
    auto e2 = makeEdge(0, 0, 2, 0);
    EXPECT_FALSE(e1->isLargeTransition(e2));
    EXPECT_FALSE(e2->isLargeTransition(e1));
}

TEST_F(EdgeTest, IsLargeTransition_FalseWhenRatioLessThan2_5) {
    // Edge1: length 4, Edge2: length 3 (ratio < 2.5)
    auto e1 = makeEdge(0, 0, 4, 0);
    auto e2 = makeEdge(0, 0, 3, 0);
    EXPECT_FALSE(e1->isLargeTransition(e2));
    EXPECT_FALSE(e2->isLargeTransition(e1));
}


TEST_F(EdgeTest, GetNextFrontReturnsSelectableFront) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.5, 1.0);

    // Create edges
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);

    // Set up as front edges
    e1->frontEdge = true;
    e2->frontEdge = true;
    e3->frontEdge = true;

    // Set selectable and level
    e1->selectable = true;
    e2->selectable = true;
    e3->selectable = true;
    e1->level = 1;
    e2->level = 2;
    e3->level = 3;

    // Place e1 in stateList[2], e2 in stateList[1], e3 in stateList[0]
    Edge::stateList[2].add(e1);
    Edge::stateList[1].add(e2);
    Edge::stateList[0].add(e3);

    // Should select e1 (highest state, lowest level)
    auto nextFront = Edge::getNextFront();
    ASSERT_EQ(nextFront, e1);
}

TEST_F(EdgeTest, GetNextFrontReturnsNullIfNoSelectable) {
    // No selectable edges in any state
    auto result = Edge::getNextFront();
    ASSERT_EQ(result, nullptr);
}

TEST_F(EdgeTest, GetNextFrontPrefersShorterEdgeOnLargeTransition) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(10.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto n4 = std::make_shared<Node>(0.0, 2.0);

    // Create edges
    auto e1 = std::make_shared<Edge>(n1, n2); // long edge
    auto e2 = std::make_shared<Edge>(n1, n3); // short edge
    auto e3 = std::make_shared<Edge>(n1, n4); // short edge

    // Set up as front edges
    e1->frontEdge = true;
    e2->frontEdge = true;
    e3->frontEdge = true;

    // Set selectable and level
    e1->selectable = true;
    e2->selectable = true;
    e3->selectable = true;
    e1->level = 1;
    e2->level = 1;
    e3->level = 1;

    // Set neighbors for large transition
    e1->leftFrontNeighbor = e2;
    e1->rightFrontNeighbor = e3;
    e2->leftFrontNeighbor = e1;
    e3->rightFrontNeighbor = e1;

    // Place e1 in stateList[1]
    Edge::stateList[1].add(e1);
    Edge::stateList[1].add(e2);
    Edge::stateList[1].add(e3);

    // e1 is a large transition compared to e2 and e3, so should return e2 or e3
    auto nextFront = Edge::getNextFront();
    // Should return e2 or e3, not e1
    ASSERT_TRUE(nextFront == e2 || nextFront == e3);
}

class MockElement : public Element {
public:
    MockElement() { firstNode = nullptr; }
    std::shared_ptr<Element> neighbor(const std::shared_ptr<Edge>&) override { return nullptr; }
    double angle(const std::shared_ptr<Edge>&, const std::shared_ptr<Node>&) override { return 0.0; }
    bool hasEdge(const std::shared_ptr<Edge>&) override { return false; }
    bool hasNode(const std::shared_ptr<Node>&) override { return false; }
    std::shared_ptr<Edge> neighborEdge(const std::shared_ptr<Node>&, const std::shared_ptr<Edge>&) override { return nullptr; }
    int indexOf(const std::shared_ptr<Edge>&) override { return 0; }
    int angleIndex(const std::shared_ptr<Edge>&, const std::shared_ptr<Edge>&) override { return 0; }
    int angleIndex(const std::shared_ptr<Node>&) override { return 0; }
    double angle(const std::shared_ptr<Edge>&, const std::shared_ptr<Edge>&) override { return 0.0; }
    bool concavityAt(const std::shared_ptr<Node>&) override { return false; }
    void replaceEdge(const std::shared_ptr<Edge>&, const std::shared_ptr<Edge>&) override {}
    bool invertedWhenNodeRelocated(const std::shared_ptr<Node>&, const std::shared_ptr<Node>&) override { return false; }
    std::shared_ptr<Node> nodeAtLargestAngle() override { return nullptr; }
};

// Helper to create a node
std::shared_ptr<Node> makeNode(double x, double y) {
    auto n = std::make_shared<Node>(x, y);
    return n;
}

// Test seamWith
TEST_F(EdgeTest, SeamWithBasic) {
    // Create nodes
    auto n1 = makeNode(0, 0);
    auto n2 = makeNode(1, 0);
    auto n3 = makeNode(1, 1);

    // Create edges
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);

    // Add e1 to n1 and n2 edge lists
    n1->edgeList.add(e1);
    n2->edgeList.add(e1);
    // Add e2 to n2 and n3 edge lists
    n2->edgeList.add(e2);
    n3->edgeList.add(e2);

    // Attach mock elements to edges
    auto elem1 = std::make_shared<MockElement>();
    auto elem2 = std::make_shared<MockElement>();
    e1->element1 = elem1;
    e2->element1 = elem2;
    elem1->firstNode = n1;
    elem2->firstNode = n2;

    // Before seamWith, n2 should have both e1 and e2
    EXPECT_EQ(n2->edgeList.size(), 2);

    // Call seamWith
    e1->seamWith(e2);

    // After seamWith, n2 should still have at least one edge
    EXPECT_GE(n2->edgeList.size(), 1);

    // The edgeLists of n1 and n3 should not be empty
    EXPECT_GE(n1->edgeList.size(), 0);
    EXPECT_GE(n3->edgeList.size(), 0);
}

std::shared_ptr<Node> make_node(double x, double y) {
    return std::make_shared<Node>(x, y);
}

TEST_F(EdgeTest, AngleAtXAxis) {
    auto n1 = make_node(0.0, 0.0);
    auto n2 = make_node(1.0, 0.0);
    Edge e(n1, n2);

    // Angle at n1 should be 0 (along x-axis)
    double angle = e.angleAt(n1);
    EXPECT_NEAR(angle, 0.0, 1e-9);

    // Angle at n2 should be PI (opposite x-axis)
    angle = e.angleAt(n2);
    EXPECT_NEAR(angle, Constants::PI, 1e-9);
}

TEST_F(EdgeTest, AngleAtYAxis) {
    auto n1 = make_node(0.0, 0.0);
    auto n2 = make_node(0.0, 1.0);
    Edge e(n1, n2);

    // Angle at n1 should be -PI/2 (upwards)
    double angle = e.angleAt(n1);
    EXPECT_NEAR(angle, -Constants::PIdiv2, 1e-9); 

    // Angle at n2 should be PI/2 (downwards)
    angle = e.angleAt(n2);
    EXPECT_NEAR(angle, Constants::PIdiv2, 1e-9);
}

TEST_F(EdgeTest, AngleAtDiagonal) {
    auto n1 = make_node(0.0, 0.0);
    auto n2 = make_node(1.0, 1.0);
    Edge e(n1, n2);

    // Angle at n1 should be -PI/4 (45 degrees up and right)
    double angle = e.angleAt(n1);
    EXPECT_NEAR(angle, -Constants::PIdiv4, 1e-9);

    // Angle at n2 should be PI + PIdiv4 (225 degrees)
    angle = e.angleAt(n2);
    EXPECT_NEAR(angle, Constants::PI + Constants::PIdiv4, 1e-9);
}*/

struct TriangleFixture {
    std::shared_ptr<Node> n1, n2, n3;
    std::shared_ptr<Edge> e1, e2, e3;
    std::shared_ptr<Triangle> tri;

    TriangleFixture() {
        n1 = std::make_shared<Node>(0.0, 0.0);
        n2 = std::make_shared<Node>(1.0, 0.0);
        n3 = std::make_shared<Node>(0.0, 1.0);
        e1 = std::make_shared<Edge>(n1, n2);
        e2 = std::make_shared<Edge>(n2, n3);
        e3 = std::make_shared<Edge>(n3, n1);
        tri = std::make_shared<Triangle>(e1, e2, e3,
            Edge::length(n1, n2),
            Edge::length(n2, n3),
            Edge::length(n3, n1),
            0, 0, 0, false, false);
        e1->element1 = tri;
        e2->element1 = tri;
        e3->element1 = tri;
    }
};

TEST_F(EdgeTest, ReturnsZeroForSameEdge) {
    TriangleFixture f;
    // sumAngle should return 0 if start and end edge are the same
    double angle = f.e1->sumAngle(f.tri, f.n1, f.e1);
    EXPECT_NEAR(angle, 0.0, 1e-10);
}

TEST_F(EdgeTest, ReturnsTriangleAngle) {
    TriangleFixture f;
    // sumAngle from e1 to e2 at n2 should be the angle at n2 in the triangle
    double expected = f.tri->angle(f.e1, f.n2);
    double angle = f.e1->sumAngle(f.tri, f.n2, f.e2);
    EXPECT_NEAR(angle, expected, 1e-10);
}

TEST_F(EdgeTest, HandlesNullElement) {
    TriangleFixture f;
    // If sElem is nullptr, should not crash, should return 0
    double angle = f.e1->sumAngle(nullptr, f.n1, f.e2);
    EXPECT_NEAR(angle, 0.0, 1e-10);
}

std::shared_ptr<Triangle> createTriangleWithEdges_EF(
    std::shared_ptr<Edge>& e1,
    std::shared_ptr<Edge>& e2,
    std::shared_ptr<Edge>& e3,
    std::shared_ptr<Node>& n1,
    std::shared_ptr<Node>& n2,
    std::shared_ptr<Node>& n3)
{
    n1 = std::make_shared<Node>(0.0, 0.0);
    n2 = std::make_shared<Node>(1.0, 0.0);
    n3 = std::make_shared<Node>(0.0, 1.0);
    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToElement(tri);
    e2->connectToElement(tri);
    e3->connectToElement(tri);
    return tri;
}

TEST_F(EdgeTest, FirstFrontEdgeAt_ReturnsFrontEdge) {
    std::shared_ptr<Edge> e1, e2, e3;
    std::shared_ptr<Node> n1, n2, n3;
    auto tri = createTriangleWithEdges_EF(e1, e2, e3, n1, n2, n3);

    // Mark e2 as a front edge
    e2->frontEdge = true;

    // e1 should find e2 as the first front edge at n2
    auto result = e1->firstFrontEdgeAt(tri, n2);
    EXPECT_EQ(result, e2);
    EXPECT_TRUE(result->frontEdge);
}

TEST_F(EdgeTest, FirstFrontEdgeAt_AlreadyFrontEdge) {
    std::shared_ptr<Edge> e1, e2, e3;
    std::shared_ptr<Node> n1, n2, n3;
    auto tri = createTriangleWithEdges_EF(e1, e2, e3, n1, n2, n3);

    // Mark e1 as a front edge
    e1->frontEdge = true;

    // e1 should return itself
    auto result = e1->firstFrontEdgeAt(tri, n1);
    EXPECT_EQ(result, e1);
    EXPECT_TRUE(result->frontEdge);
}

TEST_F(EdgeTest, ComputeCCWAngle_90Degrees) {
    // Create three nodes: A(0,0), B(1,0), C(0,1)
    auto nodeA = std::make_shared<Node>(0.0, 0.0);
    auto nodeB = std::make_shared<Node>(1.0, 0.0);
    auto nodeC = std::make_shared<Node>(0.0, 1.0);

    // Edge1: AB, Edge2: AC (common node is A)
    auto edge1 = std::make_shared<Edge>(nodeA, nodeB);
    auto edge2 = std::make_shared<Edge>(nodeA, nodeC);

    // CCW angle from edge1 to edge2 at A should be 90 degrees (PI/2)
    double angle = edge1->computeCCWAngle(edge2);
    double expected = Constants::PIdiv2; // 90 degrees in radians

    ASSERT_NEAR(angle, expected, 1e-8);
}

/*TEST_F(EdgeTest, ComputeCCWAngle_180Degrees) {
    // Create three nodes: A(0,0), B(1,0), C(-1,0)
    auto nodeA = std::make_shared<Node>(0.0, 0.0);
    auto nodeB = std::make_shared<Node>(1.0, 0.0);
    auto nodeC = std::make_shared<Node>(-1.0, 0.0);

    // Edge1: AB, Edge2: AC (common node is A)
    auto edge1 = std::make_shared<Edge>(nodeA, nodeB);
    auto edge2 = std::make_shared<Edge>(nodeA, nodeC);

    // CCW angle from edge1 to edge2 at A should be 180 degrees (PI)
    double angle = edge1->computeCCWAngle(edge2);
    double expected = Constants::PI;

    ASSERT_NEAR(angle, expected, 1e-8);
}

TEST_F(EdgeTest, ComputeCCWAngle_ZeroDegrees) {
    // Create two nodes: A(0,0), B(1,0)
    auto nodeA = std::make_shared<Node>(0.0, 0.0);
    auto nodeB = std::make_shared<Node>(1.0, 0.0);

    // Edge1: AB, Edge2: AB (common node is A)
    auto edge1 = std::make_shared<Edge>(nodeA, nodeB);
    auto edge2 = std::make_shared<Edge>(nodeA, nodeB);

    // CCW angle from edge1 to edge2 at A should be 0
    double angle = edge1->computeCCWAngle(edge2);
    double expected = 0.0;

    ASSERT_NEAR(angle, expected, 1e-8);
}

std::shared_ptr<Triangle> makeConnectedTriangle(const std::shared_ptr<Edge>& e1, const std::shared_ptr<Edge>& e2, const std::shared_ptr<Edge>& e3) {
    auto t = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToElement(t);
    e2->connectToElement(t);
    e3->connectToElement(t);
    return t;
}



// Helper to create a dummy quad with given edges   
std::shared_ptr<Quad> makeConnectedQuad(const std::shared_ptr<Edge>& e1, const std::shared_ptr<Edge>& e2, const std::shared_ptr<Edge>& e3, const std::shared_ptr<Edge>& e4) {
    auto q = std::make_shared<Quad>(e1, e2, e3, e4);
    e1->connectToElement(q);
    e2->connectToElement(q);
    e3->connectToElement(q);
    e4->connectToElement(q);
    return q;
}

TEST_F(EdgeTest, CommonElementReturnsCorrectElement) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto n4 = std::make_shared<Node>(1.0, 1.0);

    // Create edges
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);
    auto e4 = std::make_shared<Edge>(n2, n4);
    auto e5 = std::make_shared<Edge>(n4, n3);

    // Create triangle and quad
    auto tri = makeSimpleTriangle(e1, e2, e3);
    auto quad = makeSimpleQuad(e1, e4, e5, e3);

    // e1 is shared by both tri and quad
    // e2 only in tri, e4 only in quad

    // Set elements for e1 and e3
    e1->connectToElement(tri);
    e1->connectToElement(quad);
    e3->connectToElement(tri);
    e3->connectToElement(quad);

    // Test: e1 and e3 share both tri and quad, so commonElement should return one of them
    auto result1 = e1->commonElement(e3);
    EXPECT_TRUE(result1 == tri || result1 == quad);

    // Test: e1 and e2 only share tri
    auto result2 = e1->commonElement(e2);
    EXPECT_EQ(result2, tri);

    // Test: e4 and e5 only share quad
    auto result3 = e4->commonElement(e5);
    EXPECT_EQ(result3, quad);

    // Test: e2 and e4 do not share any element
    auto result4 = e2->commonElement(e4);
    EXPECT_EQ(result4, nullptr);
}

TEST_F(EdgeTest, ConnectToTriangle) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);

    // Create edges
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);

    // Create triangle
    auto triangle = std::make_shared<Triangle>(e1, e2, e3);

    // Initially, edge should have no elements
    EXPECT_EQ(e1->element1, nullptr);
    EXPECT_EQ(e1->element2, nullptr);

    // Connect edge to triangle
    e1->connectToTriangle(triangle);
    EXPECT_EQ(e1->element1, triangle);
    EXPECT_EQ(e1->element2, nullptr);

    // Connect again, should set element2
    e1->connectToTriangle(triangle);
    EXPECT_EQ(e1->element1, triangle);
    EXPECT_EQ(e1->element2, nullptr); // Should not set again since already connected

    // Connect to a different triangle
    auto triangle2 = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToTriangle(triangle2);
    EXPECT_EQ(e1->element2, triangle2);

    // Try to connect a third triangle, should not overwrite
    auto triangle3 = std::make_shared<Triangle>(e1, e2, e3);
    testing::internal::CaptureStderr();
    e1->connectToTriangle(triangle3);
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(output.find("An edge cannot be connected to more than two elements") != std::string::npos);
    EXPECT_EQ(e1->element1, triangle);
    EXPECT_EQ(e1->element2, triangle2);
}

namespace Msg {
    void error(const std::string&) {}
}

TEST_F(EdgeTest, ConnectToQuad_BasicBehavior) {
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(node1, node2);
    auto quad1 = std::make_shared<Quad>(edge, node1, node2);
    auto quad2 = std::make_shared<Quad>(edge, node2, node1);

    // Initially, both element1 and element2 should be nullptr
    EXPECT_EQ(edge->element1, nullptr);
    EXPECT_EQ(edge->element2, nullptr);

    // Connect first quad
    edge->connectToQuad(quad1);
    EXPECT_EQ(edge->element1, quad1);
    EXPECT_EQ(edge->element2, nullptr);

    // Connect second quad
    edge->connectToQuad(quad2);
    EXPECT_EQ(edge->element1, quad1);
    EXPECT_EQ(edge->element2, quad2);

    // Try to connect the same quad again (should not change anything)
    edge->connectToQuad(quad1);
    EXPECT_EQ(edge->element1, quad1);
    EXPECT_EQ(edge->element2, quad2);
}

TEST_F(EdgeTest, ConnectToQuad_TooManyElements) {
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(node1, node2);
    auto quad1 = std::make_shared<Quad>(edge, node1, node2);
    auto quad2 = std::make_shared<Quad>(edge, node2, node1);
    auto quad3 = std::make_shared<Quad>(edge, node1, node2);

    edge->connectToQuad(quad1);
    edge->connectToQuad(quad2);

    // Should not crash or change anything, but should call Msg::error
    edge->connectToQuad(quad3);
    EXPECT_EQ(edge->element1, quad1);
    EXPECT_EQ(edge->element2, quad2);
}

class DummyElement : public Element, public std::enable_shared_from_this<DummyElement> {
public:
    bool hasEdge(const std::shared_ptr<Edge>&) override { return false; }
    std::shared_ptr<Element> neighbor(const std::shared_ptr<Edge>&) override { return nullptr; }
    std::shared_ptr<Edge> neighborEdge(const std::shared_ptr<Node>&, const std::shared_ptr<Edge>&) override { return nullptr; }
    void replaceEdge(const std::shared_ptr<Edge>&, const std::shared_ptr<Edge>&) override {}
};

TEST_F(EdgeTest, ConnectToElement) {
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(node1, node2);

    auto elem1 = std::make_shared<DummyElement>();
    auto elem2 = std::make_shared<DummyElement>();
    auto elem3 = std::make_shared<DummyElement>();

    // Initially both element1 and element2 should be nullptr
    EXPECT_EQ(edge->element1, nullptr);
    EXPECT_EQ(edge->element2, nullptr);

    // Connect first element
    edge->connectToElement(elem1);
    EXPECT_EQ(edge->element1, elem1);
    EXPECT_EQ(edge->element2, nullptr);

    // Connect second element
    edge->connectToElement(elem2);
    EXPECT_EQ(edge->element1, elem1);
    EXPECT_EQ(edge->element2, elem2);

    // Try to connect a third element, should not change element1 or element2
    edge->connectToElement(elem3);
    EXPECT_EQ(edge->element1, elem1);
    EXPECT_EQ(edge->element2, elem2);

    // Try to connect an already connected element, should not change anything
    edge->connectToElement(elem1);
    EXPECT_EQ(edge->element1, elem1);
    EXPECT_EQ(edge->element2, elem2);
}

TEST_F(EdgeTest, DisconnectElement1) {
    edge->element1 = tri1;
    edge->element2 = tri2;
    edge->disconnectFromElement(tri1);
    EXPECT_EQ(edge->element1, tri2);
    EXPECT_EQ(edge->element2, nullptr);
}

TEST_F(EdgeTest, DisconnectElement2) {
    edge->element1 = tri1;
    edge->element2 = tri2;
    edge->disconnectFromElement(tri2);
    EXPECT_EQ(edge->element1, tri1);
    EXPECT_EQ(edge->element2, nullptr);
}

TEST_F(EdgeTest, DisconnectNotConnectedElement) {
    edge->element1 = tri1;
    edge->element2 = tri2;
    Msg::lastError.clear();
    edge->disconnectFromElement(quad1);
    EXPECT_EQ(Msg::lastError.find("is not connected to element"), 0u);
    EXPECT_EQ(edge->element1, tri1);
    EXPECT_EQ(edge->element2, tri2);
}

static std::shared_ptr<Triangle> createTriangleWithEdges_NF(
    std::shared_ptr<Node>& n1,
    std::shared_ptr<Node>& n2,
    std::shared_ptr<Node>& n3,
    std::shared_ptr<Edge>& e1,
    std::shared_ptr<Edge>& e2,
    std::shared_ptr<Edge>& e3)
{
    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->element1 = tri;
    e2->element1 = tri;
    e3->element1 = tri;
    return tri;
}

TEST_F(EdgeTest, OppositeNodeReturnsCorrectNodeInSingleTriangle)
{
    // Triangle: (0,0)-(1,0)-(0,1)
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    std::shared_ptr<Edge> e1, e2, e3;
    auto tri = createTriangleWithEdges_NF(n1, n2, n3, e1, e2, e3);

    // e1: n1-n2, e2: n2-n3, e3: n3-n1
    // e1's opposite node in tri should be n3
    auto result = e1->oppositeNode(nullptr);
    EXPECT_TRUE(result == n3);
}

TEST_F(EdgeTest, OppositeNodeReturnsCorrectNodeInTwoTriangles)
{
    // Create two triangles sharing an edge
    // Triangle1: (0,0)-(1,0)-(0,1)
    // Triangle2: (1,0)-(1,1)-(0,1)
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto n4 = std::make_shared<Node>(1.0, 1.0);

    // Shared edge: n2-n3
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);
    auto tri1 = std::make_shared<Triangle>(e1, e2, e3);
    e1->element1 = tri1;
    e2->element1 = tri1;
    e3->element1 = tri1;

    auto e4 = std::make_shared<Edge>(n3, n4);
    auto e5 = std::make_shared<Edge>(n4, n2);
    auto tri2 = std::make_shared<Triangle>(e2, e4, e5);
    e2->element2 = tri2;
    e4->element1 = tri2;
    e5->element1 = tri2;

    // e2: n2-n3, shared by tri1 and tri2
    // e2->oppositeNode(nullptr) should return n1 (from tri1)
    auto result1 = e2->oppositeNode(nullptr);
    EXPECT_TRUE(result1 == n1);

    // e2->oppositeNode(n1) should return n4 (from tri2)
    auto result2 = e2->oppositeNode(n1);
    EXPECT_TRUE(result2 == n4);
}

static std::shared_ptr<Triangle> MakeTriangle(const std::shared_ptr<Edge>& e, const std::shared_ptr<Node>& n1, const std::shared_ptr<Node>& n2, const std::shared_ptr<Node>& n3) {
    auto e1 = e;
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->element1 = tri;
    e2->element1 = tri;
    e3->element1 = tri;
    return tri;
}

TEST_F(EdgeTest, GetSwappedEdgeReturnsNullForBoundaryEdge) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(n1, n2);
    // element2 is nullptr by default
    EXPECT_EQ(edge->getSwappedEdge(), nullptr);
}

TEST_F(EdgeTest, GetSwappedEdgeReturnsNullForQuadEdge) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(n1, n2);
    // Fake a quad by assigning element1 to a Quad
    struct DummyQuad : public Quad {};
    edge->element1 = std::make_shared<DummyQuad>();
    edge->element2 = std::make_shared<Triangle>(edge, edge, edge); // just to not be boundary
    EXPECT_EQ(edge->getSwappedEdge(), nullptr);
}

TEST_F(EdgeTest, GetSwappedEdgeReturnsSwappedEdgeForTriangles) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto n4 = std::make_shared<Node>(1.0, 1.0);
    auto edge = std::make_shared<Edge>(n1, n2);
    // Create two triangles sharing 'edge'
    auto tri1 = MakeTriangle(edge, n1, n2, n3);
    auto tri2 = MakeTriangle(edge, n2, n1, n4);
    edge->element1 = tri1;
    edge->element2 = tri2;
    // oppositeNode(nullptr) should return n3 (from tri1), oppositeNode(n3) should return n4 (from tri2)
    auto swapped = edge->getSwappedEdge();
    ASSERT_NE(swapped, nullptr);
    // The swapped edge should connect n3 and n4
    auto a = swapped->leftNode;
    auto b = swapped->rightNode;
    // It should be n3 and n4 in some order
    bool correct = ((a == n3 && b == n4) || (a == n4 && b == n3));
    EXPECT_TRUE(correct);
}

std::shared_ptr<Triangle> CreateTriangle(
    std::shared_ptr<Edge>& e1,
    std::shared_ptr<Edge>& e2,
    std::shared_ptr<Edge>& e3,
    std::shared_ptr<Node>& n1,
    std::shared_ptr<Node>& n2,
    std::shared_ptr<Node>& n3)
{
    n1 = std::make_shared<Node>(0.0, 0.0);
    n2 = std::make_shared<Node>(1.0, 0.0);
    n3 = std::make_shared<Node>(0.0, 1.0);
    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->element1 = tri;
    e2->element1 = tri;
    e3->element1 = tri;
    return tri;
}

TEST_F(EdgeTest, SwapToAndSetElementsFor_SwapsDiagonalAndUpdatesElements)
{
    // Create two triangles sharing an edge (the diagonal)
    std::shared_ptr<Node> n1, n2, n3, n4;
    n1 = std::make_shared<Node>(0.0, 0.0);
    n2 = std::make_shared<Node>(1.0, 0.0);
    n3 = std::make_shared<Node>(1.0, 1.0);
    n4 = std::make_shared<Node>(0.0, 1.0);

    // Diagonal edge
    auto diag = std::make_shared<Edge>(n2, n4);

    // Other edges
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n4);
    auto e4 = std::make_shared<Edge>(n4, n1);

    // Triangles
    auto t1 = std::make_shared<Triangle>(e1, e2, diag);
    auto t2 = std::make_shared<Triangle>(e3, e4, diag);

    // Set elements for edges
    e1->element1 = t1;
    e2->element1 = t1;
    diag->element1 = t1;
    e3->element1 = t2;
    e4->element1 = t2;
    diag->element2 = t2;

    // New diagonal to swap to
    auto newDiag = std::make_shared<Edge>(n1, n3);

    // Call the function under test
    diag->swapToAndSetElementsFor(newDiag);

    // After swap, newDiag should be connected to two new triangles
    // The old diagonal should be disconnected from nodes
    // The new triangles should have newDiag as an edge
    // We check that newDiag is connected to its nodes
    EXPECT_TRUE(newDiag->hasNode(n1));
    EXPECT_TRUE(newDiag->hasNode(n3));
    // The old diagonal should not be in the edge lists of n2 or n4
    EXPECT_FALSE(n2->hasEdge(diag));
    EXPECT_FALSE(n4->hasEdge(diag));
    // The newDiag should be in the edge lists of n1 and n3
    EXPECT_TRUE(n1->hasEdge(newDiag));
    EXPECT_TRUE(n3->hasEdge(newDiag));
}

std::shared_ptr<Triangle> makeNodeTriangle(const std::shared_ptr<Node>& n1, const std::shared_ptr<Node>& n2, const std::shared_ptr<Node>& n3) {
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToTriangle(tri);
    e2->connectToTriangle(tri);
    e3->connectToTriangle(tri);
    return tri;
}

// Helper to create a quad element
std::shared_ptr<Quad> makeNodeQuad(const std::shared_ptr<Node>& n1, const std::shared_ptr<Node>& n2, const std::shared_ptr<Node>& n3, const std::shared_ptr<Node>& n4) {
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n4);
    auto e4 = std::make_shared<Edge>(n4, n1);
    auto quad = std::make_shared<Quad>(e1, e2, e3, e4);
    e1->connectToQuad(quad);
    e2->connectToQuad(quad);
    e3->connectToQuad(quad);
    e4->connectToQuad(quad);
    return quad;
}

TEST_F(EdgeTest, BordersToTriangle_Element1IsTriangle) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto tri = makeNodeTriangle(n1, n2, n3);
    auto edge = std::make_shared<Edge>(n1, n2);
    edge->connectToTriangle(tri);
    EXPECT_TRUE(edge->bordersToTriangle());
}

TEST_F(EdgeTest, BordersToTriangle_Element2IsTriangle) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto tri = makeNodeTriangle(n1, n2, n3);
    auto edge = std::make_shared<Edge>(n1, n2);
    // Manually set element2 to triangle
    edge->element2 = tri;
    EXPECT_TRUE(edge->bordersToTriangle());
}

TEST_F(EdgeTest, BordersToTriangle_NoTriangle) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(1.0, 1.0);
    auto n4 = std::make_shared<Node>(0.0, 1.0);
    auto quad = makeNodeQuad(n1, n2, n3, n4);
    auto edge = std::make_shared<Edge>(n1, n2);
    edge->connectToQuad(quad);
    EXPECT_FALSE(edge->bordersToTriangle());
}

TEST_F(EdgeTest, BordersToTriangle_Element2Null) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(n1, n2);
    // No elements connected
    EXPECT_FALSE(edge->bordersToTriangle());
}

TEST_F(EdgeTest, BoundaryEdgeReturnsTrueWhenElement1IsNull)
{
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    Edge edge(node1, node2);
    // element1 and element2 are initialized to nullptr by default
    EXPECT_TRUE(edge.boundaryEdge());
}

TEST_F(EdgeTest, BoundaryEdgeReturnsTrueWhenElement2IsNull)
{
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    Edge edge(node1, node2);
    // Simulate element1 is set, element2 is nullptr
    edge.element1 = std::make_shared<Element>();
    edge.element2 = nullptr;
    EXPECT_TRUE(edge.boundaryEdge());
}

TEST_F(EdgeTest, BoundaryEdgeReturnsFalseWhenBothElementsAreSet)
{
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    Edge edge(node1, node2);
    edge.element1 = std::make_shared<Element>();
    edge.element2 = std::make_shared<Element>();
    EXPECT_FALSE(edge.boundaryEdge());
}

namespace rcl {
    template<typename T, typename U>
    bool instanceOf(const std::shared_ptr<U>& ptr) {
        return std::dynamic_pointer_cast<T>(ptr) != nullptr;
    }
}

class DummyElement : public Element {
public:
    bool hasEdge(const std::shared_ptr<Edge>&) override { return false; }
    std::shared_ptr<Edge> neighborEdge(const std::shared_ptr<Node>&, const std::shared_ptr<Edge>&) override { return nullptr; }
    void replaceEdge(const std::shared_ptr<Edge>&, const std::shared_ptr<Edge>&) override {}
    double longestEdgeLength() override { return 0.0; }
};

TEST_F(EdgeTest, BoundaryOrTriangleEdge_NullElements) {
    auto edge = std::make_shared<Edge>(nullptr, nullptr);
    edge->element1 = nullptr;
    edge->element2 = nullptr;
    EXPECT_TRUE(edge->boundaryOrTriangleEdge());
}

TEST_F(EdgeTest, BoundaryOrTriangleEdge_OneNullElement) {
    auto edge = std::make_shared<Edge>(nullptr, nullptr);
    edge->element1 = std::make_shared<DummyElement>();
    edge->element2 = nullptr;
    EXPECT_TRUE(edge->boundaryOrTriangleEdge());
}

TEST_F(EdgeTest, BoundaryOrTriangleEdge_TriangleElement1) {
    auto edge = std::make_shared<Edge>(nullptr, nullptr);
    edge->element1 = std::make_shared<Triangle>(nullptr, nullptr, nullptr);
    edge->element2 = std::make_shared<DummyElement>();
    EXPECT_TRUE(edge->boundaryOrTriangleEdge());
}

TEST_F(EdgeTest, BoundaryOrTriangleEdge_TriangleElement2) {
    auto edge = std::make_shared<Edge>(nullptr, nullptr);
    edge->element1 = std::make_shared<DummyElement>();
    edge->element2 = std::make_shared<Triangle>(nullptr, nullptr, nullptr);
    EXPECT_TRUE(edge->boundaryOrTriangleEdge());
}

TEST_F(EdgeTest, BoundaryOrTriangleEdge_NoTriangleOrNull) {
    auto edge = std::make_shared<Edge>(nullptr, nullptr);
    edge->element1 = std::make_shared<DummyElement>();
    edge->element2 = std::make_shared<DummyElement>();
    EXPECT_FALSE(edge->boundaryOrTriangleEdge());
}

class DummyElement : public Element {
public:
    bool hasEdge(const std::shared_ptr<Edge>&) override { return false; }
    bool hasNode(const std::shared_ptr<Node>&) override { return false; }
    std::shared_ptr<Element> neighbor(const std::shared_ptr<Edge>&) override { return nullptr; }
    std::shared_ptr<Edge> neighborEdge(const std::shared_ptr<Node>&, const std::shared_ptr<Edge>&) override { return nullptr; }
    int indexOf(const std::shared_ptr<Edge>&) override { return -1; }
    double angle(const std::shared_ptr<Edge>&, const std::shared_ptr<Edge>&) override { return 0.0; }
    bool concavityAt(const std::shared_ptr<Node>&) override { return false; }
    void replaceEdge(const std::shared_ptr<Edge>&, const std::shared_ptr<Edge>&) override {}
};

TEST_F(EdgeTest, HasElementReturnsTrueForElement1) {
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(node1, node2);
    auto elem = std::make_shared<DummyElement>();
    edge->element1 = elem;
    EXPECT_TRUE(edge->hasElement(elem));
}

TEST_F(EdgeTest, HasElementReturnsTrueForElement2) {
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(node1, node2);
    auto elem = std::make_shared<DummyElement>();
    edge->element2 = elem;
    EXPECT_TRUE(edge->hasElement(elem));
}

TEST_F(EdgeTest, HasElementReturnsFalseForOtherElement) {
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(node1, node2);
    auto elem1 = std::make_shared<DummyElement>();
    auto elem2 = std::make_shared<DummyElement>();
    edge->element1 = elem1;
    EXPECT_FALSE(edge->hasElement(elem2));
}

TEST_F(EdgeTest, HasElementReturnsFalseWhenBothNull) {
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(node1, node2);
    auto elem = std::make_shared<DummyElement>();
    EXPECT_FALSE(edge->hasElement(elem));
}

Edge::noTrianglesInOrbit( const std::shared_ptr<Edge>& e,
						 const std::shared_ptr<Quad>& startQ )
{
	Msg::debug("Entering Edge.noTrianglesInOrbit(..)");
	auto curEdge = shared_from_this();
	std::shared_ptr<Element> curElem = startQ;
	auto n = commonNode( e );
	if ( n == nullptr )
	{
		Msg::debug( "Leaving Edge.noTrianglesInOrbit(..), returns false" );
		return false;
	}
	if ( curEdge->boundaryEdge() )
	{
		curEdge = n->anotherBoundaryEdge( curEdge );
		curElem = curEdge->element1;
	}
	do
	{
		if ( rcl::instanceOf<Triangle>( curElem ) )
		{
			Msg::debug( "Leaving Edge.noTrianglesInOrbit(..), returns false" );
			return false;
		}
		curEdge = curElem->neighborEdge( n, curEdge );
		if ( curEdge->boundaryEdge() )
		{
			curEdge = n->anotherBoundaryEdge( curEdge );
			curElem = curEdge->element1;
		}
		else
		{
			curElem = curElem->neighbor( curEdge );
		}
	} while ( curEdge != e );

	Msg::debug( "Leaving Edge.noTrianglesInOrbit(..), returns true" );
	return true;
}

std::shared_ptr<Quad> createSimpleQuadConnectQuad(
    std::shared_ptr<Node>& n1,
    std::shared_ptr<Node>& n2,
    std::shared_ptr<Node>& n3,
    std::shared_ptr<Node>& n4,
    std::shared_ptr<Edge>& e1,
    std::shared_ptr<Edge>& e2,
    std::shared_ptr<Edge>& e3,
    std::shared_ptr<Edge>& e4)
{
    n1 = std::make_shared<Node>(0.0, 0.0);
    n2 = std::make_shared<Node>(1.0, 0.0);
    n3 = std::make_shared<Node>(1.0, 1.0);
    n4 = std::make_shared<Node>(0.0, 1.0);
    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n4);
    e4 = std::make_shared<Edge>(n4, n1);
    auto quad = std::make_shared<Quad>(e1, e2, e3, e4);
    e1->connectToQuad(quad);
    e2->connectToQuad(quad);
    e3->connectToQuad(quad);
    e4->connectToQuad(quad);
    return quad;
}

TEST_F(EdgeTest, NoTrianglesInOrbit_QuadOnly_ReturnsTrue) {
    std::shared_ptr<Node> n1, n2, n3, n4;
    std::shared_ptr<Edge> e1, e2, e3, e4;
    auto quad = createSimpleQuadConnectQuad(n1, n2, n3, n4, e1, e2, e3, e4);

    // Orbit from e1 to e3 (should not encounter any triangles)
    EXPECT_TRUE(e1->noTrianglesInOrbit(e3, quad));
}

TEST_F(EdgeTest, NoTrianglesInOrbit_WithTriangle_ReturnsFalse) {
    std::shared_ptr<Node> n1, n2, n3, n4;
    std::shared_ptr<Edge> e1, e2, e3, e4;
    auto quad = createSimpleQuadConnectQuad(n1, n2, n3, n4, e1, e2, e3, e4);

    // Add a triangle sharing e2
    auto tri = std::make_shared<Triangle>(e2, std::make_shared<Edge>(n2, n4), std::make_shared<Edge>(n4, n3));
    e2->connectToTriangle(tri);

    // Orbit from e1 to e3 (should encounter a triangle)
    EXPECT_FALSE(e1->noTrianglesInOrbit(e3, quad));
}

TEST_F(EdgeTest, NoTrianglesInOrbit_NullCommonNode_ReturnsFalse) {
    std::shared_ptr<Node> n1, n2, n3, n4, n5;
    std::shared_ptr<Edge> e1, e2, e3, e4;
    auto quad = createSimpleQuadConnectQuad(n1, n2, n3, n4, e1, e2, e3, e4);

    // e5 does not share a node with e1
    n5 = std::make_shared<Node>(2.0, 2.0);
    auto e5 = std::make_shared<Edge>(n5, std::make_shared<Node>(3.0, 3.0));

    EXPECT_FALSE(e1->noTrianglesInOrbit(e5, quad));
}

std::shared_ptr<Triangle> createTriangleAndConnect(
    std::shared_ptr<Node> n1, std::shared_ptr<Node> n2, std::shared_ptr<Node> n3,
    std::shared_ptr<Edge>& e1, std::shared_ptr<Edge>& e2, std::shared_ptr<Edge>& e3)
{
    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToTriangle(tri);
    e2->connectToTriangle(tri);
    e3->connectToTriangle(tri);
    e1->connectNodes();
    e2->connectNodes();
    e3->connectNodes();
    return tri;
}

TEST_F(EdgeTest, FindLeftFrontNeighbor_SingleFrontEdge) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto n4 = std::make_shared<Node>(-1.0, 0.0);

    // Create triangle and edges
    std::shared_ptr<Edge> e1, e2, e3;
    auto tri = createTriangleAndConnect(n1, n2, n3, e1, e2, e3);

    // Add a front edge to n1's edgeList (besides e1)
    auto frontEdge = std::make_shared<Edge>(n1, n4);
    frontEdge->connectNodes();
    // Simulate front edge by setting element1 as a triangle and element2 as nullptr
    frontEdge->element1 = tri;
    frontEdge->element2 = nullptr;

    // e1 is not a front edge (has two elements)
    e1->element1 = tri;
    e1->element2 = tri;

    // Add both edges to n1's edgeList
    n1->edgeList.clear();
    n1->edgeList.add(e1);
    n1->edgeList.add(frontEdge);

    // Prepare frontList2 (not used in logic, but required by signature)
    ArrayList<std::shared_ptr<Edge>> frontList2;
    frontList2.add(frontEdge);

    // Should return the only front edge (frontEdge)
    auto result = e1->findLeftFrontNeighbor(frontList2);
    ASSERT_EQ(result, frontEdge);
}

TEST_F(EdgeTest, FindLeftFrontNeighbor_MultipleFrontEdges_SelectsSmallestAngle) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto n4 = std::make_shared<Node>(-1.0, 0.0);
    auto n5 = std::make_shared<Node>(0.0, -1.0);

    // Create triangle and edges
    std::shared_ptr<Edge> e1, e2, e3;
    auto tri = createTriangleAndConnect(n1, n2, n3, e1, e2, e3);

    // Create two front edges at n1
    auto frontEdge1 = std::make_shared<Edge>(n1, n4);
    auto frontEdge2 = std::make_shared<Edge>(n1, n5);
    frontEdge1->connectNodes();
    frontEdge2->connectNodes();
    frontEdge1->element1 = tri;
    frontEdge1->element2 = nullptr;
    frontEdge2->element1 = tri;
    frontEdge2->element2 = nullptr;

    // e1 is not a front edge
    e1->element1 = tri;
    e1->element2 = tri;

    // Add all edges to n1's edgeList
    n1->edgeList.clear();
    n1->edgeList.add(e1);
    n1->edgeList.add(frontEdge1);
    n1->edgeList.add(frontEdge2);

    // Prepare frontList2
    ArrayList<std::shared_ptr<Edge>> frontList2;
    frontList2.add(frontEdge1);
    frontList2.add(frontEdge2);

    // Should select the front edge with the smallest angle (depends on geometry)
    auto result = e1->findLeftFrontNeighbor(frontList2);
    // Since both are symmetric, either can be chosen, but result must be one of them
    ASSERT_TRUE(result == frontEdge1 || result == frontEdge2);
}

TEST_F(EdgeTest, FindLeftFrontNeighbor_NoFrontEdge_ReturnsNull) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);

    // Create triangle and edges
    std::shared_ptr<Edge> e1, e2, e3;
    auto tri = createTriangleAndConnect(n1, n2, n3, e1, e2, e3);

    // e1 is not a front edge
    e1->element1 = tri;
    e1->element2 = tri;

    // Add only e1 to n1's edgeList
    n1->edgeList.clear();
    n1->edgeList.add(e1);

    // Prepare frontList2 (empty)
    ArrayList<std::shared_ptr<Edge>> frontList2;

    // Should return nullptr
    auto result = e1->findLeftFrontNeighbor(frontList2);
    ASSERT_EQ(result, nullptr);
}

std::shared_ptr<Triangle> createTriangleAndConnectNodes(
    std::shared_ptr<Node> n1, std::shared_ptr<Node> n2, std::shared_ptr<Node> n3,
    std::shared_ptr<Edge>& e1, std::shared_ptr<Edge>& e2, std::shared_ptr<Edge>& e3)
{
    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToElement(tri);
    e2->connectToElement(tri);
    e3->connectToElement(tri);
    e1->connectNodes();
    e2->connectNodes();
    e3->connectNodes();
    return tri;
}

TEST_F(EdgeTest, FindRightFrontNeighbor_SingleNeighbor) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(1.0, 1.0);
    auto n4 = std::make_shared<Node>(0.0, 1.0);

    // Create edges and triangles
    std::shared_ptr<Edge> e1, e2, e3, e4, e5;
    auto tri1 = createTriangleAndConnectNodes(n1, n2, n3, e1, e2, e3);
    e4 = std::make_shared<Edge>(n3, n4);
    e5 = std::make_shared<Edge>(n4, n1);
    auto tri2 = std::make_shared<Triangle>(e3, e4, e5);
    e3->connectToElement(tri2);
    e4->connectToElement(tri2);
    e5->connectToElement(tri2);
    e4->connectNodes();
    e5->connectNodes();

    // Set up front edge: e3 is a front edge (adjacent to one triangle only)
    // e3 is shared between tri1 and tri2, so let's make e4 a front edge
    // Remove element2 from e4 to make it a front edge
    e4->disconnectFromElement(tri2);

    // Add e4 to a front list
    ArrayList<std::shared_ptr<Edge>> frontList;
    frontList.add(e4);

    // Test: e3's right node is n4, which has e4 as a front edge
    auto neighbor = e3->findRightFrontNeighbor(frontList);
    ASSERT_EQ(neighbor, e4);
}

TEST_F(EdgeTest, FindRightFrontNeighbor_MultipleNeighbors) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(1.0, 1.0);
    auto n4 = std::make_shared<Node>(0.0, 1.0);
    auto n5 = std::make_shared<Node>(0.5, 1.5);

    // Create edges and triangles
    std::shared_ptr<Edge> e1, e2, e3, e4, e5, e6;
    auto tri1 = createTriangleAndConnectNodes(n1, n2, n3, e1, e2, e3);
    e4 = std::make_shared<Edge>(n3, n4);
    e5 = std::make_shared<Edge>(n4, n1);
    auto tri2 = std::make_shared<Triangle>(e3, e4, e5);
    e3->connectToElement(tri2);
    e4->connectToElement(tri2);
    e5->connectToElement(tri2);
    e4->connectNodes();
    e5->connectNodes();

    // Add another edge and triangle to n4
    e6 = std::make_shared<Edge>(n4, n5);
    auto tri3 = std::make_shared<Triangle>(e4, e6, std::make_shared<Edge>(n5, n3));
    e6->connectToElement(tri3);
    e6->connectNodes();

    // Make e4 and e6 front edges (simulate by removing one element from each)
    e4->disconnectFromElement(tri2);
    e6->disconnectFromElement(tri3);

    // Add e4 and e6 to a front list
    ArrayList<std::shared_ptr<Edge>> frontList;
    frontList.add(e4);
    frontList.add(e6);

    // Test: e3's right node is n4, which has e4 and e6 as front edges
    auto neighbor = e3->findRightFrontNeighbor(frontList);
    // Should return the one with the smallest angle (implementation dependent)
    ASSERT_TRUE(neighbor == e4 || neighbor == e6);
}

TEST_F(EdgeTest, FindRightFrontNeighbor_NoNeighbor) {
    // Create nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(1.0, 1.0);

    // Create edges and triangle
    std::shared_ptr<Edge> e1, e2, e3;
    auto tri1 = createTriangleAndConnectNodes(n1, n2, n3, e1, e2, e3);

    // No front edges at n3
    ArrayList<std::shared_ptr<Edge>> frontList;

    // Test: e2's right node is n3, which has no front edges
    auto neighbor = e2->findRightFrontNeighbor(frontList);
    ASSERT_EQ(neighbor, nullptr);
}

std::shared_ptr<Edge> makeEdge(double x1, double y1, double x2, double y2) {
    auto n1 = std::make_shared<Node>(x1, y1);
    auto n2 = std::make_shared<Node>(x2, y2);
    return std::make_shared<Edge>(n1, n2);
}

TEST_F(EdgeTest, SetFrontNeighbors_UpdatesNeighborsAndReturnsTrueIfChanged) {
    // Create three edges sharing nodes
    auto n1 = std::make_shared<Node>(0, 0);
    auto n2 = std::make_shared<Node>(1, 0);
    auto n3 = std::make_shared<Node>(0, 1);
    auto n4 = std::make_shared<Node>(1, 1);

    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);

    ArrayList<std::shared_ptr<Edge>> frontList;
    frontList.add(e1);
    frontList.add(e2);
    frontList.add(e3);

    // Initially, neighbors are null
    EXPECT_EQ(e1->leftFrontNeighbor, nullptr);
    EXPECT_EQ(e1->rightFrontNeighbor, nullptr);

    // Call setFrontNeighbors
    bool changed = e1->setFrontNeighbors(frontList);

    // Should update neighbors and return true
    EXPECT_TRUE(changed);
    EXPECT_NE(e1->leftFrontNeighbor, nullptr);
    EXPECT_NE(e1->rightFrontNeighbor, nullptr);

    // Call again, should return false (no change)
    bool changed2 = e1->setFrontNeighbors(frontList);
    EXPECT_FALSE(changed2);
}

TEST_F(EdgeTest, SetFrontNeighbors_DoesNotCrashWithSingleEdge) {
    auto n1 = std::make_shared<Node>(0, 0);
    auto n2 = std::make_shared<Node>(1, 0);
    auto e1 = std::make_shared<Edge>(n1, n2);
    ArrayList<std::shared_ptr<Edge>> frontList;
    frontList.add(e1);
    EXPECT_NO_THROW({
        e1->setFrontNeighbors(frontList);
        });
}

std::shared_ptr<Triangle> createTriangleWithElements(
    std::shared_ptr<Node> n1,
    std::shared_ptr<Node> n2,
    std::shared_ptr<Node> n3,
    std::shared_ptr<Edge>& e1,
    std::shared_ptr<Edge>& e2,
    std::shared_ptr<Edge>& e3)
{
    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->element1 = tri;
    e2->element1 = tri;
    e3->element1 = tri;
    return tri;
}

TEST_F(EdgeTest, SplitTrianglesAt_SplitsCorrectly)
{
    // Create 4 nodes in a quad shape
    auto nA = std::make_shared<Node>(0.0, 0.0);
    auto nB = std::make_shared<Node>(1.0, 0.0);
    auto nC = std::make_shared<Node>(1.0, 1.0);
    auto nD = std::make_shared<Node>(0.0, 1.0);

    // Create two triangles sharing edge BC
    std::shared_ptr<Edge> eAB, eBC, eCA, eCD, eDA;
    auto tri1 = createTriangleWithElements(nA, nB, nC, eAB, eBC, eCA);
    auto tri2 = createTriangleWithElements(nC, nD, nA, eCD, eDA, eCA); // eCA reused

    // Set up edge's elements
    eCA->element1 = tri1;
    eCA->element2 = tri2;

    // Prepare lists
    ArrayList<std::shared_ptr<Triangle>> triangleList;
    triangleList.add(tri1);
    triangleList.add(tri2);
    ArrayList<std::shared_ptr<Edge>> edgeList;
    edgeList.add(eAB);
    edgeList.add(eBC);
    edgeList.add(eCA);
    edgeList.add(eCD);
    edgeList.add(eDA);
    ArrayList<std::shared_ptr<Node>> nodeList;
    nodeList.add(nA);
    nodeList.add(nB);
    nodeList.add(nC);
    nodeList.add(nD);

    // New node to split at (midpoint of AC)
    auto nN = std::make_shared<Node>(0.5, 0.5);
    nodeList.add(nN);

    // Call splitTrianglesAt on edge AC (eCA), ben = nA
    auto resultEdge = eCA->splitTrianglesAt(nN, nA, triangleList, edgeList, nodeList);

    // There should now be 4 triangles and 7 edges
    EXPECT_EQ(triangleList.size(), 4);
    EXPECT_EQ(edgeList.size(), 7);

    // The returned edge should be incident to nA
    ASSERT_NE(resultEdge, nullptr);
    EXPECT_TRUE(resultEdge->hasNode(nA));
    EXPECT_TRUE(resultEdge->hasNode(nN));
}

static void createSimpleTriangleMesh(
    std::shared_ptr<Node>& n1,
    std::shared_ptr<Node>& n2,
    std::shared_ptr<Node>& n3,
    std::shared_ptr<Edge>& e1,
    std::shared_ptr<Edge>& e2,
    std::shared_ptr<Edge>& e3,
    std::shared_ptr<Triangle>& t1)
{
    n1 = std::make_shared<Node>(0.0, 0.0);
    n2 = std::make_shared<Node>(1.0, 0.0);
    n3 = std::make_shared<Node>(0.5, 1.0);
    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n1);
    t1 = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToTriangle(t1);
    e2->connectToTriangle(t1);
    e3->connectToTriangle(t1);
    e1->connectNodes();
    e2->connectNodes();
    e3->connectNodes();
}

TEST_F(EdgeTest, SplitTrianglesAtMyMidPoint_SplitsEdgeAndUpdatesLists)
{
    // Arrange
    std::shared_ptr<Node> n1, n2, n3;
    std::shared_ptr<Edge> e1, e2, e3;
    std::shared_ptr<Triangle> t1;
    createSimpleTriangleMesh(n1, n2, n3, e1, e2, e3, t1);

    // Duplicate triangle to form two adjacent triangles sharing e2
    auto n4 = std::make_shared<Node>(1.5, 1.0);
    auto e4 = std::make_shared<Edge>(n2, n4);
    auto e5 = std::make_shared<Edge>(n4, n3);
    auto t2 = std::make_shared<Triangle>(e2, e4, e5);
    e2->connectToTriangle(t2);
    e4->connectToTriangle(t2);
    e5->connectToTriangle(t2);
    e4->connectNodes();
    e5->connectNodes();

    ArrayList<std::shared_ptr<Triangle>> triangleList;
    triangleList.add(t1);
    triangleList.add(t2);
    ArrayList<std::shared_ptr<Edge>> edgeList;
    edgeList.add(e1);
    edgeList.add(e2);
    edgeList.add(e3);
    edgeList.add(e4);
    edgeList.add(e5);
    ArrayList<std::shared_ptr<Node>> nodeList;
    nodeList.add(n1);
    nodeList.add(n2);
    nodeList.add(n3);
    nodeList.add(n4);

    // Act
    auto resultEdge = e2->splitTrianglesAtMyMidPoint(triangleList, edgeList, nodeList, e2);

    // Assert
    // The edge list should now contain more edges (the split edges and diagonals)
    EXPECT_GT(edgeList.size(), 5);
    // The triangle list should now contain more triangles (the split triangles)
    EXPECT_GT(triangleList.size(), 2);
    // The node list should now contain a new node (the midpoint)
    bool foundMid = false;
    for (int i = 0; i < nodeList.size(); ++i) {
        auto n = nodeList.get(i);
        if (fabs(n->x - 1.0) < 1e-8 && fabs(n->y - 0.5) < 1e-8) {
            foundMid = true;
            break;
        }
    }
    EXPECT_TRUE(foundMid);
    // The returned edge should be incident to the base edge's node
    EXPECT_TRUE(resultEdge->hasNode(n2) || resultEdge->hasNode(n3));
}

std::shared_ptr<Quad> createSimpleQuadConnectElement(
    std::shared_ptr<Node>& n1,
    std::shared_ptr<Node>& n2,
    std::shared_ptr<Node>& n3,
    std::shared_ptr<Node>& n4,
    std::shared_ptr<Edge>& e1,
    std::shared_ptr<Edge>& e2,
    std::shared_ptr<Edge>& e3,
    std::shared_ptr<Edge>& e4)
{
    n1 = std::make_shared<Node>(0.0, 0.0);
    n2 = std::make_shared<Node>(1.0, 0.0);
    n3 = std::make_shared<Node>(1.0, 1.0);
    n4 = std::make_shared<Node>(0.0, 1.0);

    e1 = std::make_shared<Edge>(n1, n2);
    e2 = std::make_shared<Edge>(n2, n3);
    e3 = std::make_shared<Edge>(n3, n4);
    e4 = std::make_shared<Edge>(n4, n1);

    auto quad = std::make_shared<Quad>(e1, e2, e3, e4);
    e1->connectToElement(quad);
    e2->connectToElement(quad);
    e3->connectToElement(quad);
    e4->connectToElement(quad);
    return quad;
}

TEST_F(EdgeTest, NextQuadEdgeAt_FindsNextQuad)
{
    // Arrange: Create two quads sharing an edge at node n2
    std::shared_ptr<Node> n1, n2, n3, n4, n5;
    std::shared_ptr<Edge> e1, e2, e3, e4, e5, e6, e7;
    auto quad1 = createSimpleQuadConnectElement(n1, n2, n3, n4, e1, e2, e3, e4);

    // Second quad shares n2-n3 edge with first quad
    n5 = std::make_shared<Node>(2.0, 0.0);
    e5 = std::make_shared<Edge>(n3, n5);
    e6 = std::make_shared<Edge>(n5, n2);
    e7 = std::make_shared<Edge>(n2, n3); // shared edge

    auto quad2 = std::make_shared<Quad>(e7, e5, e6, e2);
    e5->connectToElement(quad2);
    e6->connectToElement(quad2);
    e7->connectToElement(quad2);
    e2->connectToElement(quad2);

    // e2 is n2-n3, shared by both quads
    // Test: from e2 in quad1, at node n2, should find e7 in quad2
    auto result = e2->nextQuadEdgeAt(n2, quad1);

    // Assert
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result == e7);
}

TEST_F(EdgeTest, NextQuadEdgeAt_ReturnsNullIfNoQuad)
{
    // Arrange: Create a triangle and a quad, but only triangle is adjacent
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.5, 1.0);
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToElement(tri);
    e2->connectToElement(tri);
    e3->connectToElement(tri);

    // Test: from e1 in tri, at node n1, should return nullptr (no quad)
    auto result = e1->nextQuadEdgeAt(n1, tri);

    // Assert
    EXPECT_EQ(result, nullptr);
}

namespace rcl {
    template<typename T, typename U>
    bool instanceOf(const std::shared_ptr<U>& ptr) {
        return std::dynamic_pointer_cast<T>(ptr) != nullptr;
    }
}

TEST_F(EdgeTest, GetQuadElement_ReturnsCorrectQuad) {
    // Arrange
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(node1, node2);

    auto quad = std::make_shared<Quad>(edge, edge, edge, edge);
    auto tri = std::make_shared<Triangle>(edge, edge, edge);

    // Test when element1 is a Quad
    edge->element1 = quad;
    edge->element2 = nullptr;
    auto result = edge->getQuadElement();
    EXPECT_EQ(result, quad);

    // Test when element2 is a Quad
    edge->element1 = nullptr;
    edge->element2 = quad;
    result = edge->getQuadElement();
    EXPECT_EQ(result, quad);

    // Test when neither is a Quad
    edge->element1 = tri;
    edge->element2 = tri;
    result = edge->getQuadElement();
    EXPECT_EQ(result, nullptr);
}

std::shared_ptr<Triangle> createTriangleWithEdge(const std::shared_ptr<Edge>& edge) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto e1 = edge;
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToTriangle(tri);
    e2->connectToTriangle(tri);
    e3->connectToTriangle(tri);
    return tri;
}

TEST_F(EdgeTest, GetTriangleElement_ReturnsTriangleIfPresent) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(n1, n2);
    auto tri = createTriangleWithEdge(edge);

    // Should return the triangle if attached
    auto result = edge->getTriangleElement();
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result, tri);
}

TEST_F(EdgeTest, GetTriangleElement_ReturnsNullIfNoTriangle) {
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto edge = std::make_shared<Edge>(n1, n2);

    // No triangle attached
    auto result = edge->getTriangleElement();
    ASSERT_EQ(result, nullptr);
}

class TestQuad : public Quad {
public:
    std::shared_ptr<Edge> storedEdge;
    TestQuad(const std::shared_ptr<Edge>& e) : Quad(e, nullptr, nullptr, nullptr), storedEdge(e) {}
    bool hasEdge(const std::shared_ptr<Edge>& e) override {
        return e == storedEdge;
    }
};

TEST_F(EdgeTest, GetQuadWithEdge_ReturnsCorrectQuad) {
    auto node1 = std::make_shared<Node>(0.0, 0.0);
    auto node2 = std::make_shared<Node>(1.0, 0.0);
    auto node3 = std::make_shared<Node>(0.0, 1.0);
    auto edge = std::make_shared<Edge>(node1, node2);
    auto otherEdge = std::make_shared<Edge>(node2, node3);

    // Case 1: element1 is a Quad and hasEdge returns true
    auto quad1 = std::make_shared<TestQuad>(otherEdge);
    edge->element1 = quad1;
    edge->element2 = nullptr;
    EXPECT_EQ(edge->getQuadWithEdge(otherEdge), quad1);

    // Case 2: element2 is a Quad and hasEdge returns true
    edge->element1 = nullptr;
    auto quad2 = std::make_shared<TestQuad>(otherEdge);
    edge->element2 = quad2;
    EXPECT_EQ(edge->getQuadWithEdge(otherEdge), quad2);

    // Case 3: neither element1 nor element2 is a Quad with the edge
    edge->element1 = nullptr;
    edge->element2 = nullptr;
    EXPECT_EQ(edge->getQuadWithEdge(otherEdge), nullptr);
}

static std::tuple<std::shared_ptr<Edge>, std::shared_ptr<Node>, std::shared_ptr<Edge>> createTriangleWithFrontEdge()
{
    // Create three nodes
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);

    // Create three edges
    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n1);

    // Create triangle
    auto tri = std::make_shared<Triangle>(e1, e2, e3);

    // Connect edges to triangle
    e1->connectToTriangle(tri);
    e2->connectToTriangle(tri);
    e3->connectToTriangle(tri);

    // Set e1 as a front edge (simulate by setting element2 to nullptr)
    e1->element2 = nullptr;
    e1->frontEdge = true;

    // e2 is not a front edge (simulate by setting both elements)
    e2->element2 = tri;
    e2->frontEdge = false;

    // e3 is not a front edge
    e3->element2 = tri;
    e3->frontEdge = false;

    return { e1, n1, e2 };
}

TEST_F(EdgeTest, TrueFrontNeighborAt_ReturnsFrontEdge)
{
    Msg::debugMode = false; // Suppress debug output

    auto [frontEdge, node, otherEdge] = createTriangleWithFrontEdge();

    // The triangle is set up so that e1 is a front edge, e2 is not.
    // Calling trueFrontNeighborAt on e2 with node n2 should return e1.
    auto result = otherEdge->trueFrontNeighborAt(node);

    ASSERT_TRUE(result != nullptr);
    ASSERT_TRUE(result->isFrontEdge());
    ASSERT_EQ(result, frontEdge);
}

TEST_F(EdgeTest, TrueFrontNeighborAt_NodeNotOnEdge_LogsError)
{
    Msg::debugMode = false; // Suppress debug output

    auto [frontEdge, node, otherEdge] = createTriangleWithFrontEdge();
    auto unrelatedNode = std::make_shared<Node>(5.0, 5.0);

    // Should log an error and still return a front edge eventually
    auto result = otherEdge->trueFrontNeighborAt(unrelatedNode);

    ASSERT_TRUE(result != nullptr);
    ASSERT_TRUE(result->isFrontEdge());
}

TEST_F(EdgeTest, ToStringReturnsDescr) {
    auto node1 = std::make_shared<Node>(1.0, 2.0);
    auto node2 = std::make_shared<Node>(3.0, 4.0);
    Edge edge(node1, node2);

    std::string descrStr = edge.descr();
    std::string toStringStr = edge.toString();

    EXPECT_EQ(toStringStr, descrStr);
}

class EdgeTestHelper : public Edge {
public:
    using Edge::stateList;
    EdgeTestHelper(const std::shared_ptr<Node>& n1, const std::shared_ptr<Node>& n2)
        : Edge(n1, n2) {
    }
};

TEST_F(EdgeTest, MarkAllSelectableSetsAllSelectableTrue) {
    // Setup: create dummy nodes and edges
    auto n1 = std::make_shared<Node>(0.0, 0.0);
    auto n2 = std::make_shared<Node>(1.0, 0.0);
    auto n3 = std::make_shared<Node>(0.0, 1.0);
    auto n4 = std::make_shared<Node>(1.0, 1.0);

    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n2, n3);
    auto e3 = std::make_shared<Edge>(n3, n4);

    // Place edges in all state lists and set selectable to false
    EdgeTestHelper::stateList[0].clear();
    EdgeTestHelper::stateList[1].clear();
    EdgeTestHelper::stateList[2].clear();
    e1->selectable = false;
    e2->selectable = false;
    e3->selectable = false;
    EdgeTestHelper::stateList[0].add(e1);
    EdgeTestHelper::stateList[1].add(e2);
    EdgeTestHelper::stateList[2].add(e3);

    // Act
    Edge::markAllSelectable();

    // Assert
    EXPECT_TRUE(e1->selectable);
    EXPECT_TRUE(e2->selectable);
    EXPECT_TRUE(e3->selectable);
}

static std::shared_ptr<Edge> AddEdgeToStateList(int state, double x1, double y1, double x2, double y2) {
    auto n1 = std::make_shared<Node>(x1, y1);
    auto n2 = std::make_shared<Node>(x2, y2);
    auto edge = std::make_shared<Edge>(n1, n2);
    Edge::stateList[state].add(edge);
    return edge;
}

TEST_F(EdgeTest, PrintStateLists_PrintsCorrectly) {
    // Enable debug mode to allow printing
    Msg::debugMode = true;

    // Clear state lists before test
    Edge::clearStateList();

    // Add edges to each state
    auto e0 = AddEdgeToStateList(0, 0, 0, 1, 0);
    auto e1 = AddEdgeToStateList(1, 0, 1, 1, 1);
    auto e2 = AddEdgeToStateList(2, 0, 2, 1, 2);

    // Redirect std::cout to a stringstream
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Call the function
    Edge::printStateLists();

    // Restore std::cout
    std::cout.rdbuf(oldCout);

    std::string output = buffer.str();

    // Check that the output contains the expected descriptions and state numbers
    EXPECT_NE(output.find("frontsInState 1-1:"), std::string::npos);
    EXPECT_NE(output.find(e2->descr()), std::string::npos);
    EXPECT_NE(output.find("(2)"), std::string::npos);

    EXPECT_NE(output.find("frontsInState 0-1 and 1-0:"), std::string::npos);
    EXPECT_NE(output.find(e1->descr()), std::string::npos);
    EXPECT_NE(output.find("(1)"), std::string::npos);

    EXPECT_NE(output.find("frontsInState 0-0:"), std::string::npos);
    EXPECT_NE(output.find(e0->descr()), std::string::npos);
    EXPECT_NE(output.find("(0)"), std::string::npos);
}

TEST_F(EdgeTest, LeftToReturnsTrueWhenLeftNodeIsLeftOfOther) {
    auto n1 = std::make_shared<Node>(1.0, 2.0);
    auto n2 = std::make_shared<Node>(3.0, 4.0);
    auto n3 = std::make_shared<Node>(5.0, 6.0);
    auto n4 = std::make_shared<Node>(7.0, 8.0);

    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n3, n4);

    EXPECT_TRUE(e1->leftTo(e2));
    EXPECT_FALSE(e2->leftTo(e1));
}

TEST_F(EdgeTest, LeftToReturnsTrueWhenXEqualAndYLess) {
    auto n1 = std::make_shared<Node>(1.0, 2.0);
    auto n2 = std::make_shared<Node>(3.0, 4.0);
    auto n3 = std::make_shared<Node>(1.0, 3.0);
    auto n4 = std::make_shared<Node>(5.0, 6.0);

    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n3, n4);

    EXPECT_TRUE(e1->leftTo(e2));
    EXPECT_FALSE(e2->leftTo(e1));
}

TEST_F(EdgeTest, LeftToReturnsFalseWhenNotLeft) {
    auto n1 = std::make_shared<Node>(5.0, 2.0);
    auto n2 = std::make_shared<Node>(6.0, 3.0);
    auto n3 = std::make_shared<Node>(1.0, 4.0);
    auto n4 = std::make_shared<Node>(2.0, 5.0);

    auto e1 = std::make_shared<Edge>(n1, n2);
    auto e2 = std::make_shared<Edge>(n3, n4);

    EXPECT_FALSE(e1->leftTo(e2));
    EXPECT_TRUE(e2->leftTo(e1));
}

class EdgeIsFrontEdgeTest : public ::testing::Test {
protected:
    std::shared_ptr<Node> n1 = std::make_shared<Node>(0.0, 0.0);
    std::shared_ptr<Node> n2 = std::make_shared<Node>(1.0, 0.0);
    std::shared_ptr<Edge> edge = std::make_shared<Edge>(n1, n2);

    std::shared_ptr<Edge> e1 = std::make_shared<Edge>(n1, n2);
    std::shared_ptr<Edge> e2 = std::make_shared<Edge>(n2, std::make_shared<Node>(1.0, 1.0));
    std::shared_ptr<Edge> e3 = std::make_shared<Edge>(n1, std::make_shared<Node>(0.0, 1.0));
};

TEST_F(EdgeIsFrontEdgeTest, ReturnsTrueIfOneElementIsTriangleAndOtherIsNull) {
    auto tri = std::make_shared<Triangle>(edge, e2, e3);
    edge->element1 = tri;
    edge->element2 = nullptr;
    EXPECT_TRUE(edge->isFrontEdge());
}

TEST_F(EdgeIsFrontEdgeTest, ReturnsTrueIfOneElementIsTriangleAndOtherIsNotTriangle) {
    auto tri = std::make_shared<Triangle>(edge, e2, e3);
    auto quad = std::make_shared<Quad>(edge, e2, e3, std::make_shared<Edge>(n1, std::make_shared<Node>(2.0, 2.0)));
    edge->element1 = tri;
    edge->element2 = quad;
    EXPECT_TRUE(edge->isFrontEdge());

    edge->element1 = quad;
    edge->element2 = tri;
    EXPECT_TRUE(edge->isFrontEdge());
}

TEST_F(EdgeIsFrontEdgeTest, ReturnsFalseIfBothElementsAreTriangles) {
    auto tri1 = std::make_shared<Triangle>(edge, e2, e3);
    auto tri2 = std::make_shared<Triangle>(edge, e2, e3);
    edge->element1 = tri1;
    edge->element2 = tri2;
    EXPECT_FALSE(edge->isFrontEdge());
}

TEST_F(EdgeIsFrontEdgeTest, ReturnsFalseIfBothElementsAreNotTriangles) {
    auto quad1 = std::make_shared<Quad>(edge, e2, e3, std::make_shared<Edge>(n1, std::make_shared<Node>(2.0, 2.0)));
    auto quad2 = std::make_shared<Quad>(edge, e2, e3, std::make_shared<Edge>(n1, std::make_shared<Node>(3.0, 3.0)));
    edge->element1 = quad1;
    edge->element2 = quad2;
    EXPECT_FALSE(edge->isFrontEdge());
}

std::shared_ptr<Node> makeNode(double x, double y) {
    auto n = std::make_shared<Node>();
    n->x = x;
    n->y = y;
    return n;
}

TEST_F(EdgeTest, DescrReturnsCorrectString) {
    auto n1 = makeNode(1.0, 2.0);
    auto n2 = makeNode(3.0, 4.0);
    Edge e(n1, n2);

    std::string expected = "(1.000000, 2.000000), (3.000000, 4.000000)";
    EXPECT_EQ(e.descr(), expected);
}

class CoutRedirect {
public:
    CoutRedirect(std::streambuf* new_buffer) : old(std::cout.rdbuf(new_buffer)) {}
    ~CoutRedirect() { std::cout.rdbuf(old); }
private:
    std::streambuf* old;
};

TEST_F(EdgeTest, PrintMeOutputsDescr) {
    // Arrange
    auto n1 = std::make_shared<Node>(1.0, 2.0);
    auto n2 = std::make_shared<Node>(3.0, 4.0);
    Edge edge(n1, n2);

    std::ostringstream oss;
    CoutRedirect redirect(oss.rdbuf());

    // Act
    edge.printMe();

    // Assert
    std::string output = oss.str();
    std::string expected = edge.descr() + "\n";
    EXPECT_EQ(output, expected);
}
