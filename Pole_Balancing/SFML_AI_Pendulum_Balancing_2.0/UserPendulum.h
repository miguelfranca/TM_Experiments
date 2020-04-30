#pragma once
#include "GraphicsFramework/Tools/ODEsolver.hpp"
#include "GraphicsFramework/Tools/Random.hpp"
#include "GraphicsFramework/holders.h"
#include "GraphicsFramework/main.h"
#include "GraphicsFramework/sfml.h"
#include "GraphicsFramework/sfml/Orientation.h"
#include "Image.hpp"
#include "NEAT_GA.hpp"

class UserPendulum : public GF::Widget
{
  public:
    static double toPixels(double l);
    static double toMeters(double p);
    static VecD invertedPendulumRHS(const VecD &x, const VecD &params);
    void restart();

    UserPendulum(sf::RenderTarget *renderer = nullptr);

    bool handleEvent(GF::Event &event) override;

    virtual bool update(const float fElapsedTime,
                        const float fTotalTime) override;
    virtual bool draw() override;

  private:
    void move(float pos, float angle);

  protected:
    sf::Texture base_tex;
    GF::Rectangle base; // base rectangle
    GF::Line line;      // line that connects base to circle
    GF::Circle circle;  // circle wieght

    ODEsolver eq;
    VecD x0;

    float m_lenght;
};
