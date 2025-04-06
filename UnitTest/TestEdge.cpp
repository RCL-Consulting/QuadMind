#include "pch.h"

#include "Edge.h"
#include "Node.h"
#include "MyVector.h"

class EdgeTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
		node1 = std::make_shared<Node>( 0.0, 0.0 );
		node2 = std::make_shared<Node>( 1.0, 0.0 );
        edge = std::make_shared<Edge>( node1, node2 );
    }

	void TearDown() override
	{
        Edge::clearStateList();
	}

    std::shared_ptr<Node> node1;
    std::shared_ptr<Node> node2;
    std::shared_ptr<Edge> edge;
};

TEST_F( EdgeTest, EqualsSameEdge )
{
    auto node1 = std::make_shared<Node>( 1.0, 1.0 );
    auto node2 = std::make_shared<Node>( 2.0, 2.0 );
    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node1, node2 );

    EXPECT_TRUE( edge1->equals( edge2 ) );
}

TEST_F( EdgeTest, EqualsDifferentEdges )
{
    auto node1 = std::make_shared<Node>( 1.0, 1.0 );
    auto node2 = std::make_shared<Node>( 2.0, 2.0 );
    auto node3 = std::make_shared<Node>( 3.0, 3.0 );
    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    EXPECT_FALSE( edge1->equals( edge2 ) );
}

TEST_F( EdgeTest, EqualsNull )
{
    auto node1 = std::make_shared<Node>( 1.0, 1.0 );
    auto node2 = std::make_shared<Node>( 2.0, 2.0 );
    auto edge = std::make_shared<Edge>( node1, node2 );

    EXPECT_FALSE( edge->equals( nullptr ) );
}

TEST_F( EdgeTest, EqualsDifferentType )
{
    auto node1 = std::make_shared<Node>( 1.0, 1.0 );
    auto node2 = std::make_shared<Node>( 2.0, 2.0 );
    auto edge = std::make_shared<Edge>( node1, node2 );

    EXPECT_FALSE( edge->equals( node1 ) );
}

TEST_F( EdgeTest, CopyConstructor )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge originalEdge( node1, node2 );

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
    auto node3 = std::make_shared<Node>( 2.0, 2.0 );
    
    ASSERT_TRUE( edge->replaceNode( node1, node3 ) );
    ASSERT_EQ( edge->leftNode, node3 );
    ASSERT_EQ( edge->rightNode, node2 );
}

TEST_F( EdgeTest, ReplaceNode_RightNode )
{
    auto node3 = std::make_shared<Node>( 2.0, 2.0 );

    ASSERT_TRUE( edge->replaceNode( node2, node3 ) );
    ASSERT_EQ( edge->leftNode, node1 );
    ASSERT_EQ( edge->rightNode, node3 );
}

TEST_F( EdgeTest, ReplaceNode_NodeNotFound )
{
    auto node3 = std::make_shared<Node>( 2.0, 2.0 );
    auto node4 = std::make_shared<Node>( 3.0, 3.0 );
 
    ASSERT_FALSE( edge->replaceNode( node3, node4 ) );
    ASSERT_EQ( edge->leftNode, node1 );
    ASSERT_EQ( edge->rightNode, node2 );
}

TEST_F( EdgeTest, MidPointTest )
{
    auto midPoint = edge->midPoint();

    ASSERT_NE( midPoint, nullptr );
    EXPECT_DOUBLE_EQ( midPoint->x, 0.5 );
    EXPECT_DOUBLE_EQ( midPoint->y, 0.0 );
}

TEST( EdgeTests, MidPointTestNegativeCoordinates )
{
    auto node3 = std::make_shared<Node>( -2.0, -2.0 );
    auto node4 = std::make_shared<Node>( 2.0, 2.0 );
    auto edge = std::make_shared<Edge>( node3, node4 );

    auto midPoint = edge->midPoint();

    ASSERT_NE( midPoint, nullptr );
    EXPECT_DOUBLE_EQ( midPoint->x, 0.0 );
    EXPECT_DOUBLE_EQ( midPoint->y, 0.0 );
}

TEST( EdgeTests, MidPointTestSameCoordinates )
{
    auto node3 = std::make_shared<Node>( 1.0, 1.0 );
    auto node4 = std::make_shared<Node>( 1.0, 1.0 );
    auto edge = std::make_shared<Edge>( node3, node4 );

    auto midPoint = edge->midPoint();

    ASSERT_NE( midPoint, nullptr );
    EXPECT_DOUBLE_EQ( midPoint->x, 1.0 );
    EXPECT_DOUBLE_EQ( midPoint->y, 1.0 );
}

TEST( EdgeTests, LengthCalculation )
{
    double x1 = 0.0, y1 = 0.0;
    double x2 = 3.0, y2 = 4.0;
    double expectedLength = 5.0;

    double calculatedLength = Edge::length( x1, y1, x2, y2 );

    EXPECT_NEAR( calculatedLength, expectedLength, 1e-12 );
}

TEST( EdgeTests, LengthCalculationNegativeCoordinates )
{
    double x1 = -1.0, y1 = -1.0;
    double x2 = -4.0, y2 = -5.0;
    double expectedLength = 5.0;

    double calculatedLength = Edge::length( x1, y1, x2, y2 );

    EXPECT_NEAR( calculatedLength, expectedLength, 1e-12 );
}

TEST( EdgeTests, LengthCalculationZeroLength )
{
    double x1 = 1.0, y1 = 1.0;
    double x2 = 1.0, y2 = 1.0;
    double expectedLength = 0.0;

    double calculatedLength = Edge::length( x1, y1, x2, y2 );

    EXPECT_NEAR( calculatedLength, expectedLength, 1e-12 );
}

TEST( EdgeTests, DescrTest )
{
    auto node1 = std::make_shared<Node>( 1.0, 2.0 );
    auto node2 = std::make_shared<Node>( 3.0, 4.0 );
    Edge edge( node1, node2 );

    std::string expected = "(1.000000, 2.000000), (3.000000, 4.000000)";
    EXPECT_EQ( edge.descr(), expected );
}

TEST( EdgeTests, DescrTestReversedNodes )
{
    auto node1 = std::make_shared<Node>( 3.0, 4.0 );
    auto node2 = std::make_shared<Node>( 1.0, 2.0 );
    Edge edge( node1, node2 );

    std::string expected = "(1.000000, 2.000000), (3.000000, 4.000000)";
    EXPECT_EQ( edge.descr(), expected );
}

TEST( EdgeTests, ComputePosAngle_SameEdge )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto edge = std::make_shared<Edge>( node1, node2 );

    double angle = edge->computePosAngle( edge, node1 );
    EXPECT_DOUBLE_EQ( angle, 2 * Constants::PI );
}

TEST( EdgeTests, ComputePosAngle_ConnectedEdges )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 1.0, 1.0 );
    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    double angle = edge1->computePosAngle( edge2, node2 );
    EXPECT_DOUBLE_EQ( angle, Constants::PIdiv2 );
}

TEST( EdgeTests, ComputePosAngle_NotConnectedEdges )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 2.0, 0.0 );
    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    double angle = edge1->computePosAngle( edge2, node1 );
    EXPECT_DOUBLE_EQ( angle, 0.0 );
}

TEST( EdgeTests, ComputePosAngle_ObtuseAngle )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 0.0, 1.0 );
    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    double angle = edge1->computePosAngle( edge2, node2 );
    EXPECT_NEAR( angle, Constants::PIdiv4, 1e-9 );
}

TEST( EdgeTests, ConnectNodes )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    auto edge = std::make_shared<Edge>( node1, node2 );

    edge->connectNodes();

    // Check if the edge is added to the edge lists of both nodes
    ASSERT_EQ( node1->edgeList.size(), 1 );
    ASSERT_EQ( node2->edgeList.size(), 1 );
    ASSERT_EQ( node1->edgeList.get(0), edge );
    ASSERT_EQ( node2->edgeList.get(0), edge );
}

TEST( EdgeTests, DisconnectNodes )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    auto edge = std::make_shared<Edge>( node1, node2 );

    // Connect nodes
    edge->connectNodes();
    EXPECT_EQ( node1->edgeList.size(), 1 );
    EXPECT_EQ( node2->edgeList.size(), 1 );

    // Disconnect nodes
    edge->disconnectNodes();
    EXPECT_EQ( node1->edgeList.size(), 0 );
    EXPECT_EQ( node2->edgeList.size(), 0 );
}

TEST( EdgeTests, TryDisconnectNodes )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    auto edge = std::make_shared<Edge>( node1, node2 );

    // Connect nodes
    edge->connectNodes();
    EXPECT_EQ( node1->edgeList.size(), 1 );
    EXPECT_EQ( node2->edgeList.size(), 1 );

    // Disconnect nodes
    edge->tryToDisconnectNodes();
    EXPECT_EQ( node1->edgeList.size(), 0 );
    EXPECT_EQ( node2->edgeList.size(), 0 );
}

TEST( EdgeTests, TryToDisconnectNodes_NoEffectIfEdgeNotInNodeEdgeLists )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    auto edge = std::make_shared<Edge>( node1, node2 );

    // Connect nodes
    edge->connectNodes();
    edge->tryToDisconnectNodes(); // First removal
    edge->tryToDisconnectNodes(); // Second removal, should have no effect

    EXPECT_EQ( node1->edgeList.size(), 0 );
    EXPECT_EQ( node2->edgeList.size(), 0 );
}

TEST( EdgeTests, UnitNormalAt_LeftNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    Edge edge( node1, node2 );

    auto normalEdge = edge.unitNormalAt( node1 );

    EXPECT_NEAR( normalEdge->leftNode->x, 0.0, 1e-9 );
    EXPECT_NEAR( normalEdge->leftNode->y, 1.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->x, 0.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->y, 0.0, 1e-9 );
}

TEST( EdgeTests, UnitNormalAt_RightNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    Edge edge( node1, node2 );

    auto normalEdge = edge.unitNormalAt( node2 );

    EXPECT_NEAR( normalEdge->leftNode->x, 1.0, 1e-9 );
    EXPECT_NEAR( normalEdge->leftNode->y, 1.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->x, 1.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->y, 0.0, 1e-9 );
}

TEST( EdgeTests, UnitNormalAt_DiagonalEdge )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );

    auto normalEdge = edge.unitNormalAt( node1 );

    EXPECT_NEAR( normalEdge->leftNode->x, -std::sqrt( 2 ) / 2, 1e-9 );
    EXPECT_NEAR( normalEdge->leftNode->y, std::sqrt( 2 ) / 2, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->x, 0.0, 1e-9 );
    EXPECT_NEAR( normalEdge->rightNode->y, 0.0, 1e-9 );
}

TEST( EdgeTests, GetVectorFromLeftNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );

    auto vector = edge.getVector( node1 );

    EXPECT_EQ( vector.origin->x, node1->x );
    EXPECT_EQ( vector.origin->y, node1->y );
    EXPECT_EQ( vector.x, node2->x );
    EXPECT_EQ( vector.y, node2->y );
}

TEST( EdgeTests, GetVectorFromRightNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );

    auto vector = edge.getVector( node2 );

    EXPECT_EQ( vector.origin->x, node2->x );
    EXPECT_EQ( vector.origin->y, node2->y );
    EXPECT_EQ( vector.x, -1.0 );
    EXPECT_EQ( vector.y, -1.0 );
}

TEST( EdgeTests, GetVectorFromInvalidNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    auto invalidNode = std::make_shared<Node>( 2.0, 2.0 );
    Edge edge( node1, node2 );

    EXPECT_THROW( edge.getVector( invalidNode ), std::runtime_error );
}

TEST( EdgeTests, HasNode_LeftNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );

    ASSERT_TRUE( edge.hasNode( node1 ) );
}

TEST( EdgeTests, HasNode_RightNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );

    ASSERT_TRUE( edge.hasNode( node2 ) );
}

TEST( EdgeTests, HasNode_NotPresent )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    auto node3 = std::make_shared<Node>( 2.0, 2.0 );
    Edge edge( node1, node2 );

    ASSERT_FALSE( edge.hasNode( node3 ) );
}

TEST( EdgeTests, HasFalseFrontNeighbor_NoNeighbors )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );

    EXPECT_TRUE( edge.hasFalseFrontNeighbor() );
}

TEST( EdgeTests, HasFalseFrontNeighbor_LeftNeighborNull )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );
    edge.rightFrontNeighbor = std::make_shared<Edge>( node1, node2 );
    edge.rightFrontNeighbor->frontEdge = true;

    EXPECT_TRUE( edge.hasFalseFrontNeighbor() );
}

TEST( EdgeTests, HasFalseFrontNeighbor_RightNeighborNull )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );
    edge.leftFrontNeighbor = std::make_shared<Edge>( node1, node2 );
    edge.leftFrontNeighbor->frontEdge = true;

    EXPECT_TRUE( edge.hasFalseFrontNeighbor() );
}

TEST( EdgeTests, HasFalseFrontNeighbor_LeftNeighborNotFrontEdge )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );
    edge.leftFrontNeighbor = std::make_shared<Edge>( node1, node2 );
    edge.leftFrontNeighbor->frontEdge = false;
    edge.rightFrontNeighbor = std::make_shared<Edge>( node1, node2 );
    edge.rightFrontNeighbor->frontEdge = true;

    EXPECT_TRUE( edge.hasFalseFrontNeighbor() );
}

TEST( EdgeTests, HasFalseFrontNeighbor_RightNeighborNotFrontEdge )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );
    edge.leftFrontNeighbor = std::make_shared<Edge>( node1, node2 );
    edge.leftFrontNeighbor->frontEdge = true;
    edge.rightFrontNeighbor = std::make_shared<Edge>( node1, node2 );
    edge.rightFrontNeighbor->frontEdge = false;

    EXPECT_TRUE( edge.hasFalseFrontNeighbor() );
}

TEST( EdgeTests, HasFalseFrontNeighbor_BothNeighborsFrontEdge )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );
    edge.leftFrontNeighbor = std::make_shared<Edge>( node1, node2 );
    edge.leftFrontNeighbor->frontEdge = true;
    edge.rightFrontNeighbor = std::make_shared<Edge>( node1, node2 );
    edge.rightFrontNeighbor->frontEdge = true;

    EXPECT_FALSE( edge.hasFalseFrontNeighbor() );
}

TEST( EdgeTests, HasFrontNeighbor_LeftFrontNeighbor )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 0.5, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node1, node3 );

    edge1->leftFrontNeighbor = edge2;

    ASSERT_TRUE( edge1->hasFrontNeighbor( edge2 ) );
}

TEST( EdgeTests, HasFrontNeighbor_RightFrontNeighbor )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 0.5, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    edge1->rightFrontNeighbor = edge2;

    ASSERT_TRUE( edge1->hasFrontNeighbor( edge2 ) );
}

TEST( EdgeTests, HasFrontNeighbor_NoFrontNeighbor )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 0.5, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    ASSERT_FALSE( edge1->hasFrontNeighbor( edge2 ) );
}

TEST( EdgeTests, OtherNode_LeftNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );

    auto result = edge.otherNode( node1 );
    ASSERT_EQ( result, node2 );
}

TEST( EdgeTests, OtherNode_RightNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );

    auto result = edge.otherNode( node2 );
    ASSERT_EQ( result, node1 );
}

TEST( EdgeTests, OtherNode_NodeNotOnEdge )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    auto node3 = std::make_shared<Node>( 2.0, 2.0 );
    Edge edge( node1, node2 );

    auto result = edge.otherNode( node3 );
    ASSERT_EQ( result, nullptr );
}

TEST( EdgeTests, OtherNodeGivenNewLength )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 3.0, 4.0 );
    Edge edge( node1, node2 );

    double newLength = 5.0;
    auto newNode = edge.otherNodeGivenNewLength( newLength, node1 );

    ASSERT_NEAR( newNode->x, 3.0, 1e-9 );
    ASSERT_NEAR( newNode->y, 4.0, 1e-9 );
}

TEST( EdgeTests, OtherNodeGivenNewLengthWithDifferentNode )
{
    auto node1 = std::make_shared<Node>( 1.0, 1.0 );
    auto node2 = std::make_shared<Node>( 4.0, 5.0 );
    Edge edge( node1, node2 );

    double newLength = 5.0;
    auto newNode = edge.otherNodeGivenNewLength( newLength, node1 );

    ASSERT_NEAR( newNode->x, 4.0, 1e-9 );
    ASSERT_NEAR( newNode->y, 5.0, 1e-9 );
}

TEST( EdgeTests, OtherNodeGivenNewLengthWithNegativeCoordinates )
{
    auto node1 = std::make_shared<Node>( -1.0, -1.0 );
    auto node2 = std::make_shared<Node>( -4.0, -5.0 );
    Edge edge( node1, node2 );

    double newLength = 5.0;
    auto newNode = edge.otherNodeGivenNewLength( newLength, node1 );

    ASSERT_NEAR( newNode->x, -4.0, 1e-9 );
    ASSERT_NEAR( newNode->y, -5.0, 1e-9 );
}

TEST( EdgeTests, UpperNode_LeftNodeHigher )
{
    auto node1 = std::make_shared<Node>( 0.0, 1.0 );
    auto node2 = std::make_shared<Node>( 0.0, 0.0 );
    Edge edge( node1, node2 );
    EXPECT_EQ( edge.upperNode(), node1 );
}

TEST( EdgeTests, UpperNode_RightNodeHigher )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 0.0, 1.0 );
    Edge edge( node1, node2 );
    EXPECT_EQ( edge.upperNode(), node2 );
}

TEST( EdgeTests, UpperNode_NodesAtSameHeight )
{
    auto node1 = std::make_shared<Node>( 0.0, 1.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    Edge edge( node1, node2 );
    EXPECT_EQ( edge.upperNode(), node1 );
}

TEST( EdgeTests, LowerNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 1.0 );
    auto node2 = std::make_shared<Node>( 0.0, 2.0 );
    Edge edge( node1, node2 );

    EXPECT_EQ( edge.lowerNode(), node1 );

    auto node3 = std::make_shared<Node>( 0.0, 0.5 );
    Edge edge2( node1, node3 );

    EXPECT_EQ( edge2.lowerNode(), node3 );
}

TEST( EdgeTests, SetFrontNeighbor_LeftNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 0.0, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node1, node3 );

    edge1->setFrontNeighbor( edge2 );

    ASSERT_EQ( edge1->leftFrontNeighbor, edge2 );
    ASSERT_EQ( edge1->rightFrontNeighbor, nullptr );
}

TEST( EdgeTests, SetFrontNeighbor_RightNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 1.0, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    edge1->setFrontNeighbor( edge2 );

    ASSERT_EQ( edge1->rightFrontNeighbor, edge2 );
    ASSERT_EQ( edge1->leftFrontNeighbor, nullptr );
}

TEST( EdgeTests, SetFrontNeighbor_NoCommonNode )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 2.0, 0.0 );
    auto node4 = std::make_shared<Node>( 3.0, 0.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node3, node4 );

    edge1->setFrontNeighbor( edge2 );

    ASSERT_EQ( edge1->leftFrontNeighbor, nullptr );
    ASSERT_EQ( edge1->rightFrontNeighbor, nullptr );
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

TEST( EdgeTests, CommonNodeTest )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 1.0 );
    auto node3 = std::make_shared<Node>( 2.0, 2.0 );
    auto node4 = std::make_shared<Node>( 1.0, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );
    auto edge3 = std::make_shared<Edge>( node1, node4 );

    // Test common node between edge1 and edge2
    auto commonNode = edge1->commonNode( edge2 );
    ASSERT_TRUE( commonNode != nullptr );
    EXPECT_EQ( commonNode, node2 );

    // Test common node between edge1 and edge3
    commonNode = edge1->commonNode( edge3 );
    ASSERT_TRUE( commonNode != nullptr );
    EXPECT_EQ( commonNode, node1 );

    // Test no common node between edge2 and edge3
    commonNode = edge2->commonNode( edge3 );
    EXPECT_EQ( commonNode, nullptr );
}

TEST( EdgeTests, FrontNeighborAt_LeftNeighbor )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 0.5, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node1, node3 );

    edge1->leftFrontNeighbor = edge2;

    EXPECT_EQ( edge1->frontNeighborAt( node1 ), edge2 );
}

TEST( EdgeTests, FrontNeighborAt_RightNeighbor )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 1.5, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    edge1->rightFrontNeighbor = edge2;

    EXPECT_EQ( edge1->frontNeighborAt( node2 ), edge2 );
}

TEST( EdgeTests, FrontNeighborAt_NoNeighbor )
{
    auto node1 = std::make_shared<Node>( 0.0, 0.0 );
    auto node2 = std::make_shared<Node>( 1.0, 0.0 );
    auto node3 = std::make_shared<Node>( 1.5, 1.0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    EXPECT_EQ( edge1->frontNeighborAt( node3 ), nullptr );
}

TEST( EdgeTests, NextFrontNeighbor_LeftNeighbor )
{
    auto node1 = std::make_shared<Node>( 0, 0 );
    auto node2 = std::make_shared<Node>( 1, 0 );
    auto node3 = std::make_shared<Node>( 0, 1 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node1, node3 );

    edge1->leftFrontNeighbor = edge2;

    EXPECT_EQ( edge1->nextFrontNeighbor( nullptr ), edge2 );
}

TEST( EdgeTests, NextFrontNeighbor_RightNeighbor )
{
    auto node1 = std::make_shared<Node>( 0, 0 );
    auto node2 = std::make_shared<Node>( 1, 0 );
    auto node3 = std::make_shared<Node>( 1, 1 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    edge1->rightFrontNeighbor = edge2;

    EXPECT_EQ( edge1->nextFrontNeighbor( nullptr ), edge2 );
}

TEST( EdgeTests, NextFrontNeighbor_NoNeighbor )
{
    auto node1 = std::make_shared<Node>( 0, 0 );
    auto node2 = std::make_shared<Node>( 1, 0 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );

    EXPECT_EQ( edge1->nextFrontNeighbor( nullptr ), nullptr );
}

TEST( EdgeTests, NextFrontNeighbor_PrevIsLeftNeighbor )
{
    auto node1 = std::make_shared<Node>( 0, 0 );
    auto node2 = std::make_shared<Node>( 1, 0 );
    auto node3 = std::make_shared<Node>( 0, 1 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node1, node3 );

    edge1->leftFrontNeighbor = edge2;

    EXPECT_EQ( edge1->nextFrontNeighbor( edge2 ), nullptr );
}

TEST( EdgeTests, NextFrontNeighbor_PrevIsRightNeighbor )
{
    auto node1 = std::make_shared<Node>( 0, 0 );
    auto node2 = std::make_shared<Node>( 1, 0 );
    auto node3 = std::make_shared<Node>( 1, 1 );

    auto edge1 = std::make_shared<Edge>( node1, node2 );
    auto edge2 = std::make_shared<Edge>( node2, node3 );

    edge1->rightFrontNeighbor = edge2;

    EXPECT_EQ( edge1->nextFrontNeighbor( edge2 ), nullptr );
}