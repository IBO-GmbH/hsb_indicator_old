#include "Co2ProgressWidget.hpp"
#include <QPainter>
#include "ui_Co2ProgressWidget.h"

using namespace mold;

Co2ProgressWidget::Co2ProgressWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::Co2ProgressWidget) {
  ui->setupUi(this);
}

void Co2ProgressWidget::setValue(const float co2) {
  paintBar();
  setPositionProgressLine(co2);
}

Co2ProgressWidget::~Co2ProgressWidget() = default;

void Co2ProgressWidget::paintBar() { update(); }

void Co2ProgressWidget::setPositionProgressLine(const float co2) {
  auto position = calculatePositionLine(co2);
  ui->line->move(position);
}

QPoint Co2ProgressWidget::calculatePositionLine(const float co2) {
  float value = std::min(3000.f, co2);
  int progress_position = static_cast<int>(ui->bar->width() / 3000.f * value);
  int position_x = progress_position - ui->line->width() / 2;
  return {position_x, 0};
}

void Co2ProgressWidget::paintEvent(QPaintEvent* /*event*/) {
  QPainter painter(this);
  using range = std::pair<int, QColor>;
  std::array<range, 5> backgroundColors = {
      {{800, QColor(0x79, 0xd6, 0x81, 0xff)},
       {1000, QColor(0xcd, 0xfa, 0xbd, 0xff)},
       {1400, QColor(0xfe, 0xf7, 0x7b, 0xff)},
       {2000, QColor(0xfe, 0xc7, 0x7b, 0xff)},
       {3000, QColor(0xfe, 0x8a, 0x7b, 0xff)}}};
  int last{0};
  const auto end = backgroundColors.back().first;
  for (const auto& render : backgroundColors) {
    const auto width_ = render.first * ui->bar->width() / end - last;
    painter.fillRect(last, 0, width_, ui->bar->height(), render.second);
    last = render.first * ui->bar->width() / end;
  }
}
