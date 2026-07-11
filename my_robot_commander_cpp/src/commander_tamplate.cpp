#include<rclcpp/rclcpp.hpp>
#include<moveit/move_group_interface/move_group_interface.h>
#include<example_interfaces/msg/bool.hpp>
#include"the_robot_msgs/msg/arm_joint_values.hpp"
#include"the_robot_msgs/msg/arm_name_pose.hpp"
#include"the_robot_msgs/msg/arm_pose_targets.hpp"

using MoveGroupInterface = moveit::planning_interface::MoveGroupInterface;
using Bool = example_interfaces::msg::Bool;
using ArmJointValues = the_robot_msgs::msg::ArmJointValues;
using ArmPoseTargets = the_robot_msgs::msg::ArmPoseTargets;
using ArmNamePose = the_robot_msgs::msg::ArmNamePose;

using namespace std::placeholders;
class Commander
{   

public:
    Commander(std::shared_ptr<rclcpp::Node> node)
    {
        node_ =node;
        arm_ = std::make_shared<MoveGroupInterface>(node_,"arm_group");
        arm_->setMaxVelocityScalingFactor(1.0);
        arm_->setMaxAccelerationScalingFactor(1.0);

        gripper_ = std::make_shared<MoveGroupInterface>(node_,"gripper_group");

        open_griper_sub_ = node_->create_subscription<Bool>(
            "open_grip",10,std::bind(&Commander::OpenGripperCallBack,this,_1));

        joint_values_sub_ = node_->create_subscription<ArmJointValues>(
            "joint_values",10,std::bind(&Commander::JointValuesMoveCallBack,this,_1));

        arm_pose_target_sub_ = node_->create_subscription<ArmPoseTargets>(
            "pose_target",10,std::bind(&Commander::ArmPoseTargetCallBack,this,_1));

        arm_name_pose_sub_ = node_->create_subscription<ArmNamePose>(
            "pose_name",10,std::bind(&Commander::ArmNamePoseCallBack,this,_1));
    }

    void goToNameTarget(const std::string &name)
    {
        arm_->setStartStateToCurrentState();
        arm_->setNamedTarget(name);
        PlanAndExecute(arm_);
    }

    void goToJointTarget(const std::vector<double> &joints)
    {
        arm_->setStartStateToCurrentState();
        arm_->setJointValueTarget(joints);
        PlanAndExecute(arm_);
    }

    void goToPoseTarget(double x, double y, double z, double roll, double pitch, double yaw,bool cartiation_path)
    {
        tf2::Quaternion q;
        q.setRPY(roll, pitch,yaw);
        q = q.normalize();

        geometry_msgs::msg::PoseStamped target_pose ;
        target_pose.header.frame_id = "base_link";
        target_pose.pose.position.x = x;
        target_pose.pose.position.y = y;
        target_pose.pose.position.z = z;
        target_pose.pose.orientation.x = q.getX();
        target_pose.pose.orientation.y = q.getY();
        target_pose.pose.orientation.z = q.getZ();
        target_pose.pose.orientation.w = q.getW();

        arm_->setStartStateToCurrentState();

        if (!cartiation_path)
        {
            arm_->setPoseTarget(target_pose);
            PlanAndExecute(arm_);
        }
        else
        {
            std::vector<geometry_msgs::msg::Pose> waypoints;
            waypoints.push_back(target_pose.pose);
            moveit_msgs::msg::RobotTrajectory trajectory;

            double fraction = arm_->computeCartesianPath(waypoints,0.01,0.0,trajectory);

            if (fraction == 1)
            {
                arm_->execute(trajectory);
            }
        }
        
        
    }

    void openGrapper()
    {
        gripper_->setStartStateToCurrentState();
        gripper_->setNamedTarget("open");
        PlanAndExecute(gripper_);
    }

    void closeGripper()
    {
        gripper_->setStartStateToCurrentState();
        gripper_->setNamedTarget("closed");
        PlanAndExecute(gripper_);
    }

    void OpenGripperCallBack(const Bool &msg)
    {
        if (msg.data)
        {
            openGrapper();
        }
        else
        {
            closeGripper();
        }
        
    }

    void JointValuesMoveCallBack(const ArmJointValues &msg)
    {
        std::vector<double> jointsValues = {0.0,0.0,0.0,0.0,0.0,0.0};

        jointsValues.clear();
        for (int i = 1; i < 7; i++)
        {
            
            //RCLCPP_INFO(node_->get_logger(),"%f",msg.joints[i]);
            jointsValues.push_back(msg.joints[i]);
        }
        goToJointTarget(jointsValues);
        jointsValues.clear();
        
    }

    void ArmPoseTargetCallBack(const ArmPoseTargets &msg)
    {
        //RCLCPP_INFO(node_->get_logger(),"%f",msg.x);
        goToPoseTarget(msg.x, msg.y, msg.z, msg.roll, msg.pitch, msg.yaw, msg.cartiation_path);
    }

    void ArmNamePoseCallBack(const ArmNamePose &msg)
    {
        if (msg.data == "home")
        {
            goToNameTarget(msg.data);
        }
        else if (msg.data == "pose_1")
        {
            goToNameTarget(msg.data);
        }
        else if (msg.data == "pose_2")
        {
            goToNameTarget(msg.data);
        }
        else
        {
            RCLCPP_WARN(node_->get_logger(),"This is Invalid Pose Name");
        }
        
        
    }

private:

    void PlanAndExecute(const std::shared_ptr<MoveGroupInterface>&interface)
    {

        MoveGroupInterface::Plan plan;
        bool success = interface->plan(plan) == moveit::core::MoveItErrorCode::SUCCESS;

        if (success == 1)   
        {
            interface->execute(plan);
        }
        

    }

    std::shared_ptr<rclcpp::Node> node_;
    std::shared_ptr<MoveGroupInterface> arm_;
    std::shared_ptr<MoveGroupInterface> gripper_;

    rclcpp::Subscription<Bool>::SharedPtr open_griper_sub_;
    rclcpp::Subscription<ArmJointValues>::SharedPtr joint_values_sub_;
    rclcpp::Subscription<ArmPoseTargets>::SharedPtr arm_pose_target_sub_;
    rclcpp::Subscription<ArmNamePose>::SharedPtr arm_name_pose_sub_;


};

int main(int argc, char * argv[])
{


    rclcpp::init(argc,argv);

    auto node = std::make_shared<rclcpp::Node>("commander");
    auto commander = Commander(node);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;

}