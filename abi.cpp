#include "abi.h"

#include "thirdparty/trezor-crypto/sha3.h"

#include "core/io/json.h"
#include "core/os/file_access.h"

String ABI::encode_function_inputs(const String &p_name, const Array &p_inputs) {
  ERR_FAIL_COND_V_MSG(!functions.has(p_name), String(), "Method doesn't exist");
  Function function = functions[p_name];

  Vector<Parameter> inputs = function.inputs;
  ERR_FAIL_COND_V_MSG(inputs.size() != p_inputs.size(), String(), "Method inputs don't match");

  String enc;
  enc += "0x";
  enc += function.signature;

  for (int i = 0; i < p_inputs.size(); i++) {
    Parameter input = inputs[i];
    ERR_FAIL_COND_V_MSG(input.type != "address" && input.type != "uint256", String(), "Unsupported input type");

    String value = p_inputs[i];
    enc += value.trim_prefix("0x").lpad(64, "0");
  }

  return enc;
}

Error ABI::parse(const String &p_json) {
  String error_string;
  int error_line;
  Variant json;

  Error error = JSON::parse(p_json, json, error_string, error_line);
  ERR_FAIL_COND_V(error, error);

  Array entries = json;
  for (int i = 0; i < entries.size(); i++) {
    Dictionary entry = entries[i];
    String type = entry["type"];

    if (type == "function" || type == "receive" || type == "fallback") {
      parse_function(entry);
    }
  }

  return OK;
}

void ABI::parse_function(const Dictionary &p_function) {
  Function function;
  function.name = p_function["name"];
  function.type = p_function["type"];

  Array input_params = p_function["inputs"];
  for (int i = 0; i < input_params.size(); i++) {
    Dictionary param = input_params[i];
    
    Parameter input;
    input.name = param["name"];
    input.type = param["type"];

    function.inputs.push_back(input);
  }

  Array output_params = p_function["outputs"];
  for (int i = 0; i < output_params.size(); i++) {
    Dictionary param = output_params[i];

    Parameter output;
    output.name = param["name"];
    output.type = param["type"];

    function.outputs.push_back(output);
  }

  String signature;
  signature += function.name;
  signature += "(";

  for (int i = 0; i < function.inputs.size(); i++) {
    Parameter input = function.inputs[i];
    if (i > 0) {
      signature += ",";
    }
    signature += input.type;
  }

  signature += ")";

  // TODO move to helper function
  CharString cs = signature.utf8();
  unsigned char hash[32];
  keccak_256((unsigned char *)cs.ptr(), cs.length(), hash);
  function.signature = String::hex_encode_buffer(hash, 4);

  functions[function.name] = function;
}

void ABI::_bind_methods() {
  ClassDB::bind_method(D_METHOD("parse", "json"), &ABI::parse);
}

// ResourceFormatLoader

RES ResourceFormatLoaderABI::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_no_subresource_cache) {
  Vector<uint8_t> buffer = FileAccess::get_file_as_array(p_path);

  String str;
  str.parse_utf8((const char *)buffer.ptr(), buffer.size());

  Ref<ABI> abi = memnew(ABI);
  Error error = abi->parse(str);

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