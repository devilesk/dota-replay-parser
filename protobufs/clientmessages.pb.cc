// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: clientmessages.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "clientmessages.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* CClientMsg_CustomGameEvent_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  CClientMsg_CustomGameEvent_reflection_ = NULL;
const ::google::protobuf::Descriptor* CClientMsg_TrackedControllerInput_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  CClientMsg_TrackedControllerInput_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* EBaseClientMessages_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_clientmessages_2eproto() {
  protobuf_AddDesc_clientmessages_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "clientmessages.proto");
  GOOGLE_CHECK(file != NULL);
  CClientMsg_CustomGameEvent_descriptor_ = file->message_type(0);
  static const int CClientMsg_CustomGameEvent_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CClientMsg_CustomGameEvent, event_name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CClientMsg_CustomGameEvent, data_),
  };
  CClientMsg_CustomGameEvent_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      CClientMsg_CustomGameEvent_descriptor_,
      CClientMsg_CustomGameEvent::default_instance_,
      CClientMsg_CustomGameEvent_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CClientMsg_CustomGameEvent, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CClientMsg_CustomGameEvent, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(CClientMsg_CustomGameEvent));
  CClientMsg_TrackedControllerInput_descriptor_ = file->message_type(1);
  static const int CClientMsg_TrackedControllerInput_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CClientMsg_TrackedControllerInput, data_),
  };
  CClientMsg_TrackedControllerInput_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      CClientMsg_TrackedControllerInput_descriptor_,
      CClientMsg_TrackedControllerInput::default_instance_,
      CClientMsg_TrackedControllerInput_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CClientMsg_TrackedControllerInput, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CClientMsg_TrackedControllerInput, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(CClientMsg_TrackedControllerInput));
  EBaseClientMessages_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_clientmessages_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    CClientMsg_CustomGameEvent_descriptor_, &CClientMsg_CustomGameEvent::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    CClientMsg_TrackedControllerInput_descriptor_, &CClientMsg_TrackedControllerInput::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_clientmessages_2eproto() {
  delete CClientMsg_CustomGameEvent::default_instance_;
  delete CClientMsg_CustomGameEvent_reflection_;
  delete CClientMsg_TrackedControllerInput::default_instance_;
  delete CClientMsg_TrackedControllerInput_reflection_;
}

void protobuf_AddDesc_clientmessages_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\024clientmessages.proto\">\n\032CClientMsg_Cus"
    "tomGameEvent\022\022\n\nevent_name\030\001 \001(\t\022\014\n\004data"
    "\030\002 \001(\014\"1\n!CClientMsg_TrackedControllerIn"
    "put\022\014\n\004data\030\001 \001(\014*`\n\023EBaseClientMessages"
    "\022\027\n\022CM_CustomGameEvent\020\230\002\022\036\n\031CM_TrackedC"
    "ontrollerInput\020\231\002\022\020\n\013CM_MAX_BASE\020\254\002B\005H\001\200"
    "\001\000", 242);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "clientmessages.proto", &protobuf_RegisterTypes);
  CClientMsg_CustomGameEvent::default_instance_ = new CClientMsg_CustomGameEvent();
  CClientMsg_TrackedControllerInput::default_instance_ = new CClientMsg_TrackedControllerInput();
  CClientMsg_CustomGameEvent::default_instance_->InitAsDefaultInstance();
  CClientMsg_TrackedControllerInput::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_clientmessages_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_clientmessages_2eproto {
  StaticDescriptorInitializer_clientmessages_2eproto() {
    protobuf_AddDesc_clientmessages_2eproto();
  }
} static_descriptor_initializer_clientmessages_2eproto_;
const ::google::protobuf::EnumDescriptor* EBaseClientMessages_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return EBaseClientMessages_descriptor_;
}
bool EBaseClientMessages_IsValid(int value) {
  switch(value) {
    case 280:
    case 281:
    case 300:
      return true;
    default:
      return false;
  }
}


// ===================================================================

#ifndef _MSC_VER
const int CClientMsg_CustomGameEvent::kEventNameFieldNumber;
const int CClientMsg_CustomGameEvent::kDataFieldNumber;
#endif  // !_MSC_VER

CClientMsg_CustomGameEvent::CClientMsg_CustomGameEvent()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:CClientMsg_CustomGameEvent)
}

void CClientMsg_CustomGameEvent::InitAsDefaultInstance() {
}

CClientMsg_CustomGameEvent::CClientMsg_CustomGameEvent(const CClientMsg_CustomGameEvent& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:CClientMsg_CustomGameEvent)
}

void CClientMsg_CustomGameEvent::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  event_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  data_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

CClientMsg_CustomGameEvent::~CClientMsg_CustomGameEvent() {
  // @@protoc_insertion_point(destructor:CClientMsg_CustomGameEvent)
  SharedDtor();
}

void CClientMsg_CustomGameEvent::SharedDtor() {
  if (event_name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete event_name_;
  }
  if (data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete data_;
  }
  if (this != default_instance_) {
  }
}

void CClientMsg_CustomGameEvent::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* CClientMsg_CustomGameEvent::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return CClientMsg_CustomGameEvent_descriptor_;
}

const CClientMsg_CustomGameEvent& CClientMsg_CustomGameEvent::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_clientmessages_2eproto();
  return *default_instance_;
}

CClientMsg_CustomGameEvent* CClientMsg_CustomGameEvent::default_instance_ = NULL;

CClientMsg_CustomGameEvent* CClientMsg_CustomGameEvent::New() const {
  return new CClientMsg_CustomGameEvent;
}

void CClientMsg_CustomGameEvent::Clear() {
  if (_has_bits_[0 / 32] & 3) {
    if (has_event_name()) {
      if (event_name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        event_name_->clear();
      }
    }
    if (has_data()) {
      if (data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        data_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool CClientMsg_CustomGameEvent::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:CClientMsg_CustomGameEvent)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string event_name = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_event_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->event_name().data(), this->event_name().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "event_name");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_data;
        break;
      }

      // optional bytes data = 2;
      case 2: {
        if (tag == 18) {
         parse_data:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_data()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:CClientMsg_CustomGameEvent)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:CClientMsg_CustomGameEvent)
  return false;
#undef DO_
}

void CClientMsg_CustomGameEvent::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:CClientMsg_CustomGameEvent)
  // optional string event_name = 1;
  if (has_event_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->event_name().data(), this->event_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "event_name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->event_name(), output);
  }

  // optional bytes data = 2;
  if (has_data()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      2, this->data(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:CClientMsg_CustomGameEvent)
}

::google::protobuf::uint8* CClientMsg_CustomGameEvent::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:CClientMsg_CustomGameEvent)
  // optional string event_name = 1;
  if (has_event_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->event_name().data(), this->event_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "event_name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->event_name(), target);
  }

  // optional bytes data = 2;
  if (has_data()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->data(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:CClientMsg_CustomGameEvent)
  return target;
}

int CClientMsg_CustomGameEvent::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string event_name = 1;
    if (has_event_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->event_name());
    }

    // optional bytes data = 2;
    if (has_data()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->data());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void CClientMsg_CustomGameEvent::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const CClientMsg_CustomGameEvent* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const CClientMsg_CustomGameEvent*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void CClientMsg_CustomGameEvent::MergeFrom(const CClientMsg_CustomGameEvent& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_event_name()) {
      set_event_name(from.event_name());
    }
    if (from.has_data()) {
      set_data(from.data());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void CClientMsg_CustomGameEvent::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void CClientMsg_CustomGameEvent::CopyFrom(const CClientMsg_CustomGameEvent& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CClientMsg_CustomGameEvent::IsInitialized() const {

  return true;
}

void CClientMsg_CustomGameEvent::Swap(CClientMsg_CustomGameEvent* other) {
  if (other != this) {
    std::swap(event_name_, other->event_name_);
    std::swap(data_, other->data_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata CClientMsg_CustomGameEvent::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = CClientMsg_CustomGameEvent_descriptor_;
  metadata.reflection = CClientMsg_CustomGameEvent_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int CClientMsg_TrackedControllerInput::kDataFieldNumber;
#endif  // !_MSC_VER

CClientMsg_TrackedControllerInput::CClientMsg_TrackedControllerInput()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:CClientMsg_TrackedControllerInput)
}

void CClientMsg_TrackedControllerInput::InitAsDefaultInstance() {
}

CClientMsg_TrackedControllerInput::CClientMsg_TrackedControllerInput(const CClientMsg_TrackedControllerInput& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:CClientMsg_TrackedControllerInput)
}

void CClientMsg_TrackedControllerInput::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  data_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

CClientMsg_TrackedControllerInput::~CClientMsg_TrackedControllerInput() {
  // @@protoc_insertion_point(destructor:CClientMsg_TrackedControllerInput)
  SharedDtor();
}

void CClientMsg_TrackedControllerInput::SharedDtor() {
  if (data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete data_;
  }
  if (this != default_instance_) {
  }
}

void CClientMsg_TrackedControllerInput::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* CClientMsg_TrackedControllerInput::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return CClientMsg_TrackedControllerInput_descriptor_;
}

const CClientMsg_TrackedControllerInput& CClientMsg_TrackedControllerInput::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_clientmessages_2eproto();
  return *default_instance_;
}

CClientMsg_TrackedControllerInput* CClientMsg_TrackedControllerInput::default_instance_ = NULL;

CClientMsg_TrackedControllerInput* CClientMsg_TrackedControllerInput::New() const {
  return new CClientMsg_TrackedControllerInput;
}

void CClientMsg_TrackedControllerInput::Clear() {
  if (has_data()) {
    if (data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
      data_->clear();
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool CClientMsg_TrackedControllerInput::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:CClientMsg_TrackedControllerInput)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional bytes data = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_data()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:CClientMsg_TrackedControllerInput)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:CClientMsg_TrackedControllerInput)
  return false;
#undef DO_
}

void CClientMsg_TrackedControllerInput::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:CClientMsg_TrackedControllerInput)
  // optional bytes data = 1;
  if (has_data()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      1, this->data(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:CClientMsg_TrackedControllerInput)
}

::google::protobuf::uint8* CClientMsg_TrackedControllerInput::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:CClientMsg_TrackedControllerInput)
  // optional bytes data = 1;
  if (has_data()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->data(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:CClientMsg_TrackedControllerInput)
  return target;
}

int CClientMsg_TrackedControllerInput::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional bytes data = 1;
    if (has_data()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->data());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void CClientMsg_TrackedControllerInput::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const CClientMsg_TrackedControllerInput* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const CClientMsg_TrackedControllerInput*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void CClientMsg_TrackedControllerInput::MergeFrom(const CClientMsg_TrackedControllerInput& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_data()) {
      set_data(from.data());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void CClientMsg_TrackedControllerInput::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void CClientMsg_TrackedControllerInput::CopyFrom(const CClientMsg_TrackedControllerInput& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CClientMsg_TrackedControllerInput::IsInitialized() const {

  return true;
}

void CClientMsg_TrackedControllerInput::Swap(CClientMsg_TrackedControllerInput* other) {
  if (other != this) {
    std::swap(data_, other->data_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata CClientMsg_TrackedControllerInput::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = CClientMsg_TrackedControllerInput_descriptor_;
  metadata.reflection = CClientMsg_TrackedControllerInput_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
