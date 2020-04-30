#pragma once
#include "GraphicsFramework/sfml.h"

#include "GraphicsFramework/Tools/ODEsolver.hpp"
#include "GraphicsFramework/Tools/Random.hpp"
#include "GraphicsFramework/holders.h"
#include "GraphicsFramework/main.h"
#include "GraphicsFramework/sfml/Orientation.h"
#include "Image.hpp"
#include "NEAT_GA.hpp"

class UserDoublePendulum : public GF::Widget
{
  public:
    static double toPixels(double l);
    static double toMeters(double p);
    static VecD invertedPendulumRHS(const VecD &x, const VecD &params);
    void restart();

    UserDoublePendulum(sf::RenderTarget *renderer = nullptr);

    bool handleEvent(GF::Event &event) override;

    virtual bool update(const float fElapsedTime,
                        const float fTotalTime) override;
    virtual bool draw() override;

  private:
    void move(float pos, float angle1, float angle2);

  protected:
    sf::Texture base_tex;
    GF::Rectangle base; // base rectangle
    GF::Line line1;     // line that connects base to circle1
    GF::Line line2;     // line that connects base to circle2
    GF::Circle circle1; // circle wieght
    GF::Circle circle2; // circle2 wieght

    ODEsolver eq;
    VecD x0;

    float m_lenght1;
    float m_lenght2;
};
