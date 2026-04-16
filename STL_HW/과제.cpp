#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <array>
#include <ranges>
#include <print>
#include <numeric>
#include <algorithm>
#include <execution>
#include <chrono>
#include <map>
#include "Player.h"

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

static std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

void timerStart(std::string s);
void timerEnd();

int main() {
	std::ifstream in("../2026 STL 과제/2026 STL 과제 파일 - 2022180011", std::ios::binary);
	if (not in) {
		std::cerr << "파일을 열 수 없습니다." << std::endl;
		return 1;
	}

	// [문제1] 파일에 저장한 모든 Player 정보를 읽어 컨테이너에 저장하라.
	// 제일 마지막 Player의 정보를 화면에 출력하라.
	std::for_each(players.begin(), players.end(), [&in](Player& p) {
		p.read(in);
		});
	std::cout << "[문제1] 마지막 Player 출력" << std::endl;
	players.back().print();
	std::cout << "--------------------------------------------------" << std::endl;

	// [문제2] 점수가 가장 큰 Player를 찾아 화면에 출력하라. (동점 모두 출력)
	// Player의 평균 점수를 계산하여 화면에 출력하라.
	// 병렬, 순회 각각 시간측정. par과 par_unseq 둘다 해본 결과 par_unseq가 더 빠르게 나왔다. (par_unseq는 병렬과 벡터화 모두 지원하기 때문인듯)
	// 근데 par보다 순회가 더 빠르게 나오는 경우도 있었다. 이건 왜일까
	auto MaxScore = std::max_element(std::execution::par_unseq, players.begin(), players.end(),
		[](const Player& a, const Player& b) {
			return a.getScore() < b.getScore();
		});

	std::cout << "[문제2] 점수가 가장 큰 Player 출력 / 점수 : " << MaxScore->getScore() << std::endl;
	std::for_each(std::execution::par, players.begin(), players.end(), [MaxScore](const Player& p) {
		if (p.getScore() == MaxScore->getScore()) {
			p.print();
		}
		});

	long long sum = std::accumulate(players.begin(), players.end(), 0LL,
		[](long long acc, const Player& p) {
			return acc + p.getScore();
		});
	std::cout << "[문제2] 평균 점수: " << sum / players.size() << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	// [문제3] id가 서로 같은 객체를 찾아 "같은아이디.txt"에 기록하라.
	// id가 같은 객체는 모두 몇 개인지 화면에 출력하라.
	// 파일에는 id가 같은 Player 객체의 이름과 아이디를 한 줄 씩 기록한다.

	// emplace_back과 push_back의 차이 
	// - emplace_back은 내부에서 객체를 바로 생성. 임시객체 x
	// - push_back은 객체를 먼저 생성한 후 복사 또는 이동하여 컨테이너에 추가. 임시객체 o
	// 하지만 지금은 포인터를 저장하는 거라서 emplace_back과 push_back의 차이가 크지 않을 것 같다. 어차피 포인터는 복사할 때 큰 비용이 들지 않으니까

	std::cout << "[문제3] id가 같은 Player 기록 후 몇 개인지 출력하기" << std::endl;
	std::map<size_t, std::vector<Player*>> idMap;
	std::for_each(players.begin(), players.end(), [&idMap](Player& p) {
		idMap[p.getId()].push_back(&p);
		});

	std::cout << "id가 같은 Player 객체 기록하기" << std::endl;
	std::ofstream out("같은아이디.txt");
	size_t sameIdCount = 0;
	std::for_each(idMap.begin(), idMap.end(), [&out, &sameIdCount](const auto& pair) {
		if (pair.second.size() > 1) {
			for (const auto* p : pair.second) {
				out << "이름: " << p->getName() << ", 아이디: " << p->getId() << '\n';
				sameIdCount++;
			}
		}
		});
	out.close();
	std::cout << "id가 같은 Player 객체 기록 완료" << std::endl;

	std::cout << "id가 같은 Player 객체 수: " << sameIdCount << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

}

void timerStart(std::string s) {
	std::cout << "--------------------------------------------------" << std::endl;
	std::println("문제 {} 시작\n", s);
	startTime = std::chrono::high_resolution_clock::now();
}

void timerEnd() {
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::println("경과 시간: {}ms\n", duration.count());
	std::cout << "--------------------------------------------------" << std::endl;
}