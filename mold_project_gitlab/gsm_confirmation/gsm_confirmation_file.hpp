
class gsm_confirmation_file {
 public:
  gsm_confirmation_file() = delete;
  ~gsm_confirmation_file() = delete;

 public:
  static void write_confirmed();
  static bool read_confirmed();
};
