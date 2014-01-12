/* 
 * File:   LPCTest.cpp
 * Author: samuel
 * 
 * Created on 8 January, 2014, 10:37 PM
 */

// TODO: Send a dummy maneuver -> test L, U, S and 8

#include <curve_navigation/LPCTest.hpp>

LPCTest::LPCTest() {
}

LPCTest::LPCTest(ros::NodeHandle node_handle) {
    map_width = 1000;
    map_height = 1000;
    num_samples = 1000;
    pi = 3.14;

    // TODO: Obtain this data from simulator and republish it
    map_msg.info.height = map_height;
    map_msg.info.width = map_width;
    map_msg.data.resize(map_height * map_width);
    std::fill(map_msg.data.begin(), map_msg.data.end(), 0);

    maneuver_msg.poses.resize(num_samples);
    for (unsigned int pose_id = 0; pose_id < maneuver_msg.poses.size(); pose_id++) {
        maneuver_msg.poses.at(pose_id).pose.position.x = pose_id * map_width / num_samples;
        maneuver_msg.poses.at(pose_id).pose.position.y = map_height / 3;
        maneuver_msg.poses.at(pose_id).pose.orientation = tf::createQuaternionMsgFromYaw(pi / 4);
    }

    pose_msg.header.seq = 0;
    maneuver_msg.header.seq = 0;
    map_msg.header.seq = 0;

    maneuver_publisher = node_handle.advertise<nav_msgs::Path>("situational_planner/maneuver", 1);
    pose_publisher = node_handle.advertise<geometry_msgs::PoseStamped>("localization/pose", 100);
    // TODO: Obtain this data from simulator and republish it
    map_publisher = node_handle.advertise<nav_msgs::OccupancyGrid>("environment/map", 10);

    pose_subscriber = node_handle.subscribe("simulator/pose", 2,
                                            &LPCTest::publishPose, this);
}

LPCTest::LPCTest(const LPCTest& orig) {
}

LPCTest::~LPCTest() {
}

void LPCTest::publishPose(const geometry_msgs::Pose::ConstPtr& pose_ptr) {
    pose_msg.pose = *pose_ptr;
    pose_msg.header.stamp = ros::Time::now();
    pose_publisher.publish(pose_msg);
    pose_msg.header.seq += 1;
}

void LPCTest::sendTestData() {
    maneuver_msg.header.stamp = ros::Time::now();
    maneuver_publisher.publish(maneuver_msg);
    maneuver_msg.header.seq += 1;

    // TODO: Obtain this data from simulator and republish it
    map_msg.header.stamp = ros::Time::now();
    map_publisher.publish(map_msg);
    map_msg.header.seq += 1;
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "lpc_tester");
    ros::NodeHandle node_handle;

    LPCTest lpc_tester(node_handle);

    ros::Rate loop_rate(10);
    while (ros::ok()) {
        lpc_tester.sendTestData();

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}