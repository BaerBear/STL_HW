#include "Player.h"
#include <print>

void Player::read(std::istream& is)
{
	is.read((char*)this, sizeof(Player));
	p.release();
	p = std::make_unique<char[]>(num);
	is.read(p.get(), num);
}

void Player::Print(SortKey key) const 
{
	switch (key) {
	case SortKey::id:
		std::println("아이디 - {}, 이름: {}, 점수: {}, 자원수: {}, 글자(최대 10자): {}",
			id, name, score, num, std::string(p.get(), std::min<size_t>(10, num)));
		break;
	case SortKey::name:
		std::println("이름 - {}, 아이디: {}, 점수: {}, 자원수: {}, 글자(최대 10자): {}",
			name, id, score, num, std::string(p.get(), std::min<size_t>(10, num)));
		break;
	case SortKey::score:
		std::println("점수 - {}, 이름: {}, 아이디: {}, 자원수: {}, 글자(최대 10자): {}",
			score, name, id, num, std::string(p.get(), std::min<size_t>(10, num)));
		break;
	}
}

const std::string& Player::getName() const 
{ 
	return name; 
}

int Player::getScore() const 
{
	return score; 
}

size_t Player::getId() const 
{ 
	return id; 
}

size_t Player::getNum() const 
{ 
	return num; 
}

const char* Player::getP() const 
{ 
	return p.get(); 
}

char* Player::getP() 
{ 
	return p.get(); 
}

std::ostream& operator<<(std::ostream& os, const Player& pl)
{
	os << "이름:" << pl.name << ", 아이디:" << pl.id << ", 점수:" << pl.score << ", 자원수:" << pl.num << std::endl;
	os << "저장된 글자:" << std::string(pl.p.get(), pl.num) << std::endl;
	return os;
}
