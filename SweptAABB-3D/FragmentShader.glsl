/*
Title: Swept AABB-3D
File Name: FragmentShader.glsl
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

#version 400 core // Identifies the version of the shader, this line must be on a separate line from the rest of the shader code

layout(location = 0) out vec4 out_color; // Establishes the variable we will pass out of this shader.

in vec4 color;	// Take in a vec4 for color
 
void main(void)
{
	out_color = color; // Set our out_color equal to our in color, basically making this a pass-through shader.
}