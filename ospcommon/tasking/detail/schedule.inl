// ======================================================================== //
// Copyright 2009-2019 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include <utility>

#ifdef OSPCOMMON_TASKING_TBB
#  define __TBB_NO_IMPLICIT_LINKAGE 1
#  define __TBBMALLOC_NO_IMPLICIT_LINKAGE 1
#  include <tbb/task.h>
#elif defined(OSPCOMMON_TASKING_OMP)
#  include <thread>
#elif defined(OSPCOMMON_TASKING_INTERNAL)
#  include "TaskSys.h"
#endif

namespace ospcommon {
  namespace tasking {
    namespace detail {

      template<typename TASK_T>
      inline void schedule_impl(TASK_T fcn)
      {
#ifdef OSPCOMMON_TASKING_TBB
        struct LocalTBBTask : public tbb::task
        {
          TASK_T func;
          tbb::task* execute() override { func(); return nullptr; }
          LocalTBBTask(TASK_T f) : func(std::move(f)) {}
        };

        auto *tbb_node =
          new(tbb::task::allocate_root())LocalTBBTask(std::move(fcn));
        tbb::task::enqueue(*tbb_node);
#elif defined(OSPCOMMON_TASKING_OMP)
        std::thread thread(fcn);
        thread.detach();
#elif defined(OSPCOMMON_TASKING_INTERNAL)
        detail::schedule_internal(std::move(fcn));
#else// Debug --> synchronous!
        fcn();
#endif
      }

    } // ::ospcommon::tasking::detail
  } // ::ospcommon::tasking
} // ::ospcommon
