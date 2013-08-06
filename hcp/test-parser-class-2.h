class X : public Fred
{
public:
  int x_;
  X():
    x_(1) {
  }
  
  X(int y) try:
  x_(1) {
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "construct from " << y;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
  
  template<class T>
  void f(T const& y) throw();
  
  template<class T>
  void f(T const& y) throw() {
    impl();
  }

private:
  typedef std::vector<int> Numbers;
  
  Numbers numbers_;
  
};

