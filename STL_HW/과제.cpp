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
//	  os.write((char*)p.get(), num);			// Player 객체의 p가 가리키는 메모리의 시작 주소로부터 num 바이트만큼 파일에 기록한다.
// }
//	*주의 - 이 과제에서 Player 객체는 추가되거나 삭제되지 않는다.
//		- 파일을 한 번만 읽고 과제를 해결하여야 한다.(메모리에 저장 후 해결)
//		- 문제를 순서대로 해결하여야 한다.

std::array<Player, 300'0000> players;
std::array<size_t, 300'0000> IdIndex;
std::array<size_t, 300'0000> NameIndex;
std::array<size_t, 300'0000> ScoreIndex;

static std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

void timerStart(std::string s);
void timerEnd();
void printPlayerShort(const Player& p);

int main() {
	std::ifstream in("2026 STL 과제 파일 - 2022180011", std::ios::binary);
	if (not in) {
		std::cerr << "파일을 열 수 없습니다." << std::endl;
		return 1;
	}

	// [문제1] 파일에 저장한 모든 Player 정보를 읽어 컨테이너에 저장하라.
	// 제일 마지막 Player의 정보를 화면에 출력하라.
	// - 어떤 방식으로 읽어 메모리에 저장했는지 보고서에 설명하라.
	std::for_each(players.begin(), players.end(), [&in](Player& p) {
		p.read(in);
		});
	std::println("[문제1] 마지막 Player 출력");
	std::cout << players.back();
	std::println("--------------------------------------------------");

	/*
	// [문제2] 점수가 가장 큰 Player를 찾아 화면에 출력하라. (동점 모두 출력)
	// Player의 평균 점수를 계산하여 화면에 출력하라.
	// - 어떻게 찾고 계산하였는지 보고서에 설명하라
	// 병렬, 순차 각각 시간측정. par과 par_unseq 둘다 해본 결과 par_unseq가 더 빠르게 나왔다. (par_unseq는 병렬과 벡터화 모두 지원하기 때문인듯)
	// 근데 par보다 순차가 더 빠르게 나오는 경우도 있었다. 이건 왜일까
	// 단순하게 탐색하는 작업이라 그런가?
	auto MaxScore = std::max_element(std::execution::par, players.begin(), players.end(),
		[](const Player& a, const Player& b) {
			return a.getScore() < b.getScore();
		});

	std::println("[문제2] 점수가 가장 큰 Player 출력 / 점수 : {}", MaxScore->getScore());
	std::for_each(std::execution::par, players.begin(), players.end(), [MaxScore](const Player& p) {
		if (p.getScore() == MaxScore->getScore()) {
			p.print();
		}
		});

	long long sum = std::accumulate(players.begin(), players.end(), 0LL,
		[](long long acc, const Player& p) {
			return acc + p.getScore();
		});
	std::println("[문제2] 평균 점수: {}", sum / players.size());
	std::println("--------------------------------------------------");

	// [문제3] id가 서로 같은 객체를 찾아 "같은아이디.txt"에 기록하라.
	// id가 같은 객체는 모두 몇 개인지 화면에 출력하라.
	// 파일에는 id가 같은 Player 객체의 이름과 아이디를 한 줄 씩 기록한다.
	// - 어떻게 같은 id를 찾았는지 보고서에 설명하라.

	// emplace_back과 push_back의 차이
	// - emplace_back은 내부에서 객체를 바로 생성. 임시객체 x
	// - push_back은 객체를 먼저 생성한 후 복사 또는 이동하여 컨테이너에 추가. 임시객체 o
	// 하지만 지금은 포인터를 저장하는 거라서 emplace_back과 push_back의 차이가 크지 않을 것 같다. 어차피 포인터는 복사할 때 큰 비용이 들지 않으니까

	std::println("[문제3] id가 같은 Player 기록 후 몇 개인지 출력하기");
	std::map<size_t, std::vector<Player*>> idMap;
	std::for_each(players.begin(), players.end(), [&idMap](Player& p) {
		idMap[p.getId()].push_back(&p);
		});

	std::println("id가 같은 Player 객체 찾기 완료");
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
	std::println("id가 같은 Player 객체 기록 완료");
	std::println("id가 같은 Player 객체 수: {}개", sameIdCount);
	std::println("--------------------------------------------------");


	// [문제4] Player의 멤버 p가 가리키는 메모리에는 파일에서 읽은 num개의 char가 저장되어 있어야 한다.
	// 메모리에 저장된 char를 오름차순으로 정렬하라.
	// '0' 부터 '9' 까지 모든 숫자가 있는 Player를 찾아 모두 몇 객체인지 출력하라.
	// - 어떻게 찾았는지 보고서에 설명하라.

	// 정렬작업 병렬과 순차 시간측정 -> 병렬이 10배가량 빠르게 측정되었음.
	// 병렬로 해도 상관이 없는게 각각의 Player 객체의 p가 가리키는 메모리는 서로 독립적이기 때문에 병렬로 정렬해도 문제가 없다. 판단
	//
	// std::ranges::includes를 활용해서 메모리가 0~9까지의 숫자를 모두 포함하는지 확인.
	// 병렬처리하는 반복문 내에 count를 증가시키는게 속도는 좀 빠르나 원자성 문제가 발생할 수 있기 때문에
	// std::atomic<int>로 count를 선언하여 원자적으로 증가하도록 하였다.
	// 안정적으로 밖에서 한번 더 std::for_each로 확인하는 방법도 있지만,
	// 그렇게 하면 두번 반복문을 돌게 되어서 시간이 더 오래 걸릴 것 같아서 원자적으로 증가시키는 방법을 선택하였다.
	// atomic으로 진행하는게 실제로 0.4~0.5초 정도 더 빠르게 측정되었다.
	// 하지만 계수정렬을 사용할 조건이 충분히 갖춰져 있기 때문에,
	// 계수정렬을 구현하여 정렬하는 방법도 고려해볼 수 있을 것 같다. 지금은 그냥 std::sort로 정렬하였음.
	//

	std::atomic<int> count{ 0 };
	std::println("[문제4] Player의 멤버 p가 가리키는 메모리에 저장된 char 정렬 / '0'부터 '9'까지 모든 숫자가 있는 Player 찾기");
	std::for_each(std::execution::par, players.begin(), players.end(), [&count](Player& p) {
		std::sort(p.getP(), p.getP() + p.getNum());		// 추후에 계수 정렬을 구현해보는 것도 좋을 것 같다. 지금은 그냥 std::sort로 정렬하였음.
		if (std::ranges::includes(p.getP(), p.getP() + p.getNum(), "0123456789", "0123456789" + 10)) {
			count++;
		}
		});
	std::println("'0'부터 '9'까지 모든 숫자가 있는 Player 수: {}개", count.load());
	*/

	// [문제5] [LOOP] id를 입력받아 존재하는 id라면 다음 내용을 한 번에 화면 출력하라
	// 프로그램은 5번을 무한히 반복할 수 있어야 한다.

	// id 기준 인덱스 정렬
	std::iota(IdIndex.begin(), IdIndex.end(), 0);
	std::sort(std::execution::par, IdIndex.begin(), IdIndex.end(), [](size_t a, size_t b) {
		return players[a].getId() < players[b].getId();
		});

	// name 기준 인덱스 정렬
	std::iota(NameIndex.begin(), NameIndex.end(), 0);
	std::sort(std::execution::par, NameIndex.begin(), NameIndex.end(), [](size_t a, size_t b) {
		return players[a].getName() < players[b].getName();
		});

	// score 기준 인덱스 정렬
	std::iota(ScoreIndex.begin(), ScoreIndex.end(), 0);
	std::sort(std::execution::par, ScoreIndex.begin(), ScoreIndex.end(), [](size_t a, size_t b) {
		return players[a].getScore() < players[b].getScore();
		});

	size_t inputId;
	while (true) {
		std::println("[문제5] id 입력받아 해당 id 포함 앞과 뒤 Player 정보 출력하기");
		std::cout << "id를 입력하세요 (종료 - EOF):";
		std::cin >> inputId;

		if (std::cin.eof()) {
			std::println("프로그램을 종료합니다.");
			break;
		}
		else if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::println("유효하지 않은 입력.");
			continue;
		}

		std::cout << '\n';

		// 모든 Player가 id 기준 오름차순으로 정렬되어 있는 상태에서
		// 해당 id 포함 앞과 뒤 Player의 정보를 출력한다.
		// id가 같은 Player가 둘 이상이면 이들의 정보를 모두 출력하여야 한다.
		std::println("=== ID가 같은 플레이어들 출력 ===");

		auto IdFirst = std::lower_bound(IdIndex.begin(), IdIndex.end(), inputId,
			[](size_t i, size_t target) { return players[i].getId() < target; });

		if (IdFirst == IdIndex.end() || players[*IdFirst].getId() != inputId) {
			std::println("해당 ID를 가진 플레이어가 없습니다.");
			continue;
		}

		auto IdLast = std::upper_bound(IdIndex.begin(), IdIndex.end(), inputId,
			[](size_t target, size_t i) { return target < players[i].getId(); });

		if (IdFirst != IdIndex.begin()) {
			std::cout << "[prev] ";
			players[*(IdFirst - 1)].PrintById();
		}

		for (auto i = IdFirst; i != IdLast; ++i) {
			std::cout << "[해당] ";
			players[*i].PrintById();
		}

		if (IdLast != IdIndex.end()) {
			std::cout << "[next] ";
			players[*IdLast].PrintById();
		}
		std::cout << '\n';

		// Player가 name 기준 오름차순으로 정렬되어 있는 상태에서
		// 해당 name 포함 앞과 뒤 Player의 정보를 출력한다.
		// 같은 name이 여럿일 경우 바로 앞뒤의 Player 정보만 출력하면 된다.
		std::println("=== Name이 같은 플레이어들 출력 ===");

		auto NameFirst = std::lower_bound(NameIndex.begin(), NameIndex.end(), players[*IdFirst].getName(),
			[](size_t i, const std::string& target) { return players[i].getName() < target; });

		auto NameLast = std::upper_bound(NameIndex.begin(), NameIndex.end(), players[*IdFirst].getName(),
			[](const std::string& target, size_t i) { return target < players[i].getName(); });

		if (NameFirst != NameIndex.begin()) {
			std::cout << "[prev] ";
			players[*(NameFirst - 1)].PrintByName();
		}

		for (auto i = NameFirst; i != NameLast; ++i) {
			if (players[*i].getId() == players[*IdFirst].getId()) {
				std::cout << "[해당] ";
			}
			players[*i].PrintByName();
		}

		if (NameLast != NameIndex.end()) {
			std::cout << "[next] ";
			players[*NameLast].PrintByName();
		}

		std::cout << '\n';

		// Player가 score 기준 오름차순으로 정렬되어 있는 상태에서
		// 해당 score 포함 앞과 뒤 Player의 정보를 출력한다.
		// 같은 score가 여럿일 경우 바로 앞뒤 한명의 Player 정보만 출력하면 된다.
		std::println("=== Score가 같은 플레이어들 출력 ===");

		auto ScoreFirst = std::lower_bound(ScoreIndex.begin(), ScoreIndex.end(), players[*IdFirst].getScore(),
			[](size_t i, int target) { return players[i].getScore() < target; });

		auto ScoreLast = std::upper_bound(ScoreIndex.begin(), ScoreIndex.end(), players[*IdFirst].getScore(),
			[](int target, size_t i) { return target < players[i].getScore(); });

		if (ScoreFirst != ScoreIndex.begin()) {
			std::cout << "[prev] ";
			players[*(ScoreFirst - 1)].PrintByScore();
		}

		for (auto i = ScoreFirst; i != ScoreLast; ++i) {
			if (players[*i].getId() == players[*IdFirst].getId()) {
				std::cout << "[해당] ";
			}
			players[*i].PrintByScore();
		}

		if (ScoreLast != ScoreIndex.end()) {
			std::cout << "[next] ";
			players[*ScoreLast].PrintByScore();
		}

		std::println("\n--------------------------------------------------\n");
	}
}

void timerStart(std::string s) {
	std::println("--------------------------------------------------");
	std::println("문제 {} 시작\n", s);
	startTime = std::chrono::high_resolution_clock::now();
}

void timerEnd() {
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::println("경과 시간: {}ms\n", duration.count());
	std::println("--------------------------------------------------");
}
