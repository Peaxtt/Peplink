// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from pttep_alignment:srv/CalculateTransformation.idl
// generated code does not contain a copyright notice

#ifndef PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__BUILDER_HPP_
#define PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "pttep_alignment/srv/detail/calculate_transformation__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace pttep_alignment
{

namespace srv
{

namespace builder
{

class Init_CalculateTransformation_Request_reset
{
public:
  Init_CalculateTransformation_Request_reset()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::pttep_alignment::srv::CalculateTransformation_Request reset(::pttep_alignment::srv::CalculateTransformation_Request::_reset_type arg)
  {
    msg_.reset = std::move(arg);
    return std::move(msg_);
  }

private:
  ::pttep_alignment::srv::CalculateTransformation_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::pttep_alignment::srv::CalculateTransformation_Request>()
{
  return pttep_alignment::srv::builder::Init_CalculateTransformation_Request_reset();
}

}  // namespace pttep_alignment


namespace pttep_alignment
{

namespace srv
{

namespace builder
{

class Init_CalculateTransformation_Response_transform
{
public:
  explicit Init_CalculateTransformation_Response_transform(::pttep_alignment::srv::CalculateTransformation_Response & msg)
  : msg_(msg)
  {}
  ::pttep_alignment::srv::CalculateTransformation_Response transform(::pttep_alignment::srv::CalculateTransformation_Response::_transform_type arg)
  {
    msg_.transform = std::move(arg);
    return std::move(msg_);
  }

private:
  ::pttep_alignment::srv::CalculateTransformation_Response msg_;
};

class Init_CalculateTransformation_Response_message
{
public:
  explicit Init_CalculateTransformation_Response_message(::pttep_alignment::srv::CalculateTransformation_Response & msg)
  : msg_(msg)
  {}
  Init_CalculateTransformation_Response_transform message(::pttep_alignment::srv::CalculateTransformation_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return Init_CalculateTransformation_Response_transform(msg_);
  }

private:
  ::pttep_alignment::srv::CalculateTransformation_Response msg_;
};

class Init_CalculateTransformation_Response_success
{
public:
  Init_CalculateTransformation_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_CalculateTransformation_Response_message success(::pttep_alignment::srv::CalculateTransformation_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_CalculateTransformation_Response_message(msg_);
  }

private:
  ::pttep_alignment::srv::CalculateTransformation_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::pttep_alignment::srv::CalculateTransformation_Response>()
{
  return pttep_alignment::srv::builder::Init_CalculateTransformation_Response_success();
}

}  // namespace pttep_alignment

#endif  // PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__BUILDER_HPP_
