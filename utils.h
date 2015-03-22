#ifndef INC_GUARD_X1LG781TUJAXL9K2W4YPT6B57KFW2P
#define INC_GUARD_X1LG781TUJAXL9K2W4YPT6B57KFW2P

namespace noncopy_ {

/// Derive from this class to prevent the compiler from automatically
/// generating copy constructor and assignment operator. This is useful
/// for resource managing classes that don't need to be copyable. Otherwise
/// you might run into double-free errors.
class NonCopyable {
protected:
    NonCopyable() {}
    ~NonCopyable() {}
private:
    NonCopyable(NonCopyable const&) = delete;
    NonCopyable& operator=(NonCopyable const&) = delete;
};

// This class lives in its own namespace to protect from unintended ADL.

} // namespace noncopy_

using noncopy_::NonCopyable;

#endif /* include guard */

