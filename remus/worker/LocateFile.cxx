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

#include <remus/worker/LocateFile.h>
#include <remus/common/CompilerInformation.h>

REMUS_THIRDPARTY_PRE_INCLUDE
//force to use filesystem version 3
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
REMUS_THIRDPARTY_POST_INCLUDE

#if  defined(__APPLE__)
  #include <remus/worker/detail/LocateExecOSX.hxx>
#elif defined(_WIN32)
  #include <remus/worker/detail/LocateExecWin.hxx>
#elif defined(REMUS_HAVE_AUXV_H)
  #include <remus/worker/detail/LocateExecUnix.hxx>
#else
  #include <remus/worker/detail/LocateExecUnkown.hxx>
#endif

namespace  remus {
namespace  worker {

//------------------------------------------------------------------------------
std::string getExecutableLocation()
{
  //ask the per OS specific code where the executable is located
  boost::filesystem::path execLoc = remus::worker::getOSExecLocation();
  if(boost::filesystem::is_empty(execLoc))
    {
    //we default our guess of the executable location to be the current working
    //directory
    execLoc = boost::filesystem::canonical(boost::filesystem::current_path());
    }
  return execLoc.string();
}

//------------------------------------------------------------------------------
std::vector<std::string> locationsToSearch()
{
  std::vector<std::string> locations;
  locations.push_back("bin/");
  locations.push_back("../");
  locations.push_back("../bin/");
  locations.push_back("../../bin/");
  locations.push_back("../../");
  locations.push_back("../../../bin/");
  locations.push_back("../../../");

  //a search locations over to the plugin directory
  locations.push_back("../Plugins/");
  locations.push_back("../../Plugins/");
  locations.push_back("../../../Plugins/");

#ifdef _WIN32
  //only search paths that make sense on window development or dashboard machines
  locations.push_back("../../../bin/Debug/");
  locations.push_back("../../../bin/Release/");
  locations.push_back("../../bin/Debug/");
  locations.push_back("../../bin/Release/");
  locations.push_back("../bin/Debug/");
  locations.push_back("../bin/Release/");
  locations.push_back("bin/Debug/");
  locations.push_back("bin/Release/");
#endif

  return locations;
}

//------------------------------------------------------------------------------
remus::common::FileHandle findFile( const std::string& name,
                                    const std::string& ext )
{
  std::vector< std::string > absoluteLocations;
  absoluteLocations.push_back( remus::worker::getExecutableLocation() );

  std::vector<std::string> relLocations = remus::worker::locationsToSearch();

  return findFile( name, ext, relLocations, absoluteLocations );
}

//------------------------------------------------------------------------------
remus::common::FileHandle findFile( const std::string& name,
                                    const std::string& ext,
                                    const std::vector<std::string>& relativeLocations,
                                    const std::vector<std::string>& absoluteLocations )
{
  remus::common::FileHandle handle("");
  if(name.size() == 0)
    { return handle; }

  //generate the fullname which is "name+ext"
  std::string fullname = name;
  if(ext.size()>0)
    {
    if(ext[0]!='.')
      { fullname += "."; }
    fullname += ext;
    }

  typedef std::vector< std::string >::const_iterator cit;
  for(cit i=absoluteLocations.begin(); i!=absoluteLocations.end(); ++i)
    {
    boost::filesystem::path possibleFile( *i );
    possibleFile /= fullname;

    if( boost::filesystem::exists(possibleFile) &&
        boost::filesystem::is_regular_file(possibleFile) )
      {
      return remus::common::FileHandle(possibleFile.string());
      }

    for(cit j=relativeLocations.begin(); j!=relativeLocations.end(); ++j)
      {
      boost::filesystem::path nextPossibleFile( *i );
      nextPossibleFile /= *j;
      nextPossibleFile /= fullname;
      nextPossibleFile = boost::filesystem::absolute(nextPossibleFile);
      if( boost::filesystem::exists(nextPossibleFile) &&
          boost::filesystem::is_regular_file(nextPossibleFile) )
        {
        return remus::common::FileHandle(nextPossibleFile.string());
        }
      }
    }

  return handle;
}

} //namespace worker
} //namespace remus
