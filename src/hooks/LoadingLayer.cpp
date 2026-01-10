#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <Macros.hpp>

#include "../Variables.hpp"
using namespace geode::prelude;

int thing = 0;

class $modify(LoadingLayer) {
    void loadAssets() {
        LoadingLayer::loadAssets();
		thing++;
		auto loader = Loader::get();
        if (thing == 8) {
            auto creatorLayer = CreatorLayer::create(); // phantom layer rip
        	Variables::PhantomCreatorLayer = creatorLayer; // thanks happy textures --raydeeux

			// thanks minecraftify
			RD_GET_SELECTOR(creatorLayer, "cvolton.betterinfo/main-button", Variables::BISelector);
			RD_GET_SELECTOR(creatorLayer, "spaghettdev.gd-roulette/roulette-button", Variables::RouletteSelector);
			RD_GET_SELECTOR(creatorLayer, "super-expert-button", Variables::SupExSelector);
			RD_GET_SELECTOR(creatorLayer, "demon-progression-button", Variables::GDDPSelector);
			RD_GET_SELECTOR(creatorLayer, "ungeil.higher_or_lower/HLButton", Variables::HigherOrLowerSelector);
			RD_GET_SELECTOR(creatorLayer, "gdsrwave.jfp/jfp-launch-button", Variables::JFPSelector);
        	RD_GET_SELECTOR(creatorLayer, "arcticwoof.rated_layouts/rated-layouts-button", Variables::RatedLayoutsSelector);
            if (loader->isModLoaded("rainixgd.geome3dash")) Variables::G3DSelector = menu_selector(CreatorLayer::onAdventureMap);

            if (loader->isModLoaded("gdutilsdevs.gdutils")) {
                creatorLayer->retain();
            }
        }
    }
};