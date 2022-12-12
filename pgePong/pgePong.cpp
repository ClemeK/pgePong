#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
//#include "olcSoundWaveEngine.h"

#define OLC_SOUNDWAVE
#include "olcSoundWaveEngine.h"

#include <vector>

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name your application
		sAppName = "Pong";
	}

	olc::vi2d player1{ 10, (ScreenHeight() / 2) };
	olc::vi2d player2{ (ScreenWidth() - 10), (ScreenHeight() / 2) };

	int batSize{ 5 };

	int scorePlayer1{ 0 };
	int scorePlayer2{ 0 };

	struct sphere {
		int x;
		int y;
		float dx;
		float dy;
		int radius;
	};

	sphere ball;

	struct line {
		olc::vi2d start;
		olc::vi2d end;
	};

	std::vector <line> court{
		{{0,0},{ScreenWidth(),0}}, // Top
		{{0,ScreenHeight() - 1},{ScreenWidth(),ScreenHeight() - 1}}, // Bottom
		{{0,0},{0,1}}, // Top-Left
		{{ScreenWidth(),0},{ScreenWidth() - 1,1}}, // Top-Right
		{{0, ScreenHeight() - 1},{0,ScreenHeight() - 2}}, // Bottom-Left
		{{ScreenWidth() - 1,ScreenHeight() - 1},{ScreenWidth() - 1,ScreenHeight() - 2}}, // Bottom-Right
	};

	std::vector <line> goals{
		{{0,1}, {0,ScreenHeight() - 2}}, // Player 1 goal
		{{ScreenWidth() - 1,1}, {ScreenWidth() - 1,ScreenHeight() - 2}}, // Player 2 goal
	};

	// *****************************************************************
public:
	bool OnUserCreate() override
	{
		NewBall();
		ball.radius = 2;

		return true;
	}

	void NewBall() {
		// Reset to start location
		ball.x = (ScreenWidth() / 2);
		ball.y = (ScreenHeight() / 2);

		// Random Seed
		srand((unsigned int)time(NULL));

		// get random angel in degrees
		int angle = GetRandInt(1, 359);

		// convert the angle into radians and use as a velocity
		ball.dx = sin(angle * (22 / 7));
		ball.dy = cos(angle * (22 / 7));
		ball.radius = 2;
	}

	void DrawCourt()
	{
		// Draw the Court

		for (line l : court)
		{
			DrawLine(l.start, l.end, olc::WHITE);
		}

		// Centre Line
		DrawLine(ScreenWidth() / 2, 0, ScreenWidth() / 2, ScreenHeight() - 2, olc::WHITE, 0xF0F0F0F0);

		// Score Board
		std::string p1 = "P1: " + std::to_string(scorePlayer1);
		DrawString(10, 10, p1, olc::WHITE, 1);
		std::string p2 = "P2: " + std::to_string(scorePlayer2);
		DrawString(150, 10, p2, olc::WHITE, 1);

		// Draw the Goals - for Testing
		/*for (line l : goals)
		{
			DrawLine(l.start, l.end, olc::YELLOW);
		}*/
	}
	void MovePlayer1()
	{
		// Get Bats inputs
		// Left Player1
		if (GetKey(olc::Key::X).bHeld) // Down
		{
			player1.y = player1.y + 1;

			if (player1.y > ScreenHeight() - batSize)
			{
				player1.y = ScreenHeight() - batSize;
			}
		}

		if (GetKey(olc::Key::W).bHeld) // Up
		{
			player1.y = player1.y - 1;

			if (player1.y < batSize)
			{
				player1.y = batSize;
			}
		}

		// Draw the Bats
		DrawLine({ player1.x , player1.y - batSize }, { player1.x , player1.y + batSize }, olc::RED);
	}
	void MovePlayer2()
	{
		// Right Player2
		if (GetKey(olc::Key::M).bHeld) // Down
		{
			player2.y = player2.y + 1;

			if (player2.y > ScreenHeight() - batSize)
			{
				player2.y = ScreenHeight() - batSize;
			}
		}
		if (GetKey(olc::Key::U).bHeld) // Up
		{
			player2.y = player2.y - 1;

			if (player2.y < batSize)
			{
				player2.y = batSize;
			}
		}
		// Draw the Bats
		DrawLine({ player2.x , player2.y - batSize }, { player2.x , player2.y + batSize }, olc::BLUE);
	}

	int GetRandInt(int min, int max)
	{
		return (rand() % max) + min;
	}

	// vvvvvvvvvvvvvvvv Collision Detection vvvvvvvvvvvvvvvvvvvvvvvv
	bool pointCircle(float px, float py, float cx, float cy, float r) {
		// get distance between the point and circle's center
		// using the Pythagorean Theorem
		float distX = px - cx;
		float distY = py - cy;
		float distance = sqrt((distX * distX) + (distY * distY));

		// if the distance is less than the circle's
		// radius the point is inside!
		if (distance <= r) {
			return true;
		}
		return false;
	}

	float dist(float x1, float y1, float x2, float y2)
	{
		float distX = abs(x2 - x1);
		float distY = abs(y2 - y1);
		return sqrt((distX * distX) + (distY * distY));
	}

	bool linePoint(float x1, float y1, float x2, float y2, float px, float py)
	{
		// get distance from the point to the two ends of the line
		float d1 = dist(px, py, x1, y1);
		float d2 = dist(px, py, x2, y2);

		// get the length of the line
		float lineLen = dist(x1, y1, x2, y2);

		// since floats are so minutely accurate, add
		// a little buffer zone that will give collision
		float buffer = 0.1;    // higher # = less accurate

		// if the two distances are equal to the line's
		// length, the point is on the line!
		// note we use the buffer here to give a range,
		// rather than one #
		if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
			return true;
		}
		return false;
	}

	void Collided(float distX = 0, float distY = 0)
	{
		if (distX <= 0.0f)
		{
			ball.dx = ball.dx * -1;
		}
		if (distY <= 0.0f)
		{
			ball.dy = ball.dy * -1;
		}
	}

	bool CollisionDetection(line l, sphere b)
	{
		// from https://www.jeffreythompson.org/collision-detection/line-circle.php

		// If either end is inside, return true immediately and skip the rest.
		bool inside1 = pointCircle(l.start.x, l.start.y, b.x, b.y, b.radius);
		bool inside2 = pointCircle(l.end.x, l.end.y, b.x, b.y, b.radius);
		if (inside1 || inside2)
		{
			Collided();
			return true;
		}

		// get closest point on the line.
		float distX = l.start.x - l.end.x;
		float distY = l.start.y - l.end.y;
		float len = sqrt((distX * distX) + (distY * distY));

		// Get the dot product
		float dot = (((b.x - l.start.x) * (l.end.x - l.start.x)) + ((b.y - l.start.y) * (l.end.y - l.start.y))) / pow(len, 2);

		//  find the closest point on the line
		float closestX = l.start.x + (dot * (l.end.x - l.start.x));
		float closestY = l.start.y + (dot * (l.end.y - l.start.y));

		// If the point is on the line, we can keep going. If not, we can immediately return false,
		bool onSegment = linePoint(l.start.x, l.start.y, l.end.x, l.end.y, closestX, closestY);
		if (!onSegment) return false;

		// get the distance from the circle
		distX = closestX - b.x;
		distY = closestY - b.y;
		float distance = sqrt((distX * distX) + (distY * distY));

		if (distance <= b.radius)
		{
			Collided(distX, distY);
			return true;
		}

		return false;
	}
	// ^^^^^^^^^^^^^^^^ Collision Detection ^^^^^^^^^^^^^^^^^^^^^^^^

	void MoveBall(float fET)
	{
		// Move the Ball
		ball.x = ball.x + (ball.dx * fET);
		ball.y = ball.y + (ball.dy * fET);

		bool collision = false;
		// Wall Collision Detection
		for (auto l : court)
		{
			CollisionDetection(l, ball);
		}

		// Bat Collision
		CollisionDetection({ { player1.x , player1.y - batSize }, { player1.x , player1.y + batSize } }, ball);
		CollisionDetection({ { player2.x , player2.y - batSize }, { player2.x , player2.y + batSize } }, ball);

		// Draw the Ball
		FillCircle(ball.x, ball.y, ball.radius, olc::GREEN);
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		DrawCourt();

		Sleep(10);

		MovePlayer1();

		MovePlayer2();

		MoveBall(fElapsedTime);

		// Check if goal scored

		bool p2 = CollisionDetection({ { goals.at(0).start.x , goals.at(0).start.y - batSize }, { goals.at(0).start.x , goals.at(0).start.y + batSize } }, ball);
		bool p1 = CollisionDetection({ { goals.at(1).start.x , goals.at(1).start.y - batSize }, { goals.at(1).start.x , goals.at(1).start.y + batSize } }, ball);

		if (p2)
		{
			scorePlayer2 = scorePlayer2 + 1;
			NewBall();
		}

		if (p1)
		{
			scorePlayer1 = scorePlayer1 + 1;
			NewBall();
		}

		return true;
	}
};
// *****************************************************************
int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}