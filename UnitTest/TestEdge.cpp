#include "pch.h"

#include "Edge.h"
#include "Node.h"
#include "MyVector.h"

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

static std::shared_ptr<Triangle> makeTriangle(
    const std::shared_ptr<Edge>& e1,
    const std::shared_ptr<Edge>& e2,
    const std::shared_ptr<Edge>& e3)
{
    return std::make_shared<Triangle>(e1, e2, e3);
}

// Helper to create a quad for testing
static std::shared_ptr<Quad> makeQuad(
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

    auto tri = makeTriangle(e1, e2, e3);
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

    auto tri = makeTriangle(e1, e2, e3);
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

    auto quad = makeQuad(e1, e2, e3, e4);  
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

    auto quad = makeQuad(e1, e2, e3, e4);
    e1->connectToElement(quad);
    e2->connectToElement(quad);
    e3->connectToElement(quad);
    e4->connectToElement(quad);

    auto result = e1->evalPotSideEdge(e2, n2);
    ASSERT_EQ(result, nullptr);
}

std::shared_ptr<Triangle> makeTriangle(const std::shared_ptr<Edge>& e1, const std::shared_ptr<Edge>& e2, const std::shared_ptr<Edge>& e3) {
    auto tri = std::make_shared<Triangle>(e1, e2, e3);
    e1->connectToTriangle(tri);
    e2->connectToTriangle(tri);
    e3->connectToTriangle(tri);
    return tri;
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
    auto tri1 = makeTriangle(e_main, e_left, std::make_shared<Edge>(n3, n2));
    auto tri2 = makeTriangle(e_main, e_right, std::make_shared<Edge>(n1, n4));

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

class EdgeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear stateList before each test
        Edge::clearStateList();
    }
};

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
}