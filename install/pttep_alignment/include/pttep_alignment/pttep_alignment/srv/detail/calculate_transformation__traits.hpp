// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from pttep_alignment:srv/CalculateTransformation.idl
// generated code does not contain a copyright notice

#ifndef PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__TRAITS_HPP_
#define PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "pttep_alignment/srv/detail/calculate_transformation__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace pttep_alignment
{

namespace srv
{

inline void to_flow_style_yaml(
  const CalculateTransformation_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: reset
  {
    out << "reset: ";
    rosidl_generator_traits::value_to_yaml(msg.reset, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const CalculateTransformation_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: reset
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "reset: ";
    rosidl_generator_traits::value_to_yaml(msg.reset, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const CalculateTransformation_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace pttep_alignment

namespace rosidl_generator_traits
{

[[deprecated("use pttep_alignment::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const pttep_alignment::srv::CalculateTransformation_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  pttep_alignment::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use pttep_alignment::srv::to_yaml() instead")]]
inline std::string to_yaml(const pttep_alignment::srv::CalculateTransformation_Request & msg)
{
  return pttep_alignment::srv::to_yaml(msg);
}

template<>
inline const char * data_type<pttep_alignment::srv::CalculateTransformation_Request>()
{
  return "pttep_alignment::srv::CalculateTransformation_Request";
}

template<>
inline const char * name<pttep_alignment::srv::CalculateTransformation_Request>()
{
  return "pttep_alignment/srv/CalculateTransformation_Request";
}

template<>
struct has_fixed_size<pttep_alignment::srv::CalculateTransformation_Request>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<pttep_alignment::srv::CalculateTransformation_Request>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<pttep_alignment::srv::CalculateTransformation_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'transform'
#include "geometry_msgs/msg/detail/transform_stamped__traits.hpp"

namespace pttep_alignment
{

namespace srv
{

inline void to_flow_style_yaml(
  const CalculateTransformation_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << ", ";
  }

  // member: message
  {
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
    out << ", ";
  }

  // member: transform
  {
    out << "transform: ";
    to_flow_style_yaml(msg.transform, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const CalculateTransformation_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }

  // member: message
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
    out << "\n";
  }

  // member: transform
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "transform:\n";
    to_block_style_yaml(msg.transform, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const CalculateTransformation_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace pttep_alignment

namespace rosidl_generator_traits
{

[[deprecated("use pttep_alignment::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const pttep_alignment::srv::CalculateTransformation_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  pttep_alignment::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use pttep_alignment::srv::to_yaml() instead")]]
inline std::string to_yaml(const pttep_alignment::srv::CalculateTransformation_Response & msg)
{
  return pttep_alignment::srv::to_yaml(msg);
}

template<>
inline const char * data_type<pttep_alignment::srv::CalculateTransformation_Response>()
{
  return "pttep_alignment::srv::CalculateTransformation_Response";
}

template<>
inline const char * name<pttep_alignment::srv::CalculateTransformation_Response>()
{
  return "pttep_alignment/srv/CalculateTransformation_Response";
}

template<>
struct has_fixed_size<pttep_alignment::srv::CalculateTransformation_Response>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<pttep_alignment::srv::CalculateTransformation_Response>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<pttep_alignment::srv::CalculateTransformation_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<pttep_alignment::srv::CalculateTransformation>()
{
  return "pttep_alignment::srv::CalculateTransformation";
}

template<>
inline const char * name<pttep_alignment::srv::CalculateTransformation>()
{
  return "pttep_alignment/srv/CalculateTransformation";
}

template<>
struct has_fixed_size<pttep_alignment::srv::CalculateTransformation>
  : std::integral_constant<
    bool,
    has_fixed_size<pttep_alignment::srv::CalculateTransformation_Request>::value &&
    has_fixed_size<pttep_alignment::srv::CalculateTransformation_Response>::value
  >
{
};

template<>
struct has_bounded_size<pttep_alignment::srv::CalculateTransformation>
  : std::integral_constant<
    bool,
    has_bounded_size<pttep_alignment::srv::CalculateTransformation_Request>::value &&
    has_bounded_size<pttep_alignment::srv::CalculateTransformation_Response>::value
  >
{
};

template<>
struct is_service<pttep_alignment::srv::CalculateTransformation>
  : std::true_type
{
};

template<>
struct is_service_request<pttep_alignment::srv::CalculateTransformation_Request>
  : std::true_type
{
};

template<>
struct is_service_response<pttep_alignment::srv::CalculateTransformation_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__TRAITS_HPP_
