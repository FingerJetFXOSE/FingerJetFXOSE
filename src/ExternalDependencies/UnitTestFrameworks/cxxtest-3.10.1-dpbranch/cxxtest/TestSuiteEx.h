#ifndef __cxxtest__TestSuiteEx_h__
#define __cxxtest__TestSuiteEx_h__

#include <cxxtest/TestSuite.h>
#include <cxxtest/RealDescriptions.h>

namespace CxxTest {

class base_test_suite : public TestSuite {
public:
  base_test_suite() {
    lstTests.initialize();
  }

  List lstTests;
  StaticSuiteDescription sd;

  void initialize(const char *File, unsigned Line, const char *SuiteName) {
    sd.initialize(File, Line, SuiteName, *this, lstTests);
  }
};

class test_suite : public virtual base_test_suite {
};

template <class _T>
class test_suite_ : public virtual base_test_suite {
public:
  typedef _T suite_t;
};

template <class _T> class suite_instance : public _T {
public:
  suite_instance() {}
  suite_instance(const char *File, unsigned Line, const char *SuiteName) {
    base_test_suite::initialize(File, Line, SuiteName);
  }
  void initialize(const char *File, unsigned Line, const char *SuiteName) {
    base_test_suite::initialize(File, Line, SuiteName);
  }
};

}; // namespace CxxTest


#define TEST_SUITE(suite_name) suite_name : public CxxTest::test_suite_<suite_name>

#define TEST_SUITE_PTR(name) ((suite_t *)(((unsigned char *)this) - offsetof(suite_t, m_##name)))

#define TEST(name) \
  private: struct name : public CxxTest::RealTestDescription { \
    name() { initialize(TEST_SUITE_PTR(name)->lstTests, TEST_SUITE_PTR(name)->sd, __LINE__, __FUNCTION__); } \
    void runTest() { TEST_SUITE_PTR(name) -> name(); } \
  } m_##name; \
  public: void name()

#define REPEATED_TEST(name, index, count) \
  private: \
  struct name : public CxxTest::RealTestDescription { \
    suite_t * suite; \
    int i; \
    void init(suite_t * suite_, int i_) { \
      suite = suite_; i = i_; \
      initialize(suite_->lstTests, suite_->sd, __LINE__, __FUNCTION__); \
    } \
    void runTest() { suite -> name(i); } \
  }; \
  struct name##_array { \
    name tds[count]; \
    name##_array() { for (int i = 0; i < count; ++i) tds[i].init(TEST_SUITE_PTR(name), i); } \
  } m_##name; \
  public: void name(int index)

// Usage: the following will fail 2 assertions
//class TEST_SUITE(TestRepeated) {
//  REPEATED_TEST(repeated, i, 2) {
//     TS_ASSERT_EQUALS(2, i);
//  }
//};

/// The following does not work on armcc: "explicit specialization is not allowed in the current scope"
//#define REPEATED_TEST_T(name, index, count) \
//  private: \
//  template <int I> struct name : public CxxTest::RealTestDescription { \
//    name<I - 1> nested; \
//    name(suite_t * suite_) : nested(suite_) { initialize(suite()->lstTests, suite()->sd, __LINE__, __FUNCTION__); } \
//    void runTest() { suite() -> name##_index<I - 1>(); } \
//    suite_t * suite() { return nested.suite(); } \
//  }; \
//  template <> struct suite_t::name<0> {  \
//    suite_t * m_suite; \
//    name(suite_t * suite_) : m_suite(suite_) {} \
//    suite_t * suite() { return m_suite; } \
//  }; \
//  struct name##_array { \
//    name<count> tds; \
//    name##_array() : tds(TEST_SUITE_PTR(name)) {} \
//  } m_##name; \
//  public: template <int index> void name##_index()

#endif // __cxxtest__TestSuiteEx_h__
