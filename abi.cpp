#include "abi.h"

#include "keccak.h"

#include "core/io/json.h"
#include "core/os/file_access.h"

String ABI::encode_uint256(const int p_value) {
  return String::num_int64(p_value, 16).lpad(64, "0");
}

int ABI::decode_uint256(const String &p_value) {
  return p_value.hex_to_int(false);
}

String ABI::encode_address(const String &p_address) {
  return p_address.trim_prefix("0x").lpad(64, "0");
}

String ABI::decode_address(const String &p_value) {
  return p_value.substr(44);
}

String ABI::encode_array(const Array &p_array) {
  String encoded;
  for (int i = 0; i < p_array.size(); i++) {
    encoded += encode_address(p_array[i]);
  }
  return encoded;
}

Array ABI::decode_array(const String &p_value, const int p_length, const String &p_type) {
  Array array;
  for (int i = 0; i < p_length; i++) {
    String entry = p_value.substr(i * 64, 64);
    if (p_type == "address") {
      array.push_back(decode_address(entry));
    } else if (p_type == "uint256") {
      array.push_back(decode_uint256(entry));
    }
  }
  return array;
}

String ABI::encode_function(const String &p_name, const Array &p_inputs) {
  ERR_FAIL_COND_V_MSG(!functions.has(p_name), String(), "Method doesn't exist");
  
  Function function = functions[p_name];
  Vector<Parameter> inputs = function.inputs;
  
  // ERR_FAIL_COND_V_MSG(inputs.size() != p_inputs.size(), String(), "Method inputs don't match");

  String head;
  String body;

  int offset = p_inputs.size();
  for (int i = 0; i < inputs.size(); i++) {
    Parameter input = inputs[i];
    if (input.type == "address" || input.type == "uint256") {
      String value = p_inputs[i];
      head += encode_address(value);
    } else if (input.type == "address[]" || input.type == "uint256[]") {
      Array value = p_inputs[i];
      head += encode_uint256(offset * 32);
      body += encode_uint256(value.size());
      body += encode_array(value);
      offset += value.size() + 1;
    } else {
      ERR_FAIL_COND_V_MSG(true, String(), "Unsupported input type");
    }
  }

  return "0x" + function.signature + head + body;
}

Array ABI::decode_function(const String &p_name, const String &p_value) {
  ERR_FAIL_COND_V_MSG(!functions.has(p_name), Array(), "Method doesn't exist");

  Function function = functions[p_name];
  Vector<Parameter> outputs = function.outputs;

  Array out;
  String enc = p_value.trim_prefix("0x");

  for (int i = 0; i < outputs.size(); i++) {
    Parameter output = outputs[i];
    if (output.type == "address") {
      String data = enc.substr(i * 64, 64);
      String value = decode_address(data);
      out.push_back(value);
    } else if (output.type == "uint256") {
      String data = enc.substr(i * 64, 64);
      int value = decode_uint256(data);
      out.push_back(value);
    } else if (output.type == "address[]") {
      int offset = enc.substr(i * 64, 64).hex_to_int(false);
      int length = enc.substr(offset * 2, 64).hex_to_int(false);
      String data = enc.substr(offset * 2 + 64, length * 64);
      Array value = decode_array(data, length, "address");
      out.push_back(value);
    } else if (output.type == "uint256[]") {
      int offset = enc.substr(i * 64, 64).hex_to_int(false);
      int length = enc.substr(offset * 2, 64).hex_to_int(false);
      String data = enc.substr(offset * 2 + 64, length * 64);
      Array value = decode_array(data, length, "uint256");
      out.push_back(value);
    }
  }

  return out;
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

  Vector<String> inputs;
  for (int i = 0; i < function.inputs.size(); i++) {
    inputs.push_back(function.inputs[i].type);
  }

  String signature;
  signature += function.name;
  signature += "(";
  signature += String(",").join(inputs);
  signature += ")";

  CharString cs = signature.utf8();
  uint8_t hash[32];

  Keccak::hash((unsigned char *)cs.ptr(), cs.length(), hash);
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