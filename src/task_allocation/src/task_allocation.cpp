#include "task_allocation.h"

Task_Allocation::Task_Allocation(int argc,char** argv)
{
    //robot_name="Robot i"
    std::string robot_name=argv[1];
    const char * robot_ID;
    robot_ID=robot_name.substr(robot_name.size()-1).c_str();
    my_robot_.allocation_robot_info.robot_ID=atoi(robot_ID);
    my_robot_.gazebo_robot_info.robot_ID=atoi(robot_ID);
    ROS_INFO("start %s task_allocation_process",robot_name.c_str());

    nh_ = new ros::NodeHandle();
    robot2gazebo_pub_       =nh_->advertise<allocation_common::robot2gazebo_info>(robot_name+"/task_allocation/robot2gazebo_info",10);
    //robot2task_pub_         =nh_->advertise<allocation_common::allocation_task_info>("/task_allocation/task_state_info",1);
    allocation2terminal_pub_=nh_->advertise<allocation_common::allocation2terminal_info>(robot_name+"/task_allocation/allocation2terminal_info",10);
    gazebo2world_sub_  =nh_->subscribe("/allocation_gazebo/gazebo2world_info",10,&Task_Allocation::update_gazebo_world,this);
    terminal2robot_sub_=nh_->subscribe("/control_terminal/terminal2robot_info",10,&Task_Allocation::update_terminal_info,this);
    allocation_timer_  =nh_->createTimer(ros::Duration(0.03),&Task_Allocation::loopControl,this);
    my_behaviour_=new Behaviour(obstacles_);

    num_task_valid_=0;
    num_target_valid_=0;
    num_task_unallocated_=0;
    num_target_unallocated_=0;
    bid_new_task_=false;
    bid_new_target_=false;
    is_target_completed=false;
    is_task_explored=false;
    is_world_update_=false;
    all_tasks_.clear();
    all_robots_.clear();

    my_robot_.allocation_robot_info.isvalid=true;
}

Task_Allocation::~Task_Allocation()
{
    num_task_valid_=0;
    num_target_valid_=0;
    num_task_unallocated_=0;
    num_target_unallocated_=0;
    bid_new_task_=false;
    bid_new_target_=false;
    is_target_completed=false;
    is_task_explored=false;
    is_world_update_=false;
}

/// \brief update the model sate from gazebo msg
void Task_Allocation::update_gazebo_world(const allocation_common::gazebo2world_info::ConstPtr &_msg)
{
    //because the robots' pos is change,clear the obstacles and re-push the pos of robots
    obstacles_.clear();
    //first callback, if all_robots is a null vector, resize its size according to the _msg from gazebo
    if(all_robots_.size()==0)
        all_robots_.resize(_msg->gazebo_robots_info.size());

    Gazebo_robot_info _robot_info_tmp;
    //update the gazebo robots states for allocation
    for(unsigned int i=0;i<_msg->gazebo_robots_info.size();i++)
    {
        _robot_info_tmp.robot_ID=_msg->gazebo_robots_info[i].robot_ID;
        _robot_info_tmp.robot_pos.x_=_msg->gazebo_robots_info[i].robot_pose.position.x;
        _robot_info_tmp.robot_pos.y_=_msg->gazebo_robots_info[i].robot_pose.position.y;
        _robot_info_tmp.robot_ori=_msg->gazebo_robots_info[i].robot_pose.theta;
        _robot_info_tmp.robot_vel.x_=_msg->gazebo_robots_info[i].robot_twist.linear.x;
        _robot_info_tmp.robot_vel.y_=_msg->gazebo_robots_info[i].robot_twist.linear.y;
        _robot_info_tmp.robot_w=_msg->gazebo_robots_info[i].robot_twist.angular;

        //update the robot model when it is valid
        if(all_robots_[_robot_info_tmp.robot_ID].allocation_robot_info.isvalid)
            all_robots_[_robot_info_tmp.robot_ID].gazebo_robot_info=_robot_info_tmp;

        //the robot is placed on the init position, the expect_pos is robot_pos and update robot_ID
        if(all_robots_[_robot_info_tmp.robot_ID].allocation_robot_info.expect_pos.x_==-100)
        {
            all_robots_[_robot_info_tmp.robot_ID].allocation_robot_info.robot_ID=_robot_info_tmp.robot_ID;
            all_robots_[_robot_info_tmp.robot_ID].allocation_robot_info.expect_pos=_robot_info_tmp.robot_pos;
        }

        //if the robot_ID is me
        if(_robot_info_tmp.robot_ID==my_robot_.gazebo_robot_info.robot_ID)
        {
            my_robot_.allocation_robot_info.move_distance=my_robot_.allocation_robot_info.move_distance+_robot_info_tmp.robot_pos.distance(my_robot_.gazebo_robot_info.robot_pos);
            my_robot_.gazebo_robot_info=_robot_info_tmp;
            if(my_robot_.allocation_robot_info.expect_pos.x_==-100)
                my_robot_.allocation_robot_info.expect_pos=_robot_info_tmp.robot_pos;
        }

        //if the robot_ID is not me, it is a obstacle
        else
            obstacles_.push_back(DPoint(_robot_info_tmp.robot_pos.x_,_robot_info_tmp.robot_pos.y_));
    }

    //first callback, if all_tasks is a null vector, resize its size according to the _msg from gazebo, and because the position of task will not change, the callback only be executed one time
    if(all_tasks_.size()==0)
    {
        all_tasks_.resize(_msg->gazebo_tasks_info.size());

        Gazebo_task_info  _task_info_tmp;
        //update the gazebo tasks states for allocation
        for(unsigned int j=0;j<_msg->gazebo_tasks_info.size();j++)
        {
            _task_info_tmp.task_ID=_msg->gazebo_tasks_info[j].task_ID;
            _task_info_tmp.task_pos.x_=_msg->gazebo_tasks_info[j].task_pose.x;
            _task_info_tmp.task_pos.y_=_msg->gazebo_tasks_info[j].task_pose.y;

            //update the task model when it is uncomplete
            if(!all_tasks_[_task_info_tmp.task_ID].allocation_task_info.iscomplete||!all_tasks_[_task_info_tmp.task_ID].allocation_task_info.isexplored)
                all_tasks_[_task_info_tmp.task_ID].gazebo_task_info=_task_info_tmp;

            all_tasks_[_task_info_tmp.task_ID].allocation_task_info.task_ID=_task_info_tmp.task_ID;
        }
    }
}

/// \brief update the terminal info to control the allocation process
void Task_Allocation::update_terminal_info(const allocation_common::terminal2robot_info::ConstPtr &_msg)
{
    //first callback, if all_robots is a null vector, wait the gazebo_msg to initilize the vector
    if(all_robots_.size()==0)
        return;

    terminal_info_.allocation_mode=_msg->allocation_mode;
    terminal_info_.greedorprobability=_msg->greedorprobability;
    terminal_info_.marketorprediction=_msg->marketorprediction;

    Allocation_robot_info _robot_info_tmp;
    //update the allocation_info of robots, exceput my_robot
    for(unsigned int i=0;i<_msg->all_allocation_robot_info.size();i++)
    {
        if(_msg->all_allocation_robot_info[i].isupdate)
        {
            if(_msg->all_allocation_robot_info[i].robot_ID!=my_robot_.allocation_robot_info.robot_ID)
            {
                _robot_info_tmp.robot_ID=_msg->all_allocation_robot_info[i].robot_ID;
                _robot_info_tmp.isvalid=_msg->all_allocation_robot_info[i].isvalid;
                _robot_info_tmp.robot_mode=_msg->all_allocation_robot_info[i].robot_mode;
                _robot_info_tmp.robot_power=_msg->all_allocation_robot_info[i].robot_power;
                _robot_info_tmp.which_target=_msg->all_allocation_robot_info[i].which_target;
                _robot_info_tmp.which_task=_msg->all_allocation_robot_info[i].which_task;
                _robot_info_tmp.expect_pos.x_=_msg->all_allocation_robot_info[i].expect_pos.position.x;
                _robot_info_tmp.expect_pos.y_=_msg->all_allocation_robot_info[i].expect_pos.position.y;


                all_robots_[_robot_info_tmp.robot_ID].allocation_robot_info=_robot_info_tmp;
            }
            else
            {
                //the robot_power is obtained from terminal, although the id is same, need to be update
                my_robot_.allocation_robot_info.robot_power=_msg->all_allocation_robot_info[i].robot_power;
                all_robots_[my_robot_.allocation_robot_info.robot_ID].allocation_robot_info=my_robot_.allocation_robot_info;
            }
        }
    }

    //first callback, if all_tasks is a null vector, wait the gazebo_msg to initilize the vector
    if(all_tasks_.size()==0)
        return;

    Allocation_task_info _task_info_tmp;
    int _num_task=0;
    int _num_target=0;
    //update the allocation_info of tasks
    for(unsigned int j=0;j<_msg->all_allocation_task_info.size();j++)
    {
        if(_msg->all_allocation_task_info[j].isupdate)
        {
            //obtain the current allocation_task_info, then the information will part update
            _task_info_tmp=all_tasks_[_msg->all_allocation_task_info[j].task_ID].allocation_task_info;
            if(terminal_info_.marketorprediction)
            {
                //the task/target has been allocated to me will not update these information
                if(_msg->all_allocation_task_info[j].task_ID!=my_robot_.allocation_robot_info.which_target&&_msg->all_allocation_task_info[j].task_ID!=my_robot_.allocation_robot_info.which_task)
                {
                    _task_info_tmp.task_ID=_msg->all_allocation_task_info[j].task_ID;
                    _task_info_tmp.current_distance=_msg->all_allocation_task_info[j].current_distance;
                    _task_info_tmp.iscomplete=_msg->all_allocation_task_info[j].iscomplete;
                    _task_info_tmp.isexplored=_msg->all_allocation_task_info[j].isexplored;
                    _task_info_tmp.istarget=_msg->all_allocation_task_info[j].istarget;
                    _task_info_tmp.known_power=_msg->all_allocation_task_info[j].known_power;

                    //std::cout<<_task_info_tmp.isexplored<<std::endl;
                    all_tasks_[_task_info_tmp.task_ID].allocation_task_info=_task_info_tmp;
                }
                //the task/target has been allocated to me will update task_power
                else
                {
                    _task_info_tmp.task_ID=_msg->all_allocation_task_info[j].task_ID;
                    _task_info_tmp.task_power=_msg->all_allocation_task_info[j].task_power;
                    all_tasks_[_task_info_tmp.task_ID].allocation_task_info=_task_info_tmp;
                }
            }

            else
            {
                bool _isMytask=false;
                bool _isMytarget=false;

                for(unsigned int m=0;m<my_robot_.allocation_robot_info.task_list.size();m++)
                    if(_msg->all_allocation_task_info[j].task_ID==my_robot_.allocation_robot_info.task_list[m])
                    {
                        _isMytask=true;
                        break;
                    }
                for(unsigned int n=0;n<my_robot_.allocation_robot_info.target_list.size();n++)
                    if(_msg->all_allocation_task_info[j].task_ID==my_robot_.allocation_robot_info.target_list[n])
                    {
                        _isMytarget=true;
                        break;
                    }
                //the task/target has been allocated to me will update task_power
                if(_isMytarget||_isMytask)
                {
                    _task_info_tmp.task_ID=_msg->all_allocation_task_info[j].task_ID;
                    _task_info_tmp.task_power=_msg->all_allocation_task_info[j].task_power;
                    all_tasks_[_task_info_tmp.task_ID].allocation_task_info=_task_info_tmp;
                }
                //the tasks/targets have been allocated to me will not update these information
                else
                {
                    _task_info_tmp.task_ID=_msg->all_allocation_task_info[j].task_ID;
                    _task_info_tmp.iscomplete=_msg->all_allocation_task_info[j].iscomplete;
                    _task_info_tmp.isexplored=_msg->all_allocation_task_info[j].isexplored;
                    _task_info_tmp.isallocated=_msg->all_allocation_task_info[j].isallocated;
                    _task_info_tmp.istarget=_msg->all_allocation_task_info[j].istarget;
                    _task_info_tmp.known_power=_msg->all_allocation_task_info[j].known_power;

                    all_tasks_[_task_info_tmp.task_ID].allocation_task_info=_task_info_tmp;
                }
            }
        }
    }
    //record the uncomplete task and target
    for(unsigned int i=0;i<all_tasks_.size();i++)
    {
        if(!all_tasks_[i].allocation_task_info.iscomplete&&all_tasks_[i].allocation_task_info.istarget)
            _num_target++;
        else if(!all_tasks_[i].allocation_task_info.isexplored)
            _num_task++;
    }
    num_task_valid_=_num_task;
    num_target_valid_=_num_target;
    //for market_base method, the number of unallocated tasks/targets will also be recorded
    if(!terminal_info_.marketorprediction)
    {
        _num_task=0;
        _num_target=0;
        for(unsigned int i=0;i<all_tasks_.size();i++)
            if(!all_tasks_[i].allocation_task_info.isallocated)
            {
                if(all_tasks_[i].allocation_task_info.istarget)
                    _num_target++;
                else
                    _num_task++;
            }
        num_task_unallocated_=_num_task;
        num_target_unallocated_=_num_target;
    }
}

/// \brief the process that robot to explore the task
bool Task_Allocation::try2explore_()
{
    static int _explore_time=0;
    Task_info _my_task;
    int _which_task;
    //the task which execute next is different between prediction and market
    if(terminal_info_.marketorprediction)
        _which_task=my_robot_.allocation_robot_info.which_task;
    else
        _which_task=my_robot_.allocation_robot_info.task_list.front();
    _my_task=all_tasks_[_which_task];

    if(terminal_info_.marketorprediction)
        for(unsigned int i=0;i<all_robots_.size();i++)
            //there is an other robot ready for this task except me
            if(all_robots_[i].allocation_robot_info.robot_ID!=my_robot_.allocation_robot_info.robot_ID
                    &&all_robots_[i].allocation_robot_info.which_task==my_robot_.allocation_robot_info.which_task
                    &&all_robots_[i].allocation_robot_info.isvalid)
            {
                int distance_1=my_robot_.gazebo_robot_info.robot_pos.distance(_my_task.gazebo_task_info.task_pos);
                int distance_2=all_robots_[i].gazebo_robot_info.robot_pos.distance(_my_task.gazebo_task_info.task_pos);
                //other robot's distance is littler
                if(distance_1>distance_2)
                {
                    //drop this task
                    my_robot_.allocation_robot_info.robot_mode=IDLE;
                    std::cout<<"robot"<<my_robot_.allocation_robot_info.robot_ID<<" drops task "<<my_robot_.allocation_robot_info.which_task<<" because of robot "<<i<<std::endl;
                    return false;
                }
            }

    my_robot_.allocation_robot_info.robot_mode=EXECUTE;
    //there is no conflict, continue to carry out the task
    if(my_robot_.gazebo_robot_info.robot_pos.distance(_my_task.gazebo_task_info.task_pos)>LOCATION_ERROR)
    {
        float thetaofr2t = _my_task.gazebo_task_info.task_pos.angle(my_robot_.gazebo_robot_info.robot_pos).radian_;
        my_behaviour_->move2Positionwithobs(3,6,_my_task.gazebo_task_info.task_pos,4,my_robot_.gazebo_robot_info.robot_pos,my_robot_.gazebo_robot_info.robot_ori);
        all_tasks_[_which_task].allocation_task_info.current_distance=my_robot_.gazebo_robot_info.robot_pos.distance(_my_task.gazebo_task_info.task_pos);
//        my_behaviour_->rotate2AbsOrienation(2,5,thetaofr2t,5,my_robot_.gazebo_robot_info.robot_ori);
        return false;
    }
    //if reach the task pos
    else
    {
        //explore the task
        my_robot_.allocation_robot_info.robot_mode=EXPLORE;
        if(_explore_time<EXPLORE_TIME)
        {
            _explore_time++;
            return false;
        }
        else
        {
            _explore_time=0;
            all_tasks_[_which_task].allocation_task_info.isexplored=true;
            //this is a target
            if(_my_task.allocation_task_info.task_power>0)
            {
                all_tasks_[_which_task].allocation_task_info.known_power=1;
                all_tasks_[_which_task].allocation_task_info.istarget=true;
                if(terminal_info_.marketorprediction)
                    all_tasks_[_which_task].allocation_task_info.current_distance=1000;
                else
                    all_tasks_[_which_task].allocation_task_info.isallocated=false;

                cout<<"this is a target"<<endl;
            }
            else
                all_tasks_[_which_task].allocation_task_info.iscomplete=true;

            return true;
        }
    }
}

/// \brief  the process that robot to hit the target
bool Task_Allocation::try2hit_()
{
    static int _hit_time=0;
    Task_info _my_target;
    int _which_target;
    //the target which execute next is different between prediction and market
    if(terminal_info_.marketorprediction)
        _which_target=my_robot_.allocation_robot_info.which_target;
    else
        _which_target=my_robot_.allocation_robot_info.target_list.front();
    _my_target=all_tasks_[_which_target];

    if(terminal_info_.marketorprediction)
        for(unsigned int i=0;i<all_robots_.size();i++)
            //there is an other robot ready for this target except me
            if(all_robots_[i].allocation_robot_info.robot_ID!=my_robot_.allocation_robot_info.robot_ID
                    &&all_robots_[i].allocation_robot_info.which_target==my_robot_.allocation_robot_info.which_target
                    &&all_robots_[i].allocation_robot_info.isvalid)
            {
                int distance_1=my_robot_.gazebo_robot_info.robot_pos.distance(_my_target.gazebo_task_info.task_pos);
                int distance_2=all_robots_[i].gazebo_robot_info.robot_pos.distance(_my_target.gazebo_task_info.task_pos);
                //other robot's distance is littler
                if(distance_1>distance_2)
                {
                    //drop this target
                    my_robot_.allocation_robot_info.robot_mode=IDLE;
                    std::cout<<"robot"<<my_robot_.allocation_robot_info.robot_ID<<" drops target "<<my_robot_.allocation_robot_info.which_target<<" because of robot "<<i<<std::endl;
                    return false;
                }
            }

    my_robot_.allocation_robot_info.robot_mode=EXECUTE;
    //there is no conflict, continue to carry out this target
    if(my_robot_.gazebo_robot_info.robot_pos.distance(_my_target.gazebo_task_info.task_pos)>LOCATION_ERROR)
    {
        float thetaofr2t = _my_target.gazebo_task_info.task_pos.angle(my_robot_.gazebo_robot_info.robot_pos).radian_;
        my_behaviour_->move2Positionwithobs(3,6,_my_target.gazebo_task_info.task_pos,4,my_robot_.gazebo_robot_info.robot_pos,my_robot_.gazebo_robot_info.robot_ori);
        all_tasks_[_which_target].allocation_task_info.current_distance=my_robot_.gazebo_robot_info.robot_pos.distance(_my_target.gazebo_task_info.task_pos);
//        my_behaviour_->rotate2AbsOrienation(2,5,thetaofr2t,5,my_robot_.gazebo_robot_info.robot_ori);
        return false;
    }
    //reach the target position
    else
    {
        //if the target power bigger than the robot power, the robot will be destroyed
        if(_my_target.allocation_task_info.task_power>my_robot_.allocation_robot_info.robot_power)
        {
            my_robot_.allocation_robot_info.robot_mode=DAMAGE;
            my_robot_.allocation_robot_info.isvalid=false;
            all_tasks_[_which_target].allocation_task_info.known_power=my_robot_.allocation_robot_info.robot_power+1;

            if(terminal_info_.marketorprediction)
                all_tasks_[_which_target].allocation_task_info.current_distance=1000;
            else
                all_tasks_[_which_target].allocation_task_info.isallocated=false;

            return false;
        }
        //if the target power less than the robot power, the target will be destroyed
        else
        {
            //the target is complete
            my_robot_.allocation_robot_info.robot_mode=HIT;
            if(_hit_time<HIT_TIME)
            {
                _hit_time++;
                return false;
            }
            else
            {
                _hit_time=0;
                all_tasks_[_which_target].allocation_task_info.iscomplete=true;
                return true;
            }
        }
    }
}

/// \brief the robot choose the task (hit or explore)
bool Task_Allocation::choose2hitOrexplore_()
{
    //initialization the variable
    std::vector<double> distance_vec;
    std::vector<int> possible_vec;
    std::vector<int> power_vec;
    std::vector<int> lab_vec;
    float distance=0;
    int sum_possible=0;
    int _which_target=-1;
    int _which_task=-1;

    if(!num_target_valid_&&!num_task_valid_)
        return false;

    //there are targets which uncomplete
    if(num_target_valid_>0)
    {
        //make choice base on greed or probability
        if(!terminal_info_.greedorprobability)
        {
            float less_distance=1000;
            //calculate the distance between my_robot and each uncomplete target
            for(unsigned int i=0;i<all_tasks_.size();i++)
                if(all_tasks_[i].allocation_task_info.istarget==true&&all_tasks_[i].allocation_task_info.iscomplete==false
                   &&my_robot_.allocation_robot_info.robot_power>=all_tasks_[i].allocation_task_info.known_power
                   &&my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[i].gazebo_task_info.task_pos)<all_tasks_[i].allocation_task_info.current_distance)
                {
                    distance=my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[i].gazebo_task_info.task_pos);
                    if(distance<less_distance)
                    {
                        less_distance=distance;
                        my_robot_.allocation_robot_info.which_target=all_tasks_[i].allocation_task_info.task_ID;
                    }
                }
            if(my_robot_.allocation_robot_info.which_target!=-1)
            {
                my_robot_.allocation_robot_info.robot_mode=PLAN;
                _which_target=my_robot_.allocation_robot_info.which_target;
                all_tasks_[_which_target].allocation_task_info.current_distance=my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[_which_target].gazebo_task_info.task_pos);
                std::cout<<"robot"<<my_robot_.allocation_robot_info.robot_ID<<" selects target "<<_which_target<<std::endl;
                return true;
            }
        }
        else
        {
            for(unsigned int i=0;i<all_tasks_.size();i++)
                if(all_tasks_[i].allocation_task_info.istarget==true&&all_tasks_[i].allocation_task_info.iscomplete==false
                   &&my_robot_.allocation_robot_info.robot_power>=all_tasks_[i].allocation_task_info.known_power
                   &&my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[i].gazebo_task_info.task_pos)<all_tasks_[i].allocation_task_info.current_distance-3*LOCATION_ERROR)
                {
                    //cout<<all_tasks_[i].allocation_task_info.task_ID<<" "<<all_tasks_[i].allocation_task_info.current_distance<<endl;
                    distance=my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[i].gazebo_task_info.task_pos);
                    distance_vec.push_back(distance);
                    lab_vec.push_back(i);
                }
            //choose the target base on the probability
            if(distance_vec.size()==0)
                std::cout<<"There is not applicable target for Robot"<<my_robot_.allocation_robot_info.robot_ID<<std::endl;
            else
                for(unsigned int i=0;i<distance_vec.size();i++)
                {
                    if(distance_vec[i]!=0)
                        sum_possible=sum_possible+1000/distance_vec[i];
                    else
                    {
                        my_robot_.allocation_robot_info.which_target=all_tasks_[lab_vec[i]].allocation_task_info.task_ID;
                        my_robot_.allocation_robot_info.robot_mode=PLAN;
                        return true;
                    }
                    possible_vec.push_back(sum_possible);
                }
            if(possible_vec.size()>0)
            {
                srand(clock());
                int tmp=rand()%(sum_possible);
                for(unsigned int i=0;i<possible_vec.size();i++)
                    if(tmp<possible_vec[i])
                    {
                        my_robot_.allocation_robot_info.which_target=all_tasks_[lab_vec[i]].allocation_task_info.task_ID;
                        my_robot_.allocation_robot_info.robot_mode=PLAN;
                        _which_target=my_robot_.allocation_robot_info.which_target;
                        all_tasks_[_which_target].allocation_task_info.current_distance=my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[_which_target].gazebo_task_info.task_pos);
                        std::cout<<"robot"<<my_robot_.allocation_robot_info.robot_ID<<" selects target "<<_which_target<<std::endl;
                        return true;
                    }
            }
        }
    }

    //if do not find the target, reset the reriable for task selecting
    distance_vec.clear();
    possible_vec.clear();
    power_vec.clear();
    lab_vec.clear();
    distance=0;
    sum_possible=0;
    //there is no available target, but there are tasks uncomplete
    if(num_task_valid_>0)
    {
        if(!terminal_info_.greedorprobability)
        {
            float less_distance2=1000;
            //choose the nearest region for exploration
            for(unsigned int i=0;i<all_tasks_.size();i++)
                if(all_tasks_[i].allocation_task_info.istarget==false&&all_tasks_[i].allocation_task_info.isexplored==false
                   &&my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[i].gazebo_task_info.task_pos)<all_tasks_[i].allocation_task_info.current_distance)
                {
                    distance=my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[i].gazebo_task_info.task_pos);
                    if(distance<less_distance2)
                    {
                        less_distance2=distance;
                        my_robot_.allocation_robot_info.which_task=all_tasks_[i].allocation_task_info.task_ID;
                    }
                }
            if(my_robot_.allocation_robot_info.which_task!=-1)
            {
                my_robot_.allocation_robot_info.robot_mode=PLAN;
                _which_task=my_robot_.allocation_robot_info.which_task;
                all_tasks_[_which_task].allocation_task_info.current_distance=my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[_which_task].gazebo_task_info.task_pos);
                std::cout<<"robot"<<my_robot_.allocation_robot_info.robot_ID<<" selects task "<<_which_task<<std::endl;
                return true;
            }
        }
        else
        {
            for(unsigned int i=0;i<all_tasks_.size();i++)
                if(all_tasks_[i].allocation_task_info.istarget==false&&all_tasks_[i].allocation_task_info.isexplored==false
                   &&my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[i].gazebo_task_info.task_pos)<all_tasks_[i].allocation_task_info.current_distance-3*LOCATION_ERROR)
                {
                    distance=my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[i].gazebo_task_info.task_pos);
                    distance_vec.push_back(distance);
                    lab_vec.push_back(i);
                }
            //choose the task base on the probability
            if(distance_vec.size()==0)
                std::cout<<"There is not applicable task for Robot"<<my_robot_.allocation_robot_info.robot_ID<<std::endl;
            else
                for(unsigned int i=0;i<distance_vec.size();i++)
                {
                    if(distance_vec[i]!=0)
                        sum_possible=sum_possible+1000/distance_vec[i];
                    else
                    {
                        my_robot_.allocation_robot_info.which_task=all_tasks_[lab_vec[i]].allocation_task_info.task_ID;
                        my_robot_.allocation_robot_info.robot_mode=PLAN;
                        return true;
                    }
                    possible_vec.push_back(sum_possible);
                }
            if(possible_vec.size()>0)
            {
                srand(clock());
                int tmp=rand()%(sum_possible);
                for(unsigned int i=0;i<possible_vec.size();i++)
                    if(tmp<possible_vec[i])
                    {
//                        std::cout<<lab_vec[i]<<": "<<all_tasks_[lab_vec[i]].allocation_task_info.task_ID<<std::endl;
                        my_robot_.allocation_robot_info.which_task=all_tasks_[lab_vec[i]].allocation_task_info.task_ID;
                        my_robot_.allocation_robot_info.robot_mode=PLAN;
                        _which_task=my_robot_.allocation_robot_info.which_task;
                        all_tasks_[_which_task].allocation_task_info.current_distance=my_robot_.gazebo_robot_info.robot_pos.distance(all_tasks_[_which_task].gazebo_task_info.task_pos);
                        std::cout<<"robot"<<my_robot_.allocation_robot_info.robot_ID<<" selects task "<<_which_task<<std::endl;
                        return true;
                    }
            }
        }
    }
    return true;
}

/// \brief choose one robot to execute for avalid targets
bool Task_Allocation::which2hit_()
{
    if(!num_target_unallocated_)
        return false;

    float less_distance,distance;
    //choose the nearest robot for every unallocated target
    for(unsigned int i=0;i<all_tasks_.size();i++)
        if(all_tasks_[i].allocation_task_info.istarget==true&&all_tasks_[i].allocation_task_info.iscomplete==false&&all_tasks_[i].allocation_task_info.isallocated==false)
        {
            less_distance=my_robot_.allocation_robot_info.expect_pos.distance(all_tasks_[i].gazebo_task_info.task_pos);
            for(unsigned int j=0;j<all_robots_.size();j++)
            {
                if(all_robots_[j].allocation_robot_info.isvalid && all_robots_[j].allocation_robot_info.robot_ID!=my_robot_.allocation_robot_info.robot_ID)
                {
                    distance=all_robots_[j].allocation_robot_info.expect_pos.distance(all_tasks_[i].gazebo_task_info.task_pos);
                    //if there is some robot near than me, this target will not allocated to me
                    if(distance<less_distance||(distance==less_distance&&my_robot_.allocation_robot_info.robot_ID>all_robots_[j].allocation_robot_info.robot_ID))
                        return false;
                }
                //i am the nearest robot and win the auction
                if(j==all_robots_.size()-1)
                {
                    my_robot_.allocation_robot_info.target_list.push_back(all_tasks_[i].allocation_task_info.task_ID);
                    my_robot_.allocation_robot_info.expect_pos=all_tasks_[i].gazebo_task_info.task_pos;
                    all_tasks_[i].allocation_task_info.isallocated=true;
                    return true;
                }
            }
        }
    //no target has been added to the list
    return false;
}

/// \brief choose one robot to execute for avalid tasks
bool Task_Allocation::which2explore_()
{
    if(!num_task_unallocated_)
        return false;

    float less_distance,distance;
    //choose the nearest robot for every unallocated task
    for(unsigned int i=0;i<all_tasks_.size();i++)
        if(all_tasks_[i].allocation_task_info.istarget==false&&all_tasks_[i].allocation_task_info.isexplored==false&&all_tasks_[i].allocation_task_info.isallocated==false)
        {
            less_distance=my_robot_.allocation_robot_info.expect_pos.distance(all_tasks_[i].gazebo_task_info.task_pos);
            for(unsigned int j=0;j<all_robots_.size();j++)
            {
                if(all_robots_[j].allocation_robot_info.isvalid && all_robots_[j].allocation_robot_info.robot_ID!=my_robot_.allocation_robot_info.robot_ID)
                {
                    distance=all_robots_[j].allocation_robot_info.expect_pos.distance(all_tasks_[i].gazebo_task_info.task_pos);
                    //if there is some robot near than me, this target will not allocated to me
                    if(distance<less_distance||(distance==less_distance&&my_robot_.allocation_robot_info.robot_ID>all_robots_[j].allocation_robot_info.robot_ID))
                        return false;
                }
                //i am the nearest robot and win the auction
                if(j==all_robots_.size()-1)
                {
                    my_robot_.allocation_robot_info.task_list.push_back(all_tasks_[i].allocation_task_info.task_ID);
                    my_robot_.allocation_robot_info.expect_pos=all_tasks_[i].gazebo_task_info.task_pos;
                    all_tasks_[i].allocation_task_info.isallocated=true;
                    return true;
                }
            }
        }
    //no task has been added to the list
    return false;
}

/// \brief control cycle
void Task_Allocation::loopControl(const ros::TimerEvent &event)
{
    //reset some flag
    bid_new_task_=false;
    bid_new_target_=false;
    is_target_completed=false;
    is_task_explored=false;

    if(terminal_info_.allocation_mode==ALLOCATION_STOP)
        stopAllocation();
    else if(terminal_info_.allocation_mode==ALLOCATION_PAUSE)
        pauseAllocation();
    //using prediction method to complete the allocation
    else if(terminal_info_.allocation_mode==ALLOCATION_START&&terminal_info_.marketorprediction)
    {
        if(!my_robot_.allocation_robot_info.isvalid)
        {
            //if this robot is inviald, stop the loopcontrol
            std::cout<<"Robot"<<my_robot_.allocation_robot_info.robot_ID<<" is destroyed!"<<std::endl;
            allocation_timer_.stop();
            return;
        }
        else
        {
            //robot has complete the all task which allocated to it
            if(my_robot_.allocation_robot_info.which_target==-1&&my_robot_.allocation_robot_info.which_task==-1)
            {
                if(!choose2hitOrexplore_())
                {
                    //all tasks have been completed
                    std::cout<<"All tasks have been completed!"<<std::endl;
                    //allocation_timer_.stop();
                }
            }
            //the allocated target is not completed
            else if(my_robot_.allocation_robot_info.which_target!=-1)
            {
                is_target_completed=try2hit_();
                if(is_target_completed)
                    std::cout<<"Robot"<<my_robot_.allocation_robot_info.robot_ID<<" destroy the target "<<my_robot_.allocation_robot_info.which_target<<std::endl;
            }
            //the allocated task is not explored
            else if(my_robot_.allocation_robot_info.which_task!=-1)
            {
                is_task_explored=try2explore_();
                if(is_task_explored)
                    std::cout<<"Robot"<<my_robot_.allocation_robot_info.robot_ID<<" explore the task "<<my_robot_.allocation_robot_info.which_task<<std::endl;
            }
        }
        pubAllocation_info();

        //if the task or target is done or dropped, clear the task or target
        if((my_robot_.allocation_robot_info.robot_mode==EXPLORE&&is_task_explored)||my_robot_.allocation_robot_info.robot_mode==IDLE)
        {
            if(my_robot_.allocation_robot_info.robot_mode==EXPLORE&&is_task_explored)
                my_robot_.allocation_robot_info.robot_mode=IDLE;
            my_robot_.allocation_robot_info.which_task=-1;
        }
        if((my_robot_.allocation_robot_info.robot_mode==HIT&&is_target_completed)||my_robot_.allocation_robot_info.robot_mode==DAMAGE||my_robot_.allocation_robot_info.robot_mode==IDLE)
        {
            if(my_robot_.allocation_robot_info.robot_mode==HIT&&is_target_completed)
                my_robot_.allocation_robot_info.robot_mode=IDLE;
            my_robot_.allocation_robot_info.which_target=-1;
        }
    }
    //using market-base method to complete the allocation
    else if(terminal_info_.allocation_mode==ALLOCATION_START&&!terminal_info_.marketorprediction)
    {
        if(!my_robot_.allocation_robot_info.isvalid)
        {
            //if this robot is inviald, stop the loopcontrol
            std::cout<<"Robot"<<my_robot_.allocation_robot_info.robot_ID<<" is destroyed!"<<std::endl;
            allocation_timer_.stop();
            return;
        }
        //there are residual task/target that unallocated
        else if(num_target_unallocated_!=0)
        {
            my_robot_.allocation_robot_info.robot_mode=PLAN;
            bid_new_target_=which2hit_();
            if(bid_new_target_)
            {
                pubAllocation_info();
                std::cout<<"Robot"<<my_robot_.allocation_robot_info.robot_ID<<" bid a new target "<<my_robot_.allocation_robot_info.target_list.back()<<std::endl;
            }
        }
        else if(num_task_unallocated_!=0)
        {
            my_robot_.allocation_robot_info.robot_mode=PLAN;
            bid_new_task_=which2explore_();
            if(bid_new_task_)
            {
                pubAllocation_info();
                std::cout<<"Robot"<<my_robot_.allocation_robot_info.robot_ID<<" bid a new task "<<my_robot_.allocation_robot_info.task_list.back()<<std::endl;
            }
        }
        //the allocated target is not complete
        else if(my_robot_.allocation_robot_info.target_list.size()!=0)
        {
            is_target_completed=try2hit_();
            pubAllocation_info();
            if(is_target_completed)
            {
                //the target has been completed, delete it from the target_list
                std::cout<<"Robot"<<my_robot_.allocation_robot_info.robot_ID<<" destroy the target "<<my_robot_.allocation_robot_info.target_list.front()<<std::endl;
                my_robot_.allocation_robot_info.target_list.erase(my_robot_.allocation_robot_info.target_list.begin());
            }
        }
        else if(my_robot_.allocation_robot_info.task_list.size()!=0)
        {
            is_task_explored=try2explore_();
            pubAllocation_info();
            if(is_task_explored)
            {
                //the task has been explored, delete it from the task_list
                std::cout<<"Robot"<<my_robot_.allocation_robot_info.robot_ID<<" explore the task "<<my_robot_.allocation_robot_info.task_list.front()<<std::endl;
                my_robot_.allocation_robot_info.task_list.erase(my_robot_.allocation_robot_info.task_list.begin());
            }
        }
        else if(!num_target_valid_&&!num_task_valid_)
            std::cout<<"All tasks have been completed!"<<std::endl;
    }
    setVelCommond();
}

/// \brief pub the robot velocity, control the model in the gazebo
void Task_Allocation::setVelCommond()
{
    allocation_common::robot2gazebo_info _robot2gazebo_info;

    _robot2gazebo_info.robot_twist.linear.x=my_behaviour_->app_vx_;
    _robot2gazebo_info.robot_twist.linear.y=my_behaviour_->app_vy_;
    _robot2gazebo_info.robot_twist.angular=my_behaviour_->app_w_;
    _robot2gazebo_info.current_mode=my_robot_.allocation_robot_info.robot_mode;

    //clear velocity
    my_behaviour_->app_vx_=0;
    my_behaviour_->app_vy_=0;
    my_behaviour_->app_w_=0;

    robot2gazebo_pub_.publish(_robot2gazebo_info);
}

/// \brief pub the allocation_info include the robot_info and the task_info
void Task_Allocation::pubAllocation_info()
{
    allocation_common::allocation2terminal_info _allocation_info;
    int _which_target=-1;
    int _which_task=-1;

    _allocation_info.robot_info.robot_ID=my_robot_.allocation_robot_info.robot_ID;
    _allocation_info.robot_info.robot_mode=my_robot_.allocation_robot_info.robot_mode;
    _allocation_info.robot_info.isvalid=my_robot_.allocation_robot_info.isvalid;
    _allocation_info.robot_info.move_distance=my_robot_.allocation_robot_info.move_distance;
    if(terminal_info_.marketorprediction)
    {
        _allocation_info.robot_info.which_task=my_robot_.allocation_robot_info.which_task;
        _allocation_info.robot_info.which_target=my_robot_.allocation_robot_info.which_target;
        _which_target=my_robot_.allocation_robot_info.which_target;
        _which_task=my_robot_.allocation_robot_info.which_task;
    }
    else
    {
        _allocation_info.robot_info.expect_pos.position.x=my_robot_.allocation_robot_info.expect_pos.x_;
        _allocation_info.robot_info.expect_pos.position.y=my_robot_.allocation_robot_info.expect_pos.y_;
        if(bid_new_target_)
            _which_target=my_robot_.allocation_robot_info.target_list.back();
        else if(is_target_completed)
            _which_target=my_robot_.allocation_robot_info.target_list.front();
        else if(bid_new_task_)
            _which_task=my_robot_.allocation_robot_info.task_list.back();
        else if(is_task_explored)
            _which_task=my_robot_.allocation_robot_info.task_list.front();
    }

    if(_which_target!=-1)
    {
        Allocation_task_info _target_info=all_tasks_[_which_target].allocation_task_info;

        _allocation_info.task_info.task_ID=_target_info.task_ID;
        _allocation_info.task_info.current_distance=_target_info.current_distance;
        _allocation_info.task_info.task_ID=_target_info.task_ID;
        _allocation_info.task_info.known_power=_target_info.known_power;
        _allocation_info.task_info.istarget=_target_info.istarget;
        _allocation_info.task_info.iscomplete=_target_info.iscomplete;
        _allocation_info.task_info.isexplored=_target_info.isexplored;
        _allocation_info.task_info.isallocated=_target_info.isallocated;
    }
    else if(_which_task!=-1)
    {
        Allocation_task_info _task_info=all_tasks_[_which_task].allocation_task_info;

        _allocation_info.task_info.task_ID=_task_info.task_ID;
        _allocation_info.task_info.current_distance=_task_info.current_distance;
        _allocation_info.task_info.task_ID=_task_info.task_ID;
        _allocation_info.task_info.known_power=_task_info.known_power;
        _allocation_info.task_info.istarget=_task_info.istarget;
        _allocation_info.task_info.iscomplete=_task_info.iscomplete;
        _allocation_info.task_info.isexplored=_task_info.isexplored;
        _allocation_info.task_info.isallocated=_task_info.isallocated;
    }
    else
        _allocation_info.task_info.task_ID=-1;

    allocation2terminal_pub_.publish(_allocation_info);
}

/// \brief when the controlterminal click "PAUSE", clear the robot velocity
void Task_Allocation::pauseAllocation()
{
    //clear velocity
    my_behaviour_->app_vx_=0;
    my_behaviour_->app_vy_=0;
    my_behaviour_->app_w_=0;
}

/// \brief when the controlterminal click "STOP", reset all robots and tasks
void Task_Allocation::stopAllocation()
{
    //clear velocity
    my_behaviour_->app_vx_=0;
    my_behaviour_->app_vy_=0;
    my_behaviour_->app_w_=0;

    //reset
    num_task_valid_=0;
    num_target_valid_=0;
    num_task_unallocated_=0;
    num_target_unallocated_=0;
    is_world_update_=false;
    bid_new_task_=false;
    bid_new_target_=false;
    is_target_completed=false;
    is_task_explored=false;
    all_tasks_.clear();
    all_robots_.clear();

    my_robot_.allocation_robot_info.robot_reset();
}
