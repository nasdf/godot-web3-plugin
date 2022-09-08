#include "abi.h"

#include "core/io/json.h"
#include "core/os/file_access.h"

Error ABI::from_json(const String &p_json) {
  String error_string;
  int error_line;
  Variant json;

  Error error = JSON::parse(p_json, json, error_string, error_line);
  if (error != OK) {
    return error;
  }

  // for (int i = 0; i < entries.size(); i++) {
  //   Dictionary entry = entries[i];
  //   String type = entry["type"];

  //   if (type == "function" || type == "constructor" || type == "receive" || type == "fallback") {
  //     Function f;

  //     f.type = type;
  //     f.name = entry["name"];
  //     f.state_mutability = entry["stateMutability"];
      
  //     functions.push_back(f);
  //   }
  // }

  entries = json;
  return OK;
}

void ABI::set_entries(const Array &p_entries) {
  entries = p_entries;
}

Array ABI::get_entries() {
  return entries;
}

void ABI::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_entries", "entries"), &ABI::set_entries);
  ClassDB::bind_method(D_METHOD("get_entries"), &ABI::get_entries);

  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "entries"), "set_entries", "get_entries");
}

RES ResourceFormatLoaderABI::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_no_subresource_cache) {
  if (r_error) {
    *r_error = ERR_FILE_CANT_OPEN;
  }

  Vector<uint8_t> buffer = FileAccess::get_file_as_array(p_path);

  String str;
  str.parse_utf8((const char *)buffer.ptr(), buffer.size());

  Ref<ABI> abi = memnew(ABI);
  Error error = abi->from_json(str);

  if (r_error) {
    *r_error = error;
  }

  return abi;
}

void ResourceFormatLoaderABI::get_recognized_extensions(List<String> *r_extensions) const {
  r_extensions->push_back("json");
}

String ResourceFormatLoaderABI::get_resource_type(const String &p_path) const {
  String el = p_path.get_extension().to_lower();
  if (el == "json") {
    return "ABI";
  }
  return "";
}

bool ResourceFormatLoaderABI::handles_type(const String &p_type) const {
  return (p_type == "ABI");
}