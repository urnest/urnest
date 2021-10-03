// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <dion/Node.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/test/Calls.hh>
#include <xju/async.hh>
#include <xju/test/call.hh>
#include <dion/job/Executor.hh>

namespace dion
{

class NodeStub : public Node
{
public:
  using Node::Node;
  
  virtual void updateInputsAndState(Date now, job::Starter& starter) override
    // throws job::Ids if inputs are awaiting jobs
    // throws Job::Spec if needs external command run
  {
    calls_.enqueue(xju::test::callTo(*this, &Node::updateInputsAndState)(
                     now, starter))->awaitReturn();
  }
  virtual void tellInputsThatJobsAreDone(job::Collector& c,
                                         job::Ids const& js) noexcept
    override
  {
    calls_.enqueue(xju::test::callTo(*this, &Node::tellInputsThatJobsAreDone)(
                     c,js))->awaitReturn();
  }

  virtual void collectResultOfJob(job::Collector& c, job::Id const& j) noexcept
    override
  {
    calls_.enqueue(xju::test::callTo(*this, &Node::collectResultOfJob)(
                     c, j))->awaitReturn();
  }
  
  mutable xju::test::Calls calls_;
};

class ExecutorStub : public job::Executor
{
public:
  virtual job::Ids jobsInProgress() const noexcept override
  {
    return calls_.enqueue(
      xju::test::callTo(*this, &Executor::jobsInProgress)())
      ->awaitResult();
  }
  virtual job::Ids executorNotBusyJob() const noexcept override
  {
    return calls_.enqueue(
      xju::test::callTo(*this, &job::Executor::executorNotBusyJob)())
      ->awaitResult();
  }
  virtual job::Ids startJob(Date now, job::Spec const& spec) override
    // throw executorNotBusyJob() - executor has enough to do already
  {
    return calls_.enqueue(
      xju::test::callTo(*this, &Executor::startJob)(now, spec))
      ->awaitResult();
  }
  virtual job::Ids readOutputs(job::Ids const& jobs) override
  {
    return calls_.enqueue(
      xju::test::callTo(*this, &Executor::readOutputs)(jobs))
      ->awaitResult();
  }
  virtual std::set<xju::io::Input const*> jobOutputs(job::Ids const& jobs) override
  {
    return calls_.enqueue(
      xju::test::callTo(*this, &Executor::jobOutputs)(jobs))
      ->awaitResult();
  }
  mutable xju::test::Calls calls_;
};
  
}

using namespace dion;

int main(int argc, char* argv[])
{
  // already up-to-date
  {
    auto const deadline(xju::steadyNow()+std::chrono::seconds(5));

    NodeStub n(job::Ids(),Date(1));
    ExecutorStub e;
    
    auto c(xju::test::call([&](){
                        n.update(Date(1), e);
                      }));
    c->getResultBy(deadline);
  }
  
  // job-less update

  // one job, executor not busy

  // one job, executor busy

  // more complex tree, multiple jobs completed out of order
  // more jobs than executor can do at once

  // update() when one input starts a job but another input
  // cannot start job so throws FlatOut, do we lose the first job?
  // Yes we do - most importantly if we started 4 jobs and that was max
  // and then through call when busy, the select loop would not know
  // where to deliver the next done job too.
  // Hhave to go back to original design that had alwaysDone job
  // but instead make it executorNotBusy job i.e. can await that plus
  // whatever others got kicked off.
  //

  // two clients ask for same Node (but different "now")
  // two clients ask for overlapping Nodes
  //
  std::cout << "PASS" << std::endl;
  return 0;
}
