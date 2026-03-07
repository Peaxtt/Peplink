// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from pttep_alignment:srv/CalculateTransformation.idl
// generated code does not contain a copyright notice

#ifndef PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__STRUCT_H_
#define PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/CalculateTransformation in the package pttep_alignment.
typedef struct pttep_alignment__srv__CalculateTransformation_Request
{
  bool reset;
} pttep_alignment__srv__CalculateTransformation_Request;

// Struct for a sequence of pttep_alignment__srv__CalculateTransformation_Request.
typedef struct pttep_alignment__srv__CalculateTransformation_Request__Sequence
{
  pttep_alignment__srv__CalculateTransformation_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} pttep_alignment__srv__CalculateTransformation_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'message'
#include "rosidl_runtime_c/string.h"
// Member 'transform'
#include "geometry_msgs/msg/detail/transform_stamped__struct.h"

/// Struct defined in srv/CalculateTransformation in the package pttep_alignment.
typedef struct pttep_alignment__srv__CalculateTransformation_Response
{
  bool success;
  rosidl_runtime_c__String message;
  geometry_msgs__msg__TransformStamped transform;
} pttep_alignment__srv__CalculateTransformation_Response;

// Struct for a sequence of pttep_alignment__srv__CalculateTransformation_Response.
typedef struct pttep_alignment__srv__CalculateTransformation_Response__Sequence
{
  pttep_alignment__srv__CalculateTransformation_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} pttep_alignment__srv__CalculateTransformation_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // PTTEP_ALIGNMENT__SRV__DETAIL__CALCULATE_TRANSFORMATION__STRUCT_H_
