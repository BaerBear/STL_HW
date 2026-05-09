#pragma once
#include<iostream>
#include <string>
#include <memory>

enum class SortKey { id, name, score };

class Player {
public:
	Player() = default;
	~Player() = default;

	void read(std::istream& is);

	void Print(SortKey key) const;

	const std::string& getName() const;
	int getScore() const;
	size_t getId() const;
	size_t getNum() const;
	const char* getP() const;
	char* getP();

	friend std::ostream& operator<<(std::ostream& os, const Player& pl);

private:

	std::string name;			// 이름, 길이[3, 15], ['a', 'z']로만 구성
	int score{};					// 점수, 정규분포
	size_t id{};					// 아이디, 겹치는 아이디 있을 수 있음
	size_t num{};					// free store에 확보한 바이트 수
	std::unique_ptr<char[]> p;  // free store에 확보한 메모리
};