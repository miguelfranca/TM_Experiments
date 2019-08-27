#include "CarAI.h"

CarAI::CarAI(std::string t) : ga("params.txt"), AI(nullptr) { title = t; }

	// called once before the game starts
bool CarAI::onCreate()
{
	setClearColor(sf::Color::White);

	GF::Button<GF::Rectangle>* button  = new GF::Button<GF::Rectangle>(&window, sf::Vector2f(200, 100));
	button->setText(std::string("Evolver"));
	button->setPos(sf::Vector2f(200, 100));
	addWidget(button, "Evolver_button");


	Track* track = new Track(&window);
	addWidget(track, "track");

		// User's car
	addWidget(new Car(&window, CENTER_WINDOW, 40, 80, track, "res/textures/car0.png"), "car");

		// AI = new Car(&window, TOPLEFT_F, 40, 80, track, "res/textures/car0.png");
	for(unsigned i = 0; i < 50; ++i)
		cars.push_back(new Car(&window, TOPLEFT_F, 40, 80, track, "res/textures/car0.png"));

		// NEAT
	ga.track = track;
	ga.addCriteria(GA::StopReason::BestMaximum, {3*500});
	ga.setEvaluate(fitnessFunc, GA::MAXIMIZE);

		// waits until track is drawn
	ready_for_evolution = false;

		// displays cars when true
	drawing = true;

	return true;
}

bool CarAI::handle_Track_Car_events(GF::Event &event){
	static Track* track = (Track*)getWidget("track");
	static GF::Button<>* button = (GF::Button<>*)getWidget("Evolver_button");


	static bool being_dragged = false;
	static Car* car = (Car*)getWidget("car");

	if(!window.hasFocus() || event.type == sf::Event::GainedFocus) return true;

	if(GF::Mouse::Left.clicked(event) && car->isClicked())
		being_dragged = true;

	if(being_dragged && GF::Mouse::Left.isPressed() && car->isClicked())
		car->setPosition(GF::Mouse::getPosition(window));

	if(being_dragged && GF::Mouse::Wheel.moved(event))
		car->rotate(GF::Mouse::Wheel.delta(event) * 3.);

	if(GF::Mouse::Left.released(event))
		being_dragged = false;

	if(GF::Mouse::Left.isPressed() && !being_dragged && !button->isRolledOn(window)){
		track->addCircle(GF::Mouse::getPosition(window));
		track->saveAndFlip();
	}

	return true;
}

	// first thing to be called every frame
bool CarAI::onHandleEvent(GF::Event& event)
{
	static GF::Button<>* button = (GF::Button<>*)getWidget("Evolver_button");

	if(!ready_for_evolution)
		if(!handle_Track_Car_events(event)) return false;

	static GF::ToggleKey S (sf::Keyboard::S);
	if(S.isKeyReleasedOnce(event)){
		ga.step();
		reset();
	}

	if(button->isClicked(event, window)){
		ready_for_evolution = true;
		stop = ga.start();
		reset();
	}

	return true;
};

	// called every frame before draw
bool CarAI::onUpdate(const float fElapsedTime, const float fTotalTime)
{

	if(drawing && ready_for_evolution){
		// VecD out = ga.getBest().evaluate(VecD(AI->getSensors()) / SCREENWIDTH) * 2. - 1.;
		// AI->steer_gas(out[0], out[1]);
		// AI->update(1./240., fTotalTime);

		unsigned n = N; // counter for species inside the whole population
		unsigned iter = 0; // counter for individuals inside species
		unsigned a = 0; // counter for cars vector

		const Population<NEAT::Network>* pop = ga.getPopulation();

		// selects some networks
		while(n > pop->species.size()){
			for(unsigned i=0; i<pop->species.size(); ++i){
				if(pop->species[i]->bestRank == iter) continue;
				if(pop->species[i]->pop.size() <= iter) continue;
				if(cars[a]->alive){
					VecD out = pop->species[i]->pop[iter]->I.evaluate(VecD(cars[a]->getSensors()) / SCREENWIDTH) * 2. - 1.;
					cars[a]->steer_gas(out[0], out[1]);
					cars[a]->update(1./240., fTotalTime);
					cars[a]->t += fElapsedTime;
				}
				++a;
				--n;
			}
			++iter;
		}

			// selects best of each species, except for best species (for different color)
		for(unsigned i=0; i<pop->species.size() && n>1; ++i){
			if(pop->bestSpecies != i){
				if(cars[a]->alive){
					VecD out = pop->species[i]->getBest().evaluate(VecD(cars[a]->getSensors()) / SCREENWIDTH) * 2. - 1.;
					cars[a]->steer_gas(out[0], out[1]);
					cars[a]->update(1./240., fTotalTime);
					cars[a]->t += fElapsedTime;
				}
				++a;
			}
			--n;
		}

			// selects best Network
		VecD out = pop->getBest().evaluate(VecD(cars[a]->getSensors()) / SCREENWIDTH) * 2. - 1.;
		cars[a]->steer_gas(out[0], out[1]);
		cars[a]->update(1./240., fTotalTime);
		cars[a]->t += fElapsedTime;
		cars[a]->setColor(sf::Color::Green);
	}

		// if all cars died
	if(!drawing && ready_for_evolution){
		 // new generation
		stop = ga.step();
		drawing = true;
		reset();
	}

	return true;
};

	// last thing to be called every frame
bool CarAI::onDraw()
{
	if(drawing && ready_for_evolution){
		static Track* track = (Track*)getWidget("track");

		sf::Image &track_im = track->getTrack();

		// std::cout << "Start: " << track->start_pos() << "Rotation: " << track->start_rotation() << std::endl;
		// std::cout << "Position: " << AI->getPosition() << std::endl;
		// std::cout << "Checkpoint: " << AI->getCheckpoint() << std::endl;
		// std::cout << std::endl;

		// if(!alive_condition(track_im, track, AI)){
		// 	drawing = false;
		// 	if(stop != GA::StopReason::Undefined){
		// 		ga.finish(stop);
		// 		return false;
		// 	}
		// }

		// AI->draw()

		// becomes true if any car is alive
		bool alive = false; 
		for(unsigned i = 1; i < N; ++i){
			if(cars[i]->alive){
				cars[i]->alive = alive_condition(track_im, track, cars[i]);
				cars[i]->draw();
				alive |= cars[i]->alive;
			}
		}

		if(!alive){
			drawing = false;
			if(stop != GA::StopReason::Undefined){
				ga.finish(stop);
				return false;
			}
		}
		window.draw(network_im);
	}

	return true;
};

void CarAI::onDestroy()
{
	if(AI) delete AI;
	for(unsigned i = 0; i < cars.size(); ++i)
		delete cars[i];
}

void CarAI::reset(){
	calculate_size();
	static const Track* track = (Track*)getWidget("track");

	// AI->reset();
	// AI->setPosition(track->start_pos());
	// AI->setRotation(track->start_rotation());
	// AI->update(0., 0.);

	for(unsigned i = 0; i < N; ++i){
		if(cars[i]){
			cars[i]->reset();
			cars[i]->setPosition(track->start_pos());
			cars[i]->setRotation(track->start_rotation());
			cars[i]->update(0., 0.);
			cars[i]->setColor(sf::Color::White);
		}
	}

	image = new NEAT::Image(ga.getBest(),"params_image.txt");
	image->evolve();
	// image->draw(300. * SW, 200. * SH);
	network_im = image->makeImage(200 * SW, 200 * SH);
	network_im.setPosition(SCREENWIDTH - 220 * SW, 200 * SH);
}

void CarAI::calculate_size(){
	N = 0;
	const Population<NEAT::Network>* pop = ga.getPopulation();
	for(unsigned i=0; i<pop->species.size(); ++i)
		N += pop->species[i]->pop.size();

	N = std::min(50, (int)N);
}

