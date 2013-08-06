namespace xju
{
namespace mythsleep
{
namespace sql
{

std::set<Recording> getRecordings() throw(
  Exception)
{
  try {
    
  }
  catch(Exception& e) {
    std::ostringstream s;
    s << "get recordings";
    e.addContext(s, XJU_TRACED);
    throw;
  }
}


}
}
}
