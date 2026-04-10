#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <array>
#include <ranges>
#include <print>

class Player {
public:

	friend std::ostream& operator<<(std::ostream& os, const Player& pl) {
		os << "이름:" << pl.name << ", 아이디:" << pl.id << ", 점수:" << pl.score << ", 자원수:" << pl.num << std::endl;
		os << "저장된 글자:" << pl.p.get() << std::endl;
		return os;
	}

	void read(std::istream& is) {
		is.read((char*)this, sizeof(Player));
		p.release();
		p = std::make_unique<char[]>(num + 1);
		is.read((char*)p.get(), num);
		p[num] = '\0';
	}

	void print() const {
		std::print("이름: {:15}, 아이디: {}, 점수: {}, 자원수: {}\n", name, id, score, num);
		std::print("저장된 글자: {}\n\n", p.get());
	}

private:
	std::string name; // 이름, 길이[3, 15], ['a', 'z']로만 구성
	int score; // 점수, 정규분포
	size_t id; // 아이디, 겹치는 아이디 있을 수 있음
	size_t num; // free store에 확보한 바이트 수
	std::unique_ptr<char[]> p; // free store에 확보한 메모리
};

// 2026 STL 과제 파일 - 2022180011
// - 파일에는 300'0000(3백만)개의 Player 객체가 기록되어 있다.파일은 바이너리(ios::binary)로 열었으며
// 다음과 같은 멤버함수 write를 사용하여 300만개의 Player 객체를 기록하였다. 
// void Player::write(ostream & os) {
//    os.write((char*)this, sizeof(Player));	// Player 객체의 멤버변수들을 Player 객체의 시작 주소로부터 sizeof(Player) 바이트만큼 파일에 기록한다.
//	  os.write((char*)p.get(), num);				// Player 객체의 p가 가리키는 메모리의 시작 주소로부터 num 바이트만큼 파일에 기록한다.
// }
//	*주의 - 이 과제에서 Player 객체는 추가되거나 삭제되지 않는다.
//		- 파일을 한 번만 읽고 과제를 해결하여야 한다.(메모리에 저장 후 해결)
//		- 문제를 순서대로 해결하여야 한다.

std::array<Player, 300'0000> players;

int main() {
	std::ifstream in("../2026 STL 과제/2026 STL 과제 파일 - 2022180011", std::ios::binary);
	if (not in) {
		std::cerr << "파일을 열 수 없습니다." << std::endl;
		return 1;
	}

	for (Player& p : players) {
		p.read(in);
	}

	(players.end() - 1)->print();
	std::cout << "-----------------------------\n";

	for (const Player& p : players | std::views::take(100)) {
		p.print();
	}

}