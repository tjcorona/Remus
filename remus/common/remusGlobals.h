//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//=============================================================================

#ifndef __remus_common_RemusInfo_h
#define __remus_common_RemusInfo_h

#include <string>
#include <sstream>
#include <boost/cstdint.hpp>

//Define global information that the mesh server needs

namespace remus {
static const int BROKER_CLIENT_PORT = 50505;
static const int BROKER_WORKER_PORT = 50510;
static const int HEARTBEAT_INTERVAL_IN_SEC = 5;
static const int HEARTBEAT_INTERVAL = 1000000 * HEARTBEAT_INTERVAL_IN_SEC;

static const std::string INVALID_MSG = "INVALID_MSG";

//------------------------------------------------------------------------------
enum MESH_INPUT_TYPE
{
  INVALID_MESH_IN = 0,
  SCENEFILE = 1,
  MODEL = 2,
  RAWTRIANGLES = 3,
  NUM_MESH_INPUT_TYPES=4
};

//------------------------------------------------------------------------------
enum MESH_OUTPUT_TYPE
{
  INVALID_MESH_OUT = 0,
  MESH1D = 1,
  MESH2D = 2,
  MESH3D = 3,
  MESH3DSurface = 4,
  NUM_MESH_OUTPUT_TYPES=5
};


//------------------------------------------------------------------------------
enum SERVICE_TYPE
{
  INVALID_SERVICE = 0,
  MAKE_MESH = 1,
  MESH_STATUS = 2,
  CAN_MESH = 3,
  RETRIEVE_MESH = 4,
  HEARTBEAT = 5,
  SHUTDOWN = 6
};

//------------------------------------------------------------------------------
enum STATUS_TYPE
{
  INVALID_STATUS = 0,
  QUEUED = 1,
  IN_PROGRESS = 2,
  FINISHED = 3,
  FAILED = 4,
  EXPIRED = 5
};


//------------------------------------------------------------------------------
//MESH_TYPE is a union of the
//mesh input and output types into a single integer
struct MESH_TYPE
{

  //construct an invalid mesh type
  MESH_TYPE():
    combined_type(0)
    {}

  MESH_TYPE(remus::MESH_INPUT_TYPE in, remus::MESH_OUTPUT_TYPE out)
    {
    Both.input = in;
    Both.output = out;
    }

  boost::int32_t type() const { return combined_type; }
  remus::MESH_INPUT_TYPE inputType() const
    { return static_cast<remus::MESH_INPUT_TYPE>(this->Both.input); }
  remus::MESH_OUTPUT_TYPE outputType() const
    { return static_cast<remus::MESH_OUTPUT_TYPE>(this->Both.output); }

  //since zero for input and out is invalid a combined value of
  //zero for the full int32 is also invalid
  bool valid() const { return combined_type != 0; }

  //needed to see if a client request type and a workers type are equal
  bool operator ==(const MESH_TYPE& b) const { return combined_type == b.combined_type; }

  //needed to properly store mesh types into stl containers
  bool operator <(const MESH_TYPE& b) const { return combined_type < b.combined_type; }

  //needed to encode the object on the wire
  friend std::ostream& operator<<(std::ostream &os, const MESH_TYPE &ctype)
    {
    os << ctype.type();
    return os;
    }

  //needed to decode the object from the wire
  friend std::istream& operator>>(std::istream &is, MESH_TYPE &ctype)
    {
    is >> ctype.combined_type;
    return is;
    }

protected:
  union {
    boost::int32_t combined_type;
    struct {
      boost::int16_t input;
      boost::int16_t output;
      } Both;
  };
};

//------------------------------------------------------------------------------
namespace common
  {
  //a mapping of enum types to char*
  static const char *mesh_in_types[] = { "INVALID","SCENE FILE", "MODEL", "RAW TRIANGLE"};
  static const char *mesh_out_types[] = { "INVALID","1D", "2D", "3D", "3D Surface" };
  static const char *serv_types[] = { "INVALID", "MAKE MESH", "MESH STATUS", "CAN MESH", "RETRIEVE MESH", "HEARTBEAT", "SHUTDOWN" };
  static const char *stat_types[] = { "INVALID", "QUEUED", "IN PROGRESS", "FINISHED", "FAILED","EXPIRED" };
  }

//------------------------------------------------------------------------------
inline std::string to_string(remus::MESH_INPUT_TYPE t)
{
  return std::string(remus::common::mesh_in_types[(int)t]);
}

//------------------------------------------------------------------------------
inline std::string to_string(remus::MESH_OUTPUT_TYPE t)
{
  return std::string(remus::common::mesh_out_types[(int)t]);
}

//------------------------------------------------------------------------------
inline std::string to_string(remus::SERVICE_TYPE t)
{
  return std::string(remus::common::serv_types[(int)t]);
}

//------------------------------------------------------------------------------
inline std::string to_string(remus::STATUS_TYPE t)
{
  return std::string(remus::common::stat_types[(int)t]);
}


//------------------------------------------------------------------------------
inline remus::MESH_INPUT_TYPE to_meshInType(const std::string& t)
{
  for(int i=1; i <=3; i++)
    {
    remus::MESH_INPUT_TYPE mt=static_cast<remus::MESH_INPUT_TYPE>(i);
    if (remus::to_string(mt) == t)
      {
      return mt;
      }
    }
  return remus::INVALID_MESH_IN;
}

//------------------------------------------------------------------------------
inline remus::MESH_OUTPUT_TYPE to_meshOutType(const std::string& t)
{
  for(int i=1; i <=4; i++)
    {
    remus::MESH_OUTPUT_TYPE mt=static_cast<remus::MESH_OUTPUT_TYPE>(i);
    if (remus::to_string(mt) == t)
      {
      return mt;
      }
    }
  return remus::INVALID_MESH_OUT;
}

//------------------------------------------------------------------------------
//inline remus::MESH_TYPE to_meshType(const std::string& t)
//{
//  for(int i=1; i <=4; i++)
//    {
//    remus::MESH_TYPE mt=static_cast<remus::MESH_TYPE>(i);
//    if (remus::to_string(mt) == t)
//      {
//      return mt;
//      }
//    }
//  return remus::INVALID_MESH_OUT;
//}

//------------------------------------------------------------------------------
inline remus::SERVICE_TYPE to_serviceType(const std::string& t)
{
  for(int i=1; i <=6; i++)
    {
    remus::SERVICE_TYPE mt=static_cast<remus::SERVICE_TYPE>(i);
    if (remus::to_string(mt) == t)
      {
      return mt;
      }
    }
  return remus::INVALID_SERVICE;
}

namespace internal
{
//------------------------------------------------------------------------------
inline std::string extractString(std::stringstream& buffer, int size)
{
  char* rawData = new char[size+1];
  buffer.rdbuf()->sgetn(rawData,size);
  rawData[size]='\0';
  std::string msg = std::string(rawData);
  delete[] rawData;
  return msg;
}
}

}

#endif // __remus_common_RemusInfo_h
