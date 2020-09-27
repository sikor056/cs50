#include "snake game.h"
#include <vector>
#include <array>
#include <iostream>
#include <vector>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <windows.h>
#include <conio.h>
#define GAME_SIZE 22
using std::cout;
using std::endl;
using std::vector;
typedef std::size_t size_type;
typedef std::pair<size_type,size_type> coord_t;
typedef std::vector<coord_t> coordlist_t;

enum Direction{Left, Right, Up, Down};

struct SnakePiece{
	unsigned char symbol = 'X';
	std::size_t x_coord = 0;
	std::size_t y_coord = 0;

	SnakePiece(){}
	SnakePiece(unsigned char s, size_t x, size_t y){
		this->symbol = s;
		this->x_coord = x;
		this->y_coord = y;
	}
};

struct Snake{
	static const unsigned char head_piece = 'X';
	static const unsigned char body_piece = 'O';
	SnakePiece head = SnakePiece(head_piece, GAME_SIZE / 2, GAME_SIZE / 2);
	vector<SnakePiece> body = { SnakePiece(body_piece, head.x_coord, head.y_coord + 1) ,
								SnakePiece(body_piece, head.x_coord, head.y_coord + 2) ,
								SnakePiece(body_piece, head.x_coord, head.y_coord + 3)
	};

	bool snake_is_at(size_t, size_t);
	bool collision();
	void grow(Direction);
	SnakePiece piece_at(size_t, size_t);
};

struct SnakeGame{
	static const unsigned char border = '#';
	static const unsigned char space = ' ';
	static const unsigned char food = '@';


	std::array<std::array<unsigned char, GAME_SIZE>,GAME_SIZE> grid;
	Snake snake = Snake();
	Direction dir = Left;
	unsigned int speed = 100;

	typedef decltype(grid)::size_type size_type;
	void print();
	void update_snake_position();
	coordlist_t possible_food_coords();
	SnakeGame();
};

bool Snake::snake_is_at(size_t x_coord, size_t y_coord){
	if (this->head.x_coord == x_coord && this->head.y_coord == y_coord){
		return true;
	}
	else{
		for (const SnakePiece& spiece : this->body){
			if (spiece.x_coord == x_coord && spiece.y_coord == y_coord){
				return true;
			}
		}
	}
	return false;
}

bool Snake::collision(){
	if (this->head.x_coord == 0 || this->head.x_coord == GAME_SIZE - 1 ||
		this->head.y_coord == 0 || this->head.y_coord == GAME_SIZE - 1){
			return true;
	}
	for (const SnakePiece& spiece : this->body){
		if (this->head.x_coord == spiece.x_coord && this->head.y_coord == spiece.y_coord){
			return true;
		}
	}
	return false;
}

void Snake::grow(Direction dir){
	const SnakePiece& spiece = this->body[this->body.size() - 1];
	SnakePiece new_piece = spiece;
	if (dir == Up){
		new_piece.x_coord -= 1;
	}
	else if (dir == Down){
		new_piece.x_coord += 1;
	}
	else if (dir == Left){
		new_piece.y_coord -= 1;
	}
	else if (dir == Right){
		new_piece.y_coord += 1;
	}
	else{
		exit(-2);
	}
	this->body.push_back(new_piece);
}

std::ostream& operator <<(std::ostream& output, const SnakePiece& spiece){return output << spiece.symbol;}

SnakePiece Snake::piece_at(size_t x_coord, size_t y_coord){
	if (this->head.x_coord == x_coord && this->head.y_coord == y_coord){
		return head;
	}
	else{
		for (const SnakePiece& spiece : this->body){
			if (spiece.x_coord == x_coord && spiece.y_coord == y_coord){
				return spiece;
			}
		}
	}
	return SnakePiece(body_piece, x_coord, y_coord);
}

SnakeGame::SnakeGame() : grid(decltype(grid)()){
	for (size_type index1 = 0; index1 < GAME_SIZE; ++index1){
		for (size_type index2 = 0; index2 < GAME_SIZE; ++index2){
			if (index1 == 0 || index1 == GAME_SIZE - 1 || index2 == 0 || index2 == GAME_SIZE - 1){
				grid[index1][index2] = border;
			}
			else{
				grid[index1][index2] = space;
			}
		}
	}
}

void SnakeGame::print(){
	for (std::size_t index1 = 0; index1 < GAME_SIZE; ++index1){
		for (std::size_t index2 = 0; index2 < GAME_SIZE; ++index2){
			if (index1 == 0 || index1 == GAME_SIZE - 1 || index2 == 0 || index2 == GAME_SIZE - 1){
				cout << border;
			}
			else if (snake.snake_is_at(index1,index2)){
				cout << snake.piece_at(index1, index2);
			}
			else{
				cout << grid[index1][index2];
			}
			cout << ' ';
		}
		cout << endl;
	}
}

void SnakeGame::update_snake_position(){
	vector<SnakePiece>& body = this->snake.body;
	SnakePiece& head_piece = this->snake.head;
	for (decltype(body.size()) index = body.size() - 1; index >= 1; --index){
		body[index].x_coord = body[index - 1].x_coord;
		body[index].y_coord = body[index - 1].y_coord;
	}
	body[0].x_coord = head_piece.x_coord;
	body[0].y_coord = head_piece.y_coord;
	if (this->dir == Left){
		head_piece.y_coord -= 1;
	}
	else if (this->dir == Right){
		head_piece.y_coord += 1;
	}
	else if (this->dir == Up){
		head_piece.x_coord -= 1;
	}
	else{///this->dir = down
		head_piece.x_coord += 1;
	}
}

coordlist_t SnakeGame::possible_food_coords(){
	coordlist_t result = {};
	for (size_type index1 = 1; index1 < GAME_SIZE - 1; ++index1){
		for (size_type index2 = 1; index2 < GAME_SIZE - 1; ++index2){
			if (!(this->snake.snake_is_at(index1,index2))){
				result.emplace_back(std::make_pair(index1,index2));
			}
		}
	}
	return result;
}

void play(){
	srand(time(nullptr));

	static const auto input_functor = [](SnakeGame& sgame, bool& running){
		while (running){
			unsigned char c = _getch();
			if (c == 'w'){
				sgame.dir = Up;
			}
			else if (c == 'a'){
				sgame.dir = Left;
			}
			else if (c == 's'){
				sgame.dir = Down;
			}
			else if (c == 'd'){
				sgame.dir = Right;
			}
			else if (c == 't'){
				exit(0);
			}
			else{
				continue;
			}
		}
	};

	bool running = true;
	SnakeGame game;
	std::string exit_message = "GAME OVER";
	{
		std::string user_input = "     ";
		while (user_input.size() != 1 || !('1' <= user_input[0] && user_input[0] <= '5')){
			cout << "Enter difficulty (1/2/3/4/5): ";
			getline(std::cin,user_input);
		}
		switch(user_input[0]){
			default:
			case ('1'):
				game.speed = 500;
				break;
			case ('2'):
				game.speed = 400;
				break;
			case ('3'):
				game.speed = 300;
				break;
			case ('4'):
				game.speed = 200;
				break;
			case ('5'):
				game.speed = 100;
				break;
		}
	}
	std::thread(input_functor, std::ref(game), std::ref(running)).detach();
	while (true){
		coordlist_t possible_food_locations = game.possible_food_coords();
		if (possible_food_locations.size() == 0){
			exit_message = "PERFECT GAME";
			break;
		}
		coord_t chosen_coords = possible_food_locations[rand() % possible_food_locations.size()];
		game.grid[chosen_coords.first][chosen_coords.second] = game.food;
		while (!game.snake.collision() &&
				(game.snake.head.x_coord != chosen_coords.first || game.snake.head.y_coord != chosen_coords.second)
			  ){
			game.update_snake_position();
			system("cls");
			game.print();
			std::this_thread::sleep_for(std::chrono::milliseconds(game.speed));
		}
		if (game.snake.collision()){
			game.grid[game.snake.head.x_coord][game.snake.head.y_coord] = game.snake.head.symbol;
			system("cls");
			game.print();
			break;
		}
		else if (game.snake.head.x_coord == chosen_coords.first && game.snake.head.y_coord == chosen_coords.second){
			game.snake.grow(game.dir);
		}
		else{
			exit(-1);
		}
		game.grid[chosen_coords.first][chosen_coords.second] = game.space;
	}
	running = false;
	cout << endl << exit_message << endl;
}







