 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * overruleRobotAdapterROS.hpp
 *
 *  Created on: Feb 10, 2017
 *      Author: Jan Feitsma
 */

#ifndef OVERRULESTATUSADAPTERROS_HPP_
#define OVERRULESTATUSADAPTERROS_HPP_

#include "int/facilities/templatedSubject.hpp"

#include <boost/shared_ptr.hpp>

#include "ros/node_handle.h"
#include "ros/service.h"

#include "worldModel/overrule_robot_status.h"

#include "int/facilities/templatedSubject.hpp"
#include "int/types/robot/robotStatusType.hpp"

class overruleStatusAdapterROS : public templatedSubject<robotStatusType>
{
	public:
		overruleStatusAdapterROS();
		~overruleStatusAdapterROS();

		void InitializeROS();

	private:
		boost::shared_ptr<ros::NodeHandle> _hROS;
        ros::ServiceServer _srvOverruleStatus;

        bool overrule_robot_status_cb(
        		worldModel::overrule_robot_status::Request &req,
        		worldModel::overrule_robot_status::Response &resp);
};

#endif /* OVERRULESTATUSADAPTERROS_HPP_ */
