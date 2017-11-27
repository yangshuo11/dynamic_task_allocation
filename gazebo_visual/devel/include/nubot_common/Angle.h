// Generated by gencpp from file nubot_common/Angle.msg
// DO NOT EDIT!


#ifndef NUBOT_COMMON_MESSAGE_ANGLE_H
#define NUBOT_COMMON_MESSAGE_ANGLE_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace nubot_common
{
template <class ContainerAllocator>
struct Angle_
{
  typedef Angle_<ContainerAllocator> Type;

  Angle_()
    : theta(0.0)  {
    }
  Angle_(const ContainerAllocator& _alloc)
    : theta(0.0)  {
  (void)_alloc;
    }



   typedef float _theta_type;
  _theta_type theta;




  typedef boost::shared_ptr< ::nubot_common::Angle_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::nubot_common::Angle_<ContainerAllocator> const> ConstPtr;

}; // struct Angle_

typedef ::nubot_common::Angle_<std::allocator<void> > Angle;

typedef boost::shared_ptr< ::nubot_common::Angle > AnglePtr;
typedef boost::shared_ptr< ::nubot_common::Angle const> AngleConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::nubot_common::Angle_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::nubot_common::Angle_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace nubot_common

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': True, 'IsMessage': True, 'HasHeader': False}
// {'nubot_common': ['/home/nubot8/dynamic_allocation/gazebo_visual/src/nubot_common/msgs'], 'std_msgs': ['/opt/ros/kinetic/share/std_msgs/cmake/../msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::nubot_common::Angle_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::nubot_common::Angle_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::nubot_common::Angle_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::nubot_common::Angle_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::nubot_common::Angle_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::nubot_common::Angle_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::nubot_common::Angle_<ContainerAllocator> >
{
  static const char* value()
  {
    return "7328b114b91e2487fcf12e1581a80145";
  }

  static const char* value(const ::nubot_common::Angle_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x7328b114b91e2487ULL;
  static const uint64_t static_value2 = 0xfcf12e1581a80145ULL;
};

template<class ContainerAllocator>
struct DataType< ::nubot_common::Angle_<ContainerAllocator> >
{
  static const char* value()
  {
    return "nubot_common/Angle";
  }

  static const char* value(const ::nubot_common::Angle_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::nubot_common::Angle_<ContainerAllocator> >
{
  static const char* value()
  {
    return "float32 theta\n\
";
  }

  static const char* value(const ::nubot_common::Angle_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::nubot_common::Angle_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.theta);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct Angle_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::nubot_common::Angle_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::nubot_common::Angle_<ContainerAllocator>& v)
  {
    s << indent << "theta: ";
    Printer<float>::stream(s, indent + "  ", v.theta);
  }
};

} // namespace message_operations
} // namespace ros

#endif // NUBOT_COMMON_MESSAGE_ANGLE_H
