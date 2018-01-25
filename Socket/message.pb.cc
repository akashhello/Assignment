// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#include "message.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)
namespace tutorial {
class log_packetDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<log_packet>
      _instance;
} _log_packet_default_instance_;
}  // namespace tutorial
namespace protobuf_message_2eproto {
void InitDefaultslog_packetImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::tutorial::_log_packet_default_instance_;
    new (ptr) ::tutorial::log_packet();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tutorial::log_packet::InitAsDefaultInstance();
}

void InitDefaultslog_packet() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultslog_packetImpl);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tutorial::log_packet, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tutorial::log_packet, log_msg_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tutorial::log_packet)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::tutorial::_log_packet_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "message.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\rmessage.proto\022\010tutorial\"\035\n\nlog_packet\022"
      "\017\n\007log_msg\030\001 \001(\tb\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 64);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "message.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_message_2eproto
namespace tutorial {

// ===================================================================

void log_packet::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int log_packet::kLogMsgFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

log_packet::log_packet()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_message_2eproto::InitDefaultslog_packet();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:tutorial.log_packet)
}
log_packet::log_packet(const log_packet& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  log_msg_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.log_msg().size() > 0) {
    log_msg_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.log_msg_);
  }
  // @@protoc_insertion_point(copy_constructor:tutorial.log_packet)
}

void log_packet::SharedCtor() {
  log_msg_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  _cached_size_ = 0;
}

log_packet::~log_packet() {
  // @@protoc_insertion_point(destructor:tutorial.log_packet)
  SharedDtor();
}

void log_packet::SharedDtor() {
  log_msg_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void log_packet::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* log_packet::descriptor() {
  ::protobuf_message_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_message_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const log_packet& log_packet::default_instance() {
  ::protobuf_message_2eproto::InitDefaultslog_packet();
  return *internal_default_instance();
}


void log_packet::Clear() {
// @@protoc_insertion_point(message_clear_start:tutorial.log_packet)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  log_msg_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  _internal_metadata_.Clear();
}

bool log_packet::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tutorial.log_packet)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string log_msg = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_log_msg()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->log_msg().data(), static_cast<int>(this->log_msg().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "tutorial.log_packet.log_msg"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:tutorial.log_packet)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tutorial.log_packet)
  return false;
#undef DO_
}

void log_packet::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tutorial.log_packet)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string log_msg = 1;
  if (this->log_msg().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->log_msg().data(), static_cast<int>(this->log_msg().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tutorial.log_packet.log_msg");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->log_msg(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tutorial.log_packet)
}

::google::protobuf::uint8* log_packet::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tutorial.log_packet)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string log_msg = 1;
  if (this->log_msg().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->log_msg().data(), static_cast<int>(this->log_msg().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "tutorial.log_packet.log_msg");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->log_msg(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tutorial.log_packet)
  return target;
}

size_t log_packet::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tutorial.log_packet)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // string log_msg = 1;
  if (this->log_msg().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->log_msg());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void log_packet::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tutorial.log_packet)
  GOOGLE_DCHECK_NE(&from, this);
  const log_packet* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const log_packet>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tutorial.log_packet)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tutorial.log_packet)
    MergeFrom(*source);
  }
}

void log_packet::MergeFrom(const log_packet& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tutorial.log_packet)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.log_msg().size() > 0) {

    log_msg_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.log_msg_);
  }
}

void log_packet::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tutorial.log_packet)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void log_packet::CopyFrom(const log_packet& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tutorial.log_packet)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool log_packet::IsInitialized() const {
  return true;
}

void log_packet::Swap(log_packet* other) {
  if (other == this) return;
  InternalSwap(other);
}
void log_packet::InternalSwap(log_packet* other) {
  using std::swap;
  log_msg_.Swap(&other->log_msg_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata log_packet::GetMetadata() const {
  protobuf_message_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_message_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace tutorial
namespace google {
namespace protobuf {
template<> GOOGLE_PROTOBUF_ATTRIBUTE_NOINLINE ::tutorial::log_packet* Arena::Create< ::tutorial::log_packet >(Arena* arena) {
  return Arena::CreateInternal< ::tutorial::log_packet >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)