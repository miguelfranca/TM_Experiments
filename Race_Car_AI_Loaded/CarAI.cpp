#include "CarAI.h"

CarAI::CarAI(std::string t) : image(nullptr), AI(nullptr), pause(false), AI_NN(nullptr) { title = t; }

	// called once before the game starts
bool CarAI::onCreate()
{
	setClearColor(sf::Color::White);

	GF::Button<GF::Rectangle>* pause_button  = new GF::Button<GF::Rectangle>(&window, sf::Vector2f(100 * SW, 50 * SH), 
		sf::Color::White, sf::Color::Black, sf::Color::Black, sf::Color::White);
	pause_button->setText(std::string("Pause"));
	pause_button->setPos(sf::Vector2f(100 * SW, 50 * SH));
	addWidget(pause_button, "Pause_button");

	GF::Button<GF::Rectangle>* save_track_button  = new GF::Button<GF::Rectangle>(&window, sf::Vector2f(100 * SW, 50 * SH), 
		sf::Color::White, sf::Color::Black, sf::Color::Black, sf::Color::White);
	save_track_button->setTextSize(35 * SW);
	save_track_button->setText(std::string("Save Track"));
	save_track_button->setPos(sf::Vector2f(400 * SW, 50 * SH));
	addWidget(save_track_button, "save_track_button");

	Track* track = new Track(&window);
	addWidget(track, "track");

	AI = new Car(&window, TOPLEFT_F, 25 * SW, 50 * SH, track, "res/textures/car0.png");
	AI->update(0, 0);

	// User's car
	addWidget(new Car(&window, CENTER_WINDOW, 25 * SW, 50 * SH, track, "res/textures/car0.png"), "car");
	((Car*)getWidget("car"))->setColor(sf::Color::Blue);

	AI_NN = new NEAT::Network("BestCar.txt");
	std::ifstream f("BestCar.txt");
	if(f.good())
		AI_NN->read("BestCar.txt");
	else 
		exit(1);

	image = new NEAT::Image(*AI_NN, "params_image.txt");
	image->evolve();
	// image->draw(300. * SW, 200. * SH);
	network_im = image->makeImage(200 * SW, 200 * SH);
	network_im.setPosition(SCREENWIDTH - 220 * SW, 200 * SH);

	return true;
}

bool CarAI::handle_Track_Car_events(GF::Event &event){
	static Track* track = (Track*)getWidget("track");
	static GF::Button<>* save_track_button = (GF::Button<>*)getWidget("save_track_button");

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

	if(GF::Mouse::Left.isPressed() && !being_dragged && !track->isOverButton() 
		&& !save_track_button->isRolledOn(window))
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
	static Track* track = (Track*)getWidget("track");

	// event.showMessage();
	if(!pause)
		if(!handle_Track_Car_events(event)) return false;

	if(track->getMaxCheckpoint() > 1){

		if(save_track_button->isClicked(event, window)){
			track->save("track_" + std::to_string(time(NULL)) + "_" + std::to_string((int)clock()) + ".out");
		}

		if(pause_button->isClicked(event, window))
			pause = !pause;
	}
	return true;
}


	// called every frame before draw
bool CarAI::onUpdate(const float fElapsedTime, const float fTotalTime)
{
	static Track* track = (Track*)getWidget("track");

	// if(getFPS() < getMaxFPS() * 0.9) return true;

	if(track->getMaxCheckpoint() > 1){
		if(!pause){

		// selects best Network
			VecD out = AI_NN->evaluate(VecD(AI->getSensors()) * INPUT_FACTOR / SCREENWIDTH) * 2. - 1.;
			AI->steer_gas(out[0], out[1]);
			AI->update(fElapsedTime, fTotalTime);
			AI->t += fElapsedTime;
		}

	}
	return true;
};

	// last thing to be called every frame
bool CarAI::onDraw()
{
	static Track* track = (Track*)getWidget("track");

	sf::Image &track_im = track->getTrack();

		// std::cout << "Start: " << track->start_pos() << "Rotation: " << track->start_rotation() << std::endl;
		// std::cout << "Position: " << AI->getPosition() << std::endl;
		// std::cout << "Checkpoint: " << AI->getCheckpoint() << std::endl;
		// std::cout << std::endl;
	if(track->getMaxCheckpoint() > 1){
		if(!alive_condition(track_im, track, AI))
			reset();
	}
	AI->draw();	

	window.draw(network_im);

	return true;
};

void CarAI::onDestroy()
{
	if(AI) delete AI;
	if(image) delete image;
}

void CarAI::reset(){
	static const Track* track = (Track*)getWidget("track");
	AI->reset();
	AI->setPosition(track->start_pos());
	AI->setRotation(track->start_rotation());
	AI->update(0., 0.);
}


