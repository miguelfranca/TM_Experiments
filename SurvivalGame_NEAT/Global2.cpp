#include "Global2.h"

// isto e uma merda porque os includes estao todos fdds nas classes inuteis do SurvivalGame.
// Isto faz com que o SW que esta no Global.cpp do Game_Framework seja definido depois dos cpps do SurvivalGame compilarem
// mete nojo basicamente. Nao me apetece estar a olhar para os includes merdosos
float SSW = sf::VideoMode::getDesktopMode().width / 1920.f; // scale width
float SSH = sf::VideoMode::getDesktopMode().height / 1080.f; // scale height


const MyShape Player::shape_default[8] = {MyShape(MEDIA_PATH + "/player_0.png", SSW * 2.5f, SSH * 2.5f),
                                          MyShape(MEDIA_PATH + "/player_1.png", SSW * 2.5f, SSH * 2.5f),
                                          MyShape(MEDIA_PATH + "/player_2.png", SSW * 2.5f, SSH * 2.5f),
                                          MyShape(MEDIA_PATH + "/player_3.png", SSW * 2.5f, SSH * 2.5f),
                                          MyShape(MEDIA_PATH + "/player_4.png", SSW * 2.5f, SSH * 2.5f),
                                          MyShape(MEDIA_PATH + "/player_5.png", SSW * 2.5f, SSH * 2.5f),
                                          MyShape(MEDIA_PATH + "/player_6.png", SSW * 2.5f, SSH * 2.5f),
                                          MyShape(MEDIA_PATH + "/player_7.png", SSW * 2.5f, SSH * 2.5f),
                                         };

const MyShape Player::shape_dead[4] = {MyShape(MEDIA_PATH + "/player_dead_1.png", SSW * 2.5f, SSH * 2.5),
                                       MyShape(MEDIA_PATH + "/player_dead_2.png", SSW * 2.5f, SSH * 2.5),
                                       MyShape(MEDIA_PATH + "/player_dead_3.png", SSW * 2.5f, SSH * 2.5),
                                       MyShape(MEDIA_PATH + "/player_dead_4.png", SSW * 2.5f, SSH * 2.5),
                                      };


const float boundary_y = SSH*178;
const float boundary_x = SSW*30;

const MyShape Shot::shape_default = MyShape(MEDIA_PATH + "/bullet.png", SSW * 0.15f, SSH * 0.15f);
const MyShape Bot::shape_default = MyShape(MEDIA_PATH + "/warrior.png", SSW * 0.7f, SSH * 0.7f);

const int Player::dead_frames_total = dead_frames * dead_frame_duration;
const int Player::dead_frame_duration = 10 * 30.f / delay;


const int Player::max_shots = 100;

const float Shot::speed_default = SSH * 1000.f; // 3d units/sec
const float Player::speed_default = SSH * 600.f; // 3d units/sec
const float Bot::speed_default = SSH * 400.f; // 3d units/sec

const float Figure::max_health = 100.f; //associated with: player - regeneration

const float Bot::dumbness = 0.3f; //sec

////////////////////////////////////////////////////////////////////////////////////////

const float Bot::period_0 = 1.f; //sec - difficulty
const float Player::period_0 = 0.1f;//sec - shot - speed

const float Shot::damage_0 = 10.; //shot - damage
const float Bot::damage_0 = 1.0f; //player - armor

const float Shot::health_0 = 1.e-5f; //shot - penetration
const float Bot::health_0 = 20; //difficulty

////////////////////////////////////////////////////////////////////////////////////////

float Bot::period_default = Bot::period_0;

float Shot::damage_default = Shot::damage_0;
float Bot::damage_default = Bot::damage_0;

float Shot::health_default = Shot::health_0;
float Bot::health_default = Bot::health_0;

const float Bot::damageRate = 0.1f; //sec
