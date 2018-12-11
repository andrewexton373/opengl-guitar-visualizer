#pragma once


#ifndef LAB471_CAMERA_H_INCLUDED
#define LAB471_CAMERA_H_INCLUDED

#include <stack>
#include <memory>

#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"





class Camera
{
public:
	glm::vec3 pos;
	glm::vec3 rot;
	int w, a, s, d, e, f, r, t;
	Camera()
	{
		w = a = s = d = e = f = r = t = 0;
        pos = glm::vec3(0,0,-10);
		rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process()
	{
        glm::mat4 matPitch = glm::mat4(1.0f);
        glm::mat4 matYaw = glm::mat4(1.0f);
        
		float going_forward = 0.0;
        float going_right = 0.0;
        
        if (w == 1)
            going_forward += 0.4;
        if (s == 1)
            going_forward -= 0.4;
        if (a == 1)
            rot.y -= 0.1;
        if (d == 1)
            rot.y += 0.1;
        if (t == 1)
            going_right += 0.1;
        if (r == 1)
            going_right -= 0.1;
        if (e == 1)
            rot.x -= 0.05;
        if (f == 1)
            rot.x += 0.05;

        matPitch = glm::rotate(matPitch, rot.x, glm::vec3(1, 0, 0));
        
        matYaw = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
        
        glm::mat4 R = matPitch * matYaw;
        
        glm::vec4 rpos = glm::vec4(going_right, 0, going_forward, 1);
        
        rpos = R *rpos;
        pos.x += -rpos.x;
        pos.z += rpos.z;
        
        glm::mat4 T = glm::translate(glm::mat4(1), glm::vec3(pos.x, pos.y, pos.z));
        return R*T;
        
    }
    glm::mat4 getR()
	{
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
return R;
	}
};








#endif // LAB471_CAMERA_H_INCLUDED
