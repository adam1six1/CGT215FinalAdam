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
		//velocity.x = 0.3;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Left)) {
		//velocity.x = -0.3;
	}
	else {
		velocity.x = 0;
	}
	velocity.y = player.getVelocity().y;

	player.setVelocity(velocity);

	if (Keyboard::isKeyPressed(Keyboard::Up)) {
		player.applyImpulse(Vector2f(0, -0.05));
	}
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
    World world(Vector2f(0, 0.6));
    int score(0);

	//Create Player
    PhysicsCircle player;
    player.setCenter(Vector2f(100, 250));
    player.setRadius(20);
	player.setRestitution(0.5);
	player.setFillColor(Color(114, 25, 90));
    world.AddPhysicsBody(player);

	// Create the floor
	PhysicsRectangle floor;
	floor.setSize(Vector2f(800, 100));
	floor.setCenter(Vector2f(400, 590));
	floor.setStatic(true);
	floor.setFillColor(Color(126, 127, 154));
	world.AddPhysicsBody(floor);

	//Create Celing
	PhysicsRectangle celing;
	celing.setSize(Vector2f(800, 40));
	celing.setCenter(Vector2f(400, 10));
	celing.setStatic(true);
	celing.setFillColor(Color(126, 127, 154));
	world.AddPhysicsBody(celing);

	//left wall
	PhysicsRectangle leftWall;
	leftWall.setSize(Vector2f(10, 600));
	leftWall.setCenter(Vector2f(-200, 300));
	leftWall.setStatic(true);
	leftWall .setFillColor(Color(0, 0, 0));
	world.AddPhysicsBody(leftWall);

	/*temporary floor
	PhysicsRectangle tempFloor;
	tempFloor.setSize(Vector2f(800, 40));
	tempFloor.setCenter(Vector2f(400, 300));
	tempFloor.setStatic(true);
	world.AddPhysicsBody(tempFloor);
	*/

	floor.onCollision = [&player]
	(PhysicsBodyCollisionResult result) {
		//cout << "hit" << endl;
		};

	PhysicsShapeList<PhysicsRectangle> platforms;
	PhysicsShapeList<PhysicsRectangle> coins;

	Font fnt;
	if (!fnt.loadFromFile("skeleboom.ttf")) {
		cout << "Could not load font." << endl;
		exit(3);
	}

	//setup timer
	Clock clock;
	Time lastTime(clock.getElapsedTime());
	Time currentTime(lastTime);

	//Platform spawn setup
	float platformHeight = 600; //platform spawn Y location
	float platformSpawn = 2.5; //platform spawn target time
	float platformTime = platformSpawn - 0.1;
	float platformSpeed = 3;

	float randomPosition = 100;
	bool gameOver = false;

	int points = 0;

	while (gameOver == false) {
		// calculate MS since last frame
		currentTime = clock.getElapsedTime();
		Time deltaTime = currentTime - lastTime;
		long deltaMS = deltaTime.asMilliseconds();
		if (deltaMS > 9) {
			lastTime = currentTime;
			world.UpdatePhysics(deltaMS);
			DoInput(player);

			platformTime += deltaTime.asSeconds();

			for (PhysicsShape& platform : platforms) {
				float newX = platform.getCenter().x;
				float currY = platform.getCenter().y;
				newX -= platformSpeed;
				platform.setCenter(Vector2f(newX, currY));
			}

			for (PhysicsShape& coin : coins) {
				float newX = coin.getCenter().x;
				float currY = coin.getCenter().y;
				newX -= platformSpeed;
				coin.setCenter(Vector2f(newX, currY));
			}

			player.onCollision =
				[&celing, &gameOver, &floor, &coins]
				(PhysicsBodyCollisionResult result) {
				if (
					(result.object2 == celing) || 
					(result.object2 == floor)
					) {
					gameOver = true;
				}
				};

			if (platformTime > platformSpawn) {
				platformTime = 0;
				
				randomPosition = randomFloat(100, 500);

				PhysicsRectangle& platformL = platforms.Create();
				platformL.setSize(Vector2f(100, 600));
				platformL.setCenter(Vector2f(900, randomPosition - 400));
				platformL.setFillColor(Color(202, 231, 185));
				platformL.setStatic(true);
				world.AddPhysicsBody(platformL);

				PhysicsRectangle& platformR = platforms.Create();
				platformR.setSize(Vector2f(100, 600));
				platformR.setCenter(Vector2f(900, randomPosition + 400));
				platformR.setFillColor(Color(202, 231, 185));
				platformR.setStatic(true);
				world.AddPhysicsBody(platformR);

				PhysicsRectangle& coin = coins.Create();
				coin.setSize(Vector2f(50, 50));
				coin.setCenter(Vector2f(900, randomPosition));
				coin.setFillColor(Color(243, 222, 138));
				coin.setStatic(true);
				world.AddPhysicsBody(coin);

				platformL.onCollision =
					[&celing, &world, &platforms, &platformL, &leftWall, &player, &gameOver]
					(PhysicsBodyCollisionResult result) {
					if (result.object2 == player) {
						gameOver = true;
					}
					else if (result.object2 == leftWall) {
						world.RemovePhysicsBody(platformL);
						platforms.QueueRemove(platformL);
					}
					};


				platformR.onCollision =
					[&celing, &world, &platforms, &platformR, &leftWall, &player, &gameOver]
					(PhysicsBodyCollisionResult result) {
					if (result.object2 == player) {
						gameOver = true;
					}
					else if (result.object2 == leftWall) {
						world.RemovePhysicsBody(platformR);
						platforms.QueueRemove(platformR);
					}
					};

				coin.onCollision =
					[&world, &leftWall, &player, &coin, &coins, &points]
					(PhysicsBodyCollisionResult result) {
					if (result.object2 == player) {
						world.RemovePhysicsBody(coin);
						coins.QueueRemove(coin);
						points += 1;
						cout << points << endl;
					}
					else if (result.object2 == leftWall) {
						world.RemovePhysicsBody(coin);
						coins.QueueRemove(coin);
					}
					};
			}
		}

		window.clear(Color(151, 167, 179));

		platforms.DoRemovals();
		for (PhysicsShape& platformL : platforms) {
			window.draw((PhysicsSprite&)platformL);
		}
		for (PhysicsShape& platformR : platforms) {
			window.draw((PhysicsSprite&)platformR);
		}

		coins.DoRemovals();
		for (PhysicsShape& coin : coins) {
			window.draw((PhysicsSprite&)coin);
		}

		window.draw(leftWall);
		window.draw(player);
		window.draw(floor);
		window.draw(celing);

		Text scoreText;
		scoreText.setString("Points: " + to_string(points));
		scoreText.setFont(fnt);
		scoreText.setPosition(Vector2f(20, 550));
		scoreText.setFillColor(Color(243, 222, 138));
		window.draw(scoreText);

		window.display();
	}
	cout << "gameOver" << endl;

	Text gameOverText;
	gameOverText.setString("GAME OVER");
	gameOverText.setFont(fnt);
	gameOverText.setPosition(200, 250);
	gameOverText.setCharacterSize(75);
	gameOverText.setFillColor(Color(114, 25, 90));
	window.draw(gameOverText);
	window.display();

	while (true) {

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
