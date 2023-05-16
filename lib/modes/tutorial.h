#pragma once

#include "lib/modes/common/scenario_base.h"


// チュートリアルのシナリオクラス
class Tutorial : public ScenarioBase {

public:

	Tutorial() : ScenarioBase() {
		for (int i = 0; i < MAX_SCENE_NUM; i++) {
			sceneList[i] = scenes[i];
		}
		btnReset.initialize(260, 440, "タイトル");
	}

private:
	Button btnReset;
	int strColorMenu = GetColor(255, 255, 255);

	int strColor = GetColor(255, 255, 255);

	struct Scene scenes[MAX_SCENE_NUM] = {
		{ SCENE_ACTION_MUSIC,	SCENE_WHO_DESC,		"swap" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"――世界は１つの部屋で出来ている" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"チュートリアルテスト" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_YELLOW,	"まあ" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_YELLOW,	"とにかく" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_YELLOW,	"バトるか" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"clear" },
		{ SCENE_ACTION_BATTLE,	SCENE_WHO_YELLOW,	"start" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_YELLOW,	"バトル画面やで" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"clear" },
		{ SCENE_ACTION_COCK,	SCENE_WHO_DESC,		"play_once" },
		{ SCENE_ACTION_PLAY,	SCENE_WHO_DESC,		"" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"相手の手番" },
		{ SCENE_ACTION_COCK,	SCENE_WHO_DESC,		"play_once" },
		{ SCENE_ACTION_PLAY,	SCENE_WHO_DESC,		"" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"あなたの手番" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"clear" },
		{ SCENE_ACTION_COCK,	SCENE_WHO_DESC,		"local_victory" },
		{ SCENE_ACTION_PLAY,	SCENE_WHO_DESC,		"" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"終わりまで続けましょう" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"clear" },
		{ SCENE_ACTION_COCK,	SCENE_WHO_DESC,		"victory" },
		{ SCENE_ACTION_PLAY,	SCENE_WHO_DESC,		"" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"終わり" },
		{ SCENE_ACTION_BATTLE,	SCENE_WHO_YELLOW,	"end" },
		{ SCENE_ACTION_EXIBIT,	SCENE_WHO_YELLOW,	"exibit_nowait" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_YELLOW,	"おかえり" },
		{ SCENE_ACTION_TALK,	SCENE_WHO_DESC,		"つづきはまだない" },
		{ -1,					-1,					"" },
	};

public:

	int show(UserInput& ui, Music& music) {
		return ScenarioBase::show(ui, music);
	}

	void debugDump() {
		int strColor = strColorDebug;
		Scene scene = getCurrentScene();

		DrawFormatString(365, 405, strColor, "tutoTrigger: %s", battle_trigger);
		DrawFormatString(365, 425, strColor, "tutoAction: %d", scene.action);

		game.debugDump();
	}

private:

	// override
	void setBattle(string how) {
		if (how == "start") {
			game.prepare(BATTLE_PLAYER_YELLOW, BATTLE_PLAYER_RED);
			game.setTutorial();
			onBattle = true;
		}
		else if (how == "end") {
			onBattle = false;
			initializeBattle();
		}
		goNext();
	}

	// override
	void doBattle(UserInput& ui) {
		ScenarioBase::doBattle(ui);

		// TODO: タイトル画面へ戻る
		if (reset(ui)) return;
		// return_flg = FLAG_TITLE;
	}

	bool reset(UserInput& ui) {
		btnReset.display(ui, strColorMenu);
		if (btnReset.isClicked(ui)) {
			ui.reset();
			game.reset();
			return true;
		}
		return false;
	}
};
