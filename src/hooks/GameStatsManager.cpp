#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Macros.hpp>

#include "../ui/timely/RDTimelyNode.hpp"
#include "../ui/timely/RDDailyNode.hpp"
#include "../ui/timely/RDWeeklyNode.hpp"
#include "../ui/timely/RDEventNode.hpp"
#include "../ui/RDButton.hpp"

using namespace geode::prelude;

class $modify(MyGSM, GameStatsManager) {
	GJChallengeItem* getChallenge(int id) {
        	GJChallengeItem* challengeItem = GameStatsManager::getChallenge(id);

		Variables::didGetGJChallenge = true;
        	if (Variables::challengesPages.empty()) return challengeItem;

        	for (auto page : Variables::challengesPages) {
           		if (page && page->retainCount() > 0) {
                		page->release();
            		};
        	}
        	Variables::challengesPages.clear();

		return challengeItem;
	}
};
