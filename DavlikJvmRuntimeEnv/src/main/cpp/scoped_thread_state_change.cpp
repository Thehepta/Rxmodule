////
//// Created by thehepta on 2023/8/13.
////
//
//#include "runtime/scoped_thread_state_change.h"
//
//inline ScopedThreadStateChange::ScopedThreadStateChange(Thread* self, ThreadState new_thread_state)
//        : self_(self), thread_state_(new_thread_state), expected_has_no_thread_(false) {
//    if (UNLIKELY(self_ == nullptr)) {
//        // Value chosen arbitrarily and won't be used in the destructor since thread_ == null.
//        old_thread_state_ = ThreadState::kTerminated;
//        Runtime* runtime = Runtime::Current();
//        CHECK(runtime == nullptr || !runtime->IsStarted() || runtime->IsShuttingDown(self_));
//    } else {
//        DCHECK_EQ(self, Thread::Current());
//        // Read state without locks, ok as state is effectively thread local and we're not interested
//        // in the suspend count (this will be handled in the runnable transitions).
//        old_thread_state_ = self->GetState();
//        if (old_thread_state_ != new_thread_state) {
//            if (new_thread_state == ThreadState::kRunnable) {
//                self_->TransitionFromSuspendedToRunnable();
//            } else if (old_thread_state_ == ThreadState::kRunnable) {
//                self_->TransitionFromRunnableToSuspended(new_thread_state);
//            } else {
//                // A suspended transition to another effectively suspended transition, ok to use Unsafe.
//                self_->SetState(new_thread_state);
//            }
//        }
//    }
//}
//
//inline ScopedThreadStateChange::~ScopedThreadStateChange() {
//    if (UNLIKELY(self_ == nullptr)) {
//        ScopedThreadChangeDestructorCheck();
//    } else {
//        if (old_thread_state_ != thread_state_) {
//            if (old_thread_state_ == ThreadState::kRunnable) {
//                self_->TransitionFromSuspendedToRunnable();
//            } else if (thread_state_ == ThreadState::kRunnable) {
//                self_->TransitionFromRunnableToSuspended(old_thread_state_);
//            } else {
//                // A suspended transition to another effectively suspended transition, ok to use Unsafe.
//                self_->SetState(old_thread_state_);
//            }
//        }
//    }
//}
//
//template<typename T>
//inline T ScopedObjectAccessAlreadyRunnable::AddLocalReference(ObjPtr<mirror::Object> obj) const {
//    Locks::mutator_lock_->AssertSharedHeld(Self());
//    if (kIsDebugBuild) {
//        CHECK(IsRunnable());  // Don't work with raw objects in non-runnable states.
//        DCheckObjIsNotClearedJniWeakGlobal(obj);
//    }
//    return obj == nullptr ? nullptr : Env()->AddLocalReference<T>(obj);
//}
//
//template<typename T>
//inline ObjPtr<T> ScopedObjectAccessAlreadyRunnable::Decode(jobject obj) const {
//    Locks::mutator_lock_->AssertSharedHeld(Self());
//    DCHECK(IsRunnable());  // Don't work with raw objects in non-runnable states.
//    return ObjPtr<T>::DownCast(Self()->DecodeJObject(obj));
//}
//
//inline bool ScopedObjectAccessAlreadyRunnable::IsRunnable() const {
//    return self_->GetState() == ThreadState::kRunnable;
//}
//
//inline ScopedObjectAccessAlreadyRunnable::ScopedObjectAccessAlreadyRunnable(JNIEnv* g_env)
//        : self_(Thread::ForEnv(g_env)), env_(down_cast<JNIEnvExt*>(g_env)), vm_(env_->GetVm()) {}
//
//inline ScopedObjectAccessAlreadyRunnable::ScopedObjectAccessAlreadyRunnable(Thread* self)
//        : self_(self),
//          env_(down_cast<JNIEnvExt*>(self->GetJniEnv())),
//          vm_(env_ != nullptr ? env_->GetVm() : nullptr) {}
//
//inline ScopedObjectAccessUnchecked::ScopedObjectAccessUnchecked(JNIEnv* g_env)
//        : ScopedObjectAccessAlreadyRunnable(g_env), tsc_(Self(), ThreadState::kRunnable) {
//    Self()->VerifyStack();
//    Locks::mutator_lock_->AssertSharedHeld(Self());
//}
//
//inline ScopedObjectAccessUnchecked::ScopedObjectAccessUnchecked(Thread* self)
//        : ScopedObjectAccessAlreadyRunnable(self), tsc_(self, ThreadState::kRunnable) {
//    Self()->VerifyStack();
//    Locks::mutator_lock_->AssertSharedHeld(Self());
//}
//
//inline ScopedObjectAccess::ScopedObjectAccess(JNIEnv* g_env) : ScopedObjectAccessUnchecked(g_env) {}
//inline ScopedObjectAccess::ScopedObjectAccess(Thread* self) : ScopedObjectAccessUnchecked(self) {}
//inline ScopedObjectAccess::~ScopedObjectAccess() {}
//
//inline ScopedThreadSuspension::ScopedThreadSuspension(Thread* self, ThreadState suspended_state)
//        : self_(self), suspended_state_(suspended_state) {
//    DCHECK(self_ != nullptr);
//    self_->TransitionFromRunnableToSuspended(suspended_state);
//}
//
//inline ScopedThreadSuspension::~ScopedThreadSuspension() {
//    DCHECK_EQ(self_->GetState(), suspended_state_);
//    self_->TransitionFromSuspendedToRunnable();
//}