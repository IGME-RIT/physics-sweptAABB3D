/*
Title: Swept AABB-3D
File Name: Main.cpp
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

#include "GLIncludes.h"
#include "GameObject.h"
#include "GLRender.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>



// Variables for FPS and Physics Timestep calculations.
int frame = 0;
double time = 0;
double timebase = 0;
double accumulator = 0.0;
int fps = 0;
double FPSTime = 0.0;
double physicsStep = 0.012; // This is the number of milliseconds we intend for the physics to update.



// Reference to the window object being created by GLFW.
GLFWwindow* window;



// Regular AABB collision detection. (Not used in this demo, but should work just fine.)
bool TestAABB(AABB a, AABB b)
{
	// If any axis is separated, exit with no intersection.
	if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
	if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
	
	return true;
}

// Swept AABB collision detection, giving you the time of collision and thus allowing you to even calculate the point of collision and collision responses (such as bounce).
float SweptAABB(AABB* box1, AABB* box2, glm::vec3 vel1, float& normalx, float& normaly, float& normalz)
{
	// These variables stand for the distance in each axis between the moving object and the stationary object in terms of when the moving object would "enter" the colliding object.
	float xDistanceEntry, yDistanceEntry, zDistanceEntry;

	// These variables stand for the distance in each axis in terms of when the moving object would "exit" the colliding object.
	float xDistanceExit, yDistanceExit, zDistanceExit;

	// Find the distance between the objects on the near and far sides for both x and y
	// Depending on the direction of the velocity, we'll reverse the calculation order to maintain the right sign (positive/negative).
	if (vel1.x > 0.0f)
	{
		xDistanceEntry = (*box2).min.x - (*box1).max.x;
		xDistanceExit = (*box2).max.x - (*box1).min.x;
	}
	else
	{
		xDistanceEntry = (*box2).max.x - (*box1).min.x;
		xDistanceExit = (*box2).min.x - (*box1).max.x;
	}

	if (vel1.y > 0.0f)
	{
		yDistanceEntry = (*box2).min.y - (*box1).max.y;
		yDistanceExit = (*box2).max.y - (*box1).min.y;
	}
	else
	{
		yDistanceEntry = (*box2).max.y - (*box1).min.y;
		yDistanceExit = (*box2).min.y - (*box1).max.y;
	}

	if (vel1.z > 0.0f)
	{
		zDistanceEntry = (*box2).min.z - (*box1).max.z;
		zDistanceExit = (*box2).max.z - (*box1).min.z;
	}
	else
	{
		zDistanceEntry = (*box2).max.z - (*box1).min.z;
		zDistanceExit = (*box2).min.z - (*box1).max.z;
	}

	// These variables stand for the time at which the moving object would enter/exit the stationary object.
	float xEntryTime, yEntryTime, zEntryTime;
	float xExitTime, yExitTime, zExitTime;

	// Find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
	if (vel1.x == 0.0f)
	{
		// If the largest distance (entry or exit) between the two objects is greater than the size of both objects combined, then the objects are clearly not colliding.
		if (std::max(fabsf(xDistanceEntry), fabsf(xDistanceExit)) > (((*box1).max.x - (*box1).min.x) + ((*box2).max.x - (*box2).min.x)))
		{
			// Setting this to 2.0f will cause an absence of collision later in this function.
			xEntryTime = 2.0f;
		}
		else
		{
			// Otherwise, pass negative infinity to basically ignore this variable.
			xEntryTime = -std::numeric_limits<float>::infinity();
		}
		
		// Setting this to postivie infinity will ignore this variable.
		xExitTime = std::numeric_limits<float>::infinity();
	}
	else
	{
		// If there is a velocity in the x-axis, then we can determine the time of collision based on the distance divided by the velocity. (Assuming velocity does not change.)
		xEntryTime = xDistanceEntry / vel1.x;
		xExitTime = xDistanceExit / vel1.x;
	}

	if (vel1.y == 0.0f)
	{
		if (std::max(fabsf(yDistanceEntry), fabsf(yDistanceExit)) > (((*box1).max.y - (*box1).min.y) + ((*box2).max.y - (*box2).min.y)))
		{
			yEntryTime = 2.0f;
		}
		else
		{
			yEntryTime = -std::numeric_limits<float>::infinity();
		}

		yExitTime = std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntryTime = yDistanceEntry / vel1.y;
		yExitTime = yDistanceExit / vel1.y;
	}

	if (vel1.z == 0.0f)
	{
		if (std::max(fabsf(zDistanceEntry), fabsf(zDistanceExit)) > (((*box1).max.z - (*box1).min.z) + ((*box2).max.z - (*box2).min.z)))
		{
			zEntryTime = 2.0f;
		}
		else
		{
			zEntryTime = -std::numeric_limits<float>::infinity();
		}

		zExitTime = std::numeric_limits<float>::infinity();
	}
	else
	{
		zEntryTime = zDistanceEntry / vel1.z;
		zExitTime = zDistanceExit / vel1.z;
	}

	// Get the maximum entry time to determine the latest collision, which is actually when the objects are colliding. (Because all 3 axes must collide.)
	float entryTime = std::max(std::max(xEntryTime, yEntryTime), zEntryTime);

	// Get the minimum exit time to determine when the objects are no longer colliding. (AKA the objects passed through one another.)
	float exitTime = std::min(std::min(xExitTime, yExitTime), zExitTime);

	// If anything in the following statement is true, there's no collision.
	// If entryTime > exitTime, that means that one of the axes is exiting the "collision" before the other axes are crossing, thus they don't cross the object in unison and there's no collison.
	// If all three of the entry times are less than zero, then the collision already happened (or we missed it, but either way..)
	// If any of the entry times are greater than 1.0f, then the collision isn't happening this update/physics step so we'll move on.
	if (entryTime > exitTime || xEntryTime < 0.0f && yEntryTime < 0.0f && zEntryTime < 0.0f || xEntryTime > 1.0f || yEntryTime > 1.0f || zEntryTime > 1.0f)
	{
		// With no collision, we pass out zero'd normals.
		normalx = 0.0f;
		normaly = 0.0f;
		normalz = 0.0f;

		// If collision detection isn't working, try uncommenting the if statemente and putting a break point on the std::cout statement.
		// Then you can check variable values within this algorithm to make sure everything is in order.
		/*if (glm::distance(obj1->GetPosition(), obj2->GetPosition()) < 0.1)
		{
			std::cout << "Something went wrong, and the objects are inside of each other but haven't been detected as a collision.";
		}*/

		// 2.0f signifies that there was no collision.
		return 2.0f;
	}
	else // If there was a collision
	{
		// Calculate normal of collided surface
		if (xEntryTime > yEntryTime && xEntryTime > zEntryTime) // If the x-axis is the last to cross, then that is the colliding axis.
		{
			if (xDistanceEntry < 0.0f) // Determine the normal based on positive or negative.
			{
				normalx = 1.0f;
				normaly = 0.0f;
				normalz = 0.0f;
			}
			else
			{
				normalx = -1.0f;
				normaly = 0.0f;
				normalz = 0.0f;
			}
		}
		else if (yEntryTime > xEntryTime && yEntryTime > zEntryTime)
		{
			if (yDistanceEntry < 0.0f)
			{
				normalx = 0.0f;
				normaly = 1.0f;
				normalz = 0.0f;
			}
			else
			{
				normalx = 0.0f;
				normaly = -1.0f;
				normalz = 0.0f;
			}
		}
		else if (zEntryTime > xEntryTime && zEntryTime > yEntryTime)
		{
			if (zDistanceEntry < 0.0f)
			{
				normalx = 0.0f;
				normaly = 0.0f;
				normalz = 1.0f;
			}
			else
			{
				normalx = 0.0f;
				normaly = 0.0f;
				normalz = -1.0f;
			}
		}

		// Return the time of collision
		return entryTime;
	}
}

// This runs once every physics timestep.
void update(float dt)
{
	// This section just checks to make sure the object stays within a certain boundary. This is not really collision detection.
	glm::vec3 tempPos = obj2->GetPosition();
	
	if (fabsf(tempPos.x) > 0.9f)
	{
		glm::vec3 tempVel = obj2->GetVelocity();

		// "Bounce" the velocity along the axis that was over-extended.
		obj2->SetVelocity(glm::vec3(-1.0f * tempVel.x, tempVel.y, tempVel.z));
	}
	if (fabsf(tempPos.y) > 0.8f)
	{
		glm::vec3 tempVel = obj2->GetVelocity();
		obj2->SetVelocity(glm::vec3(tempVel.x, -1.0f * tempVel.y, tempVel.z));
	}
	if (fabsf(tempPos.z) > 1.0f)
	{
		glm::vec3 tempVel = obj2->GetVelocity();
		obj2->SetVelocity(glm::vec3(tempVel.x, tempVel.y, -1.0f * tempVel.z));
	}

	// Rotate the objects. This helps illustrate how the AABB recalculates as an object's orientation changes.
	obj1->Rotate(glm::vec3(glm::radians(1.0f), glm::radians(1.0f), glm::radians(0.0f)));
	obj2->Rotate(glm::vec3(glm::radians(1.0f), glm::radians(1.0f), glm::radians(0.0f)));

	// Re-calculate the Axis-Aligned Bounding Box for your object.
	// We do this because if the object's orientation changes, we should update the bounding box as well.
	// Be warned: For some objects this can actually cause a collision to be missed, so be careful.
	// (This is because we determine the time of the collision based on the AABB, but if the AABB changes significantly, the time of collision can change between frames,
	// and if that lines up just right you'll miss the collision altogether.)
	obj1->CalculateAABB();
	obj2->CalculateAABB();

	// Our normals will be passed out of the SweptAABB algorithm and used to determine where to "bounce" the object.
	float normalx, normaly, normalz;

	// This function requires that the moving object be passed in first, the second object be stationary, and that the velocity refers to the 
	// velocity of the moving object this frame. For perfection, you should have some sort of physics timestep setup. (See the checkTime() function).
	float collisionTime = SweptAABB(&obj2->GetAABB(), &obj1->GetAABB(), obj2->GetVelocity() * dt, normalx, normaly, normalz);
	
	// Since we know we'll collide at collisionTime * dt, we can define that 1.0f - collisionTime is the remaining time this frame after that collision.
	// Thus, we'll "bounce" off the collided object, then update the rest of the object's movement by remainingTime * dt.
	float remainingTime = 1.0f - collisionTime;

	// If remaining time is less than zero, there's no collision this frame (because collisionTime is > 1).
	// If remaining time = 0, then the collision happens exactly at the end of this frame.
	if (remainingTime >= 0.0f)
	{
		// Create a local velocity variable based off of the moving object's velocity.
		glm::vec3 velocity = obj2->GetVelocity();

		// If the normal is not some ridiculously small (or zero) value.
		if (abs(normalx) > 0.0001f)
		{
			// Bounce the velocity along that axis.
			velocity.x *= -1;
		}
		if (abs(normaly) > 0.0001f)
		{
			velocity.y *= -1;
		}
		if (abs(normalz) > 0.0001f)
		{
			velocity.z *= -1;
		}

		// Update the objects by the collisionTime * dt (which is the part of the update before it collides with the object).
		obj1->Update(collisionTime * dt);
		obj2->Update(collisionTime * dt);

		// Then change the velocity to be the "bounced" velocity.
		obj2->SetVelocity(velocity);

		// Now update the objects by the remainingTime * dt (which is the part of the update after the collision).
		obj1->Update(remainingTime * dt);
		obj2->Update(remainingTime * dt);
	}
	else
	{
		// No collision, update normally.
		obj1->Update(dt);
		obj2->Update(dt);
	}

	// Update your MVP matrices based on the objects' transforms.
	MVP = PV * *obj1->GetTransform();
	MVP2 = PV * *obj2->GetTransform();
}

// This runs once every frame to determine the FPS and how often to call update based on the physics step.
void checkTime()
{
	// Get the current time.
	time = glfwGetTime();

	// Get the time since we last ran an update.
	double dt = time - timebase;

	// If more time has passed than our physics timestep.
	if (dt > physicsStep)
	{
		// Calculate FPS: Take the number of frames (frame) since the last time we calculated FPS, and divide by the amount of time that has passed since the 
		// last time we calculated FPS (time - FPSTime).
		if (time - FPSTime > 1.0)
		{
			fps = frame / (time - FPSTime);

			FPSTime = time; // Now we set FPSTime = time, so that we have a reference for when we calculated the FPS
			
			frame = 0; // Reset our frame counter to 0, to mark that 0 frames have passed since we calculated FPS (since we literally just did it)

			std::string s = "FPS: " + std::to_string(fps); // This just creates a string that looks like "FPS: 60" or however much.

			glfwSetWindowTitle(window, s.c_str()); // This will set the window title to that string, displaying the FPS as the window title.
		}

		timebase = time; // Set timebase = time so we have a reference for when we ran the last physics timestep.

		// Limit dt so that we if we experience any sort of delay in processing power or the window is resizing/moving or anything, it doesn't update a bunch of times while the player can't see.
		// This will limit it to a .25 seconds.
		if (dt > 0.25)
		{
			dt = 0.25;
		}

		// The accumulator is here so that we can track the amount of time that needs to be updated based on dt, but not actually update at dt intervals and instead use our physicsStep.
		accumulator += dt;

		// Run a while loop, that runs update(physicsStep) until the accumulator no longer has any time left in it (or the time left is less than physicsStep, at which point it save that 
		// leftover time and use it in the next checkTime() call.
		while (accumulator >= physicsStep)
		{
			update(physicsStep);

			accumulator -= physicsStep;
		}
	}
}



int main(int argc, char **argv)
{
	// Initializes the GLFW library
	glfwInit();

	// Creates a window given (width, height, title, monitorPtr, windowPtr).
	// Don't worry about the last two, as they have to do with controlling which monitor to display on and having a reference to other windows. Leaving them as nullptr is fine.
	window = glfwCreateWindow(800, 600, "Swept AABB 3D Collision", nullptr, nullptr);

	// Makes the OpenGL context current for the created window.
	glfwMakeContextCurrent(window);
	
	// Sets the number of screen updates to wait before swapping the buffers.
	// Setting this to zero will disable VSync, which allows us to actually get a read on our FPS. Otherwise we'd be consistently getting 60FPS or lower, 
	// since it would match our FPS to the screen refresh rate.
	// Set to 1 to enable VSync.
	glfwSwapInterval(0);

	// Initializes most things needed before the main loop
	init();

	// Calculate the Axis-Aligned Bounding Box for your object.
	obj1->CalculateAABB();
	obj2->CalculateAABB();

	// Enter the main loop.
	while (!glfwWindowShouldClose(window))
	{
		// Call to checkTime() which will determine how to go about updating via a set physics timestep as well as calculating FPS.
		checkTime();

		// Call the render function.
		renderScene();

		// Swaps the back buffer to the front buffer
		// Remember, you're rendering to the back buffer, then once rendering is complete, you're moving the back buffer to the front so it can be displayed.
		glfwSwapBuffers(window);

		// Add one to our frame counter, since we've successfully 
		frame++;

		// Checks to see if any events are pending and then processes them.
		glfwPollEvents();
	}

	cleanup();

	return 0;
}