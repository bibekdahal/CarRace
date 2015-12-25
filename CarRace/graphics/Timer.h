#pragma once

#include "../utils/stdinc.h"

class Timer
{
public:
    Timer(double targetFPS) { Reset(targetFPS); }
    void Reset(double targetFPS)
    {
        glfwSetTime(0.0);
        m_leftOver = m_lastTime = m_totalTime = 0.0;
        m_secondCounter = m_fps = m_frameCounter = 0;
        m_target = 1.0 / targetFPS;
    }

    unsigned int GetFPS() { return m_fps; }

    void Update(std::function<void(double, double)> update) 
    {
        // get current and delta time
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - m_lastTime;
        m_lastTime = currentTime;
        // second counter keeps track whether we have crossed a second
        m_secondCounter += static_cast<unsigned int>(deltaTime * 10000000);

        if (deltaTime > 1.0f) deltaTime = 1.0f;

        // stop accumulation of small error
        if (abs(deltaTime - m_target) < 1.0 / 4000.0f) deltaTime = m_target;

        m_leftOver += deltaTime;
        // we have passed one more frame this second
        if (m_leftOver >= m_target) ++m_frameCounter;	// we don't want to count inside loop below since larger time gap means less FPS

        // if more time has been passed than targetted-time, update more times
        while (m_leftOver >= m_target)
        {
            m_totalTime += m_target;
            m_leftOver -= m_target;
            update(m_totalTime, m_target);
        }

        // calculate FPS using frameCounter and secondCounter
        if (m_secondCounter >= 10000000)
        {
            m_fps = m_frameCounter;
            m_frameCounter = 0;
            m_secondCounter %= 10000000;
        }
    }

private:
    double m_lastTime, m_target, m_leftOver, m_totalTime;
    unsigned int m_fps, m_frameCounter, m_secondCounter;
};