#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <float.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <mbstring.h>
#include <mbctype.h>
#include "lib/vect.h"

using namespace DxLib;
using namespace std;
#include "lib/const.h"
#include "lib/fireflower.h"
#include "lib/scenario.h"
#include "lib/ending.h"
#include "lib/game.h"

#pragma comment(lib, "winmm.lib")

int MultiByteLength(const char* String);
//VectorXd StateToInput(int dim, int side);
//VectorXd Reward1(const VectorXd &out, const VectorXd &in, int side);
//VectorXd softmax(const VectorXd &src, double alpha);


int Soloflg = 0;						// シナリオ管理用フラグ
int Scenflg = 0;						// シナリオ管理用フラグ

int COMGx = 1;
int COMGy = 1;
int COMLx = 1;
int COMLy = 1;							//COMの選ぶ座標
int COMWait = 0;
int waitOnCOM = 20;						//COMが手を打つまでのウェイト
int max_id = 0;
double max_val = 0.0;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetMainWindowText("Maxence 0.4.0");
	SetWindowIconID(101);
	if (DxLib_Init() == -1) {
		return -1;
	}
	SetDrawScreen(DX_SCREEN_BACK);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetBackgroundColor(0, 0, 0);	//背景色
	SetCameraNearFar(100.0, 6000.0);
	SetGlobalAmbientLight(GetColorF(1.0, 0.0, 0.0, 0.0));
	ChangeLightTypePoint(VGet(320.0, 240.0, -300.0), 2000.0, 0.0, 0.001f, 0.0);
	int LightHandle = CreateDirLightHandle(VGet(0.0, 0.0, -1.0));
	srand((unsigned)time(NULL));


	Game game;


	// 種々のハンドル
	unsigned int Green = GetColor(0, 255, 0);
	unsigned int Red = GetColor(255, 0, 0);
	unsigned int Blue = GetColor(0, 0, 255);
	unsigned int White = GetColor(255, 255, 255);
	unsigned int Black = GetColor(0, 0, 0);

	// フォント
	int Font0 = CreateFontToHandle("HG教科書体", 24, 3, DX_FONTTYPE_ANTIALIASING_EDGE);
	int Font1 = CreateFontToHandle("Times New Roman", 10, 1, -1);
	int Font2 = CreateFontToHandle("HG教科書体", 36, 4, DX_FONTTYPE_ANTIALIASING_EDGE);
	int Font3 = CreateFontToHandle("HG教科書体", 24, 3, DX_FONTTYPE_ANTIALIASING_EDGE);
	int Font4 = CreateFontToHandle("Times New Roman", 72, 6, DX_FONTTYPE_ANTIALIASING_EDGE);

	// 画像読み込み
	int MLogo = LoadGraph("graph/M.png");
	int axence = LoadGraph("graph/axence.png");
	int ClickToStart = LoadGraph("graph/click.png");
	int Logo0 = LoadGraph("graph/Maxence_after.png");
	int Logo1 = LoadGraph("graph/Maxence_after1.png");
	int Logo2 = LoadGraph("graph/Maxence_after2.png");
	int Logo3 = LoadGraph("graph/Maxence_after3.png");
	int Logo4 = LoadGraph("graph/Maxence_after4.png");
	int Room = LoadGraph("graph/room.bmp");
	int Card = LoadGraph("graph/card.bmp");
	int stone1 = LoadGraph("graph/stone1.png");
	int stone2 = LoadGraph("graph/stone2.png");
	int stone1_t = LoadGraph("graph/stone1.png");
	int stone2_t = LoadGraph("graph/stone2.png");
	int stripe[15];
	for (int i = 1; i <= 15; ++i) {
		string pict_name;
		pict_name = "graph/stripe" + to_string(i) + ".png";
		stripe[i - 1] = LoadGraph(pict_name.c_str());
	}
	int end_pict[20];
	for (int i = 1; i <= 20; ++i) {
		string pict_name;
		pict_name = "graph/end_pict" + to_string(i) + ".png";
		end_pict[i - 1] = LoadGraph(pict_name.c_str());
	}

	// 動画
	//int MovieGraphHandle = LoadGraph("movie/battle.ogv");

	// 3Dモデル関係
	/*
	int ModelHandle = MV1LoadModel("movie/max0.mv1");
	float totalTime, playTime = 0.0;
	MV1SetPosition(ModelHandle, VGet(80.0, 150.0, 100.0));
	MV1SetScale(ModelHandle, VGet(0.1, 0.1, 0.1));
	//MV1SetRotationXYZ(ModelHandle, VGet(0.0, -0.5 * DX_PI_F, 0.0));
	MV1SetRotationXYZ(ModelHandle, VGet(0.0, DX_PI_F, 0.0));
	//int AttachIndex = MV1AttachAnim(ModelHandle, 1, -1, FALSE);
	//totalTime = MV1GetAttachAnimTotalTime(ModelHandle, AttachIndex);
	//int GrHandle = MV1GetTextureGraphHandle(ModelHandle, 0);
	*/
	double theta = 0.3;

	fireflower tama[FIRE_FLOWER_NUM];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			game.child[i][j].stone1 = stone1;
			game.child[i][j].stone2 = stone2;
			game.child[i][j].stone1_t = stone1_t;
			game.child[i][j].stone2_t = stone2_t;
		}
	}

	//シナリオ関係
	int scen_char_cnt = 0;
	int scen_txt_cnt = 0;
	int scen_txt_len = 0;
	int eqx = 0; 
	int eqy = 0;
	int visible[5] = { 1,1,1,1,0 };
	string scen_txt[40];
	int scen_who[40];
	init_scene_text(scen_txt, scen_who);

	//エンディング関係
	int end_cnt = 0;
	string job[20];
	string who[20];
	init_ending_text(job, who);

	////学習関連
	//int train_game_cnt = 0;
	//int train_turn_cnt = 0;
	//int train_correct_cnt = 0;
	//int epic = 0;
	//int game_per_epic = 20;
	//int max_epic = 100;
	//double loss_per_epic[100] = { 0.0 };
	//int correct_per_epic[100] = { 0 };
	//int turn_per_epic[100] = { 0 };
	//double tmp_loss = 0.0;
	//int dbg_cnt = 0;

	////学習機械関連
	//int lay_len = 3;//6;
	//int lay_size[4] = { 162, 800, 400, 81 };
	////int lay_size[7] = { 162, 1000, 2000, 2000, 1000, 500, 81 };
	//VectorXd input(lay_size[0]);
	//VectorXd output;
	//VectorXd p_output;
	//double eps = 0.002; //学習定数
	//double gamma = 0.95; //割引率
	//double mom = 0.9;
	//double varc = 0.999;	// adamのパラメータ
	//VectorXd temp_i[100];
	//VectorXd temp_o[100];	//学習用データの一時保存用ベクトル
	//MatrixXd train_i;
	//MatrixXd train_o;		//バッチ学習用のデザイン行列
	double reward2, rwd_tmp;
	//double anl_rate = 0.0;	//epsilon-greedyの割合
	//double alpha = 2.1;	//softmaxの係数
	int anl_flg = 0;

	//// MLPの初期化
	//MatrixXd P1 = MatrixXd::Random(lay_size[0], lay_size[1]);
	//VectorXd B1 = VectorXd::Random(lay_size[1]);
	//MatrixXd P2 = MatrixXd::Random(lay_size[1], lay_size[2]);
	//VectorXd B2 = VectorXd::Random(lay_size[2]);
	//MatrixXd P3 = MatrixXd::Random(lay_size[2], lay_size[3]);
	//VectorXd B3 = VectorXd::Random(lay_size[3]);
	////MatrixXd P4 = MatrixXd::Random(lay_size[3], lay_size[4]);
	////VectorXd B4 = VectorXd::Random(lay_size[4]);
	////MatrixXd P5 = MatrixXd::Random(lay_size[4], lay_size[5]);
	////VectorXd B5 = VectorXd::Random(lay_size[5]);
	////MatrixXd P6 = MatrixXd::Random(lay_size[5], lay_size[6]);
	////VectorXd B6 = VectorXd::Random(lay_size[6]);
	//P1 = P1 * sqrt(0.1 / lay_size[0]);
	//B1 = B1 * 0.1;
	//P2 = P2 * sqrt(0.1 / lay_size[1]);
	//B2 = B2 * 0.1;
	//P3 = P3 * sqrt(0.1 / lay_size[2]);
	//B3 = B3 * 0.05;
	////P4 = P4 * sqrt(0.1 / lay_size[3]);
	////B4 = B4 * 0.05;
	////P5 = P5 * sqrt(0.1 / lay_size[4]);
	////B5 = B5 * 0.05;
	////P6 = P6 * sqrt(0.1 / lay_size[5]);
	////B6 = B6 * 0.05;
	//::Affine Q1(P1, B1, true, mom, varc);
	//::Affine Q2(P2, B2, true, mom, varc);
	//::Affine Q3(P3, B3, true, mom, varc);
	////::Affine Q4(P4, B4, true, mom, varc);
	////::Affine Q5(P5, B5, true, mom, varc);
	////::Affine Q6(P6, B6, true, mom, varc);
	//ActLayer R1("relu");
	//ActLayer R2("relu");
	////ActLayer R3("relu");
	////ActLayer R4("relu");
	////ActLayer R5("relu");
	//Machine critic(5, IdentityV, eps, "adam");
	////Machine critic(11, teacher, eps, "adam");
	//critic.setLayer(Q1, 0);
	//critic.setLayer(R1, 1);
	//critic.setLayer(Q2, 2);
	//critic.setLayer(R2, 3);
	//critic.setLayer(Q3, 4);
	////critic.setLayer(R3, 5);
	////critic.setLayer(Q4, 6);
	////critic.setLayer(R4, 7);
	////critic.setLayer(Q5, 8);
	////critic.setLayer(R5, 9);
	////critic.setLayer(Q6, 10);


	int vict = 0;	// 勝敗格納用の一時変数
	double logoX = 0.0;		// デモ画面用変数


	//メインループ
	while (!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()) {
		// 入力情報を取得
		game.key.update();
		game.mouse.update();

		// リセットボタンを表示する
		if (game.reset() == 1) {
			for (int i = 0; i < FIRE_FLOWER_NUM; ++i) {
				tama[i].initialize();
			}
		}
		game.drawLogo();

		// 設定を切り替える
		game.toggleByKey();
		if (game.option.soundFlg == 1) {
			tama[0].sound = 1;
		}
		else {
			tama[0].sound = 0;
		}

		// マウス操作か否かを判定する
		if (game.mouse.isUsed()) {
			game.keyboardFlg = 0;
		}
		if (game.key.isUsed()) {
			game.keyboardFlg = 1;
		}


		// OPアニメーション ClickToStartまで
		if (game.flg == -3){
			SetBackgroundColor(0, 0, 0);	//背景色
			if (logoX <= 120.0 ) {
				DrawExtendGraph(270, 170, 358, 260, MLogo, TRUE);
			}
			else if (logoX <= 200.0) {
				DrawExtendGraph(270 - 1.25*(logoX - 120), 170, 358 - 1.25*(logoX - 120), 260, MLogo, TRUE);
				DrawExtendGraph(350 - 1.25*(logoX - 120), 170, 358 + 1.65*(logoX - 120), 260, axence, TRUE);
			}
			else {
				DrawExtendGraph(170, 170, 258, 260, MLogo, TRUE);
				DrawExtendGraph(250, 170, 490, 260, axence, TRUE);
			}
			if (logoX >= 300.0) {
				DrawExtendGraph(200, 290, 460, 360, ClickToStart, TRUE);
			}
			if (logoX >= 300.0 && logoX <= 480.0) {
				DrawBox(0, 290, 655 - 4.0*(logoX - 300), 360, Black, TRUE);
				DrawBox(650 - 4.0*(logoX - 300), 342, 670 - 4.0*(logoX - 300), 345, White, TRUE);
			}
			logoX += 2.0;

			if (logoX > 480.0 || game.mouse.click() || game.key.onReturn()) {
				game.flg = -2;
				logoX = M_PI_2;
			}
		}
		// OPアニメーション ClickToStart点滅
		else if (game.flg == -2) {
			DrawExtendGraph(170, 170, 258, 260, MLogo, TRUE);
			DrawExtendGraph(250, 170, 490, 260, axence, TRUE);

			logoX = (logoX + 0.05); if (logoX >= 360.0) logoX -= 360.0;
			SetDrawBright(155 + 100*sin(logoX), 155 + 100*sin(logoX), 155 + 100*sin(logoX));
			DrawExtendGraph(200, 290, 460, 360, ClickToStart, TRUE);
			SetDrawBright(255, 255, 255);

			if (game.mouse.click() || game.key.onReturn()) {
				game.flg = -1;
				SetBackgroundColor(0, 128, 128);	//背景色
				SetDrawBright(255, 255, 255);
			}
		}
		// OPアニメーション メインロゴ
		else if (game.flg == -1) {
			if (logoX <= 37.5) {
				DrawExtendGraph(160, 170, 490, 260, Logo0, TRUE);
			}
			else if (logoX <= 45.0) {
				DrawExtendGraph(160, 170, 490, 260, Logo1, TRUE);
			}
			else if (logoX <= 50.0) {
				DrawExtendGraph(160, 170, 490, 260, Logo2, TRUE);
			}
			else if (logoX <= 55.0) {
				DrawExtendGraph(160, 170, 490, 260, Logo3, TRUE);
			}
			else if (logoX <= 90.0) {
				DrawExtendGraph(160, 170, 490, 260, Logo4, TRUE);
			}
			else {
				DrawExtendGraph(160 + (rand() % 11) - 5.0, 170, 485 + (rand() % 11) - 5.0, 260, Logo4, TRUE);
			}
			if(logoX < 1000.0) logoX += 1.0;
			if (logoX > 120 || game.mouse.click() || game.key.onReturn()) {
				game.flg = 0;
			}
		}
		// タイトル画面
		else if (game.flg == 0) {
			for (int i = 0; i < FIRE_FLOWER_NUM; ++i) {
				tama[i].draw();
				tama[i].tick();
			}
			DrawExtendGraph(160 + (rand() % 11) - 5.0, 170, 490 + (rand() % 11) - 5.0, 260, Logo4, TRUE);
			//タイトル画面その１
			if (game.isVsHuman()) {
				int choice = game.menuChoose();
				if (choice == 0) {
					game.mode = "ぼっちで";
					game.taijin = 1;
					game.setOrderMenu();
				}
				else if (choice == 1) {
					////PlayMovie("movie/battle.ogv", 1, DX_MOVIEPLAYTYPE_NORMAL);
					//PlayMovieToGraph(MovieGraphHandle);
					//SetBackgroundColor(0, 0, 0);
					//while (!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen() 
					//	&& GetMovieStateToGraph(MovieGraphHandle)){
					//	UpdateKey(Key);
					//	if (Key[KEY_INPUT_W] == 1) {
					//		//PauseMovieToGraph(MovieGraphHandle);
					//		break;
					//	}
					//	//DrawExtendGraph(0, 60, 640, 420, MovieGraphHandle, FALSE);
					//	WaitTimer(10);
					//}
					game.mode = "隣の人と";
					game.initialize();
					initializeTrain();
				}

				//オートモード
				game.toggleAutoLearning();
			}
			//タイトル画面その２（「ぼっちで」選択時）
			else if (game.isVsCOM()) {
				int choice = game.menuChoose();
				if (choice == 0) {
					game.goScenario();
					game.teban = 0;
				}
				else if (choice == 1) {
					game.goScenario();
					game.teban = 1;
				}
			}
		}
		// Game Loop
		else if (game.flg == 1) {
			// 盤面の描画
			//MV1DrawModel(ModelHandle);
			game.drawBase();
			game.drawGlobalState();
			game.drawHistLast();
			game.drawNextField();
			// プレイヤーの操作
			if (game.isPlayTurn() && game.playTurn()) {
				rwd_tmp = game.update();
				if (rwd_tmp > -10.0) {
					if (game.isVsCOM()) {
						COMWait = waitOnCOM;
						// reward2 = -rwd_tmp;
					}
				}
			}
			game.drawLocalState();
			game.drawCurrentCoord();

			// メッセージの描画
			game.drawBattleMessage();
			/*
			if (anl_flg) {
				DrawFormatString(540, 0, Green, "annealed!");
			}
			*/

			// 学習機械の出力描画
			if (game.option.likeliFlg >= 1) {
				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < 3; ++j) {
						for (int k = 0; k < 3; ++k) {
							for (int l = 0; l < 3; ++l) {
								/*
								double col_tmp = min(max(240.0 * (output(27 * i + 9 * j + 3 * k + l) + 0.5), 0.0), 255.0);
								DrawCircle(160 + 100 * i + 33 * k + 16, 80 + 100 * j + 33 * l + 16, 15, GetColor(255, 255, 255 - col_tmp));
								if (game.option.likeliFlg == 2) {
									char str_tmp[10];
									sprintf_s(str_tmp, "%.4f", output(27 * i + 9 * j + 3 * k + l));
									DrawStringToHandle(160 + 100 * i + 33 * k + 2, 80 + 100 * j + 33 * l + 2, str_tmp, Red, Font1);
									if (trainCnt >= 1) {
										sprintf_s(str_tmp, "%.4f", temp_o[trainCnt - 1](27 * i + 9 * j + 3 * k + l));
										DrawStringToHandle(160 + 100 * i + 33 * k + 2, 80 + 100 * j + 33 * l + 12, str_tmp, Blue, Font1);
									}
								}
								*/
							}
						}
					}
				}
			}

			// COMの手番
			if (!game.isPlayTurn()) {
				//input = StateToInput(lay_size[0], 1 - 2 * (game.cnt % 2));
				//output = critic.predict(input);
				//max_val = output.maxCoeff(&max_id);
				//while (COMWait <= 0) {
				//	if (unif(mt) < anl_rate) {
				//		COMGx = rand() % 3; COMGy = rand() % 3;
				//		COMLx = rand() % 3; COMLy = rand() % 3;
				//		comHistt[trainCnt] = COMGx * 27 + COMGy * 9 + COMLx * 3 + COMLy;
				//		anl_flg = 1;
				//	}
				//	else {
				//		COMGx = (max_id / 27) % 3;
				//		COMGy = (max_id / 9) % 3;
				//		COMLx = (max_id / 3) % 3;
				//		COMLy = max_id % 3;
				//		comHistt[trainCnt] = max_id;
				//		anl_flg = 0;
				//	}
				//	//盤面の更新
				//	rwd_tmp = game.update(COMGx, COMGy, COMLx, COMLy);
				//	if (rwd_tmp > -10.0) {
				//		temp_i[trainCnt] = input;
				//		temp_o[trainCnt] = Reward1(output, input, 1 - 2 * (game.cnt % 2));
				//		temp_o[trainCnt](comHistt[trainCnt]) = rwd_tmp;
				//		if (trainCnt >= 1) {
				//			if (game.vsCOM()) {
				//				temp_o[trainCnt - 1](comHistt[trainCnt - 1]) += gamma * max_val + reward2;
				//			}
				//			else if (game.taijin == 2) {
				//				temp_o[trainCnt - 1](comHistt[trainCnt - 1]) -= rwd_tmp;
				//				if (trainCnt >= 2) {
				//					temp_o[trainCnt - 2](comHistt[trainCnt - 2]) += gamma * max_val;
				//				}
				//			}
				//		}
				//		trainCnt++;
				//		if (game.taijin == 2) COMWait = waitOnCOM;
				//		break;
				//	}
				//}
			}

			// コメントの描画
			game.drawComment();

			// カットインアニメーション
			game.cutin.update();

			// 勝利判定
			vict = game.mother.victory();
			if (vict != 0) {
				game.goResult();
				game.key.initWait();
				//学習
				/*if (game.vsCOM()) {
					if (vict == teban * 2 - 1) {
						temp_o[trainCnt - 1](comHistt[trainCnt - 1]) = RWD_VICT;
					}else {
						temp_o[trainCnt - 1](comHistt[trainCnt - 1]) = -RWD_VICT;
					}
				}
				else if (game.taijin == 2) {
					temp_o[trainCnt - 2](comHistt[trainCnt - 2]) = -RWD_VICT;
					temp_o[trainCnt - 1](comHistt[trainCnt - 1]) = RWD_VICT;
				}
				train_i.setZero(trainCnt, lay_size[0]);
				train_o.setZero(trainCnt, lay_size[lay_len]);
				for (int i = 0; i < trainCnt; ++i) {
					train_i.block(i, 0, 1, lay_size[0]) = temp_i[i].transpose();
					train_o.block(i, 0, 1, lay_size[lay_len]) = temp_o[i].transpose();
				}
				if(!game.vsHuma()) critic.backprop(train_i, train_o);*/
			}

			// 動作の取り消し
			if (game.key.onBack() && game.goBackHist()) {
				/*
				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < 3; ++j) {
						for (int k = 0; k < 3; ++k) {
							for (int l = 0; l < 3; ++l) {
								input(27 * i + 9 * j + 3 * k + l) = game.child[i][j].state[k][l];
							}
						}
					}
				}
				output = critic.predict(input);
				*/
				COMWait = waitOnCOM;
			}

			// カメラ操作
			game.camera.set();
			if (game.mouse.click()) {
				game.mouse.set();
			}
			if (game.mouse.onClick()) {
				theta -= (game.mouse.distDragX()) * 0.05;
				game.mouse.set();
			}
			//MV1SetRotationXYZ(ModelHandle, VGet(0.0, theta + DX_PI_F, 0.0));

			// 永遠に勝敗がつかない場合の処理
			game.stopDrawGame();

			// 高速学習モードへの切り替え
			game.toggleHighSpeedLearning();

			// 対戦スキップ（一人用デバッグ）
			game.skipBattle(Scenflg);

		}
		// 勝敗表示
		else if (game.flg == 2) {
			// 盤面の描画
			//MV1DrawModel(ModelHandle);
			game.drawBase();
			game.drawGlobalState();
			game.drawHistLast();
			game.drawNextField();
			game.drawLocalState();

			// メッセージの描画
			game.drawWinner(vict);
			game.again.display(game.mouse, game.option.strColor);
			if (game.again.isClicked(game.mouse) || game.key.onReturn()){
				game.initialize();
				initializeTrain();
			}

			// 動作の取り消し
			if (game.key.onBack() && game.goBackHist()) {
				game.goBattle();
			}

			// カメラ操作
			if (game.mouse.click()) {
				game.mouse.set();
			}
			else if (game.mouse.onClick()) {
				game.camera.move(game.mouse.distDragX(), game.mouse.distDragY());
				game.mouse.set();
			}
			game.camera.zoom(game.mouse.wheel);

			// 自動学習モード
			if (game.taijin == 2) {
				game.initialize();
				initializeTrain();
			}
		}
		// Ending
		else if (game.flg == -4) {
			DrawBox(160, 80, 460, 380, GetColor(255, 255, 245), TRUE);
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					if (game.mother.state[i][j] == 1) {
						DrawBox(160 + 100 * i, 80 + 100 * j, 160 + 100 * (i + 1), 80 + 100 * (j + 1), GetColor(130, 70, 70), TRUE);
					}
					else if (game.mother.state[i][j] == -1) {
						DrawBox(160 + 100 * i, 80 + 100 * j, 160 + 100 * (i + 1), 80 + 100 * (j + 1), GetColor(70, 70, 130), TRUE);
					}
					else if (game.mother.state[i][j] != 0) {
						DrawBox(160 + 100 * i, 80 + 100 * j, 160 + 100 * (i + 1), 80 + 100 * (j + 1), GetColor(70, 130, 70), TRUE);
					}
					DrawBox(160 + 100 * i, 80 + 100 * j, 160 + 100 * (i + 1), 80 + 100 * (j + 1), Black, FALSE);
					if (game.nextField == 3 * i + j) {
						DrawBox(160 + 100 * i, 80 + 100 * j, 160 + 100 * (i + 1), 80 + 100 * (j + 1), Red, FALSE);
						DrawBox(160 + 100 * i + 1, 80 + 100 * j + 1, 160 + 100 * (i + 1) - 1, 80 + 100 * (j + 1) - 1, Red, FALSE);
					}
					else if (game.nextField == -1 && game.child[i][j].victory() == 0) {
						DrawBox(160 + 100 * i, 80 + 100 * j, 160 + 100 * (i + 1), 80 + 100 * (j + 1), Red, FALSE);
					}
					for (int k = 0; k < 3; ++k) {
						for (int l = 0; l < 3; ++l) {
							game.child[i][j].draw(176.5 + 100 * i, 96.5 + 100 * j, 33);
						}
					}
				}
			}
			game.logo.draw();

			double fade_tmp = min(255.0, 0.01 * pow(max(0.0, end_cnt - 50.0), 2.0));
			SetDrawBlendMode(DX_BLENDMODE_SUB, fade_tmp);
			DrawBox(-1, -1, 641, 481, GetColor(255, 255, 255), TRUE);
			if (end_cnt > 280 && end_cnt <= 900) {
				SetVolumeMusic(26.0 * pow(end_cnt - 250.0, 0.3));
				if (CheckMusic() != 1) PlayMusic("sound/deer.mp3", DX_PLAYTYPE_BACK);
			}
			if (end_cnt <= 300) {
				SetDrawBlendMode(DX_BLENDMODE_ADD, fade_tmp);
				DrawExtendGraph(170, 170, 255, 260, MLogo, TRUE);
				DrawExtendGraph(250, 170, 490, 260, axence, TRUE);
			}
			else if (end_cnt <= 550) {
				SetDrawBlendMode(DX_BLENDMODE_ADD, fade_tmp);
				DrawExtendGraph(170, 170 - 1.2 * (end_cnt - 300.0),
					255, 260 - 1.2 * (end_cnt - 300.0), MLogo, TRUE);
				DrawExtendGraph(250, 170 - 1.2 * (end_cnt - 300.0),
					490, 260 - 1.2 * (end_cnt - 300.0), axence, TRUE);
			}
			//スナップショット
			for (int i = 0; i < 8; ++i){
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				if (end_cnt > 640 * i + 550 && end_cnt <= 640 * (i + 1) + 550) {
					DrawExtendGraph(
						280, 
						480 - 1.2 * (end_cnt - 640 * i - 550),
						600, 
						720 - 1.2 * (end_cnt - 640 * i - 550),
						end_pict[2 * i], TRUE);
					DrawStringToHandle(
						20, 520 - 1.2 * (end_cnt - 640 * i - 550),
						job[2 * i].c_str(), White, Font2);
					DrawStringToHandle(
						20, 620 - 1.2 * (end_cnt - 640 * i - 550),
						who[2 * i].c_str(), White, Font3);
				}
				if (end_cnt > 640 * i + 870 && end_cnt <= 640 * (i + 1) + 870) {
					DrawExtendGraph(
						40, 
						480 - 1.2 * (end_cnt - 640 * i - 870),
						360, 
						720 - 1.2 * (end_cnt - 640 * i - 870),
						end_pict[2 * i + 1], TRUE);
					DrawStringToHandle(
						370, 520 - 1.2 * (end_cnt - 640 * i - 870),
						job[2 * i + 1].c_str(), White, Font2);
					DrawStringToHandle(
						370, 620 - 1.2 * (end_cnt - 640 * i - 870),
						who[2 * i + 1].c_str(), White, Font3);
				}
			}

			//Mr.K
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				//1人目
			if (end_cnt > 1650 && end_cnt <= 2500) {
				DrawGraph((end_cnt - 1750), 415, stripe[0], TRUE);
			}
				//2人目
			if (end_cnt > 2550){
				if (end_cnt <= 2900) {
					DrawGraph((end_cnt - 2650), 425, stripe[1], TRUE);
				}
				else if (end_cnt <= 2910) {
					DrawGraph(220 + 0.3 * pow(end_cnt - 2910, 2.0), 425, stripe[1], TRUE);
				}
				else if (end_cnt <= 2940) {
					DrawGraph(220 + (end_cnt - 2910), 425, stripe[1], TRUE);
				}
				else if (end_cnt <= 2950) {
					DrawGraph(220 + 0.3 * pow(end_cnt - 2950, 2.0), 425, stripe[1], TRUE);
				}
				else if (end_cnt <= 2980) {
					DrawGraph(220 + (end_cnt - 2950), 425, stripe[1], TRUE);
				}
				else if (end_cnt <= 2990) {
					DrawGraph(220 + 0.3 * pow(end_cnt - 2990, 2.0), 425, stripe[1], TRUE);
				}
				else if (end_cnt <= 3500) {
					DrawGraph(220 + (end_cnt - 2990), 425, stripe[1], TRUE);
				}
			}
				//3人目
			if (end_cnt > 3600) {
				if (end_cnt <= 3900) {
					DrawGraph((end_cnt - 3700), 420, stripe[0], TRUE);
				}
				else if (end_cnt <= 4100) {
					DrawGraph(200, 420, stripe[0], TRUE);
				}
				else if (end_cnt <= 4600) {
					DrawGraph(200, 420 - 1.2 * (end_cnt - 4100), stripe[0], TRUE);
				}
			}
				//4人目
			if (end_cnt > 4800) {
				if (end_cnt <= 5300) {
					DrawGraph((end_cnt - 4900), 395, stripe[2], TRUE);
				}
				else if (end_cnt <= 5450) {
					DrawGraph(400, 395, stripe[2], TRUE);
				}
				else if (end_cnt <= 5550) {
					if (end_cnt % 10 < 5) {
						DrawGraph(400, 395, stripe[2], TRUE);
					}
					else {
						DrawGraph(400, 395, stripe[3], TRUE);
					}
				}
				else {
					DrawGraph(400, 395, stripe[2], TRUE);
				}
			}

			//鹿
			if (end_cnt <= 830) {
				//SetDrawBlendMode(DX_BLENDMODE_ADD, fade_tmp);
				//DrawGraph(450, 380, end_str[4], TRUE);
				//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			else if (end_cnt <= 1200) {
				if (end_cnt % 60 < 15) {
					DrawGraph(650 - (end_cnt - 830), 380, stripe[4], TRUE);
				}
				else if (end_cnt % 60 < 30) {
					DrawGraph(650 - (end_cnt - 830), 380, stripe[5], TRUE);
				}
				else if (end_cnt % 60 < 45) {
					DrawGraph(650 - (end_cnt - 830), 380, stripe[6], TRUE);
				}
				else {
					DrawGraph(650 - (end_cnt - 830), 380, stripe[7], TRUE);
				}
			}
			else if (end_cnt <= 3900) {
				if (end_cnt % 60 < 15) {
					DrawGraph(280, 380, stripe[4], TRUE);
				}
				else if(end_cnt % 60 < 30) {
					DrawGraph(280, 380, stripe[5], TRUE);
				}
				else if (end_cnt % 60 < 45) {
					DrawGraph(280, 380, stripe[6], TRUE);
				}
				else {
					DrawGraph(280, 380, stripe[7], TRUE);
				}
			}
				//3人目
			else if (end_cnt <= 4550) {
				if (end_cnt <= 4200) {
					DrawGraph(280, 380, stripe[4], TRUE);
				}
				else {
					DrawGraph(280, 380, stripe[8], TRUE);
				}
			}
			else if (end_cnt <= 5300) {
				if (end_cnt % 60 < 15) {
					DrawGraph(280, 380, stripe[4], TRUE);
				}
				else if (end_cnt % 60 < 30) {
					DrawGraph(280, 380, stripe[5], TRUE);
				}
				else if (end_cnt % 60 < 45) {
					DrawGraph(280, 380, stripe[6], TRUE);
				}
				else {
					DrawGraph(280, 380, stripe[7], TRUE);
				}
			}
				//4人目
			else if (end_cnt <= 5600) {
				if (end_cnt <= 5450) {
					DrawGraph(280, 380, stripe[4], TRUE);
				}
				else if (end_cnt <= 5550) {
					DrawGraph(280, 380, stripe[9], TRUE);
				}
				else {
					DrawGraph(280, 380, stripe[4], TRUE);
				}
			}
			else  if (end_cnt <= 6000) {
				if (end_cnt % 40 < 10) {
					DrawGraph(280 - 1.2 * (end_cnt - 5600), 380, stripe[4], TRUE);
				}
				else if (end_cnt % 40 < 20) {
					DrawGraph(280 - 1.2 * (end_cnt - 5600), 380, stripe[5], TRUE);
				}
				else if (end_cnt % 40 < 30) {
					DrawGraph(280 - 1.2 * (end_cnt - 5600), 380, stripe[6], TRUE);
				}
				else {
					DrawGraph(280 - 1.2 * (end_cnt - 5600), 380, stripe[7], TRUE);
				}
			}
			//Fin
			if (end_cnt > 5680){
				if (end_cnt <= 5950) {
					DrawStringToHandle(
						270, 520 - 1.2 * (end_cnt - 5680),
						"Fin", White, Font4);
				}else{
					DrawStringToHandle(
						270, 520 - 1.2 * 270,
						"Fin", White, Font4);
				}
			}
			
			if(end_cnt <= 6500) end_cnt++;
			DrawFormatString(5, 25, game.option.strColor, "%d", end_cnt);
		}
		//高速学習モード
		//else if (game.flg == 5) {
		//	if (game.debugFlg == 1) {
		//		//デバッグ
		//		if (Key[KEY_INPUT_D] == 1) {
		//			if (dbg_cnt == 0) printf("%d\n", trainCnt);
		//			for (int j = 0; j < 9; ++j) {
		//				printf("\n");
		//				for (int k = 0; k < 9; ++k) {
		//					printf("%.3f  ", train_i(dbg_cnt, j * 9 + k));
		//					//printf("%.1f  ", temp_i[1](j));
		//				}
		//			}
		//			for (int j = 0; j < 9; ++j) {
		//				printf("\n");
		//				for (int k = 0; k < 9; ++k) {
		//					printf("%.3f  ", train_o(dbg_cnt, j * 9 + k));
		//					//printf("%.1f  ", temp_o[1](j));
		//				}
		//			}
		//			printf("\n");
		//			dbg_cnt++;
		//		}
		//		if (dbg_cnt >= trainCnt) {
		//			dbg_cnt = 0;
		//			debugFlg = 2;
		//		}
		//	}
		//	else if (game.debugFlg == 0) {
		//		//操作の処理
		//
		//		while (true) {
		//			//COMの手番
		//			input = StateToInput(lay_size[0], 1 - 2 * (game.cnt % 2));
		//			output = critic.predict(input);
		//			max_val = output.maxCoeff(&max_id);
		//			//COMGx = (max_id / 27) % 3; COMGy = (max_id / 9) % 3;
		//			//COMLx = (max_id / 3) % 3; COMLy = max_id % 3;
		//			p_output = softmax(output, alpha);
		//			double cum_tmp = 0.0;
		//			double ran_tmp = unif(mt);
		//			for (int k = 0; k < 81; ++k) {
		//				cum_tmp += p_output(k);
		//				if (ran_tmp <= cum_tmp) {
		//					COMGx = (k / 27) % 3; COMGy = (k / 9) % 3;
		//					COMLx = (k / 3) % 3; COMLy = k % 3;
		//					break;
		//				}
		//			}
		//			if (unif(mt) >= anl_rate) {
		//				//盤面の更新
		//				rwd_tmp = game.update(COMGx, COMGy, COMLx, COMLy);
		//				if (rwd_tmp > -10.0) {
		//					//comHistt[trainCnt] = max_id;
		//					comHistt[trainCnt] = COMGx * 27 + COMGy * 9 + COMLx * 3 + COMLy;
		//					train_correct_cnt++;
		//					correct_per_epic[epic]++;
		//				}
		//			}
		//			else {
		//				rwd_tmp = -100.0;
		//			}
		//			if (rwd_tmp < -10.0) {
		//				while (true) {
		//					COMGx = rand() % 3; COMGy = rand() % 3;
		//					COMLx = rand() % 3; COMLy = rand() % 3;
		//					//盤面の更新
		//					rwd_tmp = game.update(COMGx, COMGy, COMLx, COMLy);
		//					if (rwd_tmp > -10.0) {
		//						comHistt[trainCnt] = COMGx * 27 + COMGy * 9 + COMLx * 3 + COMLy;
		//						break;
		//					}
		//					//永遠に勝敗がつかない場合の処理
		//					game.stopDrawGame();
		//				}
		//			}
		//			temp_i[trainCnt] = input;
		//			temp_o[trainCnt] = Reward1(output, input, 1 - 2 * (game.cnt % 2));
		//			temp_o[trainCnt](COM_game.hist[trainCnt]) = rwd_tmp;
		//			//if (trainCnt >= 1) temp_o[trainCnt - 1](comHistt[trainCnt - 1]) -= rwd_tmp;
		//			if (trainCnt >= 2) temp_o[trainCnt - 2](comHistt[trainCnt - 2]) += gamma * max_val;
		//			trainCnt++;
		//			game.cnt++;
		//			train_turn_cnt++;
		//			turn_per_epic[epic]++;
		//
		//			//勝利判定
		//			vict = game.mother.victory();
		//			if (vict != 0) break;
		//
		//			//永遠に勝敗がつかない場合の処理
		//			game.stopDrawGame();
		//		}
		//		//学習
		//		temp_o[trainCnt - 2](comHistt[trainCnt - 2]) = -RWD_VICT;
		//		temp_o[trainCnt - 1](comHistt[trainCnt - 1]) = RWD_VICT;
		//		train_i.setZero(trainCnt, lay_size[0]);
		//		train_o.setZero(trainCnt, lay_size[lay_len]);
		//		for (int i = 0; i < trainCnt; ++i) {
		//			train_i.block(i, 0, 1, lay_size[0]) = temp_i[i].transpose();
		//			train_o.block(i, 0, 1, lay_size[lay_len]) = temp_o[i].transpose();
		//		}
		//		critic.backprop(train_i, train_o);
		//		tmp_loss = critic.loss(train_i, train_o);
		//		loss_per_epic[epic] += tmp_loss;
		//
		//		//game.debugFlg = 1;
		//		game.initialize(5);
		//
		//		//テキストの描画
		//		DrawFormatString(80, 400, StringColor, "試合数: %d", train_game_cnt);
		//		DrawFormatString(80, 424, StringColor, "試行手数: %d", train_turn_cnt);
		//		DrawFormatString(80, 448, StringColor, "正当率: %.2f%%", 100.0*(double)correct_per_epic[epic] / max(turn_per_epic[epic], 1));
		//		//DrawFormatString(240, 400, StringColor, "損失: %d", (int)tmp_loss);
		//		//DrawFormatString(240, 424, StringColor, "TPE: %d", turn_per_epic[epic]);
		//		DrawFormatString(240, 448, StringColor, "平均損失: %.2f", log(max(loss_per_epic[epic], 1.0)) - log(max(turn_per_epic[epic], 1.0)));
		//		DrawFormatString(40, 50, Red, "5.0");
		//		DrawFormatString(30, 350, Red, "-5.0");
		//		DrawFormatString(570, 50, Blue, "100%%");
		//		DrawFormatString(570, 350, Blue, "0%%");
		//
		//		train_game_cnt++;
		//		if (epic >= max_epic - 1) {
		//			if (train_game_cnt % game_per_epic == 0) {
		//				for (int k = 0; k < max_epic - 1; ++k) {
		//					loss_per_epic[k] = loss_per_epic[k + 1];
		//					correct_per_epic[k] = correct_per_epic[k + 1];
		//					turn_per_epic[k] = turn_per_epic[k + 1];
		//				}
		//				loss_per_epic[max_epic - 1] = 0;
		//				correct_per_epic[max_epic - 1] = 0;
		//				turn_per_epic[max_epic - 1] = 0;
		//				epic = max_epic - 1;
		//			}
		//		}
		//		else {
		//			epic = train_game_cnt / game_per_epic;
		//		}
		//
		//		for (int l = 0; l < 51; ++l) { DrawLine(70, 50 + 6 * l, 570, 50 + 6 * l, Black); }
		//		for (int l = 0; l < 11; ++l) { DrawLine(70, 50 + 30 * l + 1, 570, 50 + 30 * l + 1, Black); }
		//		for (int k = 1; k < max_epic; ++k) {
		//			DrawLine(500.0*(k - 1) / max_epic + 70,
		//				350.0 - 300.0*correct_per_epic[k - 1] / max(turn_per_epic[k - 1], 1),
		//				500.0*k / max_epic + 70,
		//				350.0 - 300.0*correct_per_epic[k] / max(turn_per_epic[k], 1),
		//				Blue);
		//			DrawLine(500.0*(k - 1) / max_epic + 70,
		//				350.0 - 30.0 * (5.0 + log(max(loss_per_epic[k - 1], 1.0)) - log(max(turn_per_epic[k - 1], 1))),
		//				500.0*k / max_epic + 70,
		//				350.0 - 30.0 * (5.0 + log(max(loss_per_epic[k], 1.0)) - log(max(turn_per_epic[k], 1))),
		//				Red);
		//		}
		//
		//		if (Key[KEY_INPUT_A] == 1) {
		//			game.mouse.set();
		//			game.goTitle();
		//			game.taijin = 0;
		//			for (int i = 0; i < 3; ++i) tama[i].initialize();
		//		}
		//
		//	}
		//	else if(debugFlg == 2){
		//		game.initialize(5);
		//		game.debugFlg = 0;
		//	}
		//}

		// シナリオ
		else if (game.flg == -6) {
			if (Soloflg == 0) {
				DrawExtendGraph(0 + eqx, -50, 640 + eqx, 380, Room, FALSE);
				if (visible[0]) DrawGraph(160 + eqx, 120, stripe[10], TRUE);
				if (visible[1]) DrawGraph(480 + eqx, 120, stripe[11], TRUE);
				if (visible[2]) DrawGraph(160 + eqx, 240, stripe[12], TRUE);
				if (visible[3]) DrawGraph(480 + eqx, 240, stripe[13], TRUE);
				if (Scenflg == 2) DrawExtendGraph(0, 0, 640, 400, Card, FALSE);
				DrawRoundBox(15, 380, 10, 609, 89, GetColor(250, 250, 150)); 
				DrawMessage(scen_char_cnt, 110, 390, 600, GetFontSize(), scen_txt[scen_txt_cnt].c_str(), game.option.strColor, Font0, GetColor(250, 250, 150));
				switch (scen_who[scen_txt_cnt]) {
				case 1:
					DrawGraph(30, 380, stripe[10], TRUE); break;
				case 2:
					DrawGraph(30, 380, stripe[11], TRUE); break;
				case 3:
					DrawGraph(30, 380, stripe[12], TRUE); break;
				case 4:
					DrawGraph(30, 380, stripe[13], TRUE); break;
				case 5:
					DrawGraph(30, 380, stripe[14], TRUE); break;
				}

				if (CheckMusic() != 1) {
					if (Scenflg == 0 || Scenflg == 7 || Scenflg == 12) PlayMusic("sound/bgm03.mp3", DX_PLAYTYPE_BACK);
					if (Scenflg == 21) PlayMusic("sound/bgm07.mp3", DX_PLAYTYPE_BACK);
				}
				switch (Scenflg) {
				case 1:
					//鹿が現れる
					DrawGraph(480, 120, stripe[11], TRUE);
					DrawGraph(270, 200, stripe[14], TRUE);
					if (game.mouse.click()) {
						Scenflg++; scen_txt_cnt++; scen_char_cnt = 0;
					}
					break;
				case 2:
					//カードを見つける、青消える
					scen_txt_len = MultiByteLength(scen_txt[scen_txt_cnt].c_str());
					if (game.fpsCnt % 2 == 0 && scen_char_cnt < scen_txt_len) scen_char_cnt++;
					if (game.mouse.click()) {
						if (scen_char_cnt < scen_txt_len) { scen_char_cnt = scen_txt_len; }
						else {
							visible[1] = 0;
							Scenflg++; scen_txt_cnt++; scen_char_cnt = 0;
						}
					}
					break;
				case 4:
					//地震
					eqx = 10 * sin(eqx + M_PI * (rand() % 10) / 10.0); 
					if (game.mouse.click()) {
						eqx = 0;
						Scenflg++; scen_txt_cnt++; scen_char_cnt = 0;
					}
					break;
				case 6:
					//第一戦
					PlayMusic("sound/bgm04.mp3", DX_PLAYTYPE_BACK);
					scen_txt_cnt++; scen_char_cnt = 0;
					game.initialize();
					initializeTrain();
					break;
				case 8:
					//赤が死ぬ
					visible[2] = 0; 
					if (game.mouse.click()) {
						Scenflg++;
					}
					break;
				case 9:
					//地震
					eqx = 10 * sin(eqx + M_PI * (rand() % 10) / 10.0);
					if (game.mouse.click()) {
						eqx = 0;
						Scenflg++; scen_txt_cnt++; scen_char_cnt = 0;
					}
					break;
				case 11:
					//第二戦
					PlayMusic("sound/bgm05.mp3", DX_PLAYTYPE_BACK);
					scen_txt_cnt++; scen_char_cnt = 0;
					game.initialize();
					initializeTrain();
					break;
				case 13:
					//緑が死ぬ
					visible[3] = 0; 
					if (game.mouse.click()) {
						Scenflg++; 
					}
					break;
				case 14:
					//地震
					eqx = 10 * sin(eqx + M_PI * (rand() % 10) / 10.0);
					if (game.mouse.click()) {
						eqx = 0;
						Scenflg++; scen_txt_cnt++; scen_char_cnt = 0;
					}
					break;
				case 15:
					//青が出てくる
					visible[1] = 1;
					if (game.mouse.click()) {
						Scenflg++; ;
					}
					break;
				case 17:
					//第三戦
					PlayMusic("sound/bgm06.mp3", DX_PLAYTYPE_BACK);
					scen_txt_cnt++; scen_char_cnt = 0;
					game.initialize();
					initializeTrain();
					break;
				case 19:
					//青が死ぬ
					visible[1] = 0;
					if (game.mouse.click()) {
						Scenflg++; 
					}
					break;
				case 20:
					//地震
					eqx = 10 * sin(eqx + M_PI * (rand() % 10) / 10.0);
					if (game.mouse.click()) {
						eqx = 0;
						Scenflg++; scen_txt_cnt++; scen_char_cnt = 0;
					}
					break;
				case 22:
					//第四戦
					PlayMusic("sound/bgm08.mp3", DX_PLAYTYPE_BACK);
					scen_txt_cnt++; scen_char_cnt = 0;
					game.initialize();
					initializeTrain();
					break;
				default:
					scen_txt_len = MultiByteLength(scen_txt[scen_txt_cnt].c_str());
					if (game.fpsCnt % 2 == 0 && scen_char_cnt < scen_txt_len) scen_char_cnt++;
					if (game.mouse.click()) {
						if (scen_char_cnt < scen_txt_len) { scen_char_cnt = scen_txt_len; }
						else { 
							if (scen_txt_cnt == 0 || scen_txt_cnt == 3 || scen_txt_cnt == 5 || scen_txt_cnt == 8 
								|| scen_txt_cnt == 12 || scen_txt_cnt == 14 || scen_txt_cnt == 18 || scen_txt_cnt == 20 || scen_txt_cnt == 26) {
								Scenflg++;
							}
							else {
								scen_txt_cnt = min(27, scen_txt_cnt + 1);
								scen_char_cnt = 0;
							}
						}
					}
					break;
				}
				DrawFormatString(5, 5, game.option.strColor, "Scenflg:%d", Scenflg);
				//DrawFormatString(5, 25, StringColor, "char_cnt:%d", char_cnt);
			}
		}


		// 同期処理
		game.sync();
		if (COMWait > 0) COMWait--;

		// デバッグ情報出力
		game.debugDump();
	}

	InitGraph();
	//DeleteGraph(MovieGraphHandle); 
	//MV1DeleteModel(ModelHandle);
	DxLib_End();

	return 0;
}


//VectorXd StateToInput(int dim, int side) {
//	VectorXd trg(dim);
//	for (int i1 = 0; i1 < 3; ++i1) {
//		for (int j1 = 0; j1 < 3; ++j1) {
//			for (int k1 = 0; k1 < 3; ++k1) {
//				for (int l1 = 0; l1 < 3; ++l1) {
//					trg(27 * i1 + 9 * j1 + 3 * k1 + l1) = game.child[i1][j1].state[k1][l1] * side;
//					if ((game.nextField == -1 || game.nextField == 3 * i1 + j1)
//						&& game.child[i1][j1].state[k1][l1] == 0 && game.child[i1][j1].victory() == 0) {
//						trg(27 * i1 + 9 * j1 + 3 * k1 + l1 + 81) = 1.0;
//					}
//					else {
//						trg(27 * i1 + 9 * j1 + 3 * k1 + l1 + 81) = -1.0;
//					}
//				}
//			}
//		}
//	}
//	return trg;
//}
//
//VectorXd Reward1(const VectorXd &out, const VectorXd &in, int side) {
//	VectorXd trg; trg = out;
//
//	for (int i1 = 0; i1 < 3; ++i1) {
//		for (int j1 = 0; j1 < 3; ++j1) {
//			for (int k1 = 0; k1 < 3; ++k1) {
//				for (int l1 = 0; l1 < 3; ++l1) {
//					if (in(27 * i1 + 9 * j1 + 3 * k1 + l1 + 81) == -1.0) {
//						trg(27 * i1 + 9 * j1 + 3 * k1 + l1) = RWD_FAULT;
//					}
//					else {
//						//trg(27 * i1 + 9 * j1 + 3 * k1 + l1) += RWD_CANPUT; 
//						trg(27 * i1 + 9 * j1 + 3 * k1 + l1) = RWD_CANPUT;
//					}
//				}
//			}
//		}
//	}
//	return trg;
//}
//
//VectorXd softmax(const VectorXd &src, double alpha) {
//	VectorXd trg;
//	
//	trg = (alpha * src).array().exp();
//	trg = trg / trg.sum();
//
//	return trg;
//}