#include "SurvivalGame.h"

#define BEST_FITNESS 120
#define CURSOR_RADIUS 50.

static double fitnessFunc(const NEAT::Network& net, const GA::Evolver<NEAT::Network>* ga);

class SurvivalEvolver : public NEAT::Evolver
{
public:
	SurvivalEvolver(const std::string params) : NEAT::Evolver(params) {}
	int maxUpgrades;
};

class SBestReplay : public SurvivalGame
{
public:
	SBestReplay(GF::StateMachine* stateMachine, sf::RenderWindow* window);

	// called once before the loop starts
	bool onCreate() override;

	// first thing to be called every frame
	bool onHandleEvent(GF::Event& event) override;

	// called every frame before draw
	bool onUpdate(const float fElapsedTime, const float fTotalTime) override;

	// last thing to be called every frame
	bool onDraw() override;

	// called once before exiting the app
	void onDestroy();

	void onSwitch(std::string other) {}

	// returns pair with <mouse angle, the shot was shot> to be used in onUpdate 
	static std::pair<float, bool> stepAI(GameEntities& AI, const NEAT::Network& net, float fElapsedTime);

private:
	// NEAT
	void newGeneration(bool skip_unchanged = false, unsigned skips = 0);

private:
	// NEAT
	SurvivalEvolver ga;
	NEAT::Network const * best;
	NEAT::Image*
	image; // needs to ba a pointer because NEAT::Image construtor doesnt take an evolver that hasnt started yet
	sf::Sprite network_im;
	bool started;

	GF::StateMachine* stateMachine;
	sf::RenderWindow* renderer;
};