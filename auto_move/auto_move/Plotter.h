#pragma once

class Plotter {

	std::list<double> points;
	RectF parent;          /// 描画エリア
	const Font font;       /// ラベル用フォント
	const int resolution;  /// 分解能

	double min;
	double max;

	static constexpr std::array<double, 4> graphPadding = { 20, 30, 20, 30 };  ///グラフ表示域と親要素との間隔 [上,右,下,左]

public:

	/// @brief プロッタ作成
	/// @param parent グラフを描画するエリア
	/// @param resolution 分解能
	Plotter(RectF&& parent = {}, const int resolution = 500) noexcept
		: points{                   }
		, parent{ std::move(parent) }
		, font{ 15 }
		, resolution(resolution > 0 ? resolution : 1)
		, min(std::numeric_limits<double>::max())
		, max(std::numeric_limits<double>::min())
	{}

	/// @brief サイズ更新
	/// @param r 更新するサイズ
	Plotter& resize(const RectF& r) {
		parent = r;
		return *this;
	}
	Plotter& resize(const Plotter& r) {
		parent = r.parent;
		return *this;
	}

	/// @brief プロットするデータをセット
	/// @param r プロットするデータ
	Plotter& plot(double r) {
		points.push_front(r);

		max = std::max(max, r);
		min = std::min(min, r);

		/// 設定分解能を超えたデータを破棄
		if (points.size() > resolution) {
			const auto temp = points.back();
			points.pop_back();
			/// 最小最大値再取得
			if (temp == max) {
				for (const auto& it : points) {
					max = std::max(it, max);
				}
			}
			if (temp == min) {
				for (const auto& it : points) {
					min = std::min(it, min);
				}
			}
		}
		return *this;
	}

	Plotter& minimum(double value) {
		min = value;
		return *this;
	}
	Plotter& maximum(double value) {
		max = value;
		return *this;
	}

	/// @brief グラフ描画
	/// @param color グラフの色
	Plotter& draw(const ColorF& color = Palette::White) {

		const auto plotArea = getPlotArea();

		if (plotArea.h >= 0 && plotArea.w >= 0) {


			/// 点の高さを求める
			const auto dataToPointHeight = [&](double data) ->double {
				const double ratio = plotArea.h / (max - min ? max - min : 1) * 0.9;  /// オートスケーリング用比率
				return plotArea.right().end.y - (data - min) * ratio;
			};

			/// 点間の距離
			const double plotInterval = plotArea.w / (resolution - 1);

			/// 線分開始位置
			Vec2 holdPoint = {
				plotArea.right().end.x,
				dataToPointHeight(points.front())
			};

			/// 全ての隣り合う点を線分で結ぶ
			for (auto it = (++points.begin()); it != points.end(); ++it) {
				const Vec2 begin = holdPoint;

				holdPoint.x -= plotInterval;
				holdPoint.y = dataToPointHeight(*it);

				const Vec2 end = holdPoint;

				Line{ begin, end }.draw(2, color);
			}
		}

		return *this;
	}

	/// @brief グリッド描画
	/// @param horizontal 横方向の分割個数
	/// @param vertical   縦方向の分割個数
	Plotter& drawGrid(int horizontal = 7, int vertical = 5, const ColorF& color = Palette::Gray) {
		const auto plotArea = getPlotArea();

		if (plotArea.h >= 0 && plotArea.w >= 0) {

			/// 横分割
			for (auto i : step(horizontal + 1)) {
				const double width = plotArea.w / horizontal * i;

				plotArea.left().moveBy(width, 0).draw(color);

				/// タイムスタンプ
				font(static_cast<int64>(Time::GetMillisec() - (1.0 / Profiler::FPS()) * resolution * i * 1000 / horizontal))
					.drawAt(plotArea.bottom().begin.x - width, plotArea.bottom().begin.y + 10, color);
			}
			/// 縦分割
			for (auto i : step(vertical + 1)) {
				plotArea.top().moveBy(0, plotArea.h * i / vertical).draw(color);
			}
		}
		return *this;
	}

	/// @brief グラフの表示域取得
	RectF getPlotArea() const {
		return padding(graphPadding[0], graphPadding[1], graphPadding[2], graphPadding[3]);
	}

private:
	RectF padding(double top, double right, double bottom, double left) const {
		return {
			parent.x + left,
			parent.y + top,
			parent.w - left - right,
			parent.h - top - bottom
		};
	}
};
