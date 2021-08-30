#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "px4_msgs/msg/vehicle_attitude.hpp"
#include "px4_msgs/msg/vehicle_global_position.hpp"

template <class T>
class ThrottleMessage {
public:
	typedef typename T::SharedPtr TopicSharedPtr;
	ThrottleMessage(rclcpp::Node* node, std::string sub_topic, std::string pub_topic, double rate) :
		duration(rclcpp::Duration::from_seconds(1.0 / rate)),
		last_message(node->get_clock()->now())
	{
		pub = node->create_publisher<T>(pub_topic, 10);
		sub = node->create_subscription<T>(sub_topic, 10, [this, node](const TopicSharedPtr msg) {
			rclcpp::Time now = node->get_clock()->now();
			if(now >= last_message + duration) {
				pub->publish(*msg);
				last_message = now;
			}
		});
	}
private:
	typename rclcpp::Publisher<T>::SharedPtr pub;
	typename rclcpp::Subscription<T>::SharedPtr sub;
	rclcpp::Duration duration;
	rclcpp::Time last_message;
};

class ThrottleMessages : public rclcpp::Node
{
public:
	ThrottleMessages() : 
		Node("throttle_messages"),
		vehicle_attitude_throttle(this, "/VehicleAttitude_PubSubTopic", "/vehicle_attitude_throttled", 1.0),
		vehicle_global_position_throttle(this, "/VehicleGlobalPosition_PubSubTopic", "/vehicle_global_position_throttled", 10.0)

	{
	}

private:
	ThrottleMessage<px4_msgs::msg::VehicleAttitude> vehicle_attitude_throttle;
	ThrottleMessage<px4_msgs::msg::VehicleGlobalPosition> vehicle_global_position_throttle;
};

int main(int argc, char * argv[])
{
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<ThrottleMessages>());
	rclcpp::shutdown();
	return 0;
}
