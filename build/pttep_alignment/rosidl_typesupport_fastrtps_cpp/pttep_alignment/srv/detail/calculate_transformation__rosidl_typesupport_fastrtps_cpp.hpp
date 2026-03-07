// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__rosidl_typesupport_fastrtps_cpp.hpp.em
// with input from pttep_alignment:srv/CalculateTransformation.idl
// generated code does not contain a copyright notice

#ifndef PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
#define PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_

#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "pttep_alignment/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"
#include "pttep_alignment/srv/detail/calculate_transformation__struct.hpp"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

#include "fastcdr/Cdr.h"

namespace pttep_alignment
{

namespace srv
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
cdr_serialize(
  const pttep_alignment::srv::CalculateTransformation_Request & ros_message,
  eprosima::fastcdr::Cdr & cdr);

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  pttep_alignment::srv::CalculateTransformation_Request & ros_message);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
get_serialized_size(
  const pttep_alignment::srv::CalculateTransformation_Request & ros_message,
  size_t current_alignment);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
max_serialized_size_CalculateTransformation_Request(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

}  // namespace typesupport_fastrtps_cpp

}  // namespace srv

}  // namespace pttep_alignment

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, pttep_alignment, srv, CalculateTransformation_Request)();

#ifdef __cplusplus
}
#endif

// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "pttep_alignment/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"
// already included above
// #include "pttep_alignment/srv/detail/calculate_transformation__struct.hpp"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// already included above
// #include "fastcdr/Cdr.h"

namespace pttep_alignment
{

namespace srv
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
cdr_serialize(
  const pttep_alignment::srv::CalculateTransformation_Response & ros_message,
  eprosima::fastcdr::Cdr & cdr);

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  pttep_alignment::srv::CalculateTransformation_Response & ros_message);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
get_serialized_size(
  const pttep_alignment::srv::CalculateTransformation_Response & ros_message,
  size_t current_alignment);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
max_serialized_size_CalculateTransformation_Response(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

}  // namespace typesupport_fastrtps_cpp

}  // namespace srv

}  // namespace pttep_alignment

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, pttep_alignment, srv, CalculateTransformation_Response)();

#ifdef __cplusplus
}
#endif

#include "rmw/types.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "pttep_alignment/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_pttep_alignment
const rosidl_service_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, pttep_alignment, srv, CalculateTransformation)();

#ifdef __cplusplus
}
#endif

#endif  // PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
