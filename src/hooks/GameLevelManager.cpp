#include <Geode/Geode.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include <Macros.hpp>

#include "../ui/timely/RDTimelyNode.hpp"
#include "../ui/timely/RDDailyNode.hpp"
#include "../ui/timely/RDWeeklyNode.hpp"
#include "../ui/timely/RDEventNode.hpp"
#include "../ui/RDButton.hpp"
using namespace geode::prelude;


class $modify(MyGLM, GameLevelManager) {
    void updateDailyTimer() {
        Variables::DailyLeft--;

        if (Variables::DailyLeft < 0) {
            CCScheduler::get()->unscheduleSelector(schedule_selector(MyGLM::updateDailyTimer), this);
            GameLevelManager::getGJDailyLevelState(GJTimedLevelType::Daily);
        }
    }

    void updateWeeklyTimer() {
        Variables::WeeklyLeft--;

        if (Variables::WeeklyLeft < 1) {
            CCScheduler::get()->unscheduleSelector(schedule_selector(MyGLM::updateWeeklyTimer), this);
            GameLevelManager::getGJDailyLevelState(GJTimedLevelType::Weekly);
        }
    }

    void updateEventTimer() {
        Variables::EventLeft--;

        // update event only happens if user is in menulayer
        if (auto scene = CCDirector::sharedDirector()->getRunningScene()) {
            if (auto layer = scene->getChildByType<MenuLayer>(0)) {
                if (Variables::EventLeft < 1 && Mod::get()->getSettingValue<bool>("events-check")) {
                    CCScheduler::get()->unscheduleSelector(schedule_selector(MyGLM::updateEventTimer), this);
                    GameLevelManager::getGJDailyLevelState(GJTimedLevelType::Event);
                }
            }
        }
    }

/*
    void onGetGJChallengesCompleted(gd::string response, gd::string tag) {
        GameLevelManager::onGetGJChallengesCompleted(response, tag);

        Variables::didGetGJChallenge = true;
        if (Variables::challengesPages.empty()) return;
        for (auto page : Variables::challengesPages) {
            if (page && page->retainCount() > 0) {
                page->release();
            };
        }
        Variables::challengesPages.clear();

    }
*/

    void getGJChallenges() {
        GameLevelManager::getGJChallenges();
        Variables::didGetGJChallenge = false;    
    }

    void onGetLeaderboardScoresCompleted(gd::string response, gd::string tag) {
        GameLevelManager::onGetLeaderboardScoresCompleted(response, tag);

        if (tag == "leaderboard_global") {
            if (response != "-1") {
                if (Variables::GlobalRank != -1) {
                    std::string responseStd = response;
                    std::string name = GJAccountManager::get()->m_username;
                    std::transform(responseStd.begin(), responseStd.end(), responseStd.begin(), [](unsigned char c) { return std::tolower(c); });
                    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return std::tolower(c); });
                    auto pos = responseStd.find(fmt::format("1:{}", name));
                    auto pos2 = responseStd.find("|", pos);
                    if (pos < responseStd.size()) {
                        if (pos2 >= responseStd.size()) {
                            pos2 = responseStd.size() - 1;
                        }
                        auto dict = GameLevelManager::responseToDict(responseStd.substr(pos, pos2 - pos), false);
                        Variables::GlobalRank = static_cast<CCString*>(dict->objectForKey("6"))->intValue();
                        Variables::OldStarsCount = GameStatsManager::sharedState()->getStat("6");

                        if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
                            if (auto button = typeinfo_cast<RDButton*>(layer->getChildByID("redash-menu"_spr)->getChildByID("main-menu"_spr)->getChildByID("leaderboards-button"))) {
                                button->updateLeaderboardLabel();
                            }
                        }
                    } else {
                        log::error("Failed to find player in leaderboard response");
                        log::error("response: {}", responseStd);
                        log::error("name: {}", name);
                        log::error("pos: {}; pos2: {}", pos, pos2);
                    }
                }
            } else {
                if (Variables::GlobalRank != -1) {
                    if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
                        if (auto button = typeinfo_cast<RDButton*>(layer->getChildByID("redash-menu"_spr)->getChildByID("main-menu"_spr)->getChildByID("leaderboards-button"))) {
                            button->getLeaderboardRankFailed();
                        }
                    }
                }
            }
        }
    }

	// /* 0x158 0x4 int */ int	m_dailyTimeLeft;
	// /* 0x15c 0x4 int */ int	m_dailyID;
	// /* 0x160 0x4 int */ int	m_activeDailyID;
	// /* 0x164 0x4 int */ int	m_weeklyTimeLeft;
	// /* 0x168 0x4 int */ int	m_weeklyID;
	// /* 0x16c 0x4 int */ int	m_activeWeeklyID;
	// /* 0x170 0x4 int */ int	m_eventTimeLeft;	
	// /* 0x174 0x4 int */ int	m_eventID;
	// /* 0x178 0x4 int */ int	m_activeEventID;

    void processOnDownloadLevelCompleted(gd::string response, gd::string tag, bool p2) {
        GameLevelManager::processOnDownloadLevelCompleted(response, tag, p2);

        if (response != "-1") {
            if (tag == "-1_0") {
                this->m_activeDailyID = this->m_dailyID;
            } else if (tag == "-2_0") {
                this->m_activeWeeklyID = this->m_weeklyID;
            }
        }

        if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
            if (tag == "-1_0") {
                RD_HANDLE_LEVEL("daily-node", this->m_activeDailyID);
            } else if (tag == "-2_0") {
                RD_HANDLE_LEVEL("weekly-node", this->m_activeWeeklyID);
            } else if (tag == "-3_0") {
                RD_HANDLE_LEVEL("event-node", this->m_activeEventID);
            }
        }
    }

    void onGetGJDailyLevelStateCompleted(gd::string response, gd::string tag) {
        GameLevelManager::onGetGJDailyLevelStateCompleted(response, tag);

        // log::warn("onGetGJDailyLevelStateCompleted: '{}', '{}'", response, tag);
        
        if (response != "-1") {
            auto responseStd = std::string(response.c_str());
            auto pos = responseStd.find('|') + 1;
            if (pos >= responseStd.size()) return;
            auto timeLeft = std::stoi(responseStd.substr(pos));
            
            if (tag == "daily_state") {
                if (this->m_activeDailyID == 0) {
                    this->downloadLevel(-1, false);
                }

                Variables::DailyLeft = timeLeft;
                CCScheduler::get()->scheduleSelector(schedule_selector(MyGLM::updateDailyTimer), this, 1.f, false);

                RD_HANDLE_GET_DAILY(RDDailyNode, "daily-node", this->m_dailyID, this->m_activeDailyID);
            } else if (tag == "weekly_state") {
                if (this->m_activeWeeklyID == 0) {
                    this->downloadLevel(-2, false);
                }
                
                Variables::WeeklyLeft = timeLeft;
                CCScheduler::get()->scheduleSelector(schedule_selector(MyGLM::updateWeeklyTimer), this, 1.f, false);

                RD_HANDLE_GET_DAILY(RDWeeklyNode, "weekly-node", this->m_weeklyID, this->m_activeWeeklyID);
            } else if (tag == "event_state") {
                if (this->m_activeEventID == 0) {
                    this->downloadLevel(-3, false);
                }

                Variables::EventLeft = timeLeft;
                CCScheduler::get()->scheduleSelector(schedule_selector(MyGLM::updateEventTimer), this, 1.f, false);

                // handleGetDaily("event-node", this->m_eventID, this->m_activeEventID);
                if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
                    if (auto node = typeinfo_cast<RDEventNode*>(layer->getChildByID("redash-menu"_spr)->getChildByID("dailies-menu"_spr)->getChildByID("event-node"))) {
                        if (node->m_skipButton) {
                            if (this->m_activeEventID < this->m_eventID && (node->m_currentLevel->m_normalPercent < 100 || GameStatsManager::sharedState()->hasCompletedDailyLevel(this->m_activeEventID))) {
                                node->m_skipButton->setVisible(true);
                            } else {
                                node->m_skipButton->setVisible(false);
                            }                                                                                                                                           
                        }                                                                                                                                               
                    }
                }
            }
        }
    }
};
