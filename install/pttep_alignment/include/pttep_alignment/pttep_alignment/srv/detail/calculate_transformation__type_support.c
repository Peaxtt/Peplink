// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from pttep_alignment:srv/CalculateTransformation.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "pttep_alignment/srv/detail/calculate_transformation__rosidl_typesupport_introspection_c.h"
#include "pttep_alignment/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "pttep_alignment/srv/detail/calculate_transformation__functions.h"
#include "pttep_alignment/srv/detail/calculate_transformation__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  pttep_alignment__srv__CalculateTransformation_Request__init(message_memory);
}

void pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_fini_function(void * message_memory)
{
  pttep_alignment__srv__CalculateTransformation_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_member_array[1] = {
  {
    "reset",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(pttep_alignment__srv__CalculateTransformation_Request, reset),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_members = {
  "pttep_alignment__srv",  // message namespace
  "CalculateTransformation_Request",  // message name
  1,  // number of fields
  sizeof(pttep_alignment__srv__CalculateTransformation_Request),
  pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_member_array,  // message members
  pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_type_support_handle = {
  0,
  &pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_pttep_alignment
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, pttep_alignment, srv, CalculateTransformation_Request)() {
  if (!pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_type_support_handle.typesupport_identifier) {
    pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &pttep_alignment__srv__CalculateTransformation_Request__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "pttep_alignment/srv/detail/calculate_transformation__rosidl_typesupport_introspection_c.h"
// already included above
// #include "pttep_alignment/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "pttep_alignment/srv/detail/calculate_transformation__functions.h"
// already included above
// #include "pttep_alignment/srv/detail/calculate_transformation__struct.h"


// Include directives for member types
// Member `message`
#include "rosidl_runtime_c/string_functions.h"
// Member `transform`
#include "geometry_msgs/msg/transform_stamped.h"
// Member `transform`
#include "geometry_msgs/msg/detail/transform_stamped__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  pttep_alignment__srv__CalculateTransformation_Response__init(message_memory);
}

void pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_fini_function(void * message_memory)
{
  pttep_alignment__srv__CalculateTransformation_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_member_array[3] = {
  {
    "success",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(pttep_alignment__srv__CalculateTransformation_Response, success),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "message",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(pttep_alignment__srv__CalculateTransformation_Response, message),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "transform",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(pttep_alignment__srv__CalculateTransformation_Response, transform),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_members = {
  "pttep_alignment__srv",  // message namespace
  "CalculateTransformation_Response",  // message name
  3,  // number of fields
  sizeof(pttep_alignment__srv__CalculateTransformation_Response),
  pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_member_array,  // message members
  pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_type_support_handle = {
  0,
  &pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_pttep_alignment
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, pttep_alignment, srv, CalculateTransformation_Response)() {
  pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, TransformStamped)();
  if (!pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_type_support_handle.typesupport_identifier) {
    pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &pttep_alignment__srv__CalculateTransformation_Response__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "pttep_alignment/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "pttep_alignment/srv/detail/calculate_transformation__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_service_members = {
  "pttep_alignment__srv",  // service namespace
  "CalculateTransformation",  // service name
  // these two fields are initialized below on the first access
  NULL,  // request message
  // pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_Request_message_type_support_handle,
  NULL  // response message
  // pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_Response_message_type_support_handle
};

static rosidl_service_type_support_t pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_service_type_support_handle = {
  0,
  &pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_service_members,
  get_service_typesupport_handle_function,
};

// Forward declaration of request/response type support functions
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, pttep_alignment, srv, CalculateTransformation_Request)();

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, pttep_alignment, srv, CalculateTransformation_Response)();

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_pttep_alignment
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, pttep_alignment, srv, CalculateTransformation)() {
  if (!pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_service_type_support_handle.typesupport_identifier) {
    pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, pttep_alignment, srv, CalculateTransformation_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, pttep_alignment, srv, CalculateTransformation_Response)()->data;
  }

  return &pttep_alignment__srv__detail__calculate_transformation__rosidl_typesupport_introspection_c__CalculateTransformation_service_type_support_handle;
}
