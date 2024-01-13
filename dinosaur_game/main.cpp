#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <iostream>
#include <fstream>

using namespace sf;

const int jumpPixel = 220;

int pixels = 0;
int frame = 0;
float gameSpeed = 10.;
int score = 0;

bool game = false;
bool gameover = false;
bool musicOn = false;

Texture DinoT;
Texture texture;
Texture End;
Texture CactusT;
Texture Restart;

SoundBuffer buffer;
Sound sound;
Music music;
Clock clock1;
Time elapsed1;

Font font;
Text scoreText("SCORE " + std::to_string(score), font, 30);

String playerInput;
Text playerText("", font, 30);
bool saveFlag = false;
std::ofstream saves;

class Cactus {
public:
	Sprite sprite;
	Cactus() {
		CactusT.loadFromFile("resources/cactus.png");
		sprite = Sprite(CactusT);
		sprite.setScale(2, 2);
		sprite.setPosition(-50, 250);
	}
	void update() {
		if (sprite.getPosition().x <= -25)
		{
			srand(time(NULL));
			sprite.setPosition(1200, 458);
			int random = rand() % 5;
			switch (random)
			{
			case 0: sprite.setTextureRect({ 0,4,25,53 }); break;
			case 1: sprite.setTextureRect({ 25,4,25,53 }); break;
			case 2: sprite.setTextureRect({ 50,4,25,53 }); break;
			case 3: sprite.setTextureRect({ 75,4,25,53 }); break;
			case 4: sprite.setTextureRect({ 100,4,28,53 }); break;
			}
		}
		sprite.move(-gameSpeed, 0.);
	}
};
class Dinosaur {
public:
	Sprite sprite;
	bool onGround = true;
	bool jump = false;

public:
	Dinosaur() {
		DinoT.loadFromFile("resources/dinosaur.png");
		sprite = Sprite(DinoT, { 0,0,44,47 });
		sprite.setScale(3, 3);
		sprite.setPosition(50, 430);
	}
	void checkCollision(Cactus cactus) {
		if (sprite.getGlobalBounds().intersects(cactus.sprite.getGlobalBounds())) {
			gameover = true;
			game = false;
			music.stop();
			musicOn = !musicOn;
		}
	}
	void jumpf() {

	}
};


int main() {
	RenderWindow window(VideoMode(1280, 720), "dinosaur_game");
	window.setFramerateLimit(60);

	Dinosaur dino;
	Cactus cactus;
	texture.loadFromFile("resources/DINO.png");
	End.loadFromFile("resources/gameover.png");
	Restart.loadFromFile("resources/restart.png");

	buffer.loadFromFile("resources/jump.mp3");
	sound.setBuffer(buffer);
	music.openFromFile("resources/st.wav");
	music.setLoop(true);

	font.loadFromFile("resources/ARCADECLASSIC.TTF");
	playerText.setString("Enter your name   ");

	scoreText.setFillColor(Color::Black);
	playerText.setFillColor(Color::Black);
	playerText.move(100, 100);
	elapsed1 = clock1.getElapsedTime();

	Sprite background(texture, { 2,58,600,10 });
	Sprite end(End);
	Sprite restart(Restart);

	background.setScale(6, 6);
	background.setPosition(0, 540);
	end.setPosition(315, 150);
	end.setScale(3, 3);
	restart.setPosition(546, 220);
	restart.setScale(3, 3);

	saves.open("results.txt");

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if ((event.key.code == Keyboard::Space) || ((event.type == Event::MouseButtonPressed) && (event.key.code == Mouse::Left)))
			{
				if (!game)
				{
					game = true;
					dino.sprite.setPosition(50, 430);
					cactus.sprite.setPosition(-50, 450);
					dino.sprite.setTextureRect({ 0,0,44,47 });
					pixels = 0;
					frame = 0;
					gameSpeed = 10;
					dino.jump = false;
					dino.onGround = true;
					window.draw(scoreText);
					score = 0;
					playerInput = "";
					saveFlag = true;
				}
				else if (dino.onGround)
				{
					dino.jump = true;
					dino.onGround = false;
					pixels = 0;
					dino.sprite.setTextureRect({ 88,0,44,47 });
					sound.play();
					if (!musicOn) {
						music.play();
						musicOn = !musicOn;
					}
				}
			}
		}

		if (!game)
		{
			window.clear(Color::White);
			window.draw(background);

			if (gameover)
			{
				window.draw(scoreText);
				window.draw(cactus.sprite);
				window.draw(dino.sprite);
				//window.draw(restart);
				if ((event.key.code == Keyboard::Enter) && saveFlag) {
					saves << playerInput.toAnsiString() << " " << score << std::endl;
					saveFlag = false;
				}
				if (event.type == sf::Event::TextEntered)
				{
					elapsed1 = clock1.getElapsedTime();
					if (elapsed1.asSeconds() > 0.1) {
						playerInput += event.text.unicode;
						clock1.restart();
					}
					playerText.setString("Enter your name   " + playerInput);
				}
				window.draw(playerText);
				//window.draw(end);
			}
			else
			{
				window.draw(dino.sprite);
			}
			window.display();
			continue;
		}


		if (dino.jump)
		{
			if (jumpPixel > pixels)
			{
				dino.sprite.move(0, (-jumpPixel / 25));
				pixels += jumpPixel / 25;
			}
			else
			{
				dino.jump = false;
				pixels = 0;
			}
		}
		else if (!dino.onGround)
		{
			if (jumpPixel > pixels)
			{
				dino.sprite.move(0, (jumpPixel / 25));
				pixels += jumpPixel / 25;
			}
			else
			{
				dino.onGround = true;
			}
		}
		else
		{
			if (frame <= 5)
			{
				dino.sprite.setTextureRect({ 88,0,44,47 });
				frame++;
			}
			else if (frame <= 10)
			{
				dino.sprite.setTextureRect({ 132,0,44,47 });
				frame++;
			}
			else frame = 0;
		}
		cactus.update();
		gameSpeed += 0.001;
		score += 1 * gameSpeed / 10;
		scoreText.setString("SCORE " + std::to_string(score / 10));
		dino.checkCollision(cactus);
		window.clear(Color::White);
		window.draw(background);
		window.draw(dino.sprite);
		window.draw(scoreText);
		window.draw(cactus.sprite);
		window.display();
	}
	saves.close();
	return 0;
}
