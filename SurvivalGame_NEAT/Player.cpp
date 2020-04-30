#include "GraphicsFramework/sfml.h" // vector operator <<
#include "Global2.h"

float ratio(const float ratio_double)
{
    return (float)pow(0.5, 1. / ratio_double);
}
const float difficulty_ratio = ratio(difficulty_double);
const float difficulty_ratio_armor = ratio(difficulty_armor);

Player::Player()
{
    respawn();
    shots = new Shot[max_shots];
};

void Player::respawn()
{
    setSpeed(speed_default);
    setActive(true);
    shape = Player::shape_default[0];
    setDamage(0.f);
    setHealth(getMaxHealth());

    dead_frame_count = 0;
}

float health_up(unsigned n)
{
    static float max_factor = Figure::getMaxHealth();
    static float min_factor = Bot::getHealthD();
    return (1.f - (float)pow(difficulty_ratio, n)) * (max_factor - min_factor) +
           min_factor;
};

void Player::setUpShot(float mouse_x, float mouse_y, float rect_x, float rect_y)
{

    static int shot_number = 0;

    shots[shot_number++].Shot::setUp(mouse_x, mouse_y, rect_x, rect_y);

    if (shot_number == max_shots)
        shot_number = 0;
}

void Player::drawShots(sf::RenderWindow &window)
{
    for (int i = 0; i < max_shots; i++)
    {
        shots[i].draw(window);
    }
}

bool Player::collide(Figure &bot)
{

    for (int i = 0; i < Player::max_shots; i++)
        if (Figure::collide(shots[i], bot))
        {
            kills++;
            money += (int)Bot::getHealthD();
            score += (int)Bot::getHealthD();
            Bot::setPeriodD(Bot::getPeriodD() * difficulty_ratio);
            Bot::setHealthD(health_up(kills));
            Shot::setDamageD(Shot::getDamageD() * difficulty_ratio);
        }

    return Figure::collide(bot, *this);
}

int round2nd(float x)
{
    int num = x < 10 ? 1 : (x < 100 ? 10 : (int)pow(10.f, (int)log10(x) - 1));
    return num * (int)(x / num);
}

int Player::increase(Player &player, int price)
{
    player.money -= price;
    return round2nd(price * price_increase);
}

int Player::increaseDamage(Player &player, int price)
{
    Shot::setDamageD(Shot::getDamageD() /
                     pow(difficulty_ratio, (float)price / Bot::getHealth0()));
    return increase(player, price);
}

int Player::increaseBPS(Player &player, int price)
{
    player.setPeriodD(player.getPeriodD() *
                      pow(difficulty_ratio, (float)price / Bot::getHealth0()));
    return increase(player, price);
}

int Player::Regenerate(Player &player, int price)
{
    player.setHealth(Player::getMaxHealth());
    return increase(player, price);
}

int Player::increaseArmor(Player &player, int price)
{
    Bot::setDamageD(Bot::getDamageD() * difficulty_ratio_armor);
    return increase(player, price);
}

void Player::move_shots(float fElapsedTime)
{
    for (int i = 0; i < max_shots; i++)
    {
        shots[i].move(fElapsedTime);
    }
}

void Player::draw(sf::RenderWindow &window, sf::Vector2f position, bool paused)
{
    // always draw, even if not active (dead)
    auto look_position = position;
    int index =
        (int)std::round(
            (atan2(look_position.y - getY(), look_position.x - getX()) + PI) *
            8.f / (2.f * PI)) %
        8;

    if (!dead_frame_count)
    {
        if (!paused)
            shape.sf::RectangleShape::setTexture(
                shape_default[index].getTexture());

        shape.draw(window);
    }
    else
    {
        MyShape dead_shape;
        dead_shape = shape_dead[dead_frame_count / dead_frame_duration];
        dead_shape.setPosition(shape.getPosition());
        // std::cout << dead_frame_count << std::endl;
        dead_shape.setSize(shape.getSize().x, shape.getSize().y);
        dead_shape.draw(window);
    }
}

std::vector<std::pair<float, float>> Player::closestBots(Figure *figs,
                                                         unsigned howmany)
{
    Bot *bots = (Bot *)figs;

    unsigned activeBots = 0;

    for (unsigned i = 0; i < max_bots; ++i)
        if (bots[i].getActive())
            ++activeBots;

    howmany = std::min(activeBots, howmany);

    std::vector<std::pair<float, float>> closest(activeBots);

    // std::cout << sf::Vector2f(getX(), getY()) << "\t";

    unsigned idx = 0;

    for (unsigned i = 0; i < max_bots; ++i)
    {
        if (!bots[i].getActive())
            continue;

        closest[idx].first =
            sqrt((bots[i].getX() - getX()) * (bots[i].getX() - getX()) +
                 (bots[i].getY() - getY()) * (bots[i].getY() - getY()));
        closest[idx].second =
            atan2((bots[i].getY() - getY()), (bots[i].getX() - getX()));

        if (closest[idx].second < 0)
            closest[idx].second += 2. * M_PI;

        // std::cout << closest[idx].first << "\t" << closest[idx].second <<
        // "\t" << sf::Vector2f(bots[i].getX(), bots[i].getY()) << "\t";
        ++idx;
    }

    // std::cout << std::endl;

    std::sort(closest.begin(), closest.end(),
              [](std::pair<float, float> a, std::pair<float, float> b) {
                  return a.first < b.first;
              });

    closest.resize(howmany);

    return closest;
}
