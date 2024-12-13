#include "MoldValueProgressWidget.hpp"
#include <QPainter>
#include "ui_MoldValueProgressWidget.h"
#include "wolf/math_clamp.hpp"

mold::MoldValueProgressWidget::MoldValueProgressWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::MoldValueProgressWidget) {
  ui->setupUi(this);
}

mold::MoldValueProgressWidget::~MoldValueProgressWidget() = default;

void mold::MoldValueProgressWidget::setThresholds(const int yellow,
                                                  const int red) {
  m_yellow = yellow;
  m_red = red;
}

void mold::MoldValueProgressWidget::paintBar() { update(); }

void mold::MoldValueProgressWidget::setPositions(const int mold_value) {
  setPositionProgressLine(mold_value);
  setPositionProgressValue(mold_value);
}

void mold::MoldValueProgressWidget::setValue(const int mold_value) {
  paintBar();
  setPositions(mold_value);
}

void mold::MoldValueProgressWidget::setPositionProgressLine(
    const int mold_value) {
  auto position = calculatePositionLine(mold_value);
  ui->line->move(position);
}

void mold::MoldValueProgressWidget::setPositionProgressValue(
    const int mold_value) {
  auto value_in_percent = QStringLiteral(" %1% ").arg(mold_value);
  ui->value->setText(value_in_percent);
  auto position = calculatePositionValue(mold_value);
  ui->value->move(position);
}

QPoint mold::MoldValueProgressWidget::calculatePositionLine(
    const int mold_value) const {
  int progress_position = calculateProgressPosition(mold_value);
  int position_x = progress_position - ui->line->width() / 2;
  return {position_x, 0};
}

QPoint mold::MoldValueProgressWidget::calculatePositionValue(
    const int mold_value) const {
  int progress_position = calculateProgressPosition(mold_value);
  ui->value->adjustSize();
  int position_x = progress_position - ui->value->width() / 2;
  const int max_position = ui->bar->width() - ui->value->width();
  position_x = wolf::math::clamp(position_x, 0, max_position);
  return {position_x, ui->line->height()};
}

int mold::MoldValueProgressWidget::calculateProgressPosition(
    const int mold_value) const {
  return static_cast<int>(static_cast<float>(ui->bar->width()) /
                          static_cast<float>(100) * mold_value);
}

void mold::MoldValueProgressWidget::paintEvent(QPaintEvent * /*event*/) {
  QPainter painter(this);
  using range = std::pair<int, QColor>;
  std::array<range, 3> backgroundColors = {
      {{m_yellow, QColor(0x79, 0xd6, 0x81, 0xff)},
       {m_red, QColor(0xfe, 0xf7, 0x7b, 0xff)},
       {100, QColor(0xfe, 0x8a, 0x7b, 0xff)}}};
  int last{0};
  const auto end = backgroundColors.back().first;
  for (const auto &render : backgroundColors) {
    const auto width_ = render.first * ui->bar->width() / end - last;
    painter.fillRect(last, 0, width_, ui->bar->height(), render.second);
    last = render.first * ui->bar->width() / end;
  }
}
