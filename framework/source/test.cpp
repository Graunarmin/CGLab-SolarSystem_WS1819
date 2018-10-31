#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <../../external/catch2/catch.hpp>
#include "../include/Node.hpp"
#include "../include/SceneGraph.hpp"
#include <memory>
#include <typeinfo>
#include "../include/GeometryNode.hpp"
//#include "../include/CameraNode.hpp"



/*TEST_CASE("Create Node", "[Node Constructor]" ) {
    Node node{};
    REQUIRE(node.getParent() == nullptr);
    REQUIRE(node.getDepth() == 0);
    REQUIRE(node.getName() == "Node");
    REQUIRE(node.getLocalTransform()[0].x == 1.0f);
}

TEST_CASE("Create special Node", "[Node Constructor]"){
    Node node{};
    auto p = std::make_shared<Node>(node);
    std::list<std::shared_ptr<Node>> children;
    Node cNode{p, "childNode", "node/childNode", 2};

    REQUIRE(cNode.getParent() -> getName() == node.getName());
    REQUIRE(cNode.getChildrenList().size() == 0);
    REQUIRE(cNode.getDepth() == 2);
}

TEST_CASE("Add to ChildrenList", "[addChildren()]"){

    Node root{nullptr, "root", "root", 0};
    auto root_p = std::make_shared<Node>(root);

    GeometryNode sun{root_p, "sun", "root/sun", 1};
    auto sun_p = std::make_shared<GeometryNode>(sun);

    GeometryNode mercury{sun_p, "mercury", "root/sun/mercury", 2};
    auto mercury_p = std::make_shared<GeometryNode>(mercury);

    root.addChildren(sun_p);
    sun.addChildren(mercury_p);

    auto root_first = root.getChildrenList().begin();

    REQUIRE((*(*root_first)).getName() == "sun");
    REQUIRE((*(*sun.getChildrenList().begin())).getName() == "mercury");

}

void testFunction(std::list<std::shared_ptr<Node>> const& childPlanetsList){
    for(auto& planet: childPlanetsList){
        auto childPlanets = (*planet).getChildrenList();
        //Rekursion: (funktioniert nocht nicht!)
        if(childPlanets.size() > 0){
            std::cout << "hi" << std::endl;          
            testFunction(childPlanets);
        }
    }
}

TEST_CASE("Get Children from children", "[getChildrenList()]"){

    Node root{nullptr, "root", "root", 0};
    auto root_p = std::make_shared<Node>(root);
    GeometryNode sun{root_p, "sun", "root/sun", 1};
    auto sun_p = std::make_shared<GeometryNode>(sun);
    GeometryNode mercury{sun_p, "mercury", "root/sun/mercury", 2};
    auto mercury_p = std::make_shared<GeometryNode>(mercury);
     GeometryNode venus{sun_p, "venus", "root/sun/venus", 2};
    auto venus_p = std::make_shared<GeometryNode>(venus);
    GeometryNode earth{sun_p, "earth", "root/sun/earth", 2};
    auto earth_p = std::make_shared<GeometryNode>(earth);
    GeometryNode moon{earth_p, "moon", "root/sun/earth/moon", 3};
    auto moon_p = std::make_shared<GeometryNode>(moon);

    root.addChildren(sun_p);
    sun.addChildren(mercury_p);
    sun.addChildren(venus_p);
    sun.addChildren(earth_p);
    earth.addChildren(moon_p);

    REQUIRE(sun.getChildrenList().size() == 3);

    testFunction(root.getChildrenList());
}*/