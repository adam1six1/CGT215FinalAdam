#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>
#include <random>

using namespace std;
using namespace sf;
using namespace sfp;

void DoInput(PhysicsCircle& player) {
	Vector2f velocity = player.getVelocity();
	if (Keyboard::isKeyPressed(Keyboard::Right)) {
		velocity.x = 0.3;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Left)) {
		velocity.x = -0.3;
	}
	else {
		velocity.x = 0;
	}
	velocity.y = player.getVelocity().y;

	player.setVelocity(velocity);
}

float randomFloat(float min, float max) {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

int main()
{
	//create window
    RenderWindow window(VideoMode(800, 600), "Falling Ball");
    World world(Vector2f(0, 1));
    int score(0);

	//Create Player
    PhysicsCircle player;
    player.setCenter(Vector2f(400, 200));
    player.setRadius(20);
	player.setFillColor(Color(0, 217, 255));
    world.AddPhysicsBody(player);

	// Create the floor
	PhysicsRectangle floor;
	floor.setSize(Vector2f(800, 100));
	floor.setCenter(Vector2f(400, 590));
	floor.setStatic(true);
	floor.setFillColor(Color(255, 0, 0));
	world.AddPhysicsBody(floor);

	//Create Celing
	PhysicsRectangle celing;
	celing.setSize(Vector2f(800, 40));
	celing.setCenter(Vector2f(400, 10));
	celing.setStatic(true);
	celing.setFillColor(Color(255, 0, 0));
	world.AddPhysicsBody(celing);

	floor.onCollision = [&player]
	(PhysicsBodyCollisionResult result) {
		//cout << "hit" << endl;
		};

	PhysicsShapeList<PhysicsRectangle> platforms;

	//setup timer
	Clock clock;
	Time lastTime(clock.getElapsedTime());
	Time currentTime(lastTime);

	//Platform spawn setup
	float platformHeight = 600; //platform spawn Y location
	float platformSpawn = 1.5; //platform spawn target time
	float platformTime = platformSpawn;

	float randomPosition = 100;

	while (true) {
		// calculate MS since last frame
		currentTime = clock.getElapsedTime();
		Time deltaTime = currentTime - lastTime;
		long deltaMS = deltaTime.asMilliseconds();
		if (deltaMS > 9) {
			lastTime = currentTime;
			world.UpdatePhysics(deltaMS);
			DoInput(player);

			platformTime += deltaTime.asSeconds();

			for (PhysicsShape& platformL : platforms) {
				float newY = platformL.getCenter().y;
				float currX = platformL.getCenter().x;
				newY += -1;
				platformL.setCenter(Vector2f(currX, newY));
			}

			if (platformTime > platformSpawn) {
				platformTime = 0;
				
				randomPosition = randomFloat(60, 540);

				PhysicsRectangle& platformL = platforms.Create();
				platformL.setSize(Vector2f(800, 40));
				platformL.setCenter(Vector2f(randomPosition - 500, 600));
				platformL.setStatic(true);
				world.AddPhysicsBody(platformL);

				PhysicsRectangle& platformR = platforms.Create();
				platformR.setSize(Vector2f(800, 40));
				platformR.setCenter(Vector2f(randomPosition + 500, 600));
				platformR.setStatic(true);
				world.AddPhysicsBody(platformR);

				platformL.onCollision =
					[&celing, &world, &platforms, &platformL]
					(PhysicsBodyCollisionResult result) {
					if (result.object2 == celing) {
						world.RemovePhysicsBody(platformL);
						platforms.QueueRemove(platformL);
					}
					};


				platformR.onCollision =
					[&celing, &world, &platforms, &platformR]
					(PhysicsBodyCollisionResult result) {
					if (result.object2 == celing) {
						world.RemovePhysicsBody(platformR);
						platforms.QueueRemove(platformR);
					}
					};
			}
		}

		window.clear(Color(0, 0, 0));

		platforms.DoRemovals();
		for (PhysicsShape& platformL : platforms) {
			window.draw((PhysicsSprite&)platformL);
		}
		for (PhysicsShape& platformR : platforms) {
			window.draw((PhysicsSprite&)platformR);
		}

		window.draw(player);
		window.draw(floor);
		window.draw(celing);

		window.display();
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
