#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <array>
#include <vector>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <execution>
#include <unordered_map>
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

int main() {
	std::ifstream in("2026 STL 과제 파일 - 2022180011", std::ios::binary);
	if (not in) {
		std::cout << "파일을 열 수 없습니다.\n";
		return 1;
	}

	// [문제1] 파일에 저장한 모든 Player 정보를 읽어 컨테이너에 저장하라.
	// 제일 마지막 Player의 정보를 화면에 출력하라.
	// - 어떤 방식으로 읽어 메모리에 저장했는지 보고서에 설명하라.
	for (Player& p : players) {
		p.read(in);
	}
	std::cout << "[문제1] 마지막 Player 출력\n";
	std::cout << players.back();
	std::cout << "--------------------------------------------------\n";

	// [문제2] 점수가 가장 큰 Player를 찾아 화면에 출력하라. (동점 모두 출력)
	// Player의 평균 점수를 계산하여 화면에 출력하라.
	// - 어떻게 찾고 계산하였는지 보고서에 설명하라
	auto MaxScore = std::max_element(std::execution::par, players.begin(), players.end(),
		[](const Player& a, const Player& b) {
			return a.getScore() < b.getScore();
		});

	std::cout << "[문제2] 점수가 가장 큰 Player 출력 / 점수 : " << MaxScore->getScore() << '\n';

	int maxScore = MaxScore->getScore();
	for (const Player& p : players | std::views::filter([maxScore](const Player& p) {
		return p.getScore() == maxScore;
		})) {
		std::cout << p;
	}

	long long sum = std::accumulate(players.begin(), players.end(), 0LL,
		[](long long acc, const Player& p) {
			return acc + p.getScore();
		});

	std::cout << "[문제2] 평균 점수: " << sum / players.size() << '\n';
	std::cout << "--------------------------------------------------\n";

	// [문제3] id가 서로 같은 객체를 찾아 "같은아이디.txt"에 기록하라.
	// id가 같은 객체는 모두 몇 개인지 화면에 출력하라.
	// 파일에는 id가 같은 Player 객체의 이름과 아이디를 한 줄 씩 기록한다.
	// - 어떻게 같은 id를 찾았는지 보고서에 설명하라.

	std::cout << "[문제3] id가 같은 Player 기록 후 몇 개인지 출력하기\n";

	std::unordered_map<size_t, std::vector<Player*>> idMap;
	idMap.reserve(players.size());

	for (Player& p : players) {
		idMap[p.getId()].push_back(&p);
	}

	//std::cout << "id가 같은 Player 객체 찾기 완료\n";
	std::ofstream out("같은아이디.txt");
	size_t sameIdCount = 0;

	for (const auto& [id, vec] : idMap) {
		if (vec.size() > 1) {
			for (const Player* p : vec) {
				out << "이름: " << p->getName() << ", 아이디: " << p->getId() << '\n';
				sameIdCount++;
			}
		}
	}

	/*
	// id 기준 인덱스 정렬
	std::iota(idIndex.begin(), idIndex.end(), 0);
	std::sort(std::execution::par, idIndex.begin(), idIndex.end(), [](size_t a, size_t b) {
		return players[a].getId() < players[b].getId();
		});

	std::ofstream out("같은아이디.txt");
	size_t sameIdCount = 0;

	for (size_t i = 0; i < idIndex.size(); ) {
		size_t j = i + 1;
		while (j < idIndex.size() &&
			players[idIndex[j]].getId() == players[idIndex[i]].getId()) {
			++j;
		}

		if (j - i >= 2) {
			for (size_t k = i; k < j; ++k) {
				const Player& p = players[idIndex[k]];
				out << "이름: " << p.getName() << ", 아이디: " << p.getId() << '\n';
				sameIdCount++;
			}
		}

		i = j;  // 다음 그룹으로 점프
	}
	*/

	std::cout << "id가 같은 Player 객체 기록 완료\n";
	std::cout << "id가 같은 Player 객체 수: " << sameIdCount << "개\n";
	std::cout << "--------------------------------------------------\n";


	// [문제4] Player의 멤버 p가 가리키는 메모리에는 파일에서 읽은 num개의 char가 저장되어 있어야 한다.
	// 메모리에 저장된 char를 오름차순으로 정렬하라.
	// '0' 부터 '9' 까지 모든 숫자가 있는 Player를 찾아 모두 몇 객체인지 출력하라.
	// - 어떻게 찾았는지 보고서에 설명하라.

	// 병렬처리 약 1.6초. 순차처리는 약 15초로 측정.
	std::cout << "[문제4] Player의 멤버 p가 가리키는 메모리에 저장된 char 정렬 / '0'부터 '9'까지 모든 숫자가 있는 Player 찾기\n";

	/*
	std::for_each(std::execution::par, players.begin(), players.end(), [&count](Player& p) {
		std::sort(p.getP(), p.getP() + p.getNum());
		});

	size_t count = std::count_if(players.begin(), players.end(), [](const Player& p) {
		return std::ranges::includes(p.getP(), p.getP() + p.getNum(),
									 "0123456789", "0123456789" + 10);
		});
	*/

	std::for_each(std::execution::par, players.begin(), players.end(), [](Player& p) {
		// 계수정렬
		std::array<int, 128> cnt{};
		char* data = p.getP();
		size_t num = p.getNum();

		for (size_t i = 0; i < num; ++i) {
			cnt[data[i]]++;
		}

		char* out = data;
		for (int i = 0; i < 128; ++i) {
			out = std::fill_n(out, cnt[i], (char)i);
		}
		});

	size_t count = std::count_if(players.begin(), players.end(), [](const Player& p) {
		return std::ranges::includes(p.getP(), p.getP() + p.getNum(),
			"0123456789", "0123456789" + 10);
		});


	std::cout << "'0'부터 '9'까지 모든 숫자가 있는 Player 수: " << count << "개\n";
	std::cout << "--------------------------------------------------\n";

	// [문제5] [LOOP] id를 입력받아 존재하는 id라면 다음 내용을 한 번에 화면 출력하라
	// 프로그램은 5번을 무한히 반복할 수 있어야 한다.

	std::vector<const Player*> idIndex;
	idIndex.reserve(players.size());
	for (const Player& p : players) {
		idIndex.push_back(&p);
	}

	std::vector<const Player*> nameIndex = idIndex;
	std::vector<const Player*> scoreIndex = idIndex;

	std::sort(std::execution::par, idIndex.begin(), idIndex.end(),
		[](const Player* a, const Player* b) { return a->getId() < b->getId(); });

	std::sort(std::execution::par, nameIndex.begin(), nameIndex.end(),
		[](const Player* a, const Player* b) { return a->getName() < b->getName(); });

	std::sort(std::execution::par, scoreIndex.begin(), scoreIndex.end(),
		[](const Player* a, const Player* b) { return a->getScore() < b->getScore(); });

	size_t inputId;
	while (true) {
		std::cout << "[문제5] id 입력받아 해당 id 포함 앞과 뒤 Player 정보 출력하기\n";
		std::cout << "id를 입력하세요 (종료 - EOF): ";
		std::cin >> inputId;

		if (std::cin.eof()) {
			std::cout << "프로그램을 종료합니다.\n";
			break;
		}
		
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "유효하지 않은 입력.\n";
			continue;
		}

		std::cout << '\n';

		// ============================================================
		// 모든 Player가 id 기준 오름차순으로 정렬되어 있는 상태에서
		// 해당 id 포함 앞과 뒤 Player의 정보를 출력한다.
		// id가 같은 Player가 둘 이상이면 이들의 정보를 모두 출력하여야 한다.
		// ============================================================
		std::cout << "=== ID가 같은 플레이어들 출력 ===\n";

		auto idFirst = std::lower_bound(idIndex.begin(), idIndex.end(), inputId,
			[](const Player* p, size_t target) { return p->getId() < target; });

		if (idFirst == idIndex.end() || (*idFirst)->getId() != inputId) {
			std::cout << "해당 ID를 가진 플레이어가 없습니다.\n\n";
			continue;
		}

		auto idLast = std::upper_bound(idIndex.begin(), idIndex.end(), inputId,
			[](size_t target, const Player* p) { return target < p->getId(); });

		if (idFirst != idIndex.begin()) {
			std::cout << "[prev] ";
			(*(idFirst - 1))->Print(SortKey::id);
		}
		for (auto it = idFirst; it != idLast; ++it) {
			std::cout << "[해당] ";
			(*it)->Print(SortKey::id);
		}
		if (idLast != idIndex.end()) {
			std::cout << "[next] ";
			(*idLast)->Print(SortKey::id);
		}
		std::cout << '\n';

		// ============================================================
		// Player가 name 기준 오름차순으로 정렬되어 있는 상태에서
		// 해당 name 포함 앞과 뒤 Player의 정보를 출력한다.
		// 같은 name이 여럿일 경우 바로 앞뒤의 Player 정보만 출력하면 된다.
		// ============================================================
		std::cout << "=== Name이 같은 플레이어들 출력 ===\n";

		const std::string& targetName = (*idFirst)->getName();

		auto nameFirst = std::lower_bound(nameIndex.begin(), nameIndex.end(), targetName,
			[](const Player* p, const std::string& target) { return p->getName() < target; });

		auto nameLast = std::upper_bound(nameIndex.begin(), nameIndex.end(), targetName,
			[](const std::string& target, const Player* p) { return target < p->getName(); });

		if (nameFirst != nameIndex.begin()) {
			std::cout << "[prev] ";
			(*(nameFirst - 1))->Print(SortKey::name);
		}
		for (auto it = nameFirst; it != nameLast; ++it) {
			if ((*it)->getId() == inputId) {
				std::cout << "[해당] ";
			}
			(*it)->Print(SortKey::name);
		}
		if (nameLast != nameIndex.end()) {
			std::cout << "[next] ";
			(*nameLast)->Print(SortKey::name);
		}
		std::cout << '\n';

		// ============================================================
		// Player가 score 기준 오름차순으로 정렬되어 있는 상태에서
		// 해당 score 포함 앞과 뒤 Player의 정보를 출력한다.
		// 같은 score가 여럿일 경우 바로 앞뒤 한명의 Player 정보만 출력하면 된다.
		// ============================================================
		std::cout << "=== Score가 같은 플레이어들 출력 ===\n";

		int targetScore = (*idFirst)->getScore();

		auto scoreFirst = std::lower_bound(scoreIndex.begin(), scoreIndex.end(), targetScore,
			[](const Player* p, int target) { return p->getScore() < target; });

		auto scoreLast = std::upper_bound(scoreIndex.begin(), scoreIndex.end(), targetScore,
			[](int target, const Player* p) { return target < p->getScore(); });

		if (scoreFirst != scoreIndex.begin()) {
			std::cout << "[prev] ";
			(*(scoreFirst - 1))->Print(SortKey::score);
		}
		for (auto it = scoreFirst; it != scoreLast; ++it) {
			if ((*it)->getId() == inputId) {
				std::cout << "[해당] ";
			}
			(*it)->Print(SortKey::score);
		}
		if (scoreLast != scoreIndex.end()) {
			std::cout << "[next] ";
			(*scoreLast)->Print(SortKey::score);
		}

		std::cout << "\n--------------------------------------------------\n\n";
	}
}