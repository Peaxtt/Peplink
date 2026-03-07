// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from pttep_alignment:srv/CalculateTransformation.idl
// generated code does not contain a copyright notice

#ifndef PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__STRUCT_HPP_
#define PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__pttep_alignment__srv__CalculateTransformation_Request __attribute__((deprecated))
#else
# define DEPRECATED__pttep_alignment__srv__CalculateTransformation_Request __declspec(deprecated)
#endif

namespace pttep_alignment
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct CalculateTransformation_Request_
{
  using Type = CalculateTransformation_Request_<ContainerAllocator>;

  explicit CalculateTransformation_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->reset = false;
    }
  }

  explicit CalculateTransformation_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->reset = false;
    }
  }

  // field types and members
  using _reset_type =
    bool;
  _reset_type reset;

  // setters for named parameter idiom
  Type & set__reset(
    const bool & _arg)
  {
    this->reset = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__pttep_alignment__srv__CalculateTransformation_Request
    std::shared_ptr<pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__pttep_alignment__srv__CalculateTransformation_Request
    std::shared_ptr<pttep_alignment::srv::CalculateTransformation_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const CalculateTransformation_Request_ & other) const
  {
    if (this->reset != other.reset) {
      return false;
    }
    return true;
  }
  bool operator!=(const CalculateTransformation_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct CalculateTransformation_Request_

// alias to use template instance with default allocator
using CalculateTransformation_Request =
  pttep_alignment::srv::CalculateTransformation_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace pttep_alignment


// Include directives for member types
// Member 'transform'
#include "geometry_msgs/msg/detail/transform_stamped__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__pttep_alignment__srv__CalculateTransformation_Response __attribute__((deprecated))
#else
# define DEPRECATED__pttep_alignment__srv__CalculateTransformation_Response __declspec(deprecated)
#endif

namespace pttep_alignment
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct CalculateTransformation_Response_
{
  using Type = CalculateTransformation_Response_<ContainerAllocator>;

  explicit CalculateTransformation_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : transform(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
      this->message = "";
    }
  }

  explicit CalculateTransformation_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : message(_alloc),
    transform(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
      this->message = "";
    }
  }

  // field types and members
  using _success_type =
    bool;
  _success_type success;
  using _message_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _message_type message;
  using _transform_type =
    geometry_msgs::msg::TransformStamped_<ContainerAllocator>;
  _transform_type transform;

  // setters for named parameter idiom
  Type & set__success(
    const bool & _arg)
  {
    this->success = _arg;
    return *this;
  }
  Type & set__message(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->message = _arg;
    return *this;
  }
  Type & set__transform(
    const geometry_msgs::msg::TransformStamped_<ContainerAllocator> & _arg)
  {
    this->transform = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__pttep_alignment__srv__CalculateTransformation_Response
    std::shared_ptr<pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__pttep_alignment__srv__CalculateTransformation_Response
    std::shared_ptr<pttep_alignment::srv::CalculateTransformation_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const CalculateTransformation_Response_ & other) const
  {
    if (this->success != other.success) {
      return false;
    }
    if (this->message != other.message) {
      return false;
    }
    if (this->transform != other.transform) {
      return false;
    }
    return true;
  }
  bool operator!=(const CalculateTransformation_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct CalculateTransformation_Response_

// alias to use template instance with default allocator
using CalculateTransformation_Response =
  pttep_alignment::srv::CalculateTransformation_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace pttep_alignment

namespace pttep_alignment
{

namespace srv
{

struct CalculateTransformation
{
  using Request = pttep_alignment::srv::CalculateTransformation_Request;
  using Response = pttep_alignment::srv::CalculateTransformation_Response;
};

}  // namespace srv

}  // namespace pttep_alignment

#endif  // PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__STRUCT_HPP_
