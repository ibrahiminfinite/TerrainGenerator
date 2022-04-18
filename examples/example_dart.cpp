
#include <dart/dart.hpp>
#include <dart/gui/osg/osg.hpp>
#include <dart/external/imgui/imgui.h>
#include "TerrainGenerator.h"
#include <dart/collision/ode/ode.hpp>

using  namespace dart;


dynamics::SkeletonPtr createBall(const Eigen::Vector3d& position)
{
    dynamics::SkeletonPtr ballSkel = dynamics::Skeleton::create();

    // Give the floor a body
    dynamics::BodyNodePtr ballBody
            = ballSkel->createJointAndBodyNodePair<dynamics::FreeJoint>(nullptr)
                    .second;

    // Give the body a shape
    double radius = 0.05;
    auto ballShape = std::make_shared<dynamics::SphereShape>(radius);

    dynamics::ShapeNode* shapeNode = ballBody->createShapeNodeWith<
            dynamics::VisualAspect,
            dynamics::CollisionAspect,
            dynamics::DynamicsAspect>(ballShape);

    shapeNode->getVisualAspect()->setColor(
            dart::math::Random::uniform<Eigen::Vector3d>(0.0, 1.0));



    dynamics::Inertia inertia;
    double mass = 1000 * ballShape->getVolume();
    inertia.setMass(mass);
    inertia.setMoment(ballShape->computeInertia(mass));
    ballBody->setInertia(inertia);

    const double default_restitution = 0.6;
    // Set the coefficient of restitution to make the body more bouncy
    shapeNode->getDynamicsAspect()->setRestitutionCoeff(default_restitution);


    // Create reference frames for setting the initial velocity
    Eigen::Isometry3d centerTf(Eigen::Isometry3d::Identity());
    centerTf.translation() = ballSkel->getCOM();
    dynamics::SimpleFrame center(dynamics::Frame::World(), "center", centerTf);

    // Set the velocities of the reference frames so that we can easily give the
    // Skeleton the linear and angular velocities that we want
    double angle = dart::math::toRadian(30.0); // rad
    double speed = 0.0; // m/s
    double angular_speed = 3 * dart::math::constantsd::pi(); // rad/s

    Eigen::Vector3d v = speed * Eigen::Vector3d(0.5, 0.0, 0.0);
    Eigen::Vector3d w = 0 * Eigen::Vector3d::UnitY();
    center.setClassicDerivatives(v, w);

    dynamics::SimpleFrame ref(&center, "root_reference");
    ref.setRelativeTransform(ballSkel->getBodyNode(0)->getTransform(&center));

    // Use the reference frames to set the velocity of the Skeleton's root
    ballSkel->getJoint(0)->setVelocities(ref.getSpatialVelocity());



    // Put the body into position
    Eigen::Isometry3d tf = Eigen::Isometry3d::Identity();
    tf.translation() = position;
    ballBody->getParentJoint()->setTransformFromParentBodyNode(tf);

    return ballSkel;
}

std::vector<dynamics::SkeletonPtr> createballs(double width){
    std::vector<dynamics::SkeletonPtr> temp{};
    for(int i = 0;i<21;i++){
        for(int j = 0;j<21;j++){
            double interval = width/20;
            double x = -width/2 + interval * i;
            double y= -width/2 + interval * j;
            Eigen::Vector3d pos(x, y, 0.5);
            temp.emplace_back(createBall(pos));
        }
    }
    return temp;
}


class HeightmapWorld : public gui::osg::WorldNode
{
public:
    explicit HeightmapWorld(simulation::WorldPtr world)
            : gui::osg::WorldNode(std::move(world))
    {
        // Do nothing
    }

    // Triggered at the beginning of each simulation step
    void customPreStep() override
    {
        // Do nothing
    }

protected:
};

class Widget : public gui::osg::ImGuiWidget {

private:
    dart::gui::osg::ImGuiViewer* mViewer;

public:
    Widget(gui::osg::ImGuiViewer* viewer):mViewer(viewer){}

    void render() override {
        ImGui::SetNextWindowPos(ImVec2(10, 20));
        ImGui::SetNextWindowSize(ImVec2(360, 350));
        ImGui::SetNextWindowBgAlpha(0.5f);
        if (!ImGui::Begin(
                "Heightmap Demo",
                nullptr,
                ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }


        if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen))
        {
            int e = mViewer->isSimulating() ? 0 : 1;
            if (mViewer->isAllowingSimulation())
            {
                if (ImGui::RadioButton("Play", &e, 0) && !mViewer->isSimulating())
                    mViewer->simulate(true);
                ImGui::SameLine();
                if (ImGui::RadioButton("Pause", &e, 1) && mViewer->isSimulating())
                    mViewer->simulate(false);
            }
        }
    }

};


int main() {


    dart::gui::osg::ImGuiViewer viewer;

    viewer.setUpViewInWindow(0, 0, 1280, 720);
    viewer.getCameraManipulator()->setHomePosition(
            ::osg::Vec3(2.57f, 3.14f, 1.64f),
            ::osg::Vec3(0.00f, 0.00f, 0.30f),
            ::osg::Vec3(-0.24f, -0.25f, 0.94f));
    viewer.setCameraManipulator(viewer.getCameraManipulator());


    TerrainGenerator generator;
    TerrainConfig config;

    config.terrainType = TerrainType::Steps;
    config.xSize = config.ySize = 8.0;
    config.resolution = 0.02;


    // Hills
    config.amplitude = 1.0;
    config.frequency = 0.2;
    config.roughenss = 0.000;
    config.numOctaves = 1;

    // Steps
    config.stepWidth = 0.2;
    config.stepHeight = 0.1;


    auto world = dart::simulation::World::create();

    world->getConstraintSolver()->setCollisionDetector(dart::collision::OdeCollisionDetector::create());

    world->setGravity(Eigen::Vector3d{0.0,0.0,-10.0});


    // Create an instance of our customized WorldNode
    ::osg::ref_ptr<HeightmapWorld> node = new HeightmapWorld(world);
    node->setNumStepsPerCycle(16);

    auto terrain = generator.generate(config);

    world->addSimpleFrame(terrain.visual);
    world->addSkeleton(terrain.collision);

    auto balls = createballs(10.0);
    for (const auto& ball : balls)
        world->addSkeleton(ball);


    viewer.addWorldNode(node);
    viewer.simulate(true);

    // Create grid
    ::osg::ref_ptr<gui::osg::GridVisual> grid = new gui::osg::GridVisual();

    viewer.addAttachment(grid);
    viewer.getImGuiHandler()->addWidget(std::make_shared<Widget>(&viewer));
    viewer.run();

}
