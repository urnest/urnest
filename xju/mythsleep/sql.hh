namespace xju
{
namespace mythsleep
{
namespace sql
{

class Recording
{
public:
  Recording(std::string title,
            Time start,
            Time end) throw():
    title_(title),
    start_(start),
    end_(end) {
  }
  
  std::string title_;
  Time start_;
  Time end_;

  friend bool operator<(Recording const& x,
                        Recording const& y) throw() {
    if (x.title_ < y.title_) return true;
    if (y.title_ < x.title_) return false;
    if (x.start_ < y.start_) return true;
    if (y.start_ < x.start_) return false;
    if (x.end_ < y.end_) return true;
    if (y.end_ < x.end_) return false;

    return false;
  }
  
};

std::set<Recording> getRecordings() throw(
  Exception);

}
}
}
