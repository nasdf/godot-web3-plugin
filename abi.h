#ifndef ABI_H
#define ABI_H

#include "core/io/resource_loader.h"
#include "core/resource.h"

class ABI : public Resource {
  GDCLASS(ABI, Resource);

  struct Parameter {
    String type;
    String name;
    Vector<Parameter> components;
  };

  struct Function {
    String type;
    String name;
    String signature;
    Vector<Parameter> inputs;
    Vector<Parameter> outputs;
  };

  HashMap<String, Function> functions;
  void parse_function(const Dictionary &p_function);

protected:
  static void _bind_methods();

public:
  Error parse(const String &p_json);

  Array decode_array(const String &p_value, const int p_length, const String &p_type);
  String encode_array(const Array &p_array);

  String encode_uint256(const int p_value);
  int decode_uint256(const String &p_value);

  String encode_address(const String &p_value);
  String decode_address(const String &p_value);

  String encode_function(const String &p_name, const Array &p_inputs);
  Array decode_function(const String &p_name, const String &p_value);
};

class ResourceFormatLoaderABI : public ResourceFormatLoader {
  GDCLASS(ResourceFormatLoaderABI, ResourceFormatLoader);
public:
  virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_no_subresource_cache = false);
  virtual void get_recognized_extensions(List<String> *r_extensions) const;
  virtual bool handles_type(const String &p_type) const;
  virtual String get_resource_type(const String &p_path) const;
};

#endif // ABI_H