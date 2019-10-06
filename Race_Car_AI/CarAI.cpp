#include "CarAI.h"

CarAI::CarAI(std::string t) : ga("params.txt"), image(nullptr), lastFitness(0.), AI(nullptr), pause(false) { title = t; }

	// called once before the game starts
bool CarAI::onCreate()
{
	setClearColor(sf::Color::White);

	GF::Button<GF::Rectangle>* pause_button  = new GF::Button<GF::Rectangle>(&window, sf::Vector2f(200, 100), 
		sf::Color::White, sf::Color::Black, sf::Color::Black, sf::Color::White);
	pause_button->setText(std::string("Pause"));
	pause_button->setPos(sf::Vector2f(200, 100));
	addWidget(pause_button, "Pause_button");

	GF::Button<GF::Rectangle>* evolver_button  = new GF::Button<GF::Rectangle>(&window, sf::Vector2f(200, 100), 
		sf::Color::White, sf::Color::Black, sf::Color::Black, sf::Color::White);
	evolver_button->setTextSize(35);
	evolver_button->setText(std::string("Evolver"));
	evolver_button->setPos(sf::Vector2f(800, 100));
	addWidget(evolver_button, "Evolver_button");

	GF::Button<GF::Rectangle>* save_track_button  = new GF::Button<GF::Rectangle>(&window, sf::Vector2f(200, 100), 
		sf::Color::White, sf::Color::Black, sf::Color::Black, sf::Color::White);
	save_track_button->setTextSize(35);
	save_track_button->setText(std::string("Save Track"));
	save_track_button->setPos(sf::Vector2f(1100, 100));
	addWidget(save_track_button, "save_track_button");

	GF::Button<GF::Rectangle>* save_nn_button  = new GF::Button<GF::Rectangle>(&window, sf::Vector2f(200, 100), 
		sf::Color::White, sf::Color::Black, sf::Color::Black, sf::Color::White);
	save_nn_button->setTextSize(35);
	save_nn_button->setText(std::string("Save NN"));
	save_nn_button->setPos(sf::Vector2f(1400, 100));
	addWidget(save_nn_button, "save_nn_button");


	Track* track = new Track(&window);
	addWidget(track, "track");

	// AI = new Car(&window, TOPLEFT_F, 40, 80, track, "res/textures/car0.png");
	for(unsigned i = 0; i < 50; ++i)
		cars.push_back(new Car(&window, TOPLEFT_F, 40, 80, track, "res/textures/car0.png"));


	// User's car
	addWidget(new Car(&window, CENTER_WINDOW, 40, 80, track, "res/textures/car0.png"), "car");
	((Car*)getWidget("car"))->setColor(sf::Color::Blue);

	// NEAT
	ga.track = track;
	ga.addCriteria(GA::StopReason::BestMaximum, {BEST_FITNESS});
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
	static GF::Button<>* save_track_button = (GF::Button<>*)getWidget("save_track_button");
	static GF::Button<>* save_nn_button = (GF::Button<>*)getWidget("save_nn_button");

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

	if(GF::Mouse::Left.isPressed() && !being_dragged && !button->isRolledOn(window) &&
		!track->isOverButton() && !save_track_button->isRolledOn(window) && !save_nn_button->isRolledOn(window))
	{
		track->addCircle(GF::Mouse::getPosition(window));
		track->saveAndFlip();
	}

	return true;
}

	// first thing to be called every frame
bool CarAI::onHandleEvent(GF::Event& event)
{
	static GF::Button<>* pause_button = (GF::Button<>*)getWidget("Pause_button");
	static GF::Button<>* save_track_button = (GF::Button<>*)getWidget("save_track_button");
	static GF::Button<>* save_nn_button = (GF::Button<>*)getWidget("save_nn_button");

	if(getFPS() < getMaxFPS() * 0.9) return true;

	// event.showMessage();

	if(!pause){
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
	}

	if(save_track_button->isClicked(event, window)){
		static Track* track = (Track*)getWidget("track");
		track->save("track_" + std::to_string(time(NULL)) + "_" + std::to_string((int)clock()) + ".png");
	}

	if(save_nn_button->isClicked(event, window)){
		std::string append = std::to_string(time(NULL)) + "_" + std::to_string((int)clock());
		ga.getBest().write("BestCar.txt");

		// sf::RenderTexture tt;
		// tt.create(1920, 1080);
		// tt.draw(image->makeImage(1920, 1080));
		// tt.getTexture().copyToImage().saveToFile("car_nn_image_" + append + ".png");
	}

	if(pause_button->isClicked(event, window))
		pause = !pause;

	return true;
}


	// called every frame before draw
bool CarAI::onUpdate(const float fElapsedTime, const float fTotalTime)
{
	// const float elapsedTime = 1./60.;

	if(getFPS() < getMaxFPS() * 0.9) return true;

	if(!pause){

		if(drawing && ready_for_evolution){
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
						VecD out = pop->species[i]->pop[iter]->I.evaluate(VecD(cars[a]->getSensors()) * INPUT_FACTOR / SCREENWIDTH) * 2. - 1.;
						cars[a]->steer_gas(out[0], out[1]);
						cars[a]->update(fElapsedTime, fTotalTime);
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
						VecD out = pop->species[i]->getBest().evaluate(VecD(cars[a]->getSensors()) * INPUT_FACTOR / SCREENWIDTH) * 2. - 1.;
						cars[a]->steer_gas(out[0], out[1]);
						cars[a]->update(fElapsedTime, fTotalTime);
						cars[a]->t += fElapsedTime;
					}
					++a;
				}
				--n;
			}

		// selects best Network
			VecD out = pop->getBest().evaluate(VecD(cars[a]->getSensors()) * INPUT_FACTOR / SCREENWIDTH) * 2. - 1.;
			cars[a]->steer_gas(out[0], out[1]);
			cars[a]->update(fElapsedTime, fTotalTime);
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
		for(unsigned i = 0; i < N; ++i){
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

	if(image) delete image;
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

	if(image) delete image;
	image = new NEAT::Image(ga.getBest(),"params_image.txt");
	image->evolve();
	// image->draw(300. * SW, 200. * SH);
	network_im = image->makeImage(200 * SW, 200 * SH);
	network_im.setPosition(SCREENWIDTH - 220 * SW, 200 * SH);

	unsigned count = 0;

	while(lastFitness == ga.getBestFitness() && count < 4 && ga.getBestFitness() < BEST_FITNESS){
		ga.step();
		count++;
	}

	lastFitness = ga.getBestFitness();
}

void CarAI::calculate_size(){
	N = 0;
	const Population<NEAT::Network>* pop = ga.getPopulation();
	for(unsigned i=0; i<pop->species.size(); ++i)
		N += pop->species[i]->pop.size();

	N = std::min(DRAW_SIZE, (int)N);
}

