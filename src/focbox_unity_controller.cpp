#include <focbox_unity_controller/focbox_unity_controller.h>

namespace focbox_unity_controller
{
    FocboxUnityController::FocboxUnityController(ros::NodeHandle &nh) : _nh(nh)
    {
        ROS_INFO("FocboxUnityController constructor");

        init();
        controller_manager_.reset(new controller_manager::ControllerManager(this, _nh));
        _nh.param("/focbox_unity_controller/hardware_interface/loop_hz", loop_hz_, 0.1);
        ros::Duration update_freq = ros::Duration(1.0/loop_hz_);
        non_realtime_loop_ = _nh.createTimer(update_freq, &FocboxUnityController::update, this);
    }

    FocboxUnityController::~FocboxUnityController()
    {}

    void FocboxUnityController::init()
    {
        ROS_INFO("FocboxUnityController init");

        // connect and register the joint state interface
        hardware_interface::JointStateHandle state_handle_erw("wheel_right_eff_joint", &_pos[0], &_vel[0], &_eff[0]);
        _joint_state_interface.registerHandle(state_handle_erw);

        hardware_interface::JointStateHandle state_handle_elw("wheel_left_eff_joint", &_pos[1], &_vel[1], &_eff[1]);
        _joint_state_interface.registerHandle(state_handle_elw);

        hardware_interface::JointStateHandle state_handle_vrw("wheel_right_vel_joint", &_pos[0], &_vel[0], &_eff[0]);
        _joint_state_interface.registerHandle(state_handle_vrw);

        hardware_interface::JointStateHandle state_handle_vlw("wheel_left_vel_joint", &_pos[1], &_vel[1], &_eff[1]);
        _joint_state_interface.registerHandle(state_handle_vlw);

        registerInterface(&_joint_state_interface);

        // connect and register the joint effort interface
        hardware_interface::JointHandle eff_handle_rw(state_handle_erw, &_cmd[0]);
        _effort_joint_interface.registerHandle(eff_handle_rw);

        hardware_interface::JointHandle eff_handle_lw(state_handle_elw, &_cmd[1]);
        _effort_joint_interface.registerHandle(eff_handle_lw);

        registerInterface(&_effort_joint_interface);

        // connect and register the joint velocity interface
        hardware_interface::JointHandle vel_handle_rw(state_handle_vrw, &_vel_cmd[0]);
        _velocity_joint_interface.registerHandle(vel_handle_rw);

        hardware_interface::JointHandle vel_handle_lw(state_handle_vlw, &_vel_cmd[1]);
        _velocity_joint_interface.registerHandle(vel_handle_lw);

        registerInterface(&_velocity_joint_interface);
    }

    void FocboxUnityController::update(const ros::TimerEvent& event)
    {
        ROS_INFO("FocboxUnityController update");
        double dummy = _vel_cmd[1];
        _vel_cmd[0] = 5.0;
        elapsed_time_ = ros::Duration(event.current_real - event.last_real);
        read();
        controller_manager_->update(ros::Time::now(), elapsed_time_);
        write();
    }

    void FocboxUnityController::read()
    {
        ROS_INFO("FocboxUnityController read");
        _pos[0] = 1;
        _vel[0] = 1;
        _eff[0] = 1;
        _pos[1] = 1;
        _vel[1] = 1;
        _eff[1] = 1;
    }

    void FocboxUnityController::write()
    {
        ROS_INFO("FocboxUnityController write wr:%f wl:%f", _cmd[0], _cmd[1]);
        double diff_speed_right = _cmd[0];
        double diff_speed_left = _cmd[1];
    }
}