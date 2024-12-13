#ifndef MOLD_GUI_ACTORLIST_HPP
#define MOLD_GUI_ACTORLIST_HPP

#include <QWidget>
#include <memory>
#include "ControllerTypes.hpp"

namespace Ui {
class ActorList;
}

namespace mold {

class ActorList : public QWidget {
  Q_OBJECT

 public:
  using index_array = std::array<int, 4>;
  using string_array = std::array<std::string, 4>;
  using label_array = std::array<QString, 4>;

  explicit ActorList(const ControllerTypes &type, const int button,
                     const int page, QWidget *parent = nullptr);
  ~ActorList();
  int getPage() const;
  index_array getNewActorIds();

 signals:
  void back();
  void chooseActor1();
  void chooseActor2();
  void chooseActor3();
  void chooseActor4();
  void previous();
  void next();

 private slots:
  void handlePrevious();
  void handleNext();

 private:
  void changePage(const int page);
  void changeActorName(const int page);
  index_array calculateNewIndex(const int page);
  string_array convertIndexToString(const index_array &index);
  label_array makeLabelFromString(const string_array &index_string);
  void setNewActorNames(const label_array &new_names);

 private:
  std::unique_ptr<Ui::ActorList> ui;
  index_array m_new_actor_ids;
  int m_page;
  const int m_max_actors = 128;
};
}  // namespace mold
#endif  // MOLD_GUI_ACTORLIST_HPP
