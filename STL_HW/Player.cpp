#include "Player.h"
#include <iostream>
#include <print>


// 이동
Player::Player(Player&& other) noexcept
{
	this->name = std::move(other.name);
	this->score = other.score;
	this->id = other.id;
	this->num = other.num;
	this->p = std::move(other.p);
}

Player& Player::operator=(Player&& other) noexcept
{
	if (this == &other) {
		return *this;
	}

	this->name = std::move(other.name);
	this->score = other.score;
	this->id = other.id;
	this->num = other.num;
	this->p = std::move(other.p);

	return *this;
}

void Player::read(std::istream& is) 
{
	is.read((char*)this, sizeof(Player));
	p.release();
	p = std::make_unique<char[]>(num + 1);
	is.read(p.get(), num);
	p[num] = '\0';
}

void Player::PrintById() { 
	std::println("아이디 - {}, 이름: {}, 점수: {}, 자원수: {}, 글자(최대 5자): {}",
		id, name, score, num, std::string(p.get(), std::min<size_t>(5, num)));
}

void Player::PrintByName() { 
	std::println("이름 - {}, 아이디: {}, 점수: {}, 자원수: {}, 글자(최대 5자): {}",
		name, id, score, num, std::string(p.get(), std::min<size_t>(5, num)));
}

void Player::PrintByScore() { 
	std::println("점수 - {}, 이름: {}, 아이디: {}, 자원수: {}, 글자(최대 5자): {}",
		score, name, id, num, std::string(p.get(), std::min<size_t>(5, num)));
}

std::string Player::getName() const { return name; }
int Player::getScore() const { return score; }
size_t Player::getId() const { return id; }
size_t Player::getNum() const { return num; }
char* Player::getP() const { return p.get(); }

std::ostream& operator<<(std::ostream& os, const Player& pl) 
{
	os << "이름:" << pl.name << ", 아이디:" << pl.id << ", 점수:" << pl.score << ", 자원수:" << pl.num << std::endl;
	os << "저장된 글자:" << pl.p.get() << std::endl;
	return os;
}
