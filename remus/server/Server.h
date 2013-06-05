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

#ifndef __remus_server_Server_h
#define __remus_server_Server_h

#include <remus/common/zmqHelper.h>

#include <boost/scoped_ptr.hpp>
#include <boost/uuid/random_generator.hpp>

#include <remus/server/WorkerFactory.h>
#include <remus/server/ServerPorts.h>


//included for symbol exports
#include "ServerExports.h"

namespace remus{
//forward declaration of classes only the implementation needs
  namespace common{
  class Message;
  }
  class Job;
}

namespace remus{
namespace server{
  namespace internal
    {
    //forward declaration of classes only the implementation needs
    class ActiveJobs;
    class JobQueue;
    class WorkerPool;
    }

//Server is the broker of Remus. It handles accepting client
//connections, worker connections, and manages the life cycle of submitted jobs.
class REMUSSERVER_EXPORT Server
{
public:
  //construct a new server using the default worker factory
  //and default loopback ports
  Server();

  //construct a new server with a custom factory
  //and the default loopback ports
  explicit Server(const remus::server::WorkerFactory& factory);

  //construct a new server using the given loop back ports
  //and the default factory
  explicit Server(remus::server::ServerPorts ports);

  //construct a new server using the given loop back ports
  //and the default factory
  explicit Server(remus::server::ServerPorts ports,
                  const remus::server::WorkerFactory& factory);
  ~Server();

  //when you call start brokering the server will actually start accepting
  //worker and client requests.
  bool startBrokering();

  //get back the port information that this server bound too. Since multiple
  //remus servers can be running at a single time this is a way for the server
  //to report which port it bound it self too.
  const remus::server::ServerPorts& ServerPortInfo() const {return PortInfo;}

protected:
  //processes all job queries
  void DetermineJobQueryResponse(const zmq::socketIdentity &clientIdentity,
                                 const remus::common::Message& msg);

  //These methods are all to do with send responses to job messages
  bool canMesh(const remus::common::Message& msg);
  std::string meshStatus(const remus::common::Message& msg);
  std::string queueJob(const remus::common::Message& msg);
  std::string retrieveMesh(const remus::common::Message& msg);
  std::string terminateJob(const remus::common::Message& msg);

  //Methods for processing Worker queries
  void DetermineWorkerResponse(const zmq::socketIdentity &workerIdentity,
                              const remus::common::Message& msg);
  void storeMeshStatus(const remus::common::Message& msg);
  void storeMesh(const remus::common::Message& msg);
  void assignJobToWorker(const zmq::socketIdentity &workerIdentity,
                         const remus::Job& job);

  void FindWorkerForQueuedJob();

private:
  zmq::context_t Context;
  zmq::socket_t ClientQueries;
  zmq::socket_t WorkerQueries;

//allow subclasses to override these internal containers
protected:
  boost::uuids::random_generator UUIDGenerator;
  boost::scoped_ptr<remus::server::internal::JobQueue> QueuedJobs;
  boost::scoped_ptr<remus::server::internal::WorkerPool> WorkerPool;
  boost::scoped_ptr<remus::server::internal::ActiveJobs> ActiveJobs;

  remus::server::WorkerFactory WorkerFactory;
  remus::server::ServerPorts PortInfo;
};

}

typedef remus::server::Server Server;
}


#endif