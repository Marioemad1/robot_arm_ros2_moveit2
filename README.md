# ROS 2 MoveIt 2 Robot Arm Project

This repository contains a ROS 2 Humble robotic arm project integrated with MoveIt 2, ros2_control, custom robot descriptions, custom message interfaces, and a C++ commander node for controlling the robot.

The project supports arm motion planning using MoveIt 2 and allows the robot to move using named poses, joint targets, pose targets, Cartesian path commands, and gripper open/close commands.

---


https://github.com/user-attachments/assets/9b86c948-561f-4ca5-b9cd-f214b6d07e40


---

## Features

- ROS 2 Humble robotic arm project
- MoveIt 2 motion planning integration
- Custom URDF/Xacro robot description
- MoveIt generated configuration package
- ros2_control integration
- Arm and gripper trajectory controllers
- Joint state broadcaster
- Custom C++ commander node
- Custom ROS 2 message interfaces
- Control using:
  - Named poses
  - Joint values
  - End-effector pose targets
  - Cartesian paths
  - Gripper open/close commands

---

## Repository Structure

```text
robot_arm_ros2_moveit2/
├── my_robot_commander_CPP/
│   └── C++ commander node for MoveIt control
│
├── my_robot_moveit_config/
│   └── MoveIt 2 generated configuration package
│
├── my_robot_bringup/
│   └── Launch files and ros2_control configuration
│
├── robot_description/
│   └── Main robot URDF/Xacro description files
│
├── move_robot_description/
│   └── Additional robot description and launch files
│
├── the_robot_msgs/
│   └── Custom ROS 2 message interfaces
│
├── my_robot_controllers/
│   └── Custom ros2_control controller experiments
│
└── my_robot_hardware/
    └── Custom ros2_control hardware interface experiments
```

---

## Requirements

This project was developed and tested using:

- Ubuntu 22.04
- ROS 2 Humble
- MoveIt 2
- RViz2
- Gazebo
- ros2_control
- ros2_controllers
- joint_state_broadcaster
- joint_trajectory_controller

Install the required dependencies:

```bash
sudo apt update

sudo apt install ros-humble-moveit \
                 ros-humble-moveit-ros-planning-interface \
                 ros-humble-moveit-kinematics \
                 ros-humble-ros2-control \
                 ros-humble-ros2-controllers \
                 ros-humble-joint-state-broadcaster \
                 ros-humble-joint-trajectory-controller \
                 ros-humble-xacro \
                 ros-humble-robot-state-publisher \
                 ros-humble-rviz2
```

---

## Building the Workspace

Create a ROS 2 workspace:

```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
```

Clone the repository:

```bash
git clone git@github.com:Marioemad1/robot_arm_ros2_moveit2.git
```

Build the workspace:

```bash
cd ~/ros2_ws

source /opt/ros/humble/setup.bash

colcon build --symlink-install
```

Source the workspace:

```bash
source install/setup.bash
```

---

## Clean Build

If you face build errors from old generated files or symlink issues, clean the workspace and rebuild:

```bash
cd ~/ros2_ws

rm -rf build install log

source /opt/ros/humble/setup.bash

colcon build --symlink-install

source install/setup.bash
```

If the error is only in the custom message package, clean only that package:

```bash
cd ~/ros2_ws

rm -rf build/the_robot_msgs
rm -rf install/the_robot_msgs

source /opt/ros/humble/setup.bash

colcon build --packages-select the_robot_msgs --symlink-install

source install/setup.bash
```

---

## Running the Project

Launch the MoveIt demo:

```bash
ros2 launch my_robot_moveit_config demo.launch.py
```

Or launch the custom bringup file if available:

```bash
ros2 launch my_robot_bringup arm_executer.launch.xml
```

Check that the controllers are running:

```bash
ros2 control list_controllers
```

Expected controllers:

```text
joint_state_broadcaster    active
arm_group_controller       active
gripper_group_controller   active
```

---

## Commander Node

The commander node is a C++ node that receives simple ROS 2 topic commands and converts them into MoveIt 2 planning requests.

It controls:

- The arm group
- The gripper group
- Named poses
- Joint targets
- Pose targets
- Cartesian path targets

Run the commander node:

```bash
ros2 run my_robot_commander_CPP commander
```

---

## Commander Topics

### 1. Move to Named Pose

Topic:

```text
/pose_name
```

Message type:

```text
the_robot_msgs/msg/ArmNamePose
```

Example command:

```bash
ros2 topic pub --once /pose_name the_robot_msgs/msg/ArmNamePose "{data: 'home'}"
```

Move to `pose_1`:

```bash
ros2 topic pub --once /pose_name the_robot_msgs/msg/ArmNamePose "{data: 'pose_1'}"
```

Move to `pose_2`:

```bash
ros2 topic pub --once /pose_name the_robot_msgs/msg/ArmNamePose "{data: 'pose_2'}"
```

---

### 2. Move Using Joint Values

Topic:

```text
/joint_values
```

Message type:

```text
the_robot_msgs/msg/ArmJointValues
```

Example command:

```bash
ros2 topic pub --once /joint_values the_robot_msgs/msg/ArmJointValues \
"{joints: [0.0, 1.5, 0.5, 0.0, 1.5, 0.0, -0.7]}"
```

Note: the current commander code reads joint values starting from index `1`, so the first value in the array is ignored.

---

### 3. Move to Pose Target

Topic:

```text
/pose_target
```

Message type:

```text
the_robot_msgs/msg/ArmPoseTargets
```

Example command:

```bash
ros2 topic pub --once /pose_target the_robot_msgs/msg/ArmPoseTargets \
"{x: 0.7, y: 0.0, z: 0.4, roll: 3.14, pitch: 0.0, yaw: 0.0, cartiation_path: false}"
```

Important notes:

- Pose targets use inverse kinematics.
- The position may be reachable, but the orientation may still be invalid.
- Angles must be in radians, not degrees.
- Start from a known valid pose and change one value at a time.

Useful angle conversions:

```text
90 degrees  = 1.57 rad
180 degrees = 3.14 rad
```

---

### 4. Cartesian Path Command

The Cartesian path command uses the same `/pose_target` topic.

Set:

```text
cartiation_path: true
```

Example command:

```bash
ros2 topic pub --once /pose_target the_robot_msgs/msg/ArmPoseTargets \
"{x: 0.7, y: 0.0, z: 0.4, roll: 3.14, pitch: 0.0, yaw: 0.0, cartiation_path: true}"
```

Note: the field name is currently written as `cartiation_path` in the custom message.

---

### 5. Open and Close Gripper

Topic:

```text
/open_grip
```

Message type:

```text
example_interfaces/msg/Bool
```

Open gripper:

```bash
ros2 topic pub --once /open_grip example_interfaces/msg/Bool "{data: true}"
```

Close gripper:

```bash
ros2 topic pub --once /open_grip example_interfaces/msg/Bool "{data: false}"
```

---

## Custom Message Interfaces

The package `the_robot_msgs` contains the custom message interfaces used by the commander node.

Main messages used:

```text
the_robot_msgs/msg/ArmNamePose
the_robot_msgs/msg/ArmJointValues
the_robot_msgs/msg/ArmPoseTargets
```

Check any message definition using:

```bash
ros2 interface show the_robot_msgs/msg/ArmPoseTargets
```

---

## MoveIt Configuration

The MoveIt configuration package contains:

- SRDF file
- Kinematics configuration
- Joint limits
- Planning pipeline configuration
- Controller configuration
- RViz configuration

The kinematics solver is configured using KDL:

```yaml
arm_group:
  kinematics_solver: kdl_kinematics_plugin/KDLKinematicsPlugin
  kinematics_solver_search_resolution: 0.005
  kinematics_solver_timeout: 0.005
```

Check if MoveIt loaded the kinematics solver:

```bash
ros2 param get /move_group robot_description_kinematics.arm_group.kinematics_solver
```

Expected output:

```text
String value is: kdl_kinematics_plugin/KDLKinematicsPlugin
```

---

## ros2_control

The robot uses ros2_control for trajectory execution.

Main controllers:

```text
joint_state_broadcaster
arm_group_controller
gripper_group_controller
```

The arm controller uses:

```text
joint_trajectory_controller/JointTrajectoryController
```

The gripper controller also uses:

```text
joint_trajectory_controller/JointTrajectoryController
```

Check loaded controllers:

```bash
ros2 control list_controllers
```

---

## Common Issues

### 1. Controller manager does not load controllers

If the controller manager waits for `robot_description`, make sure the remap exists:

```xml
<remap from="/controller_manager/robot_description" to="/robot_description"/>
```

Then check controller manager services:

```bash
ros2 service list | grep controller_manager
```

You should see services like:

```text
/controller_manager/list_controllers
/controller_manager/load_controller
/controller_manager/switch_controller
```

---

### 2. Pose name works, joint target works, but pose target fails

This usually means the pose target is invalid or unreachable.

A pose target must satisfy both:

```text
x, y, z
roll, pitch, yaw
```

Even if the position is reachable, the requested orientation may be impossible for the robot.

Try this known working pose:

```bash
ros2 topic pub --once /pose_target the_robot_msgs/msg/ArmPoseTargets \
"{x: 0.7, y: 0.0, z: 0.4, roll: 3.14, pitch: 0.0, yaw: 0.0, cartiation_path: false}"
```

Then change one value at a time.

---

### 3. No kinematics plugins defined

If this warning appears:

```text
No kinematics plugins defined. Fill and load kinematics.yaml!
```

Make sure the kinematics file exists:

```bash
ros2 pkg prefix my_robot_moveit_config
```

Then check:

```bash
ros2 param get /move_group robot_description_kinematics.arm_group.kinematics_solver
```

If the parameter is not set, the MoveIt launch file is not loading `kinematics.yaml`.

---

### 4. Symlink error in the custom message package

If this error appears:

```text
failed to create symbolic link because existing path cannot be removed: Is a directory
```

Clean the custom message package:

```bash
cd ~/ros2_ws

rm -rf build/the_robot_msgs
rm -rf install/the_robot_msgs

source /opt/ros/humble/setup.bash

colcon build --packages-select the_robot_msgs --symlink-install
```

Then rebuild the workspace:

```bash
colcon build --symlink-install
```

---

### 5. GitHub push asks for password

GitHub does not support password authentication for Git operations.

Use SSH:

```bash
git remote set-url origin git@github.com:Marioemad1/robot_arm_ros2_moveit2.git
```

Then push:

```bash
git push -u origin main
```

---

## Development Notes

- The robot description is written using URDF/Xacro.
- MoveIt handles high-level motion planning.
- ros2_control handles low-level trajectory execution.
- RViz2 is used for visualization and testing.
- The commander node provides a simple topic-based interface for testing motion commands.
- Pose targets require valid inverse kinematics.
- Invalid or unreachable pose targets are expected during testing.

---

## Author

Developed by Mario Emad.

GitHub: Marioemad1
