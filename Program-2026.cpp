// ============================================================================================================================================================
// === 選挙で勝とう 2026
// === 作成者: 米田優峻 (E869120)
// === 作成日: 2026. 2. 6
// ============================================================================================================================================================

#include <Siv3D.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// ============================================================================================================================================================
// === 1. 基本的な変数
// ============================================================================================================================================================
struct Player {
	int lv_enzetsu;
	int money;
	int koenkai;
	int lv_sns;
	int num_enzetsu;
};

struct Card {
	String text_head;
	String text_main;
	int prob;
	int posi_nega;
};

struct Record {
	double expc_score;
	double approval[47];
};

namespace {
	// 各都道府県の小選挙区議席数
	const int giseki[47] = {
		12,
		 3,  3,  5,  3,  3,  4,
		 7,  5,  5, 16, 14, 30, 20,
		 5,  3,  3,  2,  2,  5,  5,  8, 16,
		 4,  3,  6, 19, 12,  3,  2,
		 2,  2,  4,  6,  3,  2,  3,  3,  2,
		11,  2,  3,  4,  3,  3,  4,  4
	};

	// 各都道府県の演説コスト
	const int enzetsu_cost[47] = {
		1,
		1, 1, 1, 1, 1, 1,
		1, 1, 1, 2, 2, 4, 2,
		1, 1, 1, 1, 1, 1, 1, 1, 2,
		1, 1, 1, 2, 2, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1
	};

	// 都道府県の隣接関係
	const int connection_a[63] = {
		2, 2, 3, 3, 4, 4, 5, 6,
		8, 8, 8, 9, 9, 10, 11, 11, 12, 12, 13,
		15, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 20, 21, 22,
		24, 24, 24, 25, 26, 26, 27, 27, 27, 28, 28, 29, 31, 31, 32, 32, 33, 34,
		36, 36, 37, 38,
		40, 40, 41, 41, 43, 43, 44, 45
	};
	const int connection_b[63] = {
		3, 5, 4, 5, 6, 7, 6, 7,
		9, 11, 12, 10, 11, 11, 12, 13, 13, 14, 14,
		16, 19, 17, 20, 18, 20, 21, 20, 22, 21, 22, 23, 23, 23,
		25, 29, 30, 26, 28, 29, 28, 29, 30, 31, 33, 30, 32, 33, 34, 35, 34, 35,
		37, 39, 38, 39,
		41, 44, 42, 43, 44, 46, 45, 46
	};

	// 各都道府県の長方形領域の座標
	const double zahyou_lx[47] = { 0.736, 0.736, 0.813, 0.813, 0.736, 0.736, 0.736, 0.838, 0.786, 0.736, 0.736, 0.838, 0.736, 0.736, 0.668, 0.627, 0.585, 0.543, 0.668, 0.585, 0.543, 0.627, 0.543, 0.467, 0.467, 0.415, 0.365, 0.365, 0.415, 0.365, 0.315, 0.264, 0.315, 0.264, 0.230, 0.290, 0.290, 0.231, 0.231, 0.154, 0.104, 0.054, 0.104, 0.154, 0.154, 0.104, 0.054 };
	const double zahyou_ly[47] = { 0.047, 0.263, 0.315, 0.366, 0.315, 0.366, 0.418, 0.502, 0.502, 0.502, 0.540, 0.561, 0.578, 0.616, 0.502, 0.502, 0.502, 0.502, 0.544, 0.544, 0.544, 0.611, 0.611, 0.570, 0.502, 0.502, 0.570, 0.502, 0.570, 0.616, 0.502, 0.502, 0.539, 0.539, 0.502, 0.647, 0.610, 0.610, 0.647, 0.502, 0.502, 0.502, 0.554, 0.554, 0.629, 0.629, 0.380 };
	const double zahyou_rx[47] = { 0.925, 0.889, 0.889, 0.889, 0.813, 0.813, 0.889, 0.889, 0.838, 0.786, 0.838, 0.889, 0.838, 0.838, 0.712, 0.668, 0.627, 0.585, 0.712, 0.668, 0.585, 0.712, 0.627, 0.516, 0.516, 0.467, 0.415, 0.415, 0.467, 0.467, 0.365, 0.315, 0.365, 0.315, 0.264, 0.348, 0.348, 0.290, 0.290, 0.206, 0.154, 0.104, 0.154, 0.206, 0.206, 0.154, 0.128 };
	const double zahyou_ry[47] = { 0.235, 0.315, 0.366, 0.418, 0.366, 0.418, 0.472, 0.561, 0.540, 0.540, 0.578, 0.652, 0.616, 0.652, 0.544, 0.544, 0.544, 0.544, 0.611, 0.611, 0.611, 0.670, 0.670, 0.670, 0.570, 0.570, 0.616, 0.570, 0.616, 0.670, 0.539, 0.539, 0.577, 0.577, 0.577, 0.685, 0.647, 0.647, 0.685, 0.554, 0.554, 0.554, 0.629, 0.629, 0.704, 0.704, 0.432 };

	// チャンスカードに関する情報
	const Card cards[11] = {
		Card{U"陣中見舞い", U"資金が 500 万増える", 10, +3},
		Card{U"株が大暴落", U"資金が 300 万減る", 10, -2},
		Card{U"人柄が信頼される", U"協力者が 100 人増える", 10, +4},
		Card{U"相手が後援会を立てる", U"相手の協力者が 100 人増える", 10, -4},
		Card{U"SNS でバズる", U"自分の影響力が 1 増える", 10, +5},
		Card{U"対立候補が YouTube 出演", U"相手の影響力が 1 増える", 10, -5},
		Card{U"政策が注目される", U"全国支持率が 1% 上がる", 5, +6},
		Card{U"SNS 大炎上", U"全国支持率が (影響力×0.05) % 下がる", 5, -2},
		Card{U"演説が上手くなる", U"演説力が 1 上がる", 10, +3},
		Card{U"対立候補が猛特訓", U"相手の演説力が 1 上がる", 10, -3},
		Card{U"テレビ討論会", U"両候補の影響力が 1 増える", 10, 0},
	};

	// 連合に関する情報 (ターンは 1-indexed)
	int chukaku_turn = 5;
	const int news_day1 = 7;
	const int news_day2 = 12;
	bool chukaku_applied = false;

	// プレイヤーに関する情報
	const double ENZETSU_KEISUU = 1.9;
	const double KAKUSAN_KEISUU = 1.19;
	Player usr[109];
	double approval_rate[109];
	int cards_mirai[109];
	bool selectable[109][109];
	int sns_mystery[109][109];
	int ai_sentakusi;

	// 画面に関する情報
	int difficulty_ = 0;
	int situation = 0;
	int turns = 0;
	int rem_enzetsu = 0;
	int last_option = 0;
	int chizu_mode = 1;
	double timestamp = 0;
	double clicktime = 0;
	double button[209];
	bool button_flag[209];

	// 得票数の表示に関する情報
	double display_hirei0 = 0.0;
	double display_hirei1 = 0.0;
	double display_sub0 = 0.0;
	double display_sub1 = 0.0;
	double display_total0 = 0.0;
	double display_total1 = 0.0;

	// 記録に関する情報 (比例代表は 289 選挙区 * 20 万 = 5780 万人が投票とする)
	String playing_level;
	String ai_tomei, ai_tomei2;
	Record history[109];
	int actual_shosenkyoku[109][109];
	int actual_hirei[109][109];
	int actual_votes[109];

	// 色に関する情報
	const Color col_m = Color(13, 20, 33);
	const Color col_m2 = Color(38, 44, 56);
	const Color col_h = Color(97, 102, 110);
	const Color col_w = Color(255, 255, 255);
	const Color col_r = Color(255, 79, 79);
	const Color col_b = Color(59, 190, 237);
	const Color col_b2 = Color(57, 157, 239);
	const Color col_g = Color(239, 213, 57);
	const Color col_y = Color(215, 210, 0);

	// 戦略に関する情報 (演説 + 協力者 + 資金 + SNS)
	double strategy[12][4] = {
		{0.65, 0.01, 0.01, 0.33}, // 1 日目
		{0.14, 0.01, 0.01, 0.85}, // 2 日目
		{0.14, 0.10, 0.01, 0.75}, // 3 日目
		{0.04, 0.65, 0.01, 0.30}, // 4 日目
		{0.04, 0.71, 0.15, 0.10}, // 5 日目
		{0.04, 0.40, 0.55, 0.01}, // 6 日目
		{0.09, 0.20, 0.70, 0.01}, // 7 日目
		{0.50, 0.09, 0.40, 0.01}, // 8 日目
		{0.70, 0.09, 0.20, 0.01}, // 9 日目
		{0.90, 0.08, 0.01, 0.01}, // 10 日目
		{0.97, 0.01, 0.01, 0.01}, // 11 日目
		{0.97, 0.01, 0.01, 0.01}, // 12 日目
	};
};



// ============================================================================================================================================================
// === 2. 基本的な関数
// ============================================================================================================================================================
double Randouble() {
	double s = 0, t = 1;
	for (int i = 0; i < 3; i++) {
		t /= 1024.0;
		s += 1.0 * (rand() % 1024) * t;
	}
	return s;
}

double RandNormal() {
	double s = 0.0;
	for (int i = 0; i < 100; i++) {
		if (rand() % 2 == 0) s += 1.0;
	}
	return (s - 50.0) / 5.0;
}

// 色の中点を求める
Color Middle(Color a, Color b, double perc) {
	double va = a.r * (1.0 - perc) + b.r * perc;
	double vb = a.g * (1.0 - perc) + b.g * perc;
	double vc = a.b * (1.0 - perc) + b.b * perc;
	return Color(va, vb, vc);
}

// 支持率別の色
Color GetColor(double approve) {
	if (approve < 0.15) return col_b;
	if (approve < 0.50) return Middle(col_b, col_h, (approve - 0.15) / 0.35);
	if (approve < 0.85) return Middle(col_h, col_r, (approve - 0.50) / 0.35);
	return col_r;
}

// 初期化
void Initialize(int difficulty) {
	for (int i = 0; i < 47; i++) approval_rate[i] = 0.5;
	for (int i = 0; i < 47; i++) history[0].approval[i] = 0.5;
	history[0].expc_score = 465.0 / 2.0;

	// 難易度の調整
	difficulty_ = difficulty;
	if (difficulty == 1) {
		usr[0] = Player{ 5, 1500, 400, 5, 1 };
		usr[1] = Player{ 5,  500, 100, 0, 1 };
		ai_sentakusi = 1;
		playing_level = U"簡単";
	}
	if (difficulty == 2) {
		usr[0] = Player{ 5, 500, 300, 0, 1 };
		usr[1] = Player{ 5, 500, 300, 0, 1 };
		ai_sentakusi = 4;
		playing_level = U"普通";
	}
	if (difficulty == 3) {
		usr[0] = Player{ 5, 500, 300, 2, 1 };
		usr[1] = Player{ 5, 500, 300, 0, 1 };
		ai_sentakusi = 10;
		playing_level = U"難しい";
	}
	if (difficulty == 4) {
		usr[0] = Player{ 5, 500, 300, 0, 1 };
		usr[1] = Player{ 5, 500, 700, 0, 1 };
		ai_sentakusi = 20;
		playing_level = U"とても難しい";
	}
	if (difficulty >= 3) {
		chukaku_turn = 5;
		chukaku_applied = false;
	}
	else {
		chukaku_turn = -1;
	}
	ai_tomei = U"AI新党";
	ai_tomei2 = U"新党";

	// カード等の設定
	while (true) {
		int sum = 0; vector<int> counts(11, 0);
		for (int i = 0; i < 12; i++) cards_mirai[i] = rand() % 11;
		for (int i = 0; i < 12; i++) {
			if (i == 0 || i == chukaku_turn) continue;
			sum += cards[cards_mirai[i]].posi_nega;
			counts[cards_mirai[i]] += 1;
		}
		bool flag = true;
		for (int i = 0; i < 11; i++) {
			if (counts[i] >= 3) flag = false;
		}
		if (counts[10] >= 2) flag = false;
		if (sum <= 0 || sum >= 3) flag = false;
		if (flag == true) break;
	}
	for (int i = 0; i < 36; i++) {
		for (int j = 0; j < 47; j++) selectable[i][j] = false;
		for (int j = 0; j < 15; j++) {
			while (true) {
				int idx = rand() % 47;
				if (selectable[i][idx] == false) { selectable[i][idx] = true; break; }
			}
		}
	}
	for (int i = 0; i < 13; i++) sns_mystery[i][0] = rand() % 3;
	for (int i = 0; i < 13; i++) sns_mystery[i][1] = rand() % 3;
}

// ボタンの更新
void update_button(double delta) {
	for (int i = 0; i < 190; i++) {
		if (button_flag[i] == true) button[i] = min(1.0, button[i] + 4.0 * delta);
		else button[i] = max(0.0, button[i] - 4.0 * delta);
	}
	for (int i = 190; i < 200; i++) {
		if (button_flag[i] == true) button[i] = 1.0;
		else button[i] = max(0.0, button[i] - 0.4 * delta);
	}
	for (int i = 0; i < 200; i++) button_flag[i] = false;
}

// 支持率拡散のシミュレーション
void kakusan() {
	vector<double> next_approval(47, 0);
	vector<vector<int>> Graph(47, vector<int>(0, 0));
	for (int i = 0; i < 63; i++) Graph[connection_a[i] - 1].push_back(connection_b[i] - 1);
	for (int i = 0; i < 63; i++) Graph[connection_b[i] - 1].push_back(connection_a[i] - 1);
	for (int i = 0; i < 47; i++) {
		for (int idx : Graph[i]) next_approval[i] += 0.08 * approval_rate[idx];
		next_approval[i] += (1.00 - 0.08 * Graph[i].size()) * approval_rate[i];
	}
	for (int i = 0; i < 47; i++) {
		double r1 = next_approval[i];
		double r2 = 1.0 - next_approval[i];
		approval_rate[i] = pow(r1, KAKUSAN_KEISUU) / (pow(r1, KAKUSAN_KEISUU) + pow(r2, KAKUSAN_KEISUU));
		approval_rate[i] = max(0.0001, min(0.9999, approval_rate[i]));
	}
}

// 行けるかどうかの判定
bool Judge(int opt, int usr_id) {
	if (opt == 1 && usr[usr_id].money < 100) return false;
	if (opt == 6 && usr[usr_id].money < 2000) return false;
	return true;
}

// ステータスの更新
void Options(int opt, int usr_id) {
	if (opt == 2) {
		usr[usr_id].lv_enzetsu += 1;
		if (usr[usr_id].lv_enzetsu == 10) usr[usr_id].koenkai += 50;
		if (usr[usr_id].lv_enzetsu == 15) usr[usr_id].koenkai += 50;
	}
	if (opt == 3) {
		usr[usr_id].koenkai += 60 + 11 * usr[usr_id].lv_sns;
	}
	if (opt == 4) {
		usr[usr_id].money += usr[usr_id].koenkai;
	}
	if (opt == 5) {
		usr[usr_id].lv_sns += 1;
	}
	if (opt == 6) {
		usr[usr_id].money -= 2000;
		usr[usr_id].num_enzetsu += 1;
	}
}

// SNS を適用
void Apply_SNS(int day_) {
	if (true) {
		double force1 = pow(min(16, usr[0].lv_sns), 0.5);
		if (sns_mystery[day_ - 1][0] == 0) force1 /= (giseki[23 - 1] + giseki[27 - 1]);
		if (sns_mystery[day_ - 1][0] == 1) force1 /= (giseki[13 - 1] + giseki[27 - 1]);
		if (sns_mystery[day_ - 1][0] == 2) force1 /= (giseki[13 - 1] + giseki[23 - 1]);
		if (sns_mystery[day_ - 1][0] != 0) approval_rate[13 - 1] = approval_rate[13 - 1] + force1;
		if (sns_mystery[day_ - 1][0] != 1) approval_rate[23 - 1] = approval_rate[23 - 1] + force1;
		if (sns_mystery[day_ - 1][0] != 2) approval_rate[27 - 1] = approval_rate[27 - 1] + force1;
	}
	if (true) {
		double force2 = pow(min(16, usr[1].lv_sns), 0.5);
		if (sns_mystery[day_ - 1][1] == 0) force2 /= (giseki[23 - 1] + giseki[27 - 1]);
		if (sns_mystery[day_ - 1][1] == 1) force2 /= (giseki[13 - 1] + giseki[27 - 1]);
		if (sns_mystery[day_ - 1][1] == 2) force2 /= (giseki[13 - 1] + giseki[23 - 1]);
		if (sns_mystery[day_ - 1][1] != 0) approval_rate[13 - 1] = approval_rate[13 - 1] - force2;
		if (sns_mystery[day_ - 1][1] != 1) approval_rate[23 - 1] = approval_rate[23 - 1] - force2;
		if (sns_mystery[day_ - 1][1] != 2) approval_rate[27 - 1] = approval_rate[27 - 1] - force2;
	}
	approval_rate[13 - 1] = max(0.0001, min(0.9999, approval_rate[13 - 1]));
	approval_rate[23 - 1] = max(0.0001, min(0.9999, approval_rate[23 - 1]));
	approval_rate[27 - 1] = max(0.0001, min(0.9999, approval_rate[27 - 1]));
}

// カードを適用
void Apply_Cards(int card_id) {
	if (card_id == 0) usr[0].money += 500;
	if (card_id == 1) usr[0].money = max(0, usr[0].money - 300);
	if (card_id == 2) usr[0].koenkai += 100;
	if (card_id == 3) usr[1].koenkai += 100;
	if (card_id == 4) usr[0].lv_sns += 1;
	if (card_id == 5) usr[1].lv_sns += 1;
	if (card_id == 6) {
		for (int i = 0; i < 47; i++) approval_rate[i] = max(0.0001, min(0.9999, approval_rate[i] + 0.01));
	}
	if (card_id == 7) {
		for (int i = 0; i < 47; i++) approval_rate[i] = max(0.0001, min(0.9999, approval_rate[i] - 0.0005 * usr[0].lv_sns));
	}
	if (card_id == 8) {
		usr[0].lv_enzetsu += 1;
		if (usr[0].lv_enzetsu == 10) usr[0].koenkai += 50;
		if (usr[0].lv_enzetsu == 15) usr[0].koenkai += 50;
	}
	if (card_id == 9) {
		usr[1].lv_enzetsu += 1;
		if (usr[1].lv_enzetsu == 10) usr[1].koenkai += 50;
		if (usr[1].lv_enzetsu == 15) usr[1].koenkai += 50;
	}
	if (card_id == 10) {
		usr[0].lv_sns += 1;
		usr[1].lv_sns += 1;
	}
}

// AI 新党 + GPT = AI 連合
void Apply_Chukaku(double mokuhyou_giseki) {
	double l = -1.0, r = 1.0;
	for (int i = 0; i < 20; i++) {
		double m = (l + r) / 2.0;
		double sum = 0.0;
		for (int i = 0; i < 47; i++) {
			double app = max(0.001, min(0.999, approval_rate[i] + m));
			sum += 1.0 * giseki[i] * pow(app, 2.7) / (pow(1.0 - app, 2.7) + pow(app, 2.7));
			sum += 1.0 * giseki[i] * app * (176.0 / 289.0);
		}
		if (sum < mokuhyou_giseki) {
			l = m;
		}
		else {
			r = m;
		}
	}
	for (int i = 0; i < 47; i++) approval_rate[i] = max(0.0001, min(0.9999, approval_rate[i] + l));
	chukaku_applied = true;
	usr[1].koenkai += 170;
	ai_tomei = U"AI連合";
	ai_tomei2 = U"連合";
}

// あなたの予測議席数を計算
pair<double, double> Calc_Giseki() {
	double shosenkyoku = 0.0;
	double hirei = 0.0;
	for (int i = 0; i < 47; i++) {
		shosenkyoku += 1.0 * giseki[i] * pow(approval_rate[i], 2.7) / (pow(1.0 - approval_rate[i], 2.7) + pow(approval_rate[i], 2.7));
		hirei += 1.0 * giseki[i] * approval_rate[i] * (176.0 / 289.0);
	}
	return make_pair(shosenkyoku, hirei);
}



// ============================================================================================================================================================
// === 3. AI の戦術
// ============================================================================================================================================================
// AI の演説戦略
void AI_Enzetsu() {
	int remain = min(usr[1].money / 100, usr[1].num_enzetsu);
	vector<bool> pre_determined(47, false);
	int pre_enzetsu = -1;
	while (true) {
		int idx = rand() % 47;
		if (enzetsu_cost[idx] == 1) { pre_determined[idx] = true; break; }
	}
	for (int i = 0; i < ai_sentakusi - 1; i++) {
		while (true) {
			int idx = rand() % 47;
			if (pre_determined[idx] == false) { pre_determined[idx] = true; break; }
		}
	}
	while (remain > 0) {
		pair<double, int> maxv = make_pair(-1e9, -1);
		for (int j = 0; j < 47; j++) {
			if (enzetsu_cost[j] > remain || pre_determined[j] == false) continue;
			if (ai_sentakusi >= 4 && pre_enzetsu == j) continue;
			double moto = approval_rate[j];

			// 演説前の予測議席数を計算
			pair<double, double> v1 = Calc_Giseki();
			approval_rate[j] = max(0.0001, min(0.9999, approval_rate[j] - 0.01 * usr[1].lv_enzetsu * ENZETSU_KEISUU));

			// 演説後の予測議席数を計算
			pair<double, double> v2 = Calc_Giseki();
			approval_rate[j] = moto;

			// 差分を計算
			double diff = (v1.first + v1.second) - (v2.first + v2.second);
			maxv = max(maxv, make_pair(diff / enzetsu_cost[j], j));
		}

		// アップデート
		int idx = maxv.second;
		approval_rate[idx] = max(0.0001, min(0.9999, approval_rate[idx] - 0.01 * usr[1].lv_enzetsu * ENZETSU_KEISUU));
		remain -= enzetsu_cost[idx];
		usr[1].money -= enzetsu_cost[idx] * 100;
		pre_enzetsu = idx;
	}
}

// AI の全体戦略
int AI_Strategy(int day_, int tm_) {
	// A. 11 日目以降
	if (day_ >= 11) {
		int remains = (12 - day_) * 3 + (3 - tm_);
		if (usr[1].money < (remains * usr[1].num_enzetsu + 1) * 100) {
			Options(4, 1);
			return 4; // 資金を集める
		}
		else {
			AI_Enzetsu();
			return 1; // 演説をする
		}
	}

	// B. 資金が 2000 万以上になった場合
	if (usr[1].money >= 2000) {
		Options(6, 1);
		return 6; // 演説数を増やす
	}

	// C. 9 日目以降で演説数が 1 の場合
	if (day_ >= 9 && usr[1].num_enzetsu == 1) {
		Options(4, 1);
		return 4; // 資金を集める
	}

	// D. 資金が 300 万未満の場合
	if (usr[1].money < 300) {
		Options(4, 1);
		return 4; // 資金を集める
	}

	// E. それ以外
	double rnd = Randouble();
	if (rnd < strategy[day_ - 1][0]) {
		int nokori = (12 - day_) * 3 + (4 - tm_);
		if (0.7 * nokori <= 1.0 * usr[1].lv_enzetsu || day_ <= 3) {
			AI_Enzetsu();
			return 1;
		}
		Options(2, 1); // 演説練習
		return 2;
	}
	else if (rnd < strategy[day_ - 1][0] + strategy[day_ - 1][1]) {
		Options(3, 1);
		return 3; // 協力者を集める
	}
	else if (rnd < strategy[day_ - 1][0] + strategy[day_ - 1][1] + strategy[day_ - 1][2]) {
		Options(4, 1);
		return 4; // 資金を集める
	}
	else {
		Options(5, 1); // SNS 活動
		return 5;
	}
}



// ============================================================================================================================================================
// === 4. 開票のシミュレーション
// ============================================================================================================================================================
void OpenVote() {
	double all_deriv = RandNormal() * 0.005;
	for (int i = 0; i < 47; i++) {
		double shijiritsu1 = (approval_rate[i] + all_deriv + RandNormal() * 0.001);
		double shijiritsu2 = (approval_rate[i] + all_deriv + RandNormal() * 0.010);
		double expc_shosenkyoku = 1.0 * giseki[i] * pow(shijiritsu1, 2.7) / (pow(1.0 - shijiritsu1, 2.7) + pow(shijiritsu1, 2.7));
		actual_shosenkyoku[i][0] = max(0, min(giseki[i], (int)(expc_shosenkyoku + 0.3 * RandNormal() + 0.5)));
		actual_shosenkyoku[i][1] = giseki[i] - actual_shosenkyoku[i][0];
		actual_hirei[i][0] = (int)(200000.0 * giseki[i] * shijiritsu2 + 0.5);
		actual_hirei[i][1] = 200000 * giseki[i] - actual_hirei[i][0];
	}
	int hirei_total = 0;
	for (int i = 0; i < 47; i++) hirei_total += actual_hirei[i][0];
	actual_votes[0] += (int)(176.0 * hirei_total / 57800000.0 + 0.5);
	actual_votes[1] += 176 - (int)(176.0 * hirei_total / 57800000.0 + 0.5);
	for (int i = 0; i < 47; i++) actual_votes[0] += actual_shosenkyoku[i][0];
	for (int i = 0; i < 47; i++) actual_votes[1] += actual_shosenkyoku[i][1];
}



// ============================================================================================================================================================
// === 5. 選挙結果の記録
// ============================================================================================================================================================
void Record_History(int turn_) {
	pair<double, double> val = Calc_Giseki();
	history[turn_].expc_score = val.first + val.second;
	for (int i = 0; i < 47; i++) history[turn_].approval[i] = approval_rate[i];
}



// ============================================================================================================================================================
// === 6. メイン関数
// ============================================================================================================================================================
void Main() {
	// セットアップ
	srand((unsigned)time(NULL));
	Window::Resize(1000, 600);
	Scene::SetBackground(col_m);

# if SIV3D_PLATFORM(WEB)
	// Web 版では通常の 1.8 倍の解像度でレンダリング
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1800, 1080);
# endif

	// フォントの設定
	const Font font80{ 144, Typeface::CJK_Regular_JP };
	const Font font60{ 108, Typeface::CJK_Regular_JP };
	const Font font40{ 72, Typeface::CJK_Regular_JP };
	const Font font30{ 54, Typeface::CJK_Regular_JP };
	const Font font25{ 45, Typeface::CJK_Regular_JP };
	const Font font23{ 41, Typeface::CJK_Regular_JP };
	const Font font20{ 36, Typeface::CJK_Regular_JP };
	const Font font18{ 32, Typeface::CJK_Regular_JP };
	const Font font16{ 29, Typeface::CJK_Regular_JP };
	const Font font14{ 25, Typeface::CJK_Regular_JP };
	const Font font11{ 20, Typeface::CJK_Regular_JP };
	const Font font80b{ 144, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font60b{ 108, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font35b{ 63, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font30b{ 54, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font25b{ 45, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font23b{ 41, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font20b{ 36, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font18b{ 32, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font16b{ 29, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font14b{ 25, Typeface::CJK_Regular_JP, FontStyle::Bold };
	const Font font12b{ 22, Typeface::CJK_Regular_JP, FontStyle::Bold };

	// アイコンの設定
	const Texture icon1{ 0xf05cb_icon, 160 };
	const Texture icon2{ 0xf036c_icon, 160 };
	const Texture icon3{ 0xf0849_icon, 160 };
	const Texture icon4{ 0xf0116_icon, 160 };
	const Texture icon5{ 0xf0547_icon, 160 };
	const Texture icon6{ 0xf1323_icon, 160 };

	// メイン部分
	while (System::Update()) {
# if SIV3D_PLATFORM(WEB)
		// 描画とマウス座標を 2 倍スケーリング
		const Transformer2D tr{ Mat3x2::Scale(1.8), TransformCursor::Yes };
# endif

		double MouseX = Cursor::PosF().x;
		double MouseY = Cursor::PosF().y;
		double Delta = Scene::DeltaTime();
		timestamp += Delta;
		clicktime += Delta;
		update_button(Delta);


		// ======================================================================================================================
		// === 状態 0: 待ち受け画面
		// ======================================================================================================================
		if (situation == 0 || situation == 1) {
			font80b(U"選挙で勝とう").draw(80, 260, 100);
			font40(U"2026 年衆院選版").draw(40, 340, 220);
			Rect(250, 380, 500, 80).draw(Middle(col_h, col_y, button[0]));
			font40(U"はじめる").draw(40, 420, 390, col_m);
			int Target = -1;
			if (MouseX >= 250.0 && MouseX <= 750.0 && MouseY >= 380.0 && MouseY <= 460.0) { Target = 0; button_flag[0] = true; }
			if (clicktime >= 0.08 && MouseL.down()) {
				if (Target == 0 && situation == 0) { situation = 1; timestamp = 0; }
				clicktime = 0.0;
			}
			if (situation == 1) {
				Rect(0, 0, 1000, 600).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, 255.0 * 1.6 * timestamp)));
				if (timestamp >= 0.625) { situation = 100; timestamp = 0; }
			}
		}


		// ======================================================================================================================
		// === 状態 1: レベル選択
		// ======================================================================================================================
		else if (situation == 100 || situation == 101) {
			font60(U"レベル選択").draw(60, 350, 120);
			Rect(150, 240, 150, 200).draw(Middle(col_h, col_y, button[1]));
			Rect(333, 240, 150, 200).draw(Middle(col_h, col_y, button[2]));
			Rect(517, 240, 150, 200).draw(Middle(col_h, col_y, button[3]));
			Rect(700, 240, 150, 200).draw(Middle(col_h, col_y, button[4]));
			font30(U"簡単").draw(30, 195, 313, col_m);
			font30(U"普通").draw(30, 378, 313, col_m);
			font30(U"難しい").draw(30, 547, 313, col_m);
			font30(U"とても").draw(30, 730, 293, col_m);
			font30(U"難しい").draw(30, 730, 333, col_m);
			int Target = -1;
			if (MouseX >= 150.0 && MouseX <= 300.0 && MouseY >= 240.0 && MouseY <= 440.0) { Target = 1; button_flag[1] = true; }
			if (MouseX >= 333.0 && MouseX <= 483.0 && MouseY >= 240.0 && MouseY <= 440.0) { Target = 2; button_flag[2] = true; }
			if (MouseX >= 517.0 && MouseX <= 667.0 && MouseY >= 240.0 && MouseY <= 440.0) { Target = 3; button_flag[3] = true; }
			if (MouseX >= 700.0 && MouseX <= 850.0 && MouseY >= 240.0 && MouseY <= 440.0) { Target = 4; button_flag[4] = true; }
			if (clicktime >= 0.08 && MouseL.down()) {
				if (Target == 1 && situation == 100) { Initialize(1); situation = 101; timestamp = 0; }
				if (Target == 2 && situation == 100) { Initialize(2); situation = 101; timestamp = 0; }
				if (Target == 3 && situation == 100) { Initialize(3); situation = 101; timestamp = 0; }
				if (Target == 4 && situation == 100) { Initialize(4); situation = 101; timestamp = 0; }
				clicktime = 0.0;
			}
			if (situation == 101) {
				Rect(0, 0, 1000, 600).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, 255.0 * 1.6 * timestamp)));
				if (timestamp >= 0.625) { situation = 200; timestamp = 0; }
			}
		}


		// ======================================================================================================================
		// === 状態 2: ルール説明 (1)
		// ======================================================================================================================
		else if (situation == 200 || situation == 201) {
			font60(U"はじめに").draw(60, 380, 60);
			font23(U"ゲーム「選挙で勝とう」へようこそ。").draw(23, 75, 160);
			font23(U"あなたは、20XX 年衆議院選挙のある党の党首であり、単独過半数獲得を狙っていま").draw(23, 75, 210);
			font23(U"す。選挙の必勝ツールである三バン (地盤・カバン・看板) をかき集め、SNS の力も").draw(23, 75, 245);
			font23(U"活用して有利に選挙戦を進めていきましょう。").draw(23, 75, 280);
			font23(U"なお、本ゲームは実際の選挙とは一切関係ありません。当たり前ですが、実際の選").draw(23, 75, 330);
			font23(U"挙の方が難しいです。").draw(23, 75, 360);
			Rect(400, 460, 200, 60).draw(Middle(col_h, col_y, button[5]));
			font30(U"次へ").draw(30, 470, 468, col_m);
			int Target = -1;
			if (MouseX >= 400.0 && MouseX <= 600.0 && MouseY >= 460.0 && MouseY <= 520.0) { Target = 5; button_flag[5] = true; }
			if (clicktime >= 0.08 && MouseL.down()) {
				if (Target == 5 && situation == 200) { situation = 201; timestamp = 0; }
				clicktime = 0.0;
			}
			if (situation == 201) {
				Rect(0, 0, 1000, 600).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, 255.0 * 1.6 * timestamp)));
				if (timestamp >= 0.625) { situation = 300; timestamp = 0; }
			}
		}


		// ======================================================================================================================
		// === 状態 3: ルール説明 (2)
		// ======================================================================================================================
		else if (situation == 300 || situation == 301) {
			font60(U"注意").draw(60, 440, 60);
			font23(U"小さい場所をタップする必要があるため、PC または iPad でのプレイが推奨されま").draw(23, 75, 160);
			font23(U"す。スマホでのプレイは推奨されていません。").draw(23, 75, 195);
			font23(U"また、より詳細なルールを知りたい方は以下の URL をご覧ください。").draw(23, 75, 245);
			font23(U"https://github.com/E869120/electiongame2026").draw(23, 75, 280);
			Rect(400, 460, 200, 60).draw(Middle(col_h, col_y, button[6]));
			font30(U"スタート").draw(30, 440, 468, col_m);
			int Target = -1;
			if (MouseX >= 400.0 && MouseX <= 600.0 && MouseY >= 460.0 && MouseY <= 520.0) { Target = 6; button_flag[6] = true; }
			if (clicktime >= 0.08 && MouseL.down()) {
				if (Target == 6 && situation == 300) { situation = 301; timestamp = 0; }
				clicktime = 0.0;
			}
			if (situation == 301) {
				Rect(0, 0, 1000, 600).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, 255.0 * 1.6 * timestamp)));
				if (timestamp >= 0.625) {
					situation = 1006; timestamp = 0;
				}
			}
		}


		// ======================================================================================================================
		// === 状態 10: ゲーム画面
		// ======================================================================================================================
		else if (situation >= 1000 && situation <= 1099) {
			Rect(0, 0, 540, 600).draw(col_m2);
			int day_ = (turns / 6) + 1;
			int tm_ = ((turns / 2) % 3) + 1;
			int player_id = turns % 2;

			// A. 日本地図の描画
			for (int i = 0; i < 47; i++) {
				double lx = 550.0 * zahyou_lx[i];
				double ly = 550.0 * zahyou_ly[i];
				double rx = 550.0 * zahyou_rx[i];
				double ry = 550.0 * zahyou_ry[i];
				if (situation % 100 == 7 && player_id == 0 && selectable[turns / 2][i] == true) {
					Rect(lx - 0.0, ly - 0.0, (rx - lx) - 0.0, (ry - ly) - 0.0).draw(Middle(col_w, col_y, button[6 + i]));
				}
				Rect(lx + 2.0, ly + 2.0, (rx - lx) - 4.0, (ry - ly) - 4.0).draw(GetColor(approval_rate[i]));
				if (situation % 100 == 7) {
					if (chizu_mode == 1) {
						if (enzetsu_cost[i] == 2) font14(U"★").draw(14, (lx + rx) / 2.0 - 8.0, (ly + ry) / 2.0 - 12.5, col_m2);
						if (enzetsu_cost[i] >= 3) font14(U"◆").draw(14, (lx + rx) / 2.0 - 8.0, (ly + ry) / 2.0 - 12.5, col_m2);
					}
					else {
						if (enzetsu_cost[i] == 1) {
							font14(giseki[i]).draw(14, (lx + rx) / 2.0 - 4.0 * to_string(giseki[i]).size(), (ly + ry) / 2.0 - 12.5, col_m2);
						}
						if (enzetsu_cost[i] == 2) {
							font11(U"★").draw(11, (lx + rx) / 2.0 - 11.9, (ly + ry) / 2.0 - 10.5, col_m2);
							font11(giseki[i]).draw(11, (lx + rx) / 2.0 - 1.9, (ly + ry) / 2.0 - 10.5, col_m2);
						}
						if (enzetsu_cost[i] >= 3) {
							font11(U"◆").draw(11, (lx + rx) / 2.0 - 11.9, (ly + ry) / 2.0 - 10.5, col_m2);
							font11(giseki[i]).draw(11, (lx + rx) / 2.0 - 1.9, (ly + ry) / 2.0 - 10.5, col_m2);
						}
					}
				}
				if (situation % 100 != 7 && chizu_mode == 2) {
					font14(giseki[i]).draw(14, (lx + rx) / 2.0 - 4.0 * to_string(giseki[i]).size(), (ly + ry) / 2.0 - 12.5, col_m2);
				}
			}
			Line(550.0 * 0.054, 550.0 * 0.448, 550.0 * 0.152, 550.0 * 0.448).draw(LineStyle::SquareDot, 2, col_h);
			Line(550.0 * 0.152, 550.0 * 0.448, 550.0 * 0.187, 550.0 * 0.397).draw(LineStyle::SquareDot, 2, col_h);
			if (chizu_mode == 1) {
				Circle(40, 40, 10).draw(col_h);
				Circle(60, 40, 10).draw(col_h);
				Rect(40, 30, 20, 20).draw(col_h);
				Circle(40, 40, 8).draw(col_w);
			}
			if (chizu_mode == 2) {
				Circle(40, 40, 10).draw(col_y);
				Circle(60, 40, 10).draw(col_y);
				Rect(40, 30, 20, 20).draw(col_y);
				Circle(60, 40, 8).draw(col_m);
			}
			font16(U"小選挙区の議席数を表示").draw(16, 80, 28, col_w);

			// B. 議席予測の描画
			pair<double, double> current_expect = Calc_Giseki();
			double expect_total = current_expect.first + current_expect.second;
			if (difficulty_ <= 2 || chukaku_applied == true) {
				double pt1 = 30.0;
				double pt2 = 30.0 + 480.0 * current_expect.first / 465.0;
				double pt3 = 30.0 + 480.0 * expect_total / 465.0;
				double pt4 = 30.0 + 480.0 * (expect_total + (176.0 - current_expect.second)) / 465.0;
				double pt5 = 30.0 + 480.0;
				Rect(30, 445, 120, 45).draw(col_w);
				Rect(pt1, 495, pt5 - pt1 - 1.0, 75).draw(col_r);
				Rect(pt3, 495, pt5 - pt3, 75).draw(col_b);
				Rect(pt2, 542, pt4 - pt2, 28).draw(Color(col_m2.r, col_m2.g, col_m2.b, 127));
				Triangle(261, 480, 279, 480, 270, 495).draw(col_w);
				font25b(U"議席予測").draw(25, 40, 448, col_m);
				font16(U"※内側は比例代表").draw(16, 382, 470, col_w);
				font16b(U"あなた").draw(16, 35, 501, col_m);
				font16b(ai_tomei).draw(16, 457, 501, col_m);
				font35b((int)(expect_total + 0.5)).draw(35, 35, 520, col_m);
				font35b(465 - (int)(expect_total + 0.5)).draw(35, 505 - 21 * (int)(to_string(465 - (int)(expect_total + 0.5)).size()), 520, col_m);
				if (pt2 - pt1 >= 80 && pt3 - pt2 >= 20) {
					font16((int)(current_expect.second + 0.5)).draw(16, pt2 + 4.0, 546, col_w);
				}
				if (pt5 - pt4 >= 80 && pt4 - pt3 >= 20) {
					font16(176 - (int)(current_expect.second + 0.5)).draw(16, pt4 - 4.0 - 9.0 * (int)(to_string(176 - (int)(current_expect.second + 0.5)).size()), 546, col_w);
				}
			}
			else {
				double pt1 = 30.0;
				double pt2 = 30.0 + 440.0 * current_expect.first / 465.0;
				double pt3 = 30.0 + 440.0 * expect_total / 465.0;
				double pt4 = 30.0 + 440.0 * (expect_total + (176.0 - current_expect.second)) / 465.0;
				double pt5 = 30.0 + 440.0;
				Rect(30, 445, 120, 45).draw(col_w);
				Rect(pt1, 495, pt5 - pt1 - 1.0, 75).draw(col_r);
				Rect(pt3, 495, pt5 - pt3, 75).draw(col_b2);
				Rect(pt2, 542, pt4 - pt2, 28).draw(Color(col_m2.r, col_m2.g, col_m2.b, 127));
				Rect(469, 495, 41, 75).draw(col_g);
				Triangle(261, 480, 279, 480, 270, 495).draw(col_w);
				font25b(U"議席予測").draw(25, 40, 448, col_m);
				font16(U"※内側は比例代表").draw(16, 382, 470, col_w);
				font16b(U"あなた").draw(16, 35, 501, col_m);
				font16b(ai_tomei).draw(16, 417, 501, col_m);
				font16b(U"GPT").draw(16, 475, 501, col_m);
				font35b((int)(expect_total * (425.0 / 465.0) + 0.5)).draw(35, 35, 520, col_m);
				font35b(425 - (int)(expect_total * (425.0 / 465.0) + 0.5)).draw(35, 465 - 21 * (int)(to_string(425 - (int)(expect_total * (425.0 / 465.0) + 0.5)).size()), 520, col_m);
				font25b(U"40").draw(35, 470, 520, col_m);
				if (pt2 - pt1 >= 80 && pt3 - pt2 >= 20) {
					font16((int)(current_expect.second + 0.5)).draw(16, pt2 + 4.0, 546, col_w);
				}
				if (pt5 - pt4 >= 80 && pt4 - pt3 >= 20) {
					font16(176 - (int)(current_expect.second + 0.5)).draw(16, pt4 - 4.0 - 9.0 * (int)(to_string(176 - (int)(current_expect.second + 0.5)).size()), 546, col_w);
				}
			}

			// C. ステータスの描画
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 6; j++) {
					double keisuu = ((i == 0 || j == 0) ? 0.3 : 0.1);
					double lx = 569.0 + 402.0 * (j + 0) / 6;
					double rx = 569.0 + 402.0 * (j + 1) / 6;
					double ly = 471.0 + 99.0 * (i + 0) / 3;
					double ry = 471.0 + 99.0 * (i + 1) / 3;
					Rect(lx + 2.0, ly + 2.0, (rx - lx) - 4.0, (ry - ly) - 4.0).draw(Middle(col_m, col_w, keisuu));
				}
			}
			font16(U"あなた").draw(16, 602.5 - 24.0, 508, col_w);
			font16(ai_tomei).draw(16, 602.5 - 24.0, 541, col_w);
			font16(U"演説力").draw(16, 669.5 - 24.0, 475, col_w);
			font16(U"資金").draw(16, 736.5 - 16.0, 475, col_w);
			font16(U"協力者").draw(16, 803.5 - 24.0, 475, col_w);
			font16(U"影響力").draw(16, 870.5 - 24.0, 475, col_w);
			font16(U"演説数").draw(16, 937.5 - 24.0, 475, col_w);
			font16(U"Lv.").draw(16, 698.0 - 9.0 * (to_string(usr[0].lv_enzetsu).size() + 2) - 5.0, 508, col_r);
			font16(U"Lv.").draw(16, 698.0 - 9.0 * (to_string(usr[1].lv_enzetsu).size() + 2) - 5.0, 541, col_b);
			font16(usr[0].lv_enzetsu).draw(16, 698.0 - 9.0 * (to_string(usr[0].lv_enzetsu).size() + 0), 508, col_r);
			font16(usr[1].lv_enzetsu).draw(16, 698.0 - 9.0 * (to_string(usr[1].lv_enzetsu).size() + 0), 541, col_b);
			font16(usr[0].money).draw(16, 765.0 - 9.0 * to_string(usr[0].money).size() - 16.0, 508, col_r);
			font16(usr[1].money).draw(16, 765.0 - 9.0 * to_string(usr[1].money).size() - 16.0, 541, col_b);
			font16(U"万").draw(16, 765.0 - 16.0, 508, col_r);
			font16(U"万").draw(16, 765.0 - 16.0, 541, col_b);
			font16(usr[0].koenkai).draw(16, 832.0 - 9.0 * to_string(usr[0].koenkai).size() - 16.0, 508, col_r);
			font16(usr[1].koenkai).draw(16, 832.0 - 9.0 * to_string(usr[1].koenkai).size() - 16.0, 541, col_b);
			font16(U"人").draw(16, 832.0 - 16.0, 508, col_r);
			font16(U"人").draw(16, 832.0 - 16.0, 541, col_b);
			font16(U"Lv.").draw(16, 899.0 - 9.0 * (to_string(usr[0].lv_sns).size() + 2) - 5.0, 508, col_r);
			font16(U"Lv.").draw(16, 899.0 - 9.0 * (to_string(usr[1].lv_sns).size() + 2) - 5.0, 541, col_b);
			font16(usr[0].lv_sns).draw(16, 899.0 - 9.0 * (to_string(usr[0].lv_sns).size() + 0), 508, col_r);
			font16(usr[1].lv_sns).draw(16, 899.0 - 9.0 * (to_string(usr[1].lv_sns).size() + 0), 541, col_b);
			font16(usr[0].num_enzetsu).draw(16, 966.0 - 9.0 * to_string(usr[0].num_enzetsu).size() - 16.0, 508, col_r);
			font16(usr[1].num_enzetsu).draw(16, 966.0 - 9.0 * to_string(usr[1].num_enzetsu).size() - 16.0, 541, col_b);
			font16(U"回").draw(16, 966.0 - 16.0, 508, col_r);
			font16(U"回").draw(16, 966.0 - 16.0, 541, col_b);

			// D. 日付の描画
			Line(570, 60, 970, 60).draw(2.5, col_w);
			font23b(U"選挙戦").draw(23, 570, 25, col_w);
			font23b(day_).draw(23, 642, 25, col_w);
			font23b(U"日目").draw(23, 642 + 13 * to_string(day_).size(), 25, col_w);
			if (tm_ == 1) font23b(U"朝").draw(23, 688 + 13 * to_string(day_).size(), 25, col_w);
			if (tm_ == 2) font23b(U"昼").draw(23, 688 + 13 * to_string(day_).size(), 25, col_w);
			if (tm_ == 3) font23b(U"夜").draw(23, 688 + 13 * to_string(day_).size(), 25, col_w);
			font23b(U"(全12日)").draw(23, 714 + 13 * to_string(day_).size(), 25, col_w);
			if (player_id == 0) {
				Rect(840, 28, 130, 28).draw(col_r);
				font16b(U"あなたのターン").draw(16, 845, 30, col_w);
			}
			if (player_id == 1) {
				Rect(840, 28, 130, 28).draw(col_b);
				font16b(ai_tomei).draw(16, 880, 30, col_w);
			}

			// E. 現在の状況の描画
			// 状況 0: 形勢拡散
			if (situation % 100 == 0) {
				font18(U"重要:").draw(18, 570, 70, col_w);
				font18(U"一日の終わりには、形勢が隣に拡散します").draw(18, 570, 95, col_w);
				font18(U"(2 日目以降も同様)").draw(18, 570, 120, col_w);
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						double keisuu1 = ((i == 1 && j == 1) ? 1.0 : 0.0);
						double keisuu2 = ((i == 1 || j == 1) ? 0.2 : 0.0); if (keisuu1 > 0.8) keisuu2 = 1.0;
						Rect(570 + i * 35, 160 + j * 35, 30, 30).draw(Middle(col_h, col_r, keisuu1));
						Rect(760 + i * 35, 160 + j * 35, 30, 30).draw(Middle(col_h, col_r, keisuu2));
					}
				}
				Rect(690, 203, 30, 14).draw(col_w);
				Triangle(720, 195, 720, 225, 740, 210).draw(col_w);
				Rect(790, 320, 180, 45).draw(Middle(col_h, col_y, button[60]));
				font25(U"OK").draw(25, 866, 324, col_m);
			}

			// 状況 1: SNS 投稿
			if (situation % 100 == 1) {
				font18(U"深夜に SNS を通じた活動が行われます").draw(18, 570, 70, col_w);
				if (usr[0].lv_sns > usr[1].lv_sns + 2) {
					font18(U"(影響力の高い").draw(18, 570, 95, col_w);
					font18(U"あなたの形勢").draw(18, 683, 95, col_r);
					font18(U"が都市で僅かに上昇)").draw(18, 791, 95, col_w);
				}
				else if (usr[0].lv_sns + 2 < usr[1].lv_sns) {
					font18(U"(影響力の高い").draw(18, 570, 95, col_w);
					font18(ai_tomei + U"の形勢").draw(18, 683, 95, col_b);
					font18(U"が都市で僅かに上昇)").draw(18, 791, 95, col_w);
				}
				Rect(790, 320, 180, 45).draw(Middle(col_h, col_y, button[70]));
				font25(U"OK").draw(25, 866, 324, col_m);
			}

			// 状況 2: ニュース
			if (situation % 100 == 2) {
				int giseki_a = (int)(expect_total + 0.5);
				int giseki_b = 465 - giseki_a;
				int marume_a = ((giseki_a + 5) / 10) * 10; if (giseki_a > giseki_b) marume_a = max(240, marume_a);
				int marume_b = ((giseki_b + 5) / 10) * 10; if (giseki_a < giseki_b) marume_b = max(240, marume_b);
				int pattern = -1;
				if (day_ != 12) {
					if (giseki_a > giseki_b) {
						font18b(U"NEWS: 衆院選中盤情勢、プレイヤーがリード").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。プ").draw(18, 570, 105, col_w);
						font18(U"レイヤーが").draw(18, 570, 130, col_w); font18(marume_a).draw(18, 666, 130, col_w); font18(U"議席をうかがう状況であり、過").draw(18, 701, 130, col_w);
						font18(U"半数の 233 議席に達する公算が大きい。").draw(18, 570, 155, col_w);
					}
					else {
						font18b(U"NEWS: 衆院選中盤情勢、" + ai_tomei + U"がリード").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。AI").draw(18, 570, 105, col_w);
						font18(ai_tomei2 + U"が").draw(18, 570, 130, col_w); font18(marume_b).draw(18, 630, 130, col_w); font18(U"議席をうかがう状況であり、過半数").draw(18, 665, 130, col_w);
						font18(U"の 233 議席に達する公算が大きい。").draw(18, 570, 155, col_w);
					}
				}
				else {
					if (giseki_a >= 390) {
						font18b(U"NEWS: 衆院選終盤情勢、記録的大勝へ").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。プ").draw(18, 570, 105, col_w);
						font18(U"レイヤーの").draw(18, 570, 130, col_w); font18(marume_a).draw(18, 666, 130, col_w); font18(U"議席前後獲得が予想されており").draw(18, 701, 130, col_w);
						font18(U"これまでの日本の選挙史上、最大の差で圧勝する").draw(18, 570, 155, col_w);
						font18(U"見通しとなっている。").draw(18, 570, 180, col_w);
					}
					else if (giseki_a >= 295) {
						font18b(U"NEWS: 衆院選終盤情勢 プレイヤーが政権獲得へ").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。プ").draw(18, 570, 105, col_w);
						font18(U"レイヤーの").draw(18, 570, 130, col_w); font18(marume_a).draw(18, 666, 130, col_w); font18(U"議席前後獲得が予想されており").draw(18, 701, 130, col_w);
						font18(U"政権獲得が確実な情勢に。").draw(18, 570, 155, col_w);
					}
					else if (giseki_b >= 295) {
						font18b(U"NEWS: 衆院選終盤情勢、" + ai_tomei + U"が政権獲得へ").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。AI").draw(18, 570, 105, col_w);
						font18(ai_tomei2 + U"の").draw(18, 570, 130, col_w); font18(marume_b).draw(18, 630, 130, col_w); font18(U"議席前後獲得が予想されており、政").draw(18, 665, 130, col_w);
						font18(U"権獲得が確実な情勢に。").draw(18, 570, 155, col_w);
					}
					else if (giseki_a >= 260) {
						font18b(U"NEWS: 衆院選終盤情勢、プレイヤーが優勢").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。プ").draw(18, 570, 105, col_w);
						font18(U"レイヤーの").draw(18, 570, 130, col_w); font18(marume_a).draw(18, 666, 130, col_w); font18(U"議席前後獲得が予想されており").draw(18, 701, 130, col_w);
						font18(ai_tomei + U"の").draw(18, 570, 155, col_w); font18(marume_b).draw(18, 646, 155, col_w); font18(U"議席前後に対して優位に立ってい").draw(18, 681, 155, col_w);
						font18(U"る。果たして守り切れるのか。").draw(18, 570, 180, col_w);
					}
					else if (giseki_b >= 260) {
						font18b(U"NEWS: 衆院選終盤情勢、" + ai_tomei + U"が優勢").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。AI").draw(18, 570, 105, col_w);
						font18(ai_tomei2 + U"の").draw(18, 570, 130, col_w); font18(marume_b).draw(18, 630, 130, col_w); font18(U"議席前後獲得が予想されており、プ").draw(18, 665, 130, col_w);
						font18(U"レイヤーの").draw(18, 570, 155, col_w); font18(marume_a).draw(18, 666, 155, col_w); font18(U"議席前後に対して優位に立って").draw(18, 701, 155, col_w);
						font18(U"いる。果たして逆転はあるのか。").draw(18, 570, 180, col_w);
					}
					else if (giseki_a >= 240) {
						font18b(U"NEWS: 衆院選終盤情勢、プレイヤーがやや先行").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。プ").draw(18, 570, 105, col_w);
						font18(U"レイヤーの").draw(18, 570, 130, col_w); font18(marume_a).draw(18, 666, 130, col_w); font18(U"議席前後獲得が予想されており").draw(18, 701, 130, col_w);
						font18(ai_tomei + U"の").draw(18, 570, 155, col_w); font18(marume_b).draw(18, 646, 155, col_w); font18(U"議席前後に対してやや先行してい").draw(18, 681, 155, col_w);
						font18(U"る。しかし、1 割程度がまだ投票先を決めておら").draw(18, 570, 180, col_w);
						font18(U"ず、情勢は流動的である。").draw(18, 570, 205, col_w);
					}
					else if (giseki_b >= 240) {
						font18b(U"NEWS: 衆院選終盤情勢、" + ai_tomei + U"がやや先行").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。AI").draw(18, 570, 105, col_w);
						font18(ai_tomei2 + U"の").draw(18, 570, 130, col_w); font18(marume_b).draw(18, 630, 130, col_w); font18(U"議席前後獲得が予想されており、プ").draw(18, 665, 130, col_w);
						font18(U"レイヤーの").draw(18, 570, 155, col_w); font18(marume_a).draw(18, 666, 155, col_w); font18(U"議席前後に対してやや先行して").draw(18, 701, 155, col_w);
						font18(U"いる。しかし、1 割程度がまだ投票先を決めてお").draw(18, 570, 180, col_w);
						font18(U"らず、情勢は流動的である。").draw(18, 570, 205, col_w);
					}
					else {
						font18b(U"NEWS: 衆院選終盤情勢、大接戦の展開に").draw(18, 570, 70, col_w);
						font18(U"XX 新聞社は AA 月 BB 日に世論調査を行った。プ").draw(18, 570, 105, col_w);
						font18(U"レイヤーと " + ai_tomei + U"が歴史的な大接戦となってお").draw(18, 570, 130, col_w);
						font18(U"り、何が起こるか分からない展開だ。").draw(18, 570, 155, col_w);
					}
				}
				Rect(790, 320, 180, 45).draw(Middle(col_h, col_y, button[80]));
				font25(U"OK").draw(25, 866, 324, col_m);
			}

			// 状況 3: チャンス
			if (situation % 100 == 3) {
				font18(U"選挙戦").draw(18, 570, 70, col_w);
				font18(day_).draw(18, 630, 70, col_w);
				font18(U"日目が始まりました").draw(18, 633 + 10 * to_string(day_).size(), 70, col_w);
				font18(U"チャンスカードを引いてください").draw(18, 570, 95, col_w);
				Rect(790, 320, 180, 45).draw(Middle(col_h, col_y, button[90]));
				font25(U"引く").draw(25, 855, 324, col_m);
			}

			// 状況 4: チャンス結果
			if (situation % 100 == 4) {
				font18(U"選挙戦").draw(18, 570, 70, col_w);
				font18(day_).draw(18, 630, 70, col_w);
				font18(U"日目が始まりました").draw(18, 633 + 10 * to_string(day_).size(), 70, col_w);
				font18(U"チャンスカードを引いてください").draw(18, 570, 95, col_w);
				Rect(570, 135, 400, 120).draw(col_y);
				font23b(cards[cards_mirai[day_ - 1]].text_head).draw(23, 580, 140, col_m);
				font18(cards[cards_mirai[day_ - 1]].text_main).draw(18, 580, 180, col_m);
				Rect(790, 320, 180, 45).draw(Middle(col_h, col_y, button[90]));
				font25(U"OK").draw(25, 866, 324, col_m);
			}

			// 状況 5: 中道改革連合
			if (situation % 100 == 5) {
				font18b(U"AI 連合が結成！").draw(18, 570, 70, col_w);
				font18(U"政党「AI 新党」と「GPT」が合体し、「AI 連合」と").draw(18, 570, 105, col_w);
				font18(U"いう政党が出来ました。突然の大事件です！").draw(18, 570, 130, col_w);
				Rect(790, 320, 180, 45).draw(Middle(col_h, col_y, button[90]));
				font25(U"OK").draw(25, 866, 324, col_m);
			}

			// 状況 6: 行動選択
			if (situation % 100 == 6) {
				font18(U"次の行動を選択してください").draw(18, 570, 70, col_w);
				Rect(570, 105, 120, 120).draw(Middle(col_h, col_y, button[110]));
				Rect(710, 105, 120, 120).draw(Middle(col_h, col_y, button[111]));
				Rect(850, 105, 120, 120).draw(Middle(col_h, col_y, button[112]));
				Rect(570, 245, 120, 120).draw(Middle(col_h, col_y, button[113]));
				Rect(710, 245, 120, 120).draw(Middle(col_h, col_y, button[114]));
				Rect(850, 245, 120, 120).draw(Middle(col_h, col_y, button[115]));
				icon1.resized(60).drawAt(570 + 60, 105 + 40, col_m);
				icon2.resized(60).drawAt(710 + 60, 105 + 40, col_m);
				icon3.resized(60).drawAt(850 + 60, 105 + 40, col_m);
				icon4.resized(60).drawAt(570 + 60, 245 + 40, col_m);
				icon5.resized(60).drawAt(710 + 60, 245 + 40, col_m);
				icon6.resized(60).drawAt(850 + 60, 245 + 40, col_m);
				font16b(U"演説をする").draw(16, 630 - 40, 105 + 82, col_m);
				font16b(U"演説の練習").draw(16, 770 - 40, 105 + 82, col_m);
				font16b(U"協力者集め").draw(16, 910 - 40, 105 + 82, col_m);
				font16b(U"資金集め").draw(16, 630 - 32, 245 + 82, col_m);
				font16b(U"SNS活動").draw(16, 770 - 32, 245 + 82, col_m);
				font16b(U"演説数を増やす").draw(16, 910 - 57, 245 + 82, col_m);
				font11(U"資金 -100万/1回").draw(11, 630 - 40, 105 + 101, col_m);
				font11(U"演説力 +1").draw(11, 770 - 25, 105 + 101, col_m);
				font11(U"協力者 +60+11×影響力").draw(11, 910 - 58, 105 + 101, col_m);
				font11(U"資金 +1万×協力者").draw(11, 630 - 45, 245 + 101, col_m);
				font11(U"影響力 +1").draw(11, 770 - 25, 245 + 101, col_m);
				font11(U"資金 -2000万").draw(11, 910 - 35, 245 + 101, col_m);
				font14(U"※演説力が Lv.10, 15 となった時協力者が 50 人増える").draw(14, 630, 370, Color(col_w.r, col_w.g, col_w.b, min(255.0, max(0.0, (timestamp - 1.25) * 1020.0))));
				font20(U"資金が足りません").draw(20, 690, 390, Color(col_y.r, col_y.g, col_y.b, 255.0 * button[190]));
				Rect(570, 105, 120, 120).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, max(0.0, (0.25 - timestamp) * 1020.0))));
				Rect(710, 105, 120, 120).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, max(0.0, (0.50 - timestamp) * 1020.0))));
				Rect(850, 105, 120, 120).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, max(0.0, (0.75 - timestamp) * 1020.0))));
				Rect(570, 245, 120, 120).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, max(0.0, (1.00 - timestamp) * 1020.0))));
				Rect(710, 245, 120, 120).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, max(0.0, (1.25 - timestamp) * 1020.0))));
				Rect(850, 245, 120, 120).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, max(0.0, (1.50 - timestamp) * 1020.0))));
			}

			// 状況 7: 演説場所選択
			if (situation % 100 == 7) {
				font18(U"演説場所を選択してください").draw(18, 570, 70, col_w);
				font18(U"ただし、★の都道府県では 2 回分、◆の都道府県").draw(18, 570, 110, col_w);
				font18(U"では 4 回分の演説回数を消費します。").draw(18, 570, 135, col_w);
				if (day_ == 1) {
					font18(U"演説 1 回につき、演説力に比例して支持率が上が").draw(18, 570, 175, col_w);
					font18(U"ります。").draw(18, 570, 200, col_w);
					font20(U"残り演説回数").draw(20, 745, 240, col_y);
					font60b(rem_enzetsu).draw(60, 885, 225, col_y);
					font30b(usr[0].num_enzetsu).draw(30, 955, 259, col_y);
					Line(924, 296, 970, 250).draw(3, col_y);
					font20(U"演説回数が足りません").draw(20, 690, 330, Color(col_y.r, col_y.g, col_y.b, 255.0 * button[191]));
				}
				else {
					font20(U"残り演説回数").draw(20, 745, 210, col_y);
					font60b(rem_enzetsu).draw(60, 885, 195, col_y);
					font30b(usr[0].num_enzetsu).draw(30, 955, 229, col_y);
					Line(924, 266, 970, 220).draw(3, col_y);
					font14(U"※資金がない場合、演説回数が上限未満になることがあります").draw(14, 576, 170, col_w);
					font20(U"演説回数が足りません").draw(20, 690, 300, Color(col_y.r, col_y.g, col_y.b, 255.0 * button[191]));
				}
			}

			// 状況 8: 投開票日
			if (situation % 100 == 8) {
				font18(U"いよいよ投票日です！").draw(18, 570, 70, col_w);
				Rect(790, 320, 180, 45).draw(Middle(col_h, col_y, button[120]));
				font25(U"開票へ").draw(25, 842, 324, col_m);
			}

			// ボタンの位置の確認
			int Target = -1;
			if (situation % 100 == 0) {
				if (MouseX >= 790.0 && MouseX <= 970.0 && MouseY >= 320.0 && MouseY <= 365.0) Target = 60;
			}
			if (situation % 100 == 1) {
				if (MouseX >= 790.0 && MouseX <= 970.0 && MouseY >= 320.0 && MouseY <= 365.0) Target = 70;
			}
			if (situation % 100 == 2) {
				if (MouseX >= 790.0 && MouseX <= 970.0 && MouseY >= 320.0 && MouseY <= 365.0) Target = 80;
			}
			if (situation % 100 == 3) {
				if (MouseX >= 790.0 && MouseX <= 970.0 && MouseY >= 320.0 && MouseY <= 365.0) Target = 90;
			}
			if (situation % 100 == 4) {
				if (MouseX >= 790.0 && MouseX <= 970.0 && MouseY >= 320.0 && MouseY <= 365.0) Target = 90;
			}
			if (situation % 100 == 5) {
				if (MouseX >= 790.0 && MouseX <= 970.0 && MouseY >= 320.0 && MouseY <= 365.0) Target = 90;
			}
			if (situation % 100 == 6 && player_id == 0) {
				if (MouseX >= 570.0 && MouseX <= 690.0 && MouseY >= 105.0 && MouseY <= 225.0) Target = 110;
				if (MouseX >= 710.0 && MouseX <= 830.0 && MouseY >= 105.0 && MouseY <= 225.0) Target = 111;
				if (MouseX >= 850.0 && MouseX <= 970.0 && MouseY >= 105.0 && MouseY <= 225.0) Target = 112;
				if (MouseX >= 570.0 && MouseX <= 690.0 && MouseY >= 245.0 && MouseY <= 365.0) Target = 113;
				if (MouseX >= 710.0 && MouseX <= 830.0 && MouseY >= 245.0 && MouseY <= 365.0) Target = 114;
				if (MouseX >= 850.0 && MouseX <= 970.0 && MouseY >= 245.0 && MouseY <= 365.0) Target = 115;
			}
			if (situation % 100 == 7 && player_id == 0) {
				for (int i = 0; i < 47; i++) {
					double lx = 550.0 * zahyou_lx[i] + 2.0;
					double ly = 550.0 * zahyou_ly[i] + 2.0;
					double rx = 550.0 * zahyou_rx[i] - 2.0;
					double ry = 550.0 * zahyou_ry[i] - 2.0;
					if (MouseX >= lx && MouseX <= rx && MouseY >= ly && MouseY <= ry && selectable[turns / 2][i] == true) Target = 6 + i;
				}
			}
			if (situation % 100 == 8) {
				if (MouseX >= 790.0 && MouseX <= 970.0 && MouseY >= 320.0 && MouseY <= 365.0) Target = 120;
			}
			if (MouseX >= 30.0 && MouseX <= 70.0 && MouseY >= 30.0 && MouseY <= 50.0) Target = 53;
			if (Target >= 0) button_flag[Target] = true;

			// マウスが押された場合の対処
			if (clicktime >= 0.08 && MouseL.down() && timestamp < 9999999.0) {
				if (situation % 100 == 0) {
					if (Target == 60) { timestamp = 1.0e7; kakusan(); }
				}
				else if (situation % 100 == 1) {
					if (Target == 70) { timestamp = 1.0e7; Apply_SNS(day_); }
				}
				else if (situation % 100 == 2) {
					if (Target == 80) { timestamp = 1.0e7; }
				}
				else if (situation % 100 == 3) {
					if (Target == 90) { timestamp = 0.0; situation = 1004; Apply_Cards(cards_mirai[day_ - 1]); }
				}
				else if (situation % 100 == 4) {
					if (Target == 90) { timestamp = 1.0e7; }
				}
				else if (situation % 100 == 5) {
					if (Target == 90) { timestamp = 1.0e7; Apply_Chukaku(max(1.0, expect_total * 425.0 / 465.0 + 4.4)); }
				}
				else if (situation % 100 == 6 && timestamp >= 1.45) {
					if (Target >= 110 && Target <= 115 && Judge(Target - 109, 0) == true) {
						timestamp = 1.0e7;
						Options(Target - 109, 0);
						if (Target == 110) rem_enzetsu = min(usr[0].money / 100, usr[0].num_enzetsu);
						last_option = Target - 109;
					}
					else if (Target >= 110 && Target <= 115 && Judge(Target - 109, 0) == false) button_flag[190] = true;
				}
				else if (situation % 100 == 7) {
					if (Target >= 6 && Target <= 52) {
						if (enzetsu_cost[Target - 6] > rem_enzetsu) button_flag[191] = true;
						else {
							rem_enzetsu -= enzetsu_cost[Target - 6];
							usr[0].money -= 100 * enzetsu_cost[Target - 6];
							approval_rate[Target - 6] += 0.01 * usr[0].lv_enzetsu * ENZETSU_KEISUU;
							approval_rate[Target - 6] = max(0.0001, min(0.9999, approval_rate[Target - 6]));
							if (rem_enzetsu == 0) timestamp = 1.0e7;
						}
					}
				}
				else if (situation % 100 == 8) {
					if (Target == 120) { timestamp = 1.0e7; }
				}
				if (Target == 53) chizu_mode = 3 - chizu_mode;
				clicktime = 0.0;
			}

			// アイドル状態の場合
			if (timestamp >= 1.0e7) {
				if (situation % 100 <= 7) Rect(540, 65, 460, 380).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, (timestamp - 1.0e7) * 255.0 / 0.8)));
				if (situation % 100 == 8) Rect(0, 0, 1000, 600).draw(Color(col_m.r, col_m.g, col_m.b, min(255.0, (timestamp - 1.0e7) * 255.0 / 2.5)));
			}
			if ((situation % 100 <= 7 && timestamp >= 1.0e7 + 0.8) || timestamp >= 1.0e7 + 2.5) {
				if (situation % 100 == 0) { situation = 1001; }
				else if (situation % 100 == 1) {
					if (day_ == news_day1) situation = 1002;
					else if (day_ == news_day2) situation = 1002;
					else if (day_ == chukaku_turn) situation = 1005;
					else situation = 1003;
				}
				else if (situation % 100 == 2) situation = 1003;
				else if (situation % 100 == 3) situation = 1004;
				else if (situation % 100 == 4) situation = 1006;
				else if (situation % 100 == 5) situation = 1006;
				else if (situation % 100 == 6) {
					if (last_option == 1 && player_id == 0) situation = 1007;
					else {
						turns += 1; button[110] = 0.0; button[111] = 0.0; button[112] = 0.0; button[113] = 0.0; button[114] = 0.0; button[115] = 0.0;
						if (turns == 72) situation = 1008;
						else if (turns == 6) situation = 1000;
						else if (turns % 6 == 0) { kakusan(); situation = 1001; }
						else situation = 1006;
						if (turns % 2 == 0) {
							pair<double, double> expc_new = Calc_Giseki();
							history[turns / 2].expc_score = expc_new.first + expc_new.second;
							if (difficulty_ >= 3 && chukaku_applied == false) {
								history[turns / 2].expc_score = (expc_new.first + expc_new.second) * (425.0 / 465.0);
							}
							for (int i = 0; i < 47; i++) history[turns / 2].approval[i] = approval_rate[i];
						}
					}
				}
				else if (situation % 1000 == 7) {
					turns += 1; button[110] = 0.0; button[111] = 0.0; button[112] = 0.0; button[113] = 0.0; button[114] = 0.0; button[115] = 0.0;
					if (turns == 72) situation = 1008;
					else situation = 1006;
				}
				else if (situation % 1000 == 8) { situation = 2000; Record_History(36); OpenVote(); }
				timestamp = 0.0;
			}

			// AI の動きのシミュレーション
			if (situation % 100 == 6 && player_id == 1) {
				if (timestamp >= 1.5 && timestamp < 4999999.0) {
					last_option = AI_Strategy(day_, tm_);
					timestamp = 5.0e6;
				}
				else if (timestamp >= 5.0e6 + 0.9 && timestamp < 9999999.0) timestamp = 1.0e7;
				if (timestamp >= 5.0e6) button[109 + last_option] = 1.0;
			}
		}


		// ======================================================================================================================
		// === 状態 20: 開票速報
		// ======================================================================================================================
		else if (situation >= 2000 && situation <= 2099) {
			font60b(U"小選挙区").draw(60, 25, 15, col_w);
			font60b(U"比例代表").draw(60, 725, 15, col_w);
			Rect(30, 110, 50, 30).draw(Middle(col_m, col_r, 1.0));
			Rect(85, 110, 50, 30).draw(Middle(col_m, col_r, 0.3));
			Rect(140, 110, 50, 30).draw(Middle(col_m, col_b, 0.3));
			Rect(195, 110, 50, 30).draw(Middle(col_m, col_b, 1.0));
			font16b(U"確定").draw(16, 38, 113, col_m);
			font16b(U"優勢").draw(16, 93, 113, col_m);
			font16b(U"優勢").draw(16, 148, 113, col_m);
			font16b(U"確定").draw(16, 203, 113, col_m);
			font14(U"※数字はあなたの獲得議席数").draw(14, 30, 143, col_w);

			// 既に決定した小選挙区
			int progress = min(47, (int)(timestamp / 0.8));
			vector<bool> determined(47, false);
			for (int i = 0; i < progress; i++) determined[46 - i] = true;

			// 現在の議席数の計算
			int giseki_s0 = 0, giseki_h0 = 0;
			int giseki_s1 = 0, giseki_h1 = 0;
			int hyosuu_h0 = 0;
			int hyosuu_h1 = 0;
			for (int i = 0; i < 47; i++) {
				if (determined[i] == false) continue;
				giseki_s0 += actual_shosenkyoku[i][0];
				giseki_s1 += actual_shosenkyoku[i][1];
				hyosuu_h0 += actual_hirei[i][0];
				hyosuu_h1 += actual_hirei[i][1];
			}
			if (progress == 47) {
				giseki_h0 = (int)(176.0 * hyosuu_h0 / 57800000.0 + 0.5);
				giseki_h1 = 176 - giseki_h0;
			}
			else {
				giseki_h0 = (int)(176.0 * hyosuu_h0 / 57800000.0);
				giseki_h1 = (int)(176.0 * hyosuu_h1 / 57800000.0);
			}

			// 表示議席数の更新
			display_hirei0 += (1.0 * hyosuu_h0 - display_hirei0) * min(1.00, 24.0 * Delta);
			display_hirei1 += (1.0 * hyosuu_h1 - display_hirei1) * min(1.00, 24.0 * Delta);
			display_sub0 += (1.0 * giseki_h0 - display_sub0) * min(1.00, 8.0 * Delta);
			display_sub1 += (1.0 * giseki_h1 - display_sub1) * min(1.00, 8.0 * Delta);
			display_total0 += (1.0 * (giseki_s0 + giseki_h0) - display_total0) * min(1.00, 8.0 * Delta);
			display_total1 += (1.0 * (giseki_s1 + giseki_h1) - display_total1) * min(1.00, 8.0 * Delta);

			// 小選挙区の描画
			for (int i = 0; i < 47; i++) {
				double lx = 500.0 * zahyou_lx[i] + 2.0;
				double ly = 500.0 * zahyou_ly[i] + 2.0 + 16.0;
				double rx = 500.0 * zahyou_rx[i] - 2.0;
				double ry = 500.0 * zahyou_ry[i] - 2.0 + 16.0;
				if (determined[i] == true) {
					bool win = false;
					if (actual_shosenkyoku[i][0] > actual_shosenkyoku[i][1]) win = true;
					if (actual_shosenkyoku[i][0] == actual_shosenkyoku[i][1] && actual_hirei[i][0] >= actual_hirei[i][1]) win = true;
					if (win == true) Rect(lx, ly, rx - lx, ry - ly).draw(col_r);
					else Rect(lx, ly, rx - lx, ry - ly).draw(col_b);
					font14b(actual_shosenkyoku[i][0]).draw(14, (lx + rx) / 2.0 - 4.0 * to_string(actual_shosenkyoku[i][0]).size(), (ly + ry) / 2.0 - 12.5, col_w);
				}
				else {
					if (approval_rate[i] > 0.50000001) Rect(lx, ly, rx - lx, ry - ly).draw(Middle(col_m, col_r, 0.3));
					else if (approval_rate[i] < 0.49999999) Rect(lx, ly, rx - lx, ry - ly).draw(Middle(col_m, col_b, 0.3));
					else Rect(lx, ly, rx - lx, ry - ly).draw(Middle(col_m, col_w, 0.2));
				}
			}
			Line(500.0 * 0.054, 500.0 * 0.448 + 16.0, 500.0 * 0.152, 500.0 * 0.448 + 16.0).draw(LineStyle::SquareDot, 2, col_h);
			Line(500.0 * 0.152, 500.0 * 0.448 + 16.0, 500.0 * 0.187, 500.0 * 0.397 + 16.0).draw(LineStyle::SquareDot, 2, col_h);

			// 比例代表の描画
			vector<vector<bool>> used(18, vector<bool>(10, false));
			int yoto_rem = (int)(display_sub0 + 0.5);
			int yato_rem = (int)(display_sub1 + 0.5);
			for (int i = 0; i < 18; i++) {
				for (int j = 0; j < 10; j++) {
					if (i == 0 && j >= 8) continue;
					if (i == 17 && j >= 8) continue;
					if (used[i][j] == true || yoto_rem == 0) continue;
					Rect(541 + i * 24, 136 + j * 24, 21, 21).draw(col_r);
					used[i][j] = true;
					yoto_rem -= 1;
				}
			}
			for (int i = 17; i >= 0; i--) {
				for (int j = 9; j >= 0; j--) {
					if (i == 0 && j >= 8) continue;
					if (i == 17 && j >= 8) continue;
					if (used[i][j] == true || yato_rem == 0) continue;
					Rect(541 + i * 24, 136 + j * 24, 21, 21).draw(col_b);
					used[i][j] = true;
					yato_rem -= 1;
				}
			}
			for (int i = 0; i < 18; i++) {
				for (int j = 0; j < 10; j++) {
					if (i == 0 && j >= 8) continue;
					if (i == 17 && j >= 8) continue;
					if (used[i][j] == true) continue;
					else Rect(541 + i * 24, 136 + j * 24, 21, 21).draw(Middle(col_m, col_w, 0.2));
				}
			}

			// 票数の描画
			int pows[8] = { 1 };
			for (int i = 1; i < 8; i++) pows[i] = pows[i - 1] * 10;
			for (int i = 0; i < 8; i++) {
				int num = ((int)(display_hirei0 + 0.5) / pows[7 - i]) % 10;
				if ((int)(display_hirei0 + 0.5) < pows[7 - i] && i <= 6) font20(num).draw(20, 541 + i * 11, 104, Middle(col_m, col_r, 0.2));
				else font20(num).draw(20, 541 + i * 11, 104, col_r);
			}
			font20(U"票").draw(20, 631, 104, col_r);
			for (int i = 0; i < 8; i++) {
				int num = ((int)(display_hirei1 + 0.5) / pows[7 - i]) % 10;
				if ((int)(display_hirei1 + 0.5) < pows[7 - i] && i <= 6) font20(num).draw(20, 860 + i * 11, 104, Middle(col_m, col_b, 0.2));
				else font20(num).draw(20, 860 + i * 11, 104, col_b);
			}
			font20(U"票").draw(20, 950, 104, col_b);

			// 下の議席数の描画
			Rect(30, 510, 940, 60).draw(col_h);
			Rect(30, 510, 940.0 * display_total0 / 465.0, 60).draw(col_r);
			Rect(30 + 940.0 * (465.0 - display_total1) / 465.0, 510, 940.0 * display_total1 / 465.0, 60).draw(col_b);
			for (int i = 0; i < 3; i++) {
				int num = ((int)(display_total0 + 0.5) / pows[2 - i]) % 10;
				if ((int)(display_total0 + 0.5) < pows[2 - i] && i <= 1) font60b(num).draw(60, 30 + i * 33, 430, Middle(col_m, col_r, 0.2));
				else font60b(num).draw(60, 30 + i * 33, 430, col_r);
			}
			font30b(U"議席").draw(30, 136, 466, col_r);
			for (int i = 0; i < 3; i++) {
				int num = ((int)(display_total1 + 0.5) / pows[2 - i]) % 10;
				if ((int)(display_total1 + 0.5) < pows[2 - i] && i <= 1) font60b(num).draw(60, 804 + i * 33, 430, Middle(col_m, col_b, 0.2));
				else font60b(num).draw(60, 804 + i * 33, 430, col_b);
			}
			font30b(U"議席").draw(30, 910, 466, col_b);
			font25b(U"あなた").draw(25, 30, 409, col_r);
			font25b(ai_tomei).draw(25, 895, 409, col_b);
			Line(500, 500, 500, 570).draw(LineStyle::SquareDot, 3, col_w);
			Rect(420, 455, 160, 45).draw(col_w);
			font25b(U"過半数 233").draw(25, 436, 460, col_m);

			// 開票が終わった場合
			if (timestamp >= 0.8 * 47 + 1.5) {
				Rect(0, 0, 1000, 600).draw(Color(col_m.r, col_m.g, col_m.b, 230.0));
				if (actual_votes[0] > actual_votes[1]) font80(U"勝利！").draw(80, 380, 150, col_w);
				else font80(U"敗北…").draw(80, 380, 150, col_w);
				Rect(400, 360, 200, 60).draw(Middle(col_h, col_y, button[130]));
				font30(U"結果画面へ").draw(30, 425, 368, col_m);
				int Target = -1;
				if (MouseX >= 400.0 && MouseX <= 600.0 && MouseY >= 360.0 && MouseY <= 420.0) { Target = 130; button_flag[130] = true; }
				if (clicktime >= 0.08 && MouseL.down()) {
					if (Target == 130 && situation == 2000) { situation = 2100; timestamp = 0; }
					clicktime = 0.0;
				}
			}
		}


		// ======================================================================================================================
		// === 状態 21: 結果画面
		// ======================================================================================================================
		else if (situation >= 2100 && situation <= 2199) {
			font60b(U"最終結果").draw(60, 25, 15, col_w);
			int display_turn = min(12, (int)(timestamp / 0.3) % 19);

			// 下の議席数の描画
			int pows[3] = { 1, 10, 100 };
			Rect(30, 510, 940, 60).draw(col_h);
			Rect(30, 510, 940.0 * display_total0 / 465.0, 60).draw(col_r);
			Rect(30 + 940.0 * (465.0 - display_total1) / 465.0, 510, 940.0 * display_total1 / 465.0, 60).draw(col_b);
			for (int i = 0; i < 3; i++) {
				int num = ((int)(display_total0 + 0.5) / pows[2 - i]) % 10;
				if ((int)(display_total0 + 0.5) < pows[2 - i] && i <= 1) font60b(num).draw(60, 30 + i * 33, 430, Middle(col_m, col_r, 0.2));
				else font60b(num).draw(60, 30 + i * 33, 430, col_r);
			}
			font30b(U"議席").draw(30, 136, 466, col_r);
			for (int i = 0; i < 3; i++) {
				int num = ((int)(display_total1 + 0.5) / pows[2 - i]) % 10;
				if ((int)(display_total1 + 0.5) < pows[2 - i] && i <= 1) font60b(num).draw(60, 804 + i * 33, 430, Middle(col_m, col_b, 0.2));
				else font60b(num).draw(60, 804 + i * 33, 430, col_b);
			}
			font30b(U"議席").draw(30, 910, 466, col_b);
			font25b(U"あなた").draw(25, 30, 409, col_r);
			font25b(ai_tomei).draw(25, 895, 409, col_b);
			Line(500, 500, 500, 570).draw(LineStyle::SquareDot, 3, col_w);
			Rect(420, 455, 160, 45).draw(col_w);
			font25b(U"過半数 233").draw(25, 436, 460, col_m);

			// 日本地図の描画
			font30b(display_turn).draw(30, 570 - 16 * to_string(display_turn).size(), 25, col_w);
			font30b(U"/12日目の形勢").draw(30, 572, 25, col_w);
			for (int i = 0; i < 47; i++) {
				double lx = 500.0 * zahyou_lx[i] + 2.0 + 507.0;
				double ly = 500.0 * zahyou_ly[i] + 2.0 + 16.0;
				double rx = 500.0 * zahyou_rx[i] - 2.0 + 507.0;
				double ry = 500.0 * zahyou_ry[i] - 2.0 + 16.0;
				Rect(lx, ly, rx - lx, ry - ly).draw(GetColor(history[display_turn * 3].approval[i]));
			}
			Line(500.0 * 0.054 + 507.0, 500.0 * 0.448 + 16.0, 500.0 * 0.152 + 507.0, 500.0 * 0.448 + 16.0).draw(LineStyle::SquareDot, 2, col_h);
			Line(500.0 * 0.152 + 507.0, 500.0 * 0.448 + 16.0, 500.0 * 0.187 + 507.0, 500.0 * 0.397 + 16.0).draw(LineStyle::SquareDot, 2, col_h);

			// 形勢グラフの描画
			Rect(108, 121, 344, 224).draw(col_w);
			Rect(110, 123, 340, 220).draw(col_m);
			Line(110, 133, 450, 133).draw(LineStyle::SquareDot, 2, Middle(col_m, col_w, 0.6));
			Line(110, 222, 450, 222).draw(LineStyle::SquareDot, 2, Middle(col_m, col_w, 0.6));
			Line(110, 333, 450, 333).draw(LineStyle::SquareDot, 2, Middle(col_m, col_w, 0.6));
			font20(U"300議席").draw(20, 29, 116, col_w);
			font20(U"233議席").draw(20, 29, 205, col_w);
			font20(U"150議席").draw(20, 29, 316, col_w);
			font20(U"初日").draw(20, 110, 345, col_w);
			font20(U"最終日").draw(20, 390, 345, col_w);
			for (int i = 0; i < 36; i++) {
				double lx = 110.0 + 340.0 * (i + 0) / 36.0;
				double rx = 110.0 + 340.0 * (i + 1) / 36.0;
				double ly = 333.0 - 200.0 * (max(150.0, min(300.0, history[i + 0].expc_score)) - 150.0) / 150.0;
				double ry = 333.0 - 200.0 * (max(150.0, min(300.0, history[i + 1].expc_score)) - 150.0) / 150.0;
				Line(lx, ly, rx, ry).draw(2, col_y);
			}

			// ツイート画面
			Rect(290, 45, 160, 45).draw(Middle(col_h, col_y, button[140]));
			font25(U"ツイート").draw(25, 320, 49, col_m);
			int Target = -1;
			if (MouseX >= 290.0 && MouseX <= 450.0 && MouseY >= 45.0 && MouseY <= 90.0) { Target = 140; button_flag[140] = true; }
			if (clicktime >= 0.08 && MouseL.down()) {
				if (actual_votes[0] > actual_votes[1]) {
					Twitter::OpenTweetWindow(U"難易度「{}」の選挙が行われ、{} 議席対 {} 議席で勝利しました！ #election_game \nhttps://e869120.github.io/electiongame2026/index.html"_fmt(
						playing_level, actual_votes[0], actual_votes[1]));
				}
				else {
					Twitter::OpenTweetWindow(U"難易度「{}」の選挙が行われ、{} 議席対 {} 議席で敗北しました… #election_game \nhttps://e869120.github.io/electiongame2026/index.html"_fmt(
						playing_level, actual_votes[0], actual_votes[1]));
				}
				clicktime = 0.0;
			}
		}
	}
}
