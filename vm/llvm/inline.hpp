#include "llvm/jit_operations.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/iseq.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/module.hpp"

namespace rubinius {

  class NativeFunction;
  class Context;

  class Inliner {
    Context* ctx_;
    JITOperations& ops_;
    InlineCache** cache_ptr_;
    InlineCache* cache_;
    int count_;
    int self_pos_;
    BasicBlock* class_id_failure_;
    BasicBlock* serial_id_failure_;
    Value* result_;
    bool check_for_exception_;
    JITInlineBlock* inline_block_;
    JITInlineBlock* block_info_;

    bool block_on_stack_;
    bool from_unboxed_array_;

    JITMethodInfo* creator_info_;

    bool fail_to_send_;

    type::KnownType guarded_type_;

  public:

    Inliner(Context* ctx,
            JITOperations& ops, InlineCache** cache_ptr, int count,
            BasicBlock* class_id_failure, BasicBlock* serial_id_failure)
      : ctx_(ctx)
      , ops_(ops)
      , cache_ptr_(cache_ptr)
      , cache_(*cache_ptr)
      , count_(count)
      , self_pos_(count)
      , class_id_failure_(class_id_failure)
      , serial_id_failure_(serial_id_failure)
      , result_(0)
      , check_for_exception_(true)
      , inline_block_(0)
      , block_info_(0)
      , block_on_stack_(false)
      , from_unboxed_array_(false)
      , creator_info_(0)
      , fail_to_send_(false)
    {}

    Inliner(Context* ctx, JITOperations& ops, int count)
      : ctx_(ctx)
      , ops_(ops)
      , cache_(0)
      , count_(count)
      , class_id_failure_(0)
      , serial_id_failure_(0)
      , result_(0)
      , check_for_exception_(true)
      , inline_block_(0)
      , block_info_(0)
      , block_on_stack_(false)
      , from_unboxed_array_(false)
      , creator_info_(0)
    {}

    Context* context() {
      return ctx_;
    }

    Value* recv() {
      return ops_.stack_back(self_pos_);
    }

    Value* arg(int which) {
      return ops_.stack_back(self_pos_ - (which + 1));
    }

    BasicBlock* class_id_failure() {
      return class_id_failure_;
    }

    BasicBlock* serial_id_failure() {
      return serial_id_failure_;
    }

    Value* result() {
      return result_;
    }

    void set_result(Value* val) {
      result_ = val;
    }

    void set_class_id_failure(BasicBlock* class_id_failure) {
      class_id_failure_ = class_id_failure;
    }

    void set_serial_id_failure(BasicBlock* serial_id_failure) {
      serial_id_failure_ = serial_id_failure;
    }

    void exception_safe() {
      check_for_exception_ = false;
    }

    bool check_for_exception() {
      return check_for_exception_;
    }

    void set_inline_block(JITInlineBlock* ib) {
      inline_block_ = ib;
    }

    JITInlineBlock* inline_block() {
      return inline_block_;
    }

    void set_block_on_stack() {
      self_pos_++;
      block_on_stack_ = true;
    }

    void set_from_unboxed_array() {
      from_unboxed_array_ = true;
    }

    void set_creator(JITMethodInfo* home) {
      creator_info_ = home;
    }

    void set_block_info(JITInlineBlock* ib) {
      block_info_ = ib;
    }

    bool fail_to_send() {
      return fail_to_send_;
    }

    void use_send_for_failure() {
      fail_to_send_ = true;
    }

    type::KnownType guarded_type() {
      return guarded_type_;
    }

    bool consider_mono();

    bool consider_poly();

    bool inline_for_class(InlineCacheEntry* ice, int hits);

    void inline_block(JITInlineBlock* ib, Value* self);

    void inline_generic_method(InlineCacheEntry* ice, Module* mod, CompiledCode* code, MachineCode* mcode, int hits);

    bool detect_trivial_method(MachineCode* mcode, CompiledCode* code = 0);

    void inline_trivial_method(InlineCacheEntry* ice, CompiledCode* code);

    void inline_ivar_write(InlineCacheEntry* ice, AccessVariable* acc);

    void inline_ivar_access(InlineCacheEntry* ice, AccessVariable* acc);

    bool inline_primitive(InlineCacheEntry* ice, CompiledCode* code, executor prim);

    bool inline_ffi(InlineCacheEntry* klass, NativeFunction* nf);

    void emit_inline_block(JITInlineBlock* ib, Value* val);

    int detect_jit_intrinsic(InlineCacheEntry* ice, CompiledCode* code);
    void inline_intrinsic(InlineCacheEntry* ice, CompiledCode* code, int which);

    void check_class(llvm::Value* recv, InlineCacheEntry* ice);
    void check_recv(InlineCacheEntry* ice);

    void prime_info(JITMethodInfo& info);
  };

}
