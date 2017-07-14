/*
Title: Swept AABB-3D
File Name: GLIncludes.h
Copyright © 2015
Original authors: Brockton Roth
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Description:
This is a Swept Axis Aligned Bounding Box collision test. This goes beyond a
standard AABB test to determine the time and axis of collision. This is in 3D.
Contains two cubes, one that is stationary and one that is moving. They are bounded
by AABBs (Axis-Aligned Bounding Boxes) and when these AABBs collide the moving object
"bounces" on the axis of collision.
There is a physics timestep such that every update runs at the same delta time, regardless
of how fast or slow the computer is running. The Swept portion of this algorithm determines
when the collision will actually happen (so if your velocity is 10, and you are a distance of
5 away from the collision, it will detect this) and will perform the collision response
(bounce, in this case) before the end of the frame, so you can prevent tunneling (where the
object passes through or into the middle of the colliding object).
*/

#ifndef _GL_INCLUDES_H
#define _GL_INCLUDES_H

#include "gl\glew.h"
#include "glfw\glfw3.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\quaternion.hpp"

// We create a VertexFormat struct, which defines how the data passed into the shader code wil be formatted
struct VertexFormat
{
	glm::vec4 color;	// A vector4 for color has 4 floats: red, green, blue, and alpha
	glm::vec3 position;	// A vector3 for position has 3 float: x, y, and z coordinates

	// Default constructor
	VertexFormat()
	{
		color = glm::vec4(0.0f);
		position = glm::vec3(0.0f);
	}

	// Constructor
	VertexFormat(const glm::vec3 &pos, const glm::vec4 &iColor)
	{
		position = pos;
		color = iColor;
	}
};

#endif _GL_INCLUDES_H