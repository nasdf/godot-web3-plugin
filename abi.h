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

  String encode_function_inputs(const String &p_name, const Array &p_inputs);
  // void decode_function_outputs(const String &p_name, const Array &p_outputs);
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