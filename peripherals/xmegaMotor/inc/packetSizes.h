// Copyright 2015 Tim Kouters
// Licensed under the Apache License version 2.0
// You may not use this file except in compliance with this License
// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

#ifndef PACKETSIZES_H_
#define PACKETSIZES_H_

/*
 * Commands
 */
#define PACKETSIZE_CMD_SET_PRIMARY_SETPOINT 2
#define PACKETSIZE_CMD_SET_ANGLE_SETPOINT 2
#define PACKETSIZE_CMD_SET_PID_ANGLE_PROPERTIES 8
#define PACKETSIZE_CMD_SET_PID_PRIMARY_PROPERTIES 8
#define PACKETSIZE_CMD_SET_GAIN 1
#define PACKETSIZE_CMD_SET_MOTOR_TIMEOUT 2
#define PACKETSIZE_CMD_SET_ANGLE_DIRECTION 1
#define PACKETSIZE_CMD_SET_DRV8301 4
#define PACKETSIZE_CMD_CLEAR_ERRORS 14
#define PACKETSIZE_CMD_DEFAULT_RESPONSE 40
/*
 * Response
 */
#define PACKETSIZE_RESP_PID_ANGLE_PROPERTIES 8
#define PACKETSIZE_RESP_AA_BOARD 24
#define PACKETSIZE_RESP_LED_GREEN 1
#define PACKETSIZE_RESP_LED_YELLOW 1
#define PACKETSIZE_RESP_ANGLE_SETPOINT 2
#define PACKETSIZE_RESP_PRIMARY_SETPOINT 2
#define PACKETSIZE_RESP_PID_PRIMARY_PROPERTIES 8
#define PACKETSIZE_RESP_PWM_LIMIT 2
#define PACKETSIZE_RESP_PWM_MANUAL 2
#define PACKETSIZE_RESP_PWM_DELTA 2
#define PACKETSIZE_RESP_GAIN 1
#define PACKETSIZE_RESP_MOTOR_TIMEOUT 2
#define PACKETSIZE_RESP_ANGLE_DIRECTION 1
#define PACKETSIZE_RESP_DRV8301 12
#define PACKETSIZE_RESP_MODE 1
#define PACKETSIZE_RESP_ANGLE_TACHO_ZERO 4
#define PACKETSIZE_RESP_ERROR 14


#endif /* PACKETSIZES_H_ */
